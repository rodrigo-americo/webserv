"""Categoria C - Integridade de upload."""
import hashlib
import os

import helpers
import pytest


def _sha(data):
    return hashlib.sha256(data).hexdigest()


def test_c1_octet_stream_binary_integrity(server):
    """C1: octet-stream com bytes nulos e CRLF no meio -> checksum identico."""
    payload = b"\x00\x01\x02ABC\r\n\r\nmiddle\x00\xff\xfe" + os.urandom(4096)
    res = server.request("POST", "/upload/blob.bin",
                         headers={"Content-Type": "application/octet-stream"},
                         body=payload)
    assert res.status in (200, 201), res
    saved = server.upload_files()
    assert saved, "nenhum arquivo salvo"
    with open(server.upload_path(saved[0]), "rb") as f:
        assert _sha(f.read()) == _sha(payload)


def test_c2_multipart_single_file(server):
    payload = os.urandom(2048)
    body, ct = helpers.multipart_body([("a.bin", payload)])
    res = server.request("POST", "/upload/", headers={"Content-Type": ct}, body=body)
    assert res.status in (200, 201), res
    files = server.upload_files()
    assert len(files) == 1
    with open(server.upload_path(files[0]), "rb") as f:
        assert _sha(f.read()) == _sha(payload)


def test_c2b_multipart_boundary_on_chunk_edge(server):
    """C2 critico: boundary caindo perto da fronteira do buffer de 1024 bytes.

    O servidor le em blocos de 1024; se a remontagem do multipart tiver bug
    de fronteira, o arquivo corrompe exatamente aqui.
    """
    for size in (1000, 1020, 1024, 1030, 2048, 2100):
        srv_files_before = set(server.upload_files())
        payload = bytes((i % 251) for i in range(size))
        body, ct = helpers.multipart_body([("edge_%d.bin" % size, payload)])
        res = server.request("POST", "/upload/",
                            headers={"Content-Type": ct}, body=body)
        assert res.status in (200, 201), "size=%d -> %s" % (size, res)
        new = set(server.upload_files()) - srv_files_before
        assert len(new) == 1, "size=%d nao salvou exatamente 1 arquivo" % size
        with open(server.upload_path(list(new)[0]), "rb") as f:
            got = f.read()
        assert _sha(got) == _sha(payload), \
            "CORRUPCAO em size=%d: %d bytes salvos vs %d" % (size, len(got), size)


def test_c2c_multipart_multiple_files(server):
    files = [("f%d.bin" % i, os.urandom(500 + i * 100)) for i in range(3)]
    body, ct = helpers.multipart_body(files)
    res = server.request("POST", "/upload/", headers={"Content-Type": ct}, body=body)
    assert res.status in (200, 201), res
    assert len(server.upload_files()) == 3


def test_c3_chunked_upload_integrity(server):
    payload = os.urandom(5000)
    body = helpers.chunked_body(payload, chunk_size=777)
    res = server.request(
        "POST", "/upload/blob.bin",
        headers={"Content-Type": "application/octet-stream",
                 "Transfer-Encoding": "chunked", "Content-Length": None},
        body=body)
    assert res.status in (200, 201), res
    files = server.upload_files()
    assert files
    with open(server.upload_path(files[0]), "rb") as f:
        assert _sha(f.read()) == _sha(payload)


def test_c4_same_filename_no_overwrite(server):
    for _ in range(2):
        body, ct = helpers.multipart_body([("dup.txt", b"conteudo")])
        res = server.request("POST", "/upload/",
                            headers={"Content-Type": ct}, body=body)
        assert res.status in (200, 201)
    assert len(server.upload_files()) == 2, "sobrescreveu arquivo de mesmo nome"


def test_c5_path_traversal_blocked(server):
    payload = b"pwned"
    body, ct = helpers.multipart_body([("../../escape.txt", payload)])
    server.request("POST", "/upload/", headers={"Content-Type": ct}, body=body)
    # nada pode ter sido escrito fora do upload_dir
    escaped = os.path.join(os.path.dirname(server.uploads), "escape.txt")
    assert not os.path.exists(escaped), "path traversal escreveu fora do upload_dir!"


def test_c6_body_over_limit_rejected(server_limited):
    """C6: limite 1024. 1024 ok, 1025 -> 413 e sem arquivo parcial."""
    ok = b"x" * 1024
    res = server_limited.request("POST", "/upload/ok.bin",
                                headers={"Content-Type": "application/octet-stream"},
                                body=ok)
    assert res.status in (200, 201), "1024 bytes deveria passar: %s" % res

    before = set(server_limited.upload_files())
    over = b"x" * 1025
    res = server_limited.request("POST", "/upload/over.bin",
                                headers={"Content-Type": "application/octet-stream"},
                                body=over)
    assert res.status == 413, "1025 bytes deveria dar 413: %s" % res
    after = set(server_limited.upload_files())
    assert after == before, "arquivo parcial gravado apesar do 413"


def test_c7_huge_content_length_rejected(server_limited):
    """C7: Content-Length gigante com limite 1024 -> 413, servidor sobrevive."""
    body = b"y" * 4096  # ja passa do limite
    res = server_limited.request(
        "POST", "/upload/huge.bin",
        headers={"Content-Type": "application/octet-stream"}, body=body)
    assert res.status == 413, res
    assert server_limited.is_alive()
