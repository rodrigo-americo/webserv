# 🧪 Plano de Testes — webserv

> Autor: QA · Baseado em investigação do código em `try01/` (commit atual de main).
> Regra: **nada de implementação antes deste plano ser aprovado.**

## 1. O que a investigação encontrou

Arquitetura relevante para teste:

- Loop de eventos single-thread em `ConnectionPool::waitConnections()`; multiplexer configurável (`select` / `poll` / `epoll`).
- Sockets de cliente non-blocking + `FD_CLOEXEC`; resposta é **enfileirada** (`queueWrite`/`queueFile`) e drenada só quando o multiplexer sinaliza writable. Arquivos estáticos são servidos em streaming (`FileChunkSource`).
- Leitura de request em chunks de **1024 bytes** (`_handleRequest`), com suporte a `Transfer-Encoding: chunked` (limite de 100MB por chunk).
- CGI: `fork` + 2 pipes registrados no multiplexer; timeout fixo de **30s** verificado a cada iteração do loop; cleanup com `SIGKILL` + `waitpid`.
- Upload: handlers `multipart/form-data` e `octet-stream`; **body inteiro fica em RAM** (`req.body` é `std::string`) e é **copiado de novo** para o CGI (`_body_to_write`).
- Sessões com timeout default de 15s (`SessionManager`).
- Testes existentes: só unit tests (parser/config/lexer/schema) via framework próprio (`make tests`), coverage com gcovr, CI roda build + unit tests em PR. **Não existe nenhum teste E2E contra o servidor vivo.**

## 2. Riscos identificados (guiam a prioridade dos testes)

| ID | Risco | Evidência no código |
|----|-------|---------------------|
| R1 | **Sem timeout de cliente** (slowloris): só CGI tem timeout; `_pending_request` nunca expira | `waitConnections()` só varre `_running_cgis` |
| R2 | `client_max_body_size` é checado **depois** do body completo estar em RAM → DoS de memória | `handleRequest()` checa `req.body.size()` após request completo |
| R3 | Body duplicado em memória no CGI | `CgiProcess::_body_to_write(req.body)` |
| R4 | `flushWrite()` ignora erro de `write()` nos headers (`n<=0` só retorna) → possível stall/loop no fd | `SocketConnection::flushWrite()` |
| R5 | Conexão é removida do multiplexer após cada resposta → keep-alive inexistente? Verificar se `remove` fecha/deleta (leak de fd vs close prematuro) | `waitConnections()` pós-resposta |
| R6 | Cleanup `CGI_CLIENT_GONE` retorna antes de `_removePending(conn)` → pending órfão? | `_cleanupCgi()` |
| R7 | Singletons sem teardown + `WebServer::clean_up()` vazio → "still reachable" no valgrind | `ConnectionPool`, `SessionManager` |
| R8 | **Risco de nota 0**: subject proíbe read/write sem passar pelo poll, mas `_handleRequest` faz múltiplos `read()` em loop após **um único** evento do multiplexer | `while (bytes_read > 0)` em `_handleRequest()` |

**Decisões do time (09/07):**

- **R1** aceito como gap conhecido. Atenção: o subject exige "uma solicitação ao seu servidor nunca deve travar indefinidamente" (IV.1) — F3 fica vermelho documentado e é risco de defesa.
- **R5** resolvido: keep-alive **não é requisito** (subject sugere HTTP 1.0 como referência). Teste passa a verificar fechamento limpo pós-resposta + `Connection: close` para compatibilidade com navegador.
- **C6/C7**: `client_max_body_size` de referência para testes de limite = **1024 bytes**.

## 3. Escopo

**Dentro**: servidor vivo (E2E), comportamento não-bloqueante, leaks de memória/fd/zumbis, integridade de upload, carga, interrupção de requisição (normal e CGI), timeouts, robustez de protocolo.
**Fora**: unit tests de parser/config (já cobertos), SSL, HTTP/2.

