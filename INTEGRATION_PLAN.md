# Plano de Integração

## Estado atual (o que existe e onde)

### Infraestrutura pronta
| Componente | Localização | Estado |
|---|---|---|
| Parser de config (lexer + AST) | `parser/` | ✓ pronto |
| Classes de config + visitor | `config/` | ✓ pronto |
| Socket / SocketListenner | `try01/Socket/` | ✓ pronto |
| Multiplexer (epoll/poll/select/kqueue) | `try01/Multiplexer/` | ✓ pronto |
| ConnectionPool (event loop) | `try01/ConnectionPool/` | ✓ pronto |
| HttpRequest + RequestBuilder | `try01/Http/` | ✓ pronto (parcial) |
| HttpResponse + conversor HTTP/1.1 | `try01/Http/` | ✓ pronto |
| Logger | `tools/Logger/` | ✓ pronto |

### O que o main.cpp faz hoje
Lê o config, valida, imprime "Configuração carregada com sucesso" e sai.
O `try01/main.cpp` sobe o servidor na porta 8080 hardcoded, sem ler config.

Os dois ainda não se falam.

---

## O que falta — em ordem de dependência

### ETAPA 1 — Conectar config no servidor (desbloqueador de tudo)
O `main.cpp` precisa:
1. Parsear o config → `WebServerConfig*`
2. Para cada `listen` em cada `ServerConfig`: criar um `SocketListenner` naquela porta
3. Registrar cada listenner no `ConnectionPool` com o `Server` correspondente
4. Chamar `pool.waitConnections()`

**Interface que falta:** `WebServerConfig::match_server(port, host_header)`
- Filtra servers por porta → busca `server_name` no `Host` header → fallback pro primeiro

**Impacto:** sem isso nada funciona de ponta a ponta.

---

### ETAPA 2 — Request Handler real (substitui o "Hello World")
`Server::handleRequest(req, res)` hoje retorna Hello World hardcoded.

Precisa virar:
```
1. match_server(porta, req.headers["Host"]) → ServerConfig*
2. match_location(req.path)                 → LocationConfig*
3. checar req.method contra location->getMethods()  → 405 se não permitido
4. checar body size contra client_max_body_size      → 413 se exceder
5. se location->getRedirect() não vazio              → 301/302
6. senão dispatch para: arquivo estático / CGI / upload
```

---

### ETAPA 3 — Servir arquivo estático
```
path_real = location->getRoot() + req.path (relativo ao prefix da location)

se path_real é diretório:
    se tem index file → servir esse arquivo
    se autoindex on   → gerar HTML de listagem (opendir/readdir)
    senão             → 403

se arquivo existe     → ler e enviar com Content-Type correto
senão                 → 404
se sem permissão      → 403
```
Detectar Content-Type pela extensão: `.html`, `.css`, `.js`, `.png`, `.jpg`, etc.

---

### ETAPA 4 — Páginas de erro padrão
Se `error_page[código]` configurado → servir esse arquivo.
Senão → gerar HTML mínimo inline para: 400, 403, 404, 405, 413, 500, 501.

---

### ETAPA 5 — Upload (POST com multipart)
```
Content-Type: multipart/form-data; boundary=...
→ parsear boundary
→ extrair arquivo do body
→ salvar em location->getUploadDir()
→ responder 201 Created
```

---

### ETAPA 6 — CGI (mais complexo, obrigatório)
Acionado quando extensão do arquivo bate com config de CGI da location.

```
fork()
├── filho:
│   chdir(diretório do script)
│   montar env: REQUEST_METHOD, QUERY_STRING, CONTENT_LENGTH,
│               CONTENT_TYPE, PATH_INFO, SCRIPT_FILENAME, ...
│   dup2(pipe_in[0],  STDIN)   ← body do request
│   dup2(pipe_out[1], STDOUT)  ← output vira body da response
│   execve(interpretador, [script], env)
└── pai:
    escrever body no pipe_in
    ler stdout do CGI via pipe_out (até EOF se sem Content-Length)
    waitpid com timeout → kill se demorar demais
    parsear headers CGI (antes do \r\n\r\n)
    montar HttpResponse com o resultado
```

Suportar pelo menos Python ou PHP-CGI.

---

### ETAPA 7 — Enforçar non-blocking no CGI
Os pipes do CGI precisam estar em modo não bloqueante e monitorados pelo multiplexer.
Atualmente o multiplexer só monitora sockets — vai precisar suportar fds genéricos ou
usar uma fila separada para operações CGI em andamento.

---

## Problemas de integração identificados

### 1. Dois enums para método HTTP
- `try01`: `RequestMethod::GET/POST/PUT/PATCH/DELETE`
- `config/`: `HttpMethod::GET/POST/DELETE` (em `lib/http/http.hpp`)

Precisam convergir. Sugestão: usar o do config (que reflete o que o subject pede) e
adaptar o RequestBuilder para usar o mesmo enum.

### 2. `ConfigServerListen` vs `struct Listen`
- `try01/main.cpp` cria `ConfigServerListen { address, port, is_ipv4, backlog }`
- `config/ServerConfig.hpp` tem `struct Listen { host, port }`

Na Etapa 1, ao criar os `SocketListenner` a partir do config, converter `Listen → ConfigServerListen`.

### 3. `try01/` como pasta provisória
Todo o código de socket/http está em `try01/`. Para o projeto final provavelmente
deve ser movido para a raiz ou uma pasta `src/`. Não é bloqueante agora mas
vai criar ruído no Makefile.

### 4. `RequestBuilder` marcado como "CLASSE MERDA"
O próprio autor deixou comentário. Não parseia `cursor` corretamente na linha de request
(há um bug onde `_cursor` é atualizado mas não avançado antes de ler o path).
Precisa revisão antes de depender dele para CGI e upload.

---

## Ordem de execução sugerida

```
[Etapa 1] conectar config → sockets        ← desbloqueia tudo
[Etapa 2] request handler esqueleto         ← dá estrutura para os próximos
[Etapa 3] arquivo estático                  ← demo funcional básica
[Etapa 4] páginas de erro                   ← polimento
[Etapa 5] upload                            ← POST
[Etapa 6] CGI                               ← obrigatório, mais trabalhoso
[Etapa 7] non-blocking CGI no multiplexer   ← exigência do subject
```

---

## O que está além do obrigatório (pode ignorar)
- Regex em location (`~`, `~*`) — subject diz "nenhum regex é necessário"
- Virtual hosts / `server_name` matching — "fora do escopo" segundo o subject
- SSL/TLS — requer OpenSSL, fora do escopo
- Proxy / upstream / load balancer
- `log_format` configurável
