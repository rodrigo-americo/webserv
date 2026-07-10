"""Categoria F - Timeouts."""
import time

import helpers
import pytest


@pytest.mark.slow
def test_f1_cgi_infinite_loop_times_out(server):
    """F1: CGI em loop infinito -> 504 em ~30s, processo colhido."""
    t0 = time.time()
    res = server.get("/cgi-bin/loop.py", timeout=40)
    elapsed = time.time() - t0
    assert res.status == 504, "esperado 504 Gateway Timeout, veio %s" % res
    assert 25 < elapsed < 38, "timeout fora da janela esperada: %.1fs" % elapsed
    time.sleep(1)
    assert not helpers.zombie_children(server.pid)


@pytest.mark.slow
def test_f2_cgi_not_reading_stdin_no_deadlock(server):
    """F2: CGI que nunca le stdin com body grande -> nao trava, aplica timeout."""
    body = b"z" * (256 * 1024)
    t0 = time.time()
    res = server.request("POST", "/cgi-bin/noread.py",
                        headers={"Content-Type": "application/octet-stream"},
                        body=body, timeout=40)
    elapsed = time.time() - t0
    # o importante: nao deadlockou; retornou algo (504) dentro do timeout do CGI
    assert res.status in (502, 504), res
    assert elapsed < 38, "deadlock: nao respondeu no tempo do timeout"
    assert server.is_alive()


def test_f3_slowloris_client(server):
    """F3: slowloris (headers gotejados sem terminar).

    GAP CONHECIDO (decisao do time 09/07): nao existe timeout de cliente,
    entao o servidor NAO fecha essa conexao. Marcado xfail. O subject exige
    que "uma solicitacao nunca deve travar indefinidamente" (IV.1) -> risco
    de defesa. Quando R1 for implementado, remover o xfail.
    """
    c = server.client(timeout=8)
    c.send("GET /index.html HTTP/1.1\r\n")
    deadline = time.time() + 6
    got_response = False
    try:
        while time.time() < deadline:
            c.send("X-Pad: aaaa\r\n")  # nunca envia o \r\n final
            time.sleep(0.5)
            c.sock.settimeout(0.3)
            try:
                if c.sock.recv(16):
                    got_response = True
                    break
            except Exception:  # noqa: BLE001
                pass
    finally:
        c.close()
    if not got_response:
        pytest.xfail("R1: sem timeout de cliente (gap conhecido)")


@pytest.mark.slow
def test_f4_session_expires(server):
    """F4: cookie de sessao expira em 15s -> rota protegida volta a barrar."""
    login = server.get("/login")
    cookie = login.headers.get("set-cookie", "")
    assert "session_id=" in cookie, "login nao setou cookie: %s" % login
    sid = cookie.split("session_id=")[1].split(";")[0]

    ok = server.get("/secrets/top.html", headers={"Cookie": "session_id=" + sid})
    assert ok.status == 200, "sessao valida deveria acessar: %s" % ok

    time.sleep(16)
    expired = server.get("/secrets/top.html", headers={"Cookie": "session_id=" + sid})
    assert expired.status == 403, "sessao expirada deveria dar 403: %s" % expired