## 4. Estratégia e ferramentas

- **Harness**: Python + pytest em `tests/e2e/` (novo). Sockets crus para simular clientes lentos, parciais e abortados. Fixture sobe o binário com config própria em porta aleatória e mata no teardown.
- **Leaks**: `valgrind --leak-check=full --track-fds=yes` + contagem de `/proc/<pid>/fd` antes/depois de cada cenário + `ps` para zumbis.
- **Carga**: `siege` (disponível na máquina de avaliação) + rajadas geradas pelo próprio harness.
- **Targets novos no Makefile**: `make e2e`, `make stress`, `make leaks`.
- Cada categoria roda **3x**: uma por multiplexer (`select`, `poll`, `epoll`) — o comportamento sob carga difere (limite de 1024 fds do select).

## 5. Casos de teste

### A — Não-bloqueante (nada pode travar o loop)

| ID | Cenário | Resultado esperado |
|----|---------|--------------------|
| A1 | Cliente lento baixando arquivo grande (`curl --limit-rate 10k`) enquanto outro faz GETs rápidos | GETs rápidos respondem < 200ms durante todo o download |
| A2 | Cliente conecta e não envia nada; outros clientes seguem | Servidor continua atendendo normalmente |
| A3 | CGI com `sleep 20` em uma conexão; GETs paralelos | GETs não esperam o CGI |
| A4 | Upload gotejando (1 byte/s) em paralelo a tráfego normal | Tráfego normal não degrada |
| A5 | Auditoria com `strace -f`: nenhuma syscall de I/O bloqueante fora do `wait()` do multiplexer | Sem `read`/`write` bloqueante em socket |
| A6 | Auditoria R8: correlacionar no strace cada `read`/`write` em socket com um retorno do poll imediatamente anterior | **1 read/write por evento do poll** (critério de nota 0 do subject); errno nunca consultado após read/write |

### B — Leaks (memória, fd, zumbis)

| ID | Cenário | Resultado esperado |
|----|---------|--------------------|
| B1 | Suíte funcional inteira sob valgrind | 0 definitely/indirectly lost; documentar still-reachable (R7) |
| B2 | 500 ciclos de: conexão normal, conexão abortada, CGI ok, CGI timeout, upload | Contagem de fds em `/proc/<pid>/fd` volta ao baseline |
| B3 | CGI normal / timeout / cliente sumiu, 50x cada | Zero processos `<defunct>` |
| B4 | RSS medido antes/depois de onda de 1000 requests | Sem crescimento monotônico |

### C — Integridade de upload

| ID | Cenário | Resultado esperado |
|----|---------|--------------------|
| C1 | Octet-stream binário com bytes nulos e sequências `\r\n\r\n` no meio | sha256 do arquivo salvo == original |
| C2 | Multipart: 1 arquivo, N arquivos, e **boundary caindo exatamente na fronteira do chunk de 1024 bytes** | Todos os arquivos íntegros (ponto crítico: buffer de leitura) |
| C3 | Upload com `Transfer-Encoding: chunked` | Remontagem correta, checksum ok |
| C4 | Mesmo filename 2x | `_uniquePath` gera `nome(1).ext` sem sobrescrever |
| C5 | `filename="../../etc/pwn"` | Não escreve fora do upload_dir |
| C6 | Config com limite **1024**: body de 1024 bytes vs 1025 bytes | 1024 → 201/200; 1025 → 413, **nenhum arquivo parcial no disco** |
| C7 | Config com limite 1024: cliente envia `Content-Length: 10MB` (e variante chunked) | 413; idealmente rejeitado **pelo header, sem bufferizar o body** (expõe R2 — subject: servidor não pode morrer nem sem memória) |

> Nota: os testes de integridade (C1–C5) usam uma fixture de config **sem limite apertado**; os de limite (C6/C7) usam a fixture com `client_max_body_size 1024`.

### D — Alta carga

