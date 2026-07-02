# ✅ Checklist — Projeto Webserv (42)

## 📦 Entregáveis / Estrutura

- [x] `Makefile` com regras: `$(NAME)`, `all`, `clean`, `fclean`, `re`
- [x] Makefile não faz relink desnecessário
- [x] Compila com `c++ -Wall -Wextra -Werror`
- [x] Compila com `-std=c++98` sem erros
- [x] Usa `<cstring>` e afins (versões C++) em vez de `<string.h>` quando possível
- [x] Nenhuma biblioteca externa/Boost usada
- [x] Arquivos entregues: `*.h/.hpp`, `*.cpp`, `*.tpp/.ipp`, arquivo(s) de configuração
- [x] Executável roda como `./webserv [arquivo de configuração]`
- [ ] Arquivo de configuração default disponível se nenhum argumento for passado
- [ ] `README.md` na raiz do repositório:
  - [ ] Primeira linha itálica: *Este projeto foi criado como parte do currículo da 42 por \<login1\>[, \<login2\>...]*
  - [ ] Seção "Descrição"
  - [ ] Seção "Instruções" (compilar/instalar/rodar)
  - [ ] Seção "Recursos" (referências + como a IA foi usada, em quais tarefas/partes)

---

## 🌐 Comportamento geral (regras que zeram o projeto se violadas)

- [ ] Programa **nunca** crasha, mesmo sem memória disponível
- [ ] Programa **nunca** termina inesperadamente
- [x] **Apenas 1 poll()** (ou equivalente: select/epoll/kqueue) monitorando **todos** os fds de I/O com clientes, incluindo o(s) listen socket(s)
- [x] poll() (ou equivalente) monitora leitura **e** escrita simultaneamente
- [x] Nenhum read/recv ou write/send é feito **sem passar pelo poll()** antes (exceto leitura do arquivo de config)
- [x] `errno` **nunca** é checado após read/write para decidir comportamento
- [x] Todos os fds usados em I/O de cliente são **não bloqueantes**
- [x] `fork()` usado **apenas** para CGI (nunca para outra coisa)
- [x] Não usa `execve` para rodar outro servidor web
- [ ] Nenhuma requisição trava indefinidamente
- [ ] Servidor permanece disponível/operacional o tempo todo (resiliência sob carga, múltiplos clientes)
- [x] (macOS) `fcntl()` usado só com `F_SETFL`, `O_NONBLOCK`, `FD_CLOEXEC`

---

## 🔌 Sockets e conexões

- [x] Suporta múltiplos servidores (múltiplos pares `interface:porta`)
- [x] Escuta múltiplas portas simultaneamente
- [x] Lida corretamente com múltiplos clientes conectados ao mesmo tempo
- [ ] Lida com desconexão inesperada de clientes sem travar/crashar
- [ ] Compatível com navegador web real (testado com Chrome/Firefox/etc.)
- [ ] Comportamento comparado/validado contra NGINX quando há dúvidas

---

## 📨 Protocolo HTTP

- [x] Suporta método **GET**
- [x] Suporta método **POST**
- [x] Suporta método **DELETE**
- [x] Códigos de status HTTP retornados são precisos/corretos
- [ ] Páginas de erro **padrão** existem quando nenhuma é configurada
- [x] Serve um site **totalmente estático** (HTML, CSS, imagens etc.)
- [x] Suporta **upload de arquivos** vindos do cliente
- [x] Parsing correto de request line, headers e body
- [x] Lida com corpos com `Content-Length`
- [ ] Lida com `chunked transfer encoding` (desagrupar chunks)
- [x] Host virtual **não é obrigatório** (fora do escopo, mas pode implementar)

---

## ⚙️ Arquivo de configuração

- [x] Parser de configuração próprio (inspirado no NGINX, sem regex obrigatório)
- [x] Define pares `interface:porta` para múltiplos servidores/sites
- [x] Configura páginas de erro customizadas por código
- [x] Define tamanho máximo do corpo da requisição (client_max_body_size)
- [x] Por rota/localização, é possível configurar:
  - [x] Métodos HTTP aceitos
  - [x] Redirecionamento HTTP
  - [x] Diretório raiz (root) mapeando URL → filesystem
  - [x] Ativar/desativar listagem de diretórios (autoindex)
  - [x] Arquivo padrão (index) servido quando o recurso é um diretório
  - [x] Local de armazenamento para uploads de clientes
  - [x] Execução de CGI por extensão de arquivo (ex: `.php`, `.py`)
