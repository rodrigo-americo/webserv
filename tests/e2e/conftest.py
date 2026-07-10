"""Fixtures E2E: builda o webserv, sobe instancias com config propria e derruba no fim.

Variaveis de ambiente:
  WEBSERV_MUX      select | poll | epoll   (default: poll)
  WEBSERV_VALGRIND 1 -> roda o servidor sob valgrind (make leaks)
  WEBSERV_BIN      caminho do binario (default: <repo>/webserver, builda se faltar)
"""
import os
import shutil
import signal
import socket
import subprocess
import sys
import tempfile
import time

import pytest

sys.path.insert(0, os.path.dirname(__file__))
import helpers  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
MUX = os.environ.get("WEBSERV_MUX", "poll")
USE_VALGRIND = os.environ.get("WEBSERV_VALGRIND") == "1"

CONFIG_TEMPLATE = """
events {{
    worker_connections 1024;
    use {mux};
}}

http {{
    server {{
        listen 127.0.0.1:{port};
        server_name localhost;
        client_max_body_size {max_body};

        location / {{
            root {www};
            index index.html;
            autoindex on;
            allow_methods GET;
        }}

        location /upload/ {{
            upload_dir {uploads};
            allow_methods POST DELETE;
        }}

        location /cgi-bin/ {{
            root {www};
            cgi_extension .py {python};
            allow_methods GET POST;
        }}

        location /secrets/ {{
            root {www};
            require_auth on;
        }}

        location /redirect/ {{
            return 301 http://example.com/;
        }}
    }}
}}
"""

CGI_ECHO = """#!/usr/bin/env python3
import os, sys
length = int(os.environ.get("CONTENT_LENGTH") or 0)
body = sys.stdin.buffer.read(length) if length else sys.stdin.buffer.read()
sys.stdout.write("Content-Type: application/octet-stream\\r\\n")
sys.stdout.write("Content-Length: %d\\r\\n\\r\\n" % len(body))
sys.stdout.flush()
sys.stdout.buffer.write(body)
"""

CGI_SLEEP = """#!/usr/bin/env python3
import os, sys, time
qs = os.environ.get("QUERY_STRING", "")
secs = 3.0
for kv in qs.split("&"):
    if kv.startswith("secs="):
        secs = float(kv[5:])
time.sleep(secs)
sys.stdout.write("Content-Type: text/plain\\r\\n\\r\\nslept %s\\n" % secs)
"""

CGI_LOOP = """#!/usr/bin/env python3
while True:
    pass
"""

CGI_NOREAD = """#!/usr/bin/env python3
# Nunca le o stdin: forca o pipe a encher (F2).
import time
time.sleep(120)
"""


def free_port():
    s = socket.socket()
    s.bind(("127.0.0.1", 0))
    port = s.getsockname()[1]
    s.close()
    return port


def _write(path, content, executable=False):
    with open(path, "w") as f:
        f.write(content)
    if executable:
        os.chmod(path, 0o755)


