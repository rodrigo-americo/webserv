"""Helpers de baixo nivel para os testes E2E do webserv.

Sockets crus para controlar exatamente o que vai pro fio:
requests parciais, lentas, abortadas, RST, etc.
"""
import os
import socket
import struct
import time


DEFAULT_TIMEOUT = 10.0


class HttpResponse(object):
    def __init__(self, status, reason, headers, body, raw):
        self.status = status
        self.reason = reason
        self.headers = headers  # dict lower-case key -> value
        self.body = body        # bytes
        self.raw = raw          # bytes

    def __repr__(self):
        return "<HttpResponse %d %s len(body)=%d>" % (
            self.status, self.reason, len(self.body))


def parse_response(raw):
    """Parseia UMA resposta HTTP a partir de bytes crus."""
    if not raw:
        raise AssertionError("resposta vazia (servidor fechou sem responder?)")
    head, sep, rest = raw.partition(b"\r\n\r\n")
    if not sep:
        raise AssertionError("resposta sem fim de headers: %r" % raw[:200])
    lines = head.split(b"\r\n")
    parts = lines[0].split(b" ", 2)
    assert parts[0].startswith(b"HTTP/"), "status line invalida: %r" % lines[0]
    status = int(parts[1])
    reason = parts[2].decode("latin-1", "replace") if len(parts) > 2 else ""
    headers = {}
    for line in lines[1:]:
        k, _, v = line.partition(b":")
        headers[k.strip().lower().decode("latin-1")] = v.strip().decode("latin-1")
    return HttpResponse(status, reason, headers, rest, raw)


class RawClient(object):
    """Conexao TCP crua com o servidor."""

    def __init__(self, host, port, timeout=DEFAULT_TIMEOUT):
        self.sock = socket.create_connection((host, port), timeout=timeout)
        self.sock.settimeout(timeout)

    def send(self, data):
        if isinstance(data, str):
            data = data.encode("latin-1")
        self.sock.sendall(data)
        return self

    def recv_until_close(self, max_bytes=64 * 1024 * 1024, timeout=None):
        if timeout is not None:
            self.sock.settimeout(timeout)
        chunks = []
        total = 0
        while total < max_bytes:
            try:
                chunk = self.sock.recv(65536)
            except socket.timeout:
                break
            if not chunk:
                break
            chunks.append(chunk)
            total += len(chunk)
        return b"".join(chunks)

    def recv_response(self, timeout=None):
        """Le ate o servidor fechar (sem keep-alive) e parseia."""
        return parse_response(self.recv_until_close(timeout=timeout))

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass

    def abort(self):
        """Fecha com RST (SO_LINGER 0) em vez de FIN."""
        try:
            self.sock.setsockopt(
                socket.SOL_SOCKET, socket.SO_LINGER, struct.pack("ii", 1, 0))
        except OSError:
            pass
        self.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()


def build_request(method, path, host="localhost", headers=None, body=b""):
    if isinstance(body, str):
        body = body.encode("latin-1")
    h = {"Host": host}
    if body:
        h["Content-Length"] = str(len(body))
    if headers:
        h.update(headers)
    lines = ["%s %s HTTP/1.1" % (method, path)]
    for k, v in h.items():
        if v is None:
            continue
        lines.append("%s: %s" % (k, v))
    raw = ("\r\n".join(lines) + "\r\n\r\n").encode("latin-1") + body
    return raw


def request(host, port, method, path, headers=None, body=b"",
            timeout=DEFAULT_TIMEOUT):
    """Request completo -> resposta parseada. Uma conexao por request."""
    with RawClient(host, port, timeout=timeout) as c:
        c.send(build_request(method, path, headers=headers, body=body))
        return c.recv_response()


def get(host, port, path, timeout=DEFAULT_TIMEOUT, headers=None):
    return request(host, port, "GET", path, headers=headers, timeout=timeout)


def multipart_body(files, boundary="XteStBoundarY42"):
    """files: lista de (filename, bytes). Retorna (body, content_type)."""
    parts = []
    for name, data in files:
        parts.append(b"--" + boundary.encode() + b"\r\n")
        parts.append(
            ('Content-Disposition: form-data; name="file"; filename="%s"\r\n'
             % name).encode())
        parts.append(b"Content-Type: application/octet-stream\r\n\r\n")
        parts.append(data if isinstance(data, bytes) else data.encode())
        parts.append(b"\r\n")
    parts.append(b"--" + boundary.encode() + b"--\r\n")
    body = b"".join(parts)
    return body, "multipart/form-data; boundary=%s" % boundary


def chunked_body(data, chunk_size=1000):
    """Codifica bytes em Transfer-Encoding: chunked."""
    if isinstance(data, str):
        data = data.encode("latin-1")
    out = []
    for i in range(0, len(data), chunk_size):
        chunk = data[i:i + chunk_size]
        out.append(("%x\r\n" % len(chunk)).encode())
        out.append(chunk)
        out.append(b"\r\n")
    out.append(b"0\r\n\r\n")
    return b"".join(out)


# ---------------------------------------------------------------- /proc utils

def fd_count(pid):
    return len(os.listdir("/proc/%d/fd" % pid))


def open_fds(pid):
    out = {}
    base = "/proc/%d/fd" % pid
    for name in os.listdir(base):
        try:
            out[name] = os.readlink(os.path.join(base, name))
        except OSError:
            pass
    return out


def rss_kb(pid):
    with open("/proc/%d/status" % pid) as f:
        for line in f:
            if line.startswith("VmRSS:"):
                return int(line.split()[1])
    return -1


def zombie_children(pid):
    """PIDs de filhos <defunct> do processo."""
    zombies = []
    for entry in os.listdir("/proc"):
        if not entry.isdigit():
            continue
        try:
            with open("/proc/%s/stat" % entry) as f:
                fields = f.read().split()
            if fields[3] == str(pid) and fields[2] == "Z":
                zombies.append(int(entry))
        except (IOError, OSError, IndexError):
            continue
    return zombies


def wait_until(predicate, timeout=5.0, interval=0.05):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if predicate():
            return True
        time.sleep(interval)
    return False
