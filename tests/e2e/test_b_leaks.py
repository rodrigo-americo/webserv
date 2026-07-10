"""Categoria B - Leaks: fds, zumbis e memoria.

Estes testes inspecionam /proc do processo do servidor diretamente.
Nao dependem de valgrind (que roda separado via `make leaks`).
"""
import time

import helpers
import pytest


def _baseline_fds(server):
    # deixa o servidor estabilizar antes de medir
    server.get("/index.html")
    time.sleep(0.2)
    return helpers.fd_count(server.pid)


def test_b2_fd_count_stable_after_many_cycles(server):
    """B2: apos centenas de ciclos variados, a contagem de fds volta ao baseline."""
    base = _baseline_fds(server)

    for _ in range(150):
        # request normal
        server.get("/index.html")
        # request abortada no meio dos headers
        c = server.client()
        c.send("GET /index.html HTTP/1.1\r\nHost: x\r\n")  # sem terminar
        c.abort()
        # CGI ok
        server.get("/cgi-bin/echo.py")

    assert helpers.wait_until(
        lambda: helpers.fd_count(server.pid) <= base + 2, timeout=5), \
        "fds vazando: baseline=%d agora=%d\n%s" % (
            base, helpers.fd_count(server.pid), helpers.open_fds(server.pid))


def test_b3_no_zombie_processes_after_cgi(server):
    """B3: CGI normal, timeout e cliente-sumiu nao podem deixar <defunct>."""
    for _ in range(20):
        server.get("/cgi-bin/echo.py")
        # cliente some no meio de um CGI
        c = server.client()
        c.send(helpers.build_request("GET", "/cgi-bin/sleep.py?secs=2"))
        c.sock.recv(1)  # pode dar timeout/vazio, tudo bem
        c.abort()

    zombies = helpers.zombie_children(server.pid)
    assert not helpers.wait_until(
        lambda: helpers.zombie_children(server.pid), timeout=3), \
        "processos zumbis: %s" % zombies


def test_b4_rss_does_not_grow_unbounded(server):
    """B4: onda de 1000 requests nao pode fazer RSS crescer monotonicamente."""
    server.get("/index.html")
    time.sleep(0.2)
    rss_start = helpers.rss_kb(server.pid)

    for _ in range(1000):
        server.get("/index.html")

    time.sleep(0.3)
    rss_end = helpers.rss_kb(server.pid)
    growth = rss_end - rss_start
    # tolerancia generosa: allocador pode reter memoria. Alerta se dobrar.
    assert growth < rss_start, \
        "RSS cresceu %d KB (de %d para %d) - possivel leak" % (
            growth, rss_start, rss_end)