class ServerInstance(object):
    """Uma instancia do webserv com config e www proprios."""

    def __init__(self, binary, mux=MUX, max_body=0, valgrind=USE_VALGRIND):
        self.host = "127.0.0.1"
        self.port = free_port()
        self.dir = tempfile.mkdtemp(prefix="webserv_e2e_")
        self.www = os.path.join(self.dir, "www")
        self.uploads = os.path.join(self.dir, "uploads")
        self.valgrind_log = os.path.join(self.dir, "valgrind.log")
        self._build_tree()
        self.config = os.path.join(self.dir, "server.conf")
        _write(self.config, CONFIG_TEMPLATE.format(
            mux=mux, port=self.port, www=self.www, uploads=self.uploads,
            max_body=max_body, python=shutil.which("python3") or "/usr/bin/python3"))

        cmd = [binary, self.config]
        if valgrind:
            cmd = ["valgrind", "--leak-check=full", "--show-leak-kinds=definite,indirect",
                   "--track-fds=yes", "--error-exitcode=42",
                   "--log-file=" + self.valgrind_log] + cmd
        self.proc = subprocess.Popen(
            cmd, cwd=self.dir,
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        self._wait_ready()

    def _build_tree(self):
        os.makedirs(os.path.join(self.www, "cgi-bin"))
        os.makedirs(os.path.join(self.www, "secrets"))
        os.makedirs(self.uploads)
        _write(os.path.join(self.www, "index.html"),
               "<html><body>hello e2e</body></html>\n")
        _write(os.path.join(self.www, "secrets", "top.html"), "secret\n")
        # arquivo grande para testes de streaming / cliente lento (8MB)
        with open(os.path.join(self.www, "big.bin"), "wb") as f:
            f.write(os.urandom(8 * 1024 * 1024))
        cgi = os.path.join(self.www, "cgi-bin")
        _write(os.path.join(cgi, "echo.py"), CGI_ECHO, executable=True)
        _write(os.path.join(cgi, "sleep.py"), CGI_SLEEP, executable=True)
        _write(os.path.join(cgi, "loop.py"), CGI_LOOP, executable=True)
        _write(os.path.join(cgi, "noread.py"), CGI_NOREAD, executable=True)
        _write(os.path.join(cgi, "env.py"), CGI_ENV, executable=True)

    def _wait_ready(self, timeout=15.0):
        deadline = time.time() + timeout
        while time.time() < deadline:
            if self.proc.poll() is not None:
                raise RuntimeError(
                    "servidor morreu no boot (exit=%s)" % self.proc.returncode)
            try:
                socket.create_connection((self.host, self.port), 0.2).close()
                return
            except OSError:
                time.sleep(0.05)
        raise RuntimeError("servidor nao abriu a porta %d" % self.port)

    # -------------------------------------------------------------- consultas
    @property
    def pid(self):
        return self.proc.pid

    def is_alive(self):
        return self.proc.poll() is None

    def get(self, path, **kw):
        return helpers.get(self.host, self.port, path, **kw)

    def request(self, method, path, **kw):
        return helpers.request(self.host, self.port, method, path, **kw)

    def client(self, timeout=helpers.DEFAULT_TIMEOUT):
        return helpers.RawClient(self.host, self.port, timeout=timeout)

    def upload_files(self):
        return sorted(os.listdir(self.uploads))

    def upload_path(self, name):
        return os.path.join(self.uploads, name)

    # ---------------------------------------------------------------- takedown
    def stop(self):
        if self.proc.poll() is None:
            self.proc.send_signal(signal.SIGINT)
            try:
                self.proc.wait(timeout=10 if not USE_VALGRIND else 60)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait()
        shutil.rmtree(self.dir, ignore_errors=not USE_VALGRIND)

    def valgrind_report(self):
        if os.path.exists(self.valgrind_log):
            with open(self.valgrind_log) as f:
                return f.read()
        return ""


# ------------------------------------------------------------------- fixtures

@pytest.fixture(scope="session")
def binary():
    path = os.environ.get("WEBSERV_BIN", os.path.join(REPO_ROOT, "webserver"))
    if not os.path.exists(path):
        subprocess.check_call(["make", "-j4"], cwd=REPO_ROOT)
    assert os.path.exists(path), "binario nao encontrado: %s" % path
    return path


@pytest.fixture()
def server(binary):
    """Instancia limpa por teste (sem limite de body)."""
    srv = ServerInstance(binary)
    yield srv
    alive = srv.is_alive()
    srv.stop()
    assert alive, "SERVIDOR MORREU durante o teste (exit=%s)" % srv.proc.returncode


@pytest.fixture()
def server_limited(binary):
    """Instancia com client_max_body_size 1024 (decisao do time, C6/C7)."""
    srv = ServerInstance(binary, max_body=1024)
    yield srv
    alive = srv.is_alive()
    srv.stop()
    assert alive, "SERVIDOR MORREU durante o teste (exit=%s)" % srv.proc.returncode


def pytest_configure(config):
    config.addinivalue_line("markers", "slow: testes que esperam timeouts reais (30s+)")
    config.addinivalue_line("markers", "stress: testes de carga pesada")

CGI_ENV = """#!/usr/bin/env python3
import os, sys
body = "QUERY_STRING=%s\\nREQUEST_METHOD=%s\\n" % (
    os.environ.get("QUERY_STRING", ""), os.environ.get("REQUEST_METHOD", ""))
sys.stdout.write("Content-Type: text/plain\\r\\n")
sys.stdout.write("Content-Length: %d\\r\\n\\r\\n" % len(body))
sys.stdout.write(body)
"""
