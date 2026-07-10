# Testes E2E do webserv

Suite de testes de sistema que sobe o servidor de verdade e o ataca via sockets crus.
Baseada em `docs/test_plan.md`. Complementa os unit tests (`make tests`), que cobrem
parser/config.

## Rodar

    make e2e                 # suite rapida (exclui slow e stress)
    make e2e-full            # tudo, inclusive timeouts de 30s
    make leaks               # suite sob valgrind (--leak-check --track-fds)
    make stress              # so os testes de carga (siege)

Ou direto:

    cd tests/e2e
    WEBSERV_MUX=poll python3 -m pytest -m "not slow and not stress"
    WEBSERV_MUX=epoll python3 -m pytest        # troca o multiplexer

## Variaveis de ambiente

- `WEBSERV_MUX` — `select` | `poll` | `epoll` (default `poll`). Cada categoria
  deveria passar nos tres.
- `WEBSERV_VALGRIND=1` — roda o servidor sob valgrind (usado por `make leaks`).
- `WEBSERV_BIN` — caminho do binario (default `../../webserver`, builda se faltar).

## Categorias (ver docs/test_plan.md)

| Arquivo | Categoria |
|---------|-----------|
| `test_a_nonblocking.py` | A — nao-bloqueante |
| `test_b_leaks.py`       | B — fds / zumbis / RSS via /proc |
| `test_c_upload.py`      | C — integridade de upload |
| `test_d_load.py`        | D — alta carga (marcado `stress`) |
| `test_e_interrupted.py` | E — requisicoes interrompidas |
| `test_f_timeouts.py`    | F — timeouts (marcado `slow`) |
| `test_g_protocol.py`    | G — robustez de protocolo |

## Gaps conhecidos (decisao do time 09/07)

- `test_f3_slowloris_client` fica **xfail**: nao ha timeout de cliente (R1). O
  subject exige que nenhuma requisicao trave indefinidamente — risco de defesa.