- [x] Suporta configurações extras (ex: nome do servidor, se for implementar virtual host)
- [ ] Arquivos de configuração de teste + arquivos padrão fornecidos no repositório para demonstrar todos os recursos na avaliação

---

## 🧩 CGI

- [x] Detecta e executa CGI com base na extensão do arquivo
- [x] Usa `fork()` + `execve()` para rodar o CGI
- [x] Variáveis de ambiente CGI corretamente montadas (ex: `REQUEST_METHOD`, `QUERY_STRING`, `CONTENT_LENGTH`, `PATH_INFO`, etc.)
- [x] Corpo completo da requisição e argumentos do cliente disponíveis para o CGI
- [ ] Requisições chunked são desagrupadas antes de repassar ao CGI (CGI espera EOF como fim do corpo)
- [x] Saída do CGI: se não houver `Content-Length` retornado, EOF marca o fim dos dados
- [x] CGI é executado no diretório correto (para paths relativos funcionarem)
- [x] Comunicação com CGI via `pipe()` + stdin/stdout redirecionados (`dup2`)
- [x] Suporta pelo menos **um** tipo de CGI (ex: php-cgi ou Python)
- [ ] Timeout/tratamento para CGI que trava (não pode travar o servidor)

---

## 🧪 Testes

- [ ] Testado com `telnet` (requisições manuais)
- [ ] Testado com `curl`
- [ ] Testado com `nc`
- [x] Testes automatizados escritos em linguagem adequada (Python, Golang, C/C++, etc.) — não depender de um único programa de teste
- [ ] Comportamento comparado com NGINX (headers, status codes, edge cases)
- [ ] Testado com o testador fornecido pela 42 (opcional, mas recomendado)
- [ ] Testes de carga / múltiplos clientes simultâneos
- [ ] Teste de envio de arquivos grandes (limite de body size)
- [ ] Teste de métodos não permitidos por rota
- [ ] Teste de redirecionamentos configurados

---

## 🌟 Bônus (só avaliado se o obrigatório estiver 100% ok)

- [ ] Suporte a cookies
- [ ] Gerenciamento de sessão (com exemplos simples de uso)
- [ ] Suporte a múltiplos tipos de CGI simultaneamente

---

## 🎓 Preparação para a avaliação

- [ ] Você consegue explicar **cada linha** do loop de eventos (poll/select/epoll/kqueue)
- [ ] Você consegue explicar o parser HTTP e da configuração sem ajuda
- [ ] Você consegue explicar como o CGI se comunica via pipes/fork/execve
- [ ] Você sabe justificar por que `errno` não é checado após I/O
- [ ] Você sabe justificar as escolhas de arquitetura (structs, classes, fluxo de dados)
- [ ] Preparado para uma modificação ao vivo pedida durante a defesa (pequena mudança de comportamento/feature)

---

## 📋 O que falta ajustar

### 🟠 Obrigatório faltando (implementação)
- **Chunked transfer-encoding**: sem dechunking no parser HTTP (`HttpRequestBuilder.hpp`) nem antes de repassar ao CGI.
- **Página de erro padrão**: sem fallback HTML hardcoded quando nenhuma é configurada (`Server.cpp` cai para texto simples).
- **Config default sem argumento**: `main.cpp:15` exige `argc == 2`; o subject (pág. 9) pede fallback para caminho padrão quando `argc == 1`. Falta também o arquivo de config padrão em si (só existem fixtures em `tests/`).
- **README.md fora do formato da 42**: falta linha itálica de créditos, seções "Descrição"/"Instruções"/"Recursos", e menção a uso de IA.

### 🟡 Não verificado (precisa teste manual antes da avaliação)
- Resiliência a crash/término inesperado sob carga real ou sem memória.
- Desconexão inesperada de clientes fora do fluxo de CGI.
- Compatibilidade com browser real (Chrome/Firefox) e comparação lado a lado com NGINX.
- Testes com `telnet`, `curl`, `nc` (nenhum documentado no repo).
- Testes de carga/múltiplos clientes simultâneos, upload de arquivo grande, métodos não permitidos por rota, redirecionamentos configurados.
- Teste automatizado cobrindo runtime do CGI (hoje só parsing/config são testados).

### 🟢 Bônus pendente (só vale se o obrigatório ficar 100%)
- Cookies: infraestrutura pronta (`Set-Cookie`/`Cookie`), falta demonstrar uso real (ex. sessão simples).
- Gerenciamento de sessão com exemplo de uso.
- Múltiplos tipos de CGI simultâneos (hoje só Python nos exemplos).
