"""Extensoes aos helpers E2E.

O `helpers.py` original assume "uma resposta por conexao, servidor fecha no fim"
(recv_until_close). Isso quebra em dois cenarios que os novos testes exercitam:

  1. keep-alive: precisamos ler UMA resposta e deixar a conexao aberta para
     mandar a proxima na mesma conexao.
  2. integridade: se o servidor responder com Transfer-Encoding: chunked, o body
     cru contem o enquadramento dos chunks; comparar sha direto daria falso
     positivo de "corrupcao".

`recv_one_response` le exatamente uma resposta respeitando o enquadramento
(Content-Length, chunked ou delimitacao-por-fechamento) e deixa o socket no
ponto certo para a proxima leitura. Referencias: RFC 9112 (HTTP/1.1 message
syntax) secoes de enquadramento e conexoes persistentes.
"""
import socket

import helpers


def _try_dechunk(buf):
    """Decodifica um corpo chunked incremental.

    Retorna (decoded_bytes, complete). complete=True quando o chunk terminador
    de tamanho 0 foi visto. Reparseia o buffer inteiro a cada chamada: os corpos
    dos testes sao pequenos, entao o custo O(n^2) e irrelevante e o codigo fica
    simples e obviamente correto.
    """
    out = bytearray()
    i = 0
    n = len(buf)
    while True:
        j = buf.find(b"\r\n", i)
        if j == -1:
            return bytes(out), False  # linha de tamanho ainda incompleta
        size_token = buf[i:j].split(b";")[0].strip()  # ignora chunk-extensions
        try:
            size = int(size_token, 16)
        except ValueError:
            # chunked malformado: nao ha o que decodificar com seguranca
            return None, True
        i = j + 2
        if size == 0:
            return bytes(out), True
        if i + size + 2 > n:
            return bytes(out), False  # dados do chunk ainda nao chegaram todos
        out += buf[i:i + size]
        i += size + 2  # pula dados + CRLF final do chunk


def recv_one_response(sock, timeout=helpers.DEFAULT_TIMEOUT, method=None):
    """Le UMA resposta HTTP do socket, honrando o enquadramento.

    `method` permite tratar corretamente HEAD (sem corpo). 204/304 e 1xx tambem
    nao tem corpo. Nao fecha o socket: deixa-o pronto para a proxima resposta
    (keep-alive) ou para o teste inspecionar EOF.
    """
    sock.settimeout(timeout)
    buf = b""
    while b"\r\n\r\n" not in buf:
        chunk = sock.recv(4096)
        if not chunk:
            break
        buf += chunk
    if b"\r\n\r\n" not in buf:
        raise AssertionError("resposta sem fim de headers: %r" % buf[:200])

    head, _, rest = buf.partition(b"\r\n\r\n")
    parsed = helpers.parse_response(head + b"\r\n\r\n")
    headers = parsed.headers
    status = parsed.status
    raw_prefix = head + b"\r\n\r\n"

    no_body = (
        method == "HEAD"
        or status in (204, 304)
        or (100 <= status < 200)
    )
    if no_body:
        return helpers.HttpResponse(status, parsed.reason, headers, b"", raw_prefix)

    te = headers.get("transfer-encoding", "").lower()
    if "chunked" in te:
        body = rest
        decoded, complete = _try_dechunk(body)
        while not complete:
            chunk = sock.recv(4096)
            if not chunk:
                break
            body += chunk
            decoded, complete = _try_dechunk(body)
        return helpers.HttpResponse(
            status, parsed.reason, headers, decoded or b"", raw_prefix + body)

    if "content-length" in headers:
        clen = int(headers["content-length"])
        body = rest
        while len(body) < clen:
            chunk = sock.recv(4096)
            if not chunk:
                break
            body += chunk
        return helpers.HttpResponse(
            status, parsed.reason, headers, body[:clen], raw_prefix + body)

    # sem Content-Length nem chunked -> delimitado por fechamento
    body = rest
    while True:
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            break
        if not chunk:
            break
        body += chunk
    return helpers.HttpResponse(status, parsed.reason, headers, body, raw_prefix + body)


def request_full(server, method, path, headers=None, body=b"",
                 timeout=helpers.DEFAULT_TIMEOUT):
    """Faz um request numa conexao nova e le a resposta respeitando o framing.

    Use quando a integridade do corpo importa (pode vir chunked) ou quando o
    helpers.request (que le ate fechar) nao serve.
    """
    c = server.client(timeout=timeout)
    try:
        c.send(helpers.build_request(method, path, headers=headers, body=body))
        return recv_one_response(c.sock, timeout=timeout, method=method)
    finally:
        c.close()


def get_full(server, path, headers=None, timeout=helpers.DEFAULT_TIMEOUT):
    return request_full(server, "GET", path, headers=headers, timeout=timeout)
