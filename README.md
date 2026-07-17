# Webserv

> Servidor HTTP/1.1 implementado do zero em C++98 — projeto em trio da **42SP**, avaliado com **125/100** na defesa.

![C++98](https://img.shields.io/badge/C%2B%2B-98-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey?logo=linux&logoColor=white)
![42SP](https://img.shields.io/badge/42SP-125%2F100-brightgreen)
![Status](https://img.shields.io/badge/status-conclu%C3%ADdo-success)

## Sobre o projeto

Servidor HTTP construído sem frameworks nem libs de rede prontas: sockets, multiplexing de I/O, parser HTTP e engine de configuração foram implementados manualmente em C++98. O objetivo não era só passar nos testes de avaliação, mas produzir uma base arquiteturalmente sólida — camadas bem definidas, dependências invertidas via interfaces e design patterns aplicados onde resolvem um problema real, não como enfeite.

O resultado é um servidor compatível com HTTP/1.1, com CGI, upload de arquivos, sessões, configuração no estilo Nginx e uma aplicação de demonstração rodando inteiramente sobre ele.

## Destaques técnicos

* **Abstração de multiplexing via Strategy** — `ConnectionPool` depende de `IMultiplexer`, nunca de `epoll`/`poll`/`select` diretamente. O mecanismo é escolhido em runtime pela configuração (`events { use poll; }`), e adicionar um novo backend (ex.: `io_uring`) significa implementar a interface, não tocar no pool.
* **Observer entre `ConnectionPool` e `Server`** — o pool não interpreta requisições HTTP, apenas notifica quem deve tratá-las, mantendo a camada de rede isolada da camada de aplicação.
* **Visitor no parser de configuração** — o AST gerado pelo lexer/parser é percorrido por visitors especializados (`ParserVisitorValidator`, `ParserVisitorToConfig`, `ParserVisitorPrinter`, `ParserVisitorFreeAst`), separando validação, construção de config e debug sem poluir os nós do AST.
* **Builder** — `HttpRequestBuilder` e `ConfigBuilderVisitor` constroem objetos complexos (requisição HTTP, árvore de configuração) passo a passo, isolando a lógica de parsing da representação final.
* **Factory + Singleton genéricos via CRTP** — `patterns::factory` e `patterns::singleton` são templates reutilizados por múltiplos módulos (`UploadFactoyry`, `WebServerConfig`, `Logger`) sem duplicar boilerplate de instância única/registro.
* **Herança de configuração hierárquica** — blocos `http` → `server` → `location` resolvem herança de diretivas via `ConfigInheritanceResolver`, replicando o comportamento do Nginx.

## Funcionalidades

* Conformidade HTTP/1.1, métodos `GET`, `POST`, `DELETE`
* CGI (Python, shell, PHP, etc.) com repasse de variáveis de ambiente
* Upload via `multipart/form-data` e `application/octet-stream`
* Sessões e cookies
* Virtual hosts e blocos `location` configuráveis
* Listagem automática de diretórios (autoindex)
* Páginas de erro customizáveis por status (400, 403, 404, 405, 413, 415, 500...)
* Configuração no estilo Nginx, com escolha de `epoll`/`poll`/`select` por config
* IPv6

## Stack

C++98 · Linux (syscalls POSIX) · GitHub Actions (CI de build) · framework de testes próprio integrado ao Makefile · `gcovr` para coverage

## Como rodar

```bash
make              # compila o binário `webserver`
./webserver test.config
```

```bash
make tests          # roda a suíte de testes
make tests_verbose  # com saída detalhada
make tests_coverage # com relatório de coverage (gcovr)
```

## Testes

Framework de testes próprio, com descoberta automática por diretório (`*_test/`), saída colorida, rastreamento de erro por arquivo/linha e execução isolada (um binário por teste). Cobre parser, config, schema, tools, path, utils, integração e ponta a ponta (e2e).

Documentação completa: [docs/test-framework.md](docs/test-framework.md)

## Arquitetura em milestones

O projeto foi construído em quatro etapas incrementais, cada uma documentada:

* [Fundação](docs/milestones/fundacao.md) — sockets, multiplexing, engine HTTP, config, logging
* [Servidor Estático](docs/milestones/servidor_estatico.md) — roteamento, arquivos estáticos, virtual hosts
* [Servidor Dinâmico](docs/milestones/servidor_dinamico.md) — CGI, upload, redirecionamentos
* [Validação e Demonstração](docs/milestones/validacao_e_demonstracao.md) — stress test, sessões, aplicação de demonstração

## Time

Projeto desenvolvido em trio na 42SP:

* [Rodrigo Gregório Américo](https://github.com/rodrigo-americo)
* [Bruno Fernandes Valero Nunes](https://github.com/bruno-valero)
* [Isadora Ghannam Ferreira](https://github.com/isaghannam)

## Status

✅ Concluído — avaliado com **125/100** na defesa da 42SP.
