# 🧪 Achados da primeira execução da suíte E2E

> Data: 09/07/2026 · Ambiente: Linux (multiplexer `poll`), binário buildado do `main` atual.
> A suíte foi validada rodando contra o servidor de verdade. Dos 14 smokes de
> validação, **10 passaram e 4 falharam por bugs reais do servidor** (não dos testes).
> Infra confirmada: boot com config gerada, GET estático, 404, redirect, CGI,
> upload octet-stream, no-overwrite e path-traversal já passam.

## 🔴 CRÍTICO — vazamento de file descriptors (pipes)

**Sintoma medido:** cada requisição — inclusive **GET estático, sem CGII** — vaza
exatamente **4 fds de pipe**, de forma linear e ilimitada:

```
após 10 GET -> pipes=45
após 20 GET -> pipes=85
após 30 GET -> pipes=125   (+4 por request)
```

**Impacto:** com o limite default de 1024 fds, o servidor bate em `EMFILE` por
volta de ~250 requisições e para de aceitar conexões. O subject exige que o
servidor "permaneça disponível em todos os momentos" (IV.1) — isto reprova.

**Onde investigar:** 4 fds = 2 chamadas `pipe()` por request nunca fechadas.
Como acontece até em rota estática, suspeita recai sobre algo construído no
caminho comum (Router / setup de CGI feito antes de decidir se é CGI, ou
`PipeChannel` criado e não destruído). Testes que pegam isso: `test_b_leaks.py`
(B2), `test_e_interrupted.py` (E1/E4). **Prioridade máxima.**

## 🟠 MÉDIO — upload multipart anexa `\r\n` ao final do arquivo

**Sintoma:** upload multipart de um arquivo de 1000 bytes grava **1002 bytes** no
disco; o prefixo bate exatamente e os 2 bytes extras são `\r\n`.

```
payload 1000 -> disk 1002 ; extra tail = b'\r\n'
```

**Causa provável:** na remontagem do corpo, o `\r\n` que precede o boundary de
fechamento está sendo incluído no conteúdo da parte. Pelo padrão multipart esse
CRLF **não** faz parte do arquivo. Corrompe qualquer upload binário (imagem, zip).
Teste que pega: `test_c_upload.py::test_c2b_multipart_boundary_on_chunk_edge`.

> Nota: o handler antigo em `ServerUpload.cpp::_handleMultipart` recorta com
> `body.find("\r\n" + boundary)` (correto). O dispatch usa `_serveUpload2` /
> `UploadMultipartFormDataHandler` — é aí que o CRLF está vazando. Vale alinhar
> os dois caminhos.

## ⚪ GAP CONHECIDO — slowloris (R1)

Sem timeout de cliente: conexão que gotejar headers sem terminar fica presa para
sempre. Decisão do time: aceito como gap, teste `test_f3` fica `xfail`. Risco de
defesa porque o subject diz que "uma solicitação nunca deve travar
indefinidamente".

## Como reproduzir

```
make e2e                 # roda a suíte (as 3 falhas acima aparecem)
make e2e E2E_MUX=epoll   # idem no epoll
make leaks               # servidor sob valgrind
```

## Recomendação de ordem de correção

1. Pipe fd leak (crítico, bloqueia tudo — inclusive os números de carga).
2. CRLF do multipart (corrompe uploads binários).
3. Decidir sobre timeout de cliente (R1) antes da defesa.
