"""Categoria E - Requisicoes interrompidas (normal e CGI)."""
import time

import helpers
import pytest


def test_e1_close_during_headers(server):
    """E1: cliente fecha no meio dos headers. Servidor segue vivo, sem leak."""
    base = helpers.fd_count(server.pid)
    for _ in range(30):
        c = server.client()
        c.send("GET /index.html HTTP/1.1\r\nHost: x\r\nX-Partial: yes")  # incompleto
        c.close()
    # servidor continua atendendo
    assert server.get("/index.html").status == 200
    assert helpers.wait_until(
        lambda: helpers.fd_count(server.pid) <= base + 2, timeout=5), \
        "fds vazaram apos fechar no meio dos headers"


def test_e2_close_during_body(server):
    """E2: cliente anuncia Content-Length grande e fecha no meio do body."""
    base = helpers.fd_count(server.pid)
    for _ in range(20):
        c = server.client()
        c.send("POST /upload/x.bin HTTP/1.1\r\nHost: x\r\n"
               "Content-Type: application/octet-stream\r\n"
               "Content-Length: 100000\r\n\r\n")
        c.send(b"only-a-little")  # muito menos que o anunciado
        c.abort()
    assert server.get("/index.html").status == 200
    assert helpers.wait_until(
        lambda: helpers.fd_count(server.pid) <= base + 2, timeout=5)


def test_e3_close_during_response_drain(server):
    """E3: cliente fecha enquanto uma resposta grande esta sendo drenada."""
    base = helpers.fd_count(server.pid)
    for _ in range(20):
        c = server.client()
        c.send(helpers.build_request("GET", "/big.bin"))
        c.sock.recv(512)   # le so o comeco
        c.abort()          # RST no meio da drenagem
    assert server.get("/index.html").status == 200
    assert helpers.wait_until(
        lambda: helpers.fd_count(server.pid) <= base + 2, timeout=5), \
        "FileChunkSource nao fechado apos cliente sumir na drenagem (R4)"


def test_e4_close_during_cgi(server):
    """E4: cliente some enquanto o CGI roda. Sem zumbi, sem leak de pipe."""
    base = helpers.fd_count(server.pid)
    for _ in range(10):
        c = server.client()
        c.send(helpers.build_request("GET", "/cgi-bin/sleep.py?secs=3"))
        time.sleep(0.1)  # garante que o CGI ja iniciou
        c.abort()
    time.sleep(1)
    assert server.get("/index.html").status == 200
    assert not helpers.zombie_children(server.pid), "CGI virou zumbi apos cliente sumir"
    assert helpers.wait_until(
        lambda: helpers.fd_count(server.pid) <= base + 2, timeout=8), \
        "pipes do CGI vazaram apos CGI_CLIENT_GONE (R6)"


def test_e5_rst_vs_fin(server):
    """E5: RST abrupto e FIN normal, ambos tratados sem crash."""
    # FIN normal
    c = server.client()
    c.send("GET /index.html HTTP/1.1\r\nHost: x\r\n")
    c.close()
    # RST
    c = server.client()
    c.send("GET /index.html HTTP/1.1\r\nHost: x\r\n")
    c.abort()
    assert server.get("/index.html").status == 200
