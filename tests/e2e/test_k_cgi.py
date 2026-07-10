"""Categoria K - Correcao do CGI.

Reaproveita o /cgi-bin/echo.py do conftest (le CONTENT_LENGTH bytes do stdin e
devolve o mesmo body). Isso permite testar sem novos scripts:
  - CGI responde GET e POST;
  - integridade do body que passa pelo pipe stdin do CGI (round-trip);
  - headers emitidos pelo script (Content-Type) chegam ao cliente;
  - body grande nao causa deadlock no pipe (caminho feliz do F2).

test_k5 depende de um script env.py opcional (ver instrucoes no fim da entrega):
faz skip se ele nao existir.
"""
import os

import helpers_ext
import pytest


def test_k1_cgi_get_200(server):
    res = server.get("/cgi-bin/echo.py")
    assert res.status == 200, "CGI GET deveria responder 200: %s" % res


def test_k2_cgi_post_body_roundtrip(server):
    """POST -> stdin do CGI -> stdout -> cliente, byte a byte igual."""
    payload = b"cgi-\x00\x01\r\n\r\n\xfe\xff-body" + os.urandom(1024)
    res = helpers_ext.request_full(
        server, "POST", "/cgi-bin/echo.py",
        headers={"Content-Type": "application/octet-stream"}, body=payload)
    assert res.status == 200, res
    assert res.body == payload, \
        "echo.py deveria devolver o body identico (%d vs %d bytes)" % (
            len(res.body), len(payload))


def test_k3_cgi_script_headers_passthrough(server):
    """O Content-Type emitido pelo script deve chegar ao cliente."""
    res = server.get("/cgi-bin/echo.py")
    assert res.status == 200, res
    ct = res.headers.get("content-type", "").lower()
    assert "application/octet-stream" in ct, \
        "Content-Type do CGI nao repassado: %r" % ct


def test_k4_cgi_large_post_no_deadlock(server):
    """256KB no stdin do CGI: nao pode deadlockar e o body tem que voltar intacto."""
    payload = os.urandom(256 * 1024)
    res = helpers_ext.request_full(
        server, "POST", "/cgi-bin/echo.py",
        headers={"Content-Type": "application/octet-stream"},
        body=payload, timeout=30)
    assert res.status == 200, res
    assert res.body == payload, \
        "CGI perdeu/corrompeu body grande (%d vs %d bytes)" % (
            len(res.body), len(payload))
    assert server.is_alive()


def test_k5_cgi_query_string(server):
    """QUERY_STRING deve chegar ao CGI. Precisa de env.py (opcional) no conftest."""
    res = server.get("/cgi-bin/env.py?foo=bar&x=1")
    if res.status == 404:
        pytest.skip("env.py ausente no conftest; adicione CGI_ENV (ver instrucoes)")
    assert res.status == 200, res
    assert b"foo=bar&x=1" in res.body, \
        "QUERY_STRING nao repassada ao CGI: %r" % res.body[:200]