| ID | Cenário | Resultado esperado |
|----|---------|--------------------|
| D1 | `siege -c 50 -t 30s` GET estático | Availability 100%, sem crash |
| D2 | Siege misto: estático + CGI + upload simultâneos | Sem crash, sem leak pós-teste |
| D3 | > 1024 conexões simultâneas | epoll/poll sobrevivem; select degrada **graciosamente** (sem crash) |
| D4 | Esgotamento de fds (abrir conexões até `EMFILE`) | `accept` falha limpo, servidor se recupera quando conexões fecham |

### E — Requisições interrompidas

| ID | Cenário | Resultado esperado |
|----|---------|--------------------|
| E1 | Cliente fecha no meio dos **headers** | Pending removido, fd fechado, sem leak |
| E2 | Cliente fecha no meio do **body** de upload | Sem arquivo corrompido/parcial, fd liberado |
| E3 | Cliente fecha enquanto resposta grande está sendo drenada | Erro/EOF detectado, `FileChunkSource` fechado (expõe R4) |
| E4 | Cliente fecha durante CGI rodando | `CGI_CLIENT_GONE`: processo morto, pipes removidos do multiplexer, sem zumbi (verificar R6) |
| E5 | RST abrupto (`SO_LINGER 0`) vs FIN normal, em cada fase acima | Ambos tratados sem crash |

### F — Timeouts

| ID | Cenário | Resultado esperado |
|----|---------|--------------------|
| F1 | CGI em loop infinito | 504 em ~30s, processo morto e colhido |
| F2 | CGI que nunca lê stdin (upload grande via CGI) | Sem deadlock; timeout aplica |
| F3 | **Slowloris**: headers parciais + silêncio | Gap conhecido (decisão do time). Teste fica marcado `xfail` documentando que o subject exige "nunca travar indefinidamente" |
| F4 | Cookie de sessão após 15s | 403 em rota com `require_auth` |

### G — Robustez de protocolo

| ID | Cenário | Resultado esperado |
|----|---------|--------------------|
| G1 | Request line malformada, método desconhecido, versão inválida | 400/405/505, nunca crash |
| G2 | `Content-Length` mentiroso (maior e menor que o body real), dois `Content-Length`, chunked malformado | 400 ou comportamento igual ao NGINX |
| G3 | Headers gigantes (> 8KB), URI gigante | Rejeição limpa |
| G4 | Comparação lado a lado com NGINX nos casos acima (subject exige) | Comportamento compatível |

## 6. Fases de execução

1. **Infra** — fixture pytest (build + sobe servidor + teardown), helpers de socket cru, `make e2e`.
2. **C + G** — integridade e protocolo (rápidos, alto valor).
3. **E + F** — interrupções e timeouts (onde moram R1, R4, R6).
4. **A** — não-bloqueante.
5. **B** — suíte sob valgrind/track-fds (usa tudo acima).
6. **D** — carga com siege (por último: só faz sentido com o resto verde).

## 7. Critérios de entrada/saída

- **Entrada**: `make` compila sem warning; unit tests atuais verdes.
- **Saída**: A–G verdes nos 3 multiplexers; valgrind sem definitely lost; zero zumbis; zero fds vazados; F3 (slowloris) resolvido ou formalmente aceito como gap conhecido.

## 8. Questões em aberto para o time

1. ~~R1 timeout de cliente~~ → aceito como gap (F3 = `xfail`), mas é risco de defesa.
2. ~~Keep-alive~~ → não é requisito; testes verificam fechamento limpo + navegador funciona.
3. ~~Limite para C6/C7~~ → 1024 bytes na fixture de limite.
4. **NOVO — R8**: o loop de `read()` em `_handleRequest` pode ser interpretado como "read sem passar pelo poll" (**nota 0** segundo o subject IV.1). Precisa de decisão: refatorar para 1 read por evento antes da defesa? O teste A6 vai medir isso.
