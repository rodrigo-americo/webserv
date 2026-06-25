# Checklist Webserv 42

## Requisitos Obrigatórios

- [x] Executável `./webserv [config]`
- [x] Sem `execve` de outro servidor
- [x] Servidor não-bloqueante (poll/epoll/select)
- [x] Apenas 1 poll() para todo I/O
- [x] poll() monitora leitura E escrita simultaneamente — `POLLIN | POLLOUT` em `MultiplexerPoll.hpp:45`
- [x] Nunca read/write sem passar pelo poll() — todo I/O gateado em `ConnectionPool::waitConnections()`
- [ ] Sem checar `errno` após read/write ⚠️ verificar manualmente
- [ ] Request nunca trava indefinidamente ❌ `poll(..., -1)` sem timeout em `MultiplexerPoll.hpp:70`
- [x] Compatível com navegador web
- [ ] Códigos de status HTTP precisos ⚠️ parcial
- [ ] Páginas de erro padrão (fallback quando não configurado) ⚠️ `error_page` configurado mas `Server::_sendError()` não usa o mapa — retorna texto plano
- [x] `fork` só para CGI

---

## Métodos HTTP

- [x] GET
- [x] POST (upload multipart + octet-stream)
- [x] DELETE — implementado em `ServerDelete.cpp` com validação de path, stat e `std::remove()`

---

## Arquivo de Configuração

- [x] `listen` (porta/interface)
- [x] `server_name`
- [x] `error_page`
- [x] `client_max_body_size`
- [x] `location` com `root`
- [ ] `location` com métodos aceitos (`allow GET POST;`) ❌ `PT_LIMIT_EXCEPT` reconhecido mas `addMethod()` nunca chamado em `ConfigBuilderVisitor.cpp`
- [x] `location` com redirect HTTP (`return`)
- [x] `location` com `autoindex`
- [x] `location` com `index` (arquivo padrão de diretório)
- [x] `location` com `upload_dir` — parseado em `ConfigBuilderVisitor.cpp:284`
- [x] Múltiplas portas / múltiplos servers

---

## Funcionalidades

- [x] Servir site estático completo
- [x] Upload de arquivos
- [ ] CGI (php, python, etc) ❌ `ServerCgi.cpp` retorna 501 Not Implemented imediatamente
- [ ] Chunked transfer encoding ❌ header `transfer_encoding` existe mas sem parsing/geração de chunks
- [ ] Query string separada do path ❌ path armazenado como `/path?query=value` sem separação em `HttpRequestBuilder.hpp:51`

---

## Prioridade de Implementação

1. **CGI** — exigido pela norma, bloqueia nota ❌
2. **Timeout de request** — obrigatório, `poll(..., -1)` trava indefinidamente ❌
3. **Métodos por location no config** — `allow GET POST DELETE;` não processado ❌
4. **Páginas de erro padrão** — `_sendError()` precisa consultar mapa `_error_page` e servir HTML ⚠️
5. **Query string** — separar `path` de `query_string` no `HttpRequestBuilder` ❌
6. **Chunked transfer encoding** — implementar parsing de body chunked ❌
