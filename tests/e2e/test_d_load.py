"""Categoria D - Alta carga. Marcados 'stress' (rode com: pytest -m stress)."""
import shutil
import subprocess
import threading

import helpers
import pytest
import json

pytestmark = pytest.mark.stress


def test_d1_siege_static(server):
    """D1: siege -c 50 -t 15s em arquivo estatico. Disponibilidade 100%, sem crash."""
    if not shutil.which("siege"):
        pytest.skip("siege nao instalado")
    url = "http://%s:%d/index.html" % (server.host, server.port)
    out = subprocess.run(
        ["siege", "-b", "-c", "50", "-t", "15S", url],
        capture_output=True, text=True, timeout=60)
    report = out.stdout + out.stderr
    assert server.is_alive(), "servidor caiu sob carga do siege"
    # siege reporta "Availability: 100.00 %"
    start = report.find("{")
    end = report.rfind("}")
    assert start != -1 and end != -1, f"sem bloco JSON no output:\n{report[:500]}"
    stats = json.loads(report[start:end + 1])

    assert stats["availability"] == 100.00, f"disponibilidade < 100%: {stats['availability']}"
    assert stats["failed_transactions"] == 0, f"transacoes falhas: {stats['failed_transactions']}"


def test_d2_mixed_load_no_leak(server):
    """D2: rajada mista (estatico + CGI + upload) concorrente, sem leak apos."""
    server.get("/index.html")
    base = helpers.fd_count(server.pid)
    errors = []

    def hammer_static():
        try:
            for _ in range(100):
                server.get("/index.html")
        except Exception as e:  # noqa: BLE001
            errors.append(("static", repr(e)))

    def hammer_cgi():
        try:
            for _ in range(30):
                server.get("/cgi-bin/echo.py")
        except Exception as e:  # noqa: BLE001
            errors.append(("cgi", repr(e)))

    def hammer_upload():
        try:
            for i in range(30):
                body, ct = helpers.multipart_body([("u%d.bin" % i, b"data" * 50)])
                server.request("POST", "/upload/",
                             headers={"Content-Type": ct}, body=body)
        except Exception as e:  # noqa: BLE001
            errors.append(("upload", repr(e)))

    threads = ([threading.Thread(target=hammer_static) for _ in range(4)] +
               [threading.Thread(target=hammer_cgi) for _ in range(2)] +
               [threading.Thread(target=hammer_upload) for _ in range(2)])
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    assert not errors, "erros sob carga mista: %s" % errors[:3]
    assert server.is_alive()
    assert helpers.wait_until(
        lambda: helpers.fd_count(server.pid) <= base + 3, timeout=8), \
        "fds vazaram apos carga mista"


def test_d4_fd_exhaustion_recovery(server):
    """D4: muitas conexoes abertas simultaneamente; ao fechar, servidor se recupera."""
    conns = []
    try:
        for _ in range(200):
            try:
                conns.append(server.client(timeout=2))
            except OSError:
                break
    finally:
        for c in conns:
            c.close()
    # depois de liberar, tem que voltar a atender
    assert helpers.wait_until(
        lambda: _try_get(server), timeout=8), "servidor nao se recuperou de EMFILE"


def _try_get(server):
    try:
        return server.get("/index.html", timeout=2).status == 200
    except OSError:
        return False
