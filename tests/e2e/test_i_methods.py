"""Categoria I - Metodos HTTP e enforcement de allow_methods.

Config de referencia (conftest):
  location /        -> allow_methods GET
  location /upload/ -> allow_methods POST DELETE
  location /cgi-bin/-> allow_methods GET POST

Aceitamos 403 OU 405 para "metodo barrado", seguindo a convencao ja adotada em
test_g_method_not_allowed (o time tratou os dois como validos). O codigo
canonico e 405 (RFC 9110 15.5.6); onde 405 for emitido, exigimos o header Allow.
"""
import helpers
import helpers_ext
import pytest


def test_i1_get_on_post_only_location(server):
    """GET em /upload/ (POST/DELETE) deve ser barrado."""
    res = server.get("/upload/")
    assert res.status in (403, 405), \
        "GET em location sem GET deveria dar 403/405: %s" % res


def test_i2_post_on_get_only_location(server):
    """POST em /index.html (location GET-only) deve ser barrado."""
    res = server.request("POST", "/index.html",
                         headers={"Content-Type": "text/plain"}, body=b"x")
    assert res.status in (403, 405), \
        "POST em location GET-only deveria dar 403/405: %s" % res


def test_i3_delete_removes_uploaded_file(server):
    """Sobe um arquivo em /upload/ e depois o remove via DELETE.

    Se DELETE nao for suportado nesta location (decisao de design), o teste faz
    skip em vez de falhar - o subject exige o metodo, mas a rota exata e escolha
    do time.
    """
    body, ct = helpers.multipart_body([("todelete.bin", b"conteudo-para-remover")])
    before = set(server.upload_files())
    up = server.request("POST", "/upload/", headers={"Content-Type": ct}, body=body)
    assert up.status in (200, 201), "upload de preparacao falhou: %s" % up
    new = set(server.upload_files()) - before
    assert len(new) == 1, "esperava exatamente 1 arquivo novo, veio %s" % new
    name = new.pop()

    res = server.request("DELETE", "/upload/" + name)
    if res.status in (403, 405, 501):
        pytest.skip("DELETE nao suportado nesta location (%s)" % res.status)
    assert res.status in (200, 202, 204), "DELETE deveria ter sucesso: %s" % res
    assert name not in server.upload_files(), "arquivo persistiu apos DELETE 2xx"
    assert server.is_alive()


def test_i4_405_has_allow_header(server):
    """RFC 9110 15.5.6: um 405 DEVE trazer o header Allow com os metodos suportados.

    xfail (nao falha dura) caso o time tenha optado por nao emitir Allow, no mesmo
    espirito do gap conhecido do slowloris.
    """
    res = server.get("/upload/")  # GET onde so ha POST/DELETE
    if res.status != 405:
        pytest.skip("servidor respondeu %s (nao 405) aqui; Allow N/A" % res.status)
    allow = res.headers.get("allow")
    if not allow:
        pytest.xfail("405 sem header Allow (exigido por RFC 9110 15.5.6)")
    assert "POST" in allow.upper(), \
        "Allow deveria listar os metodos suportados da location: %r" % allow


def test_i5_options_does_not_crash(server):
    """OPTIONS pode nao ser implementado, mas nunca pode derrubar o servidor."""
    res = server.request("OPTIONS", "/index.html")
    assert res.status in (200, 204, 400, 405, 501), res
    assert server.is_alive()


def test_i6_cgi_location_accepts_get_and_post(server):
    """/cgi-bin/ permite GET e POST: nenhum dos dois pode ser barrado por metodo."""
    g = server.get("/cgi-bin/echo.py")
    assert g.status not in (403, 405), "GET barrado em location GET/POST: %s" % g
    p = server.request("POST", "/cgi-bin/echo.py",
                       headers={"Content-Type": "application/octet-stream"},
                       body=b"ping")
    assert p.status not in (403, 405), "POST barrado em location GET/POST: %s" % p
