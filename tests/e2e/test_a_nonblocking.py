"""Categoria A - Nao-bloqueante: nada pode travar o loop de eventos."""
import threading
import time

import helpers
import pytest


def test_a1_slow_download_does_not_block_fast_gets(server):
    """A1: um cliente lento baixando arquivo grande nao pode segurar GETs rapidos."""
    slow = server.client(timeout=30)
    slow.send(helpers.build_request("GET", "/big.bin"))
    # le so um pedacinho e para de drenar, simulando cliente lento
    slow.sock.recv(256)

    # enquanto isso, um GET rapido tem que responder depressa
    t0 = time.time()
    res = server.get("/index.html", timeout=5)
    elapsed = time.time() - t0

    assert res.status == 200
    assert elapsed < 2.0, "GET rapido bloqueou por %.2fs com cliente lento ativo" % elapsed
    slow.close()


def test_a2_idle_connection_does_not_block(server):
    """A2: conexao que abre e nao envia nada nao pode travar o servidor."""
    idle = server.client(timeout=30)  # nao envia nada
    try:
        res = server.get("/index.html", timeout=5)
        assert res.status == 200
    finally:
        idle.close()


def test_a3_slow_cgi_does_not_block_other_clients(server):
    """A3: CGI lento (sleep 3s) em uma conexao nao pode segurar GETs paralelos."""
    cgi = server.client(timeout=30)
    cgi.send(helpers.build_request("GET", "/cgi-bin/sleep.py?secs=3"))

    t0 = time.time()
    res = server.get("/index.html", timeout=5)
    elapsed = time.time() - t0

    assert res.status == 200
    assert elapsed < 2.0, "GET esperou o CGI lento (%.2fs)" % elapsed
    cgi.close()


def test_a4_concurrent_fast_requests(server):
    """A4: rajada de requests concorrentes, todas respondem."""
    results = []
    errors = []

    def worker():
        try:
            r = server.get("/index.html", timeout=10)
            results.append(r.status)
        except Exception as e:  # noqa: BLE001
            errors.append(repr(e))

    threads = [threading.Thread(target=worker) for _ in range(30)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    assert not errors, "erros em requests concorrentes: %s" % errors[:3]
    assert results.count(200) == 30
