"""Categoria J - Conexoes persistentes e semantica de Connection.

O helpers original assume "servidor fecha apos cada resposta". Isso pode ser uma
decisao valida (nao implementar keep-alive), mas entao o servidor DEVE sinalizar
isso via 'Connection: close', senao um cliente HTTP/1.1 - que assume persistencia
por padrao (RFC 9112) - fica esperando por respostas na mesma conexao.

Estes testes checam CONSISTENCIA, nao uma escolha especifica:
  - ou keep-alive funciona (2 requests numa conexao), ou
  - o servidor fecha mas avisa com 'Connection: close'.
Nao ha suposicao sobre qual das duas o time escolheu.
"""
import socket

import helpers
import helpers_ext
import pytest


def test_j1_keepalive_or_signaled_close(server):
    c = server.client(timeout=8)
    try:
        c.send(helpers.build_request("GET", "/index.html"))
        r1 = helpers_ext.recv_one_response(c.sock, timeout=8, method="GET")
        assert r1.status == 200, r1

        conn_hdr = r1.headers.get("connection", "").lower()
        framed = ("content-length" in r1.headers or
                  "chunked" in r1.headers.get("transfer-encoding", "").lower())

        # tenta uma 2a request na MESMA conexao
        try:
            c.send(helpers.build_request("GET", "/index.html"))
            r2 = helpers_ext.recv_one_response(c.sock, timeout=5, method="GET")
            alive = (r2.status == 200)
        except (AssertionError, OSError):
            alive = False

        if alive:
            return  # keep-alive funcional: ok

        # servidor fechou apos 1 request
        if not framed:
            # resposta era delimitada por fechamento: keep-alive nao se aplica
            pytest.skip("resposta delimitada por fechamento (sem CL/chunked)")
        assert "close" in conn_hdr, (
            "fechou apos 1 request numa resposta enquadrada, sem enviar "
            "'Connection: close' - cliente HTTP/1.1 ficaria pendurado")
    finally:
        c.close()


def test_j2_connection_close_is_honored(server):
    """Com 'Connection: close' no request, o servidor deve fechar apos responder."""
    c = server.client(timeout=8)
    try:
        c.send(helpers.build_request("GET", "/index.html",
                                     headers={"Connection": "close"}))
        r1 = helpers_ext.recv_one_response(c.sock, timeout=8, method="GET")
        assert r1.status == 200, r1

        c.sock.settimeout(3)
        try:
            extra = c.sock.recv(64)
            closed = (extra == b"")     # EOF limpo
        except socket.timeout:
            closed = False              # conexao seguiu aberta e ociosa
        except OSError:
            closed = True               # RST/erro: tambem fechou
        assert closed, "servidor nao fechou a conexao apos 'Connection: close'"
    finally:
        c.close()


def test_j3_second_request_content_is_correct(server):
    """Se keep-alive existe, a 2a resposta nao pode 'vazar' bytes da 1a."""
    c = server.client(timeout=8)
    try:
        c.send(helpers.build_request("GET", "/index.html"))
        r1 = helpers_ext.recv_one_response(c.sock, timeout=8, method="GET")
        assert r1.status == 200, r1
        try:
            c.send(helpers.build_request("GET", "/index.html"))
            r2 = helpers_ext.recv_one_response(c.sock, timeout=5, method="GET")
        except (AssertionError, OSError):
            pytest.skip("servidor nao mantem keep-alive; nada a checar aqui")
        assert r2.status == 200, r2
        assert r2.body == r1.body, "2a resposta divergiu da 1a na mesma conexao"
    finally:
        c.close()
