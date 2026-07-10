"""Categoria H - Correcao semantica das respostas estaticas.

A suite existente cobre bem "nao crashar" e codigos de status. Falta verificar
que o CONTEUDO das respostas normais esta correto: body identico ao arquivo em
disco, Content-Length coerente, Content-Type plausivel e integridade de download
grande. Usa `server.www` (atributo da fixture) como fonte da verdade.
"""
import hashlib
import os

import helpers
import helpers_ext
import pytest


def _sha(b):
    return hashlib.sha256(b).hexdigest()


def test_h1_static_get_body_matches_disk(server):
    """GET /index.html deve devolver exatamente o arquivo em disco."""
    res = helpers_ext.get_full(server, "/index.html")
    assert res.status == 200, res
    with open(os.path.join(server.www, "index.html"), "rb") as f:
        original = f.read()
    assert res.body == original, \
        "body servido difere do arquivo (%d vs %d bytes)" % (len(res.body), len(original))


def test_h2_content_length_matches_body(server):
    """Se ha Content-Length, ele tem que bater com o tamanho real do corpo."""
    res = helpers_ext.get_full(server, "/index.html")
    assert res.status == 200, res
    cl = res.headers.get("content-length")
    te = res.headers.get("transfer-encoding", "").lower()
    if cl is None:
        if "chunked" in te:
            pytest.skip("resposta chunked, sem Content-Length (valido)")
        pytest.fail("resposta sem Content-Length nem chunked: %r" % res.headers)
    assert int(cl) == len(res.body), \
        "Content-Length=%s difere do body real=%d" % (cl, len(res.body))


def test_h3_content_type_html(server):
    """.html deve sair como text/html (deteccao de MIME por extensao)."""
    res = helpers_ext.get_full(server, "/index.html")
    assert res.status == 200, res
    ct = res.headers.get("content-type", "").lower()
    assert "text/html" in ct, "Content-Type inesperado para .html: %r" % ct


def test_h4_status_line_wellformed(server):
    """A status line deve ser 'HTTP/1.x <codigo> <razao>'."""
    with server.client() as c:
        c.send(helpers.build_request("GET", "/index.html"))
        raw = c.recv_until_close(timeout=5)
    assert raw, "servidor nao respondeu nada"
    first = raw.split(b"\r\n", 1)[0]
    assert first.startswith(b"HTTP/1."), "status line nao e HTTP/1.x: %r" % first
    parts = first.split(b" ", 2)
    assert len(parts) >= 2 and parts[1].isdigit(), "status line malformada: %r" % first


def test_h5_large_download_integrity(server):
    """Download de arquivo grande (8MB aleatorio) deve chegar byte a byte igual.

    Honra Content-Length/chunked via helpers_ext, entao pega corrupcao real de
    streaming, nao artefato de enquadramento.
    """
    res = helpers_ext.get_full(server, "/big.bin", timeout=30)
    assert res.status == 200, res
    with open(os.path.join(server.www, "big.bin"), "rb") as f:
        original = f.read()
    assert len(res.body) == len(original), \
        "tamanho baixado %d != original %d" % (len(res.body), len(original))
    assert _sha(res.body) == _sha(original), "download corrompido: sha256 difere"


def test_h6_not_found_is_404(server):
    res = server.get("/nao-existe-abc-123.html")
    assert res.status == 404, res


def test_h7_index_served_for_directory(server):
    """location / tem 'index index.html': GET / deve entregar o index (nao listagem)."""
    root = helpers_ext.get_full(server, "/")
    idx = helpers_ext.get_full(server, "/index.html")
    assert root.status == 200, root
    assert idx.status == 200, idx
    assert root.body == idx.body, "GET / nao devolveu o index configurado"
