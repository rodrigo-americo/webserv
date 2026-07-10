"""Categoria G - Robustez de protocolo. Nada aqui pode derrubar o servidor."""
import helpers
import pytest


def test_g1_malformed_request_line(server):
    for raw in (b"GET\r\n\r\n",
                b"GET /index.html\r\n\r\n",
                b"GET /index.html BANANA/9.9\r\nHost: x\r\n\r\n",
                b"\r\n\r\n"):
        with server.client() as c:
            c.send(raw)
            data = c.recv_until_close(timeout=3)
        # aceitamos qualquer 4xx/5xx OU fechamento; o proibido e crashar
        assert server.is_alive(), "servidor caiu com request line malformada: %r" % raw


def test_g1b_unknown_method(server):
    res = server.request("BREW", "/index.html")
    assert res.status in (400, 405, 501), res


def test_g2_bogus_content_length(server):
    """Content-Length maior que o body real: servidor nao pode travar eternamente."""
    with server.client(timeout=5) as c:
        c.send("POST /upload/x.bin HTTP/1.1\r\nHost: x\r\n"
               "Content-Type: application/octet-stream\r\n"
               "Content-Length: 1000000\r\n\r\nshort")
        # nao vamos completar o body. Servidor deve seguir vivo para outros.
    assert server.get("/index.html").status == 200


def test_g2b_double_content_length(server):
    with server.client() as c:
        c.send("POST /upload/x.bin HTTP/1.1\r\nHost: x\r\n"
               "Content-Length: 5\r\nContent-Length: 10\r\n\r\nhello")
        c.recv_until_close(timeout=3)
    assert server.is_alive()


def test_g2c_malformed_chunked(server):
    with server.client() as c:
        c.send("POST /upload/x.bin HTTP/1.1\r\nHost: x\r\n"
               "Content-Type: application/octet-stream\r\n"
               "Transfer-Encoding: chunked\r\n\r\n"
               "ZZZZ\r\ngarbage\r\n")
        c.recv_until_close(timeout=3)
    assert server.is_alive()


def test_g3_huge_headers(server):
    big = "X-Big: " + ("a" * 16384)
    with server.client(timeout=5) as c:
        c.send("GET /index.html HTTP/1.1\r\nHost: x\r\n" + big + "\r\n\r\n")
        c.recv_until_close(timeout=3)
    assert server.is_alive()


def test_g3b_huge_uri(server):
    uri = "/" + ("a" * 20000)
    with server.client(timeout=5) as c:
        c.send("GET %s HTTP/1.1\r\nHost: x\r\n\r\n" % uri)
        c.recv_until_close(timeout=3)
    assert server.is_alive()


def test_g_method_not_allowed(server):
    """DELETE numa location so-GET -> 405."""
    res = server.request("DELETE", "/index.html")
    assert res.status in (403, 405), res


def test_g_not_found(server):
    res = server.get("/nao-existe-xyz.html")
    assert res.status == 404, res


def test_g_redirect(server):
    res = server.get("/redirect/")
    assert res.status in (301, 302), res
    assert res.headers.get("location")
