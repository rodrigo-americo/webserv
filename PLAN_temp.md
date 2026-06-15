# Plano: Singleton Webserv — Classe Mestre

## Context

O projeto já possui um pipeline de parsing completo (Lexer → Parser → AST + Validators). O próximo passo é criar a "classe mestre" que:
1. Lê o arquivo de configuração usando o `ParserBuilder` existente
2. Interpreta a AST em objetos de configuração tipados
3. Gerencia o loop de eventos (`poll`) e os sockets do servidor
4. É acessível globalmente para signal handlers (padrão Singleton)

O `main.cpp` atual é apenas um esqueleto HTTP que não usa nada do parser.

---

## Novas estruturas a criar

### 1. `webserv/config/LocationConfig.hpp`

Struct que representa um bloco `location` interpretado:

| Campo | Tipo | Diretiva de origem |
|---|---|---|
| `path` | `std::string` | argumento do `location` |
| `modifier` | `ParserTokenType` | `=`, `~`, `~*`, `^~` |
| `root` | `std::string` | `root` |
| `index` | `std::vector<std::string>` | `index` |
| `autoindex` | `bool` | `autoindex` |
| `allowed_methods` | `std::vector<std::string>` | GET/POST/DELETE |
| `redirect` | `std::pair<int, std::string>` | `return` |
| `cgi_extension` | `std::string` | por extensão (ex: `.php`) |
| `upload_store` | `std::string` | caminho de upload |
| `client_max_body_size` | `size_t` | herdado do server se ausente |
| `try_files` | `std::vector<std::string>` | `try_files` |

### 2. `webserv/config/ConfigServerConfig.hpp`

Struct que representa um bloco `server` interpretado:

| Campo | Tipo | Diretiva de origem |
|---|---|---|
| `listen` | `std::vector<std::pair<std::string,int>>` | `listen` (host:porta) |
| `server_names` | `std::vector<std::string>` | `server_name` |
| `root` | `std::string` | `root` |
| `error_pages` | `std::map<int, std::string>` | `error_page` |
| `client_max_body_size` | `size_t` | `client_max_body_size` |
| `locations` | `std::vector<LocationConfig>` | blocos `location` filhos |

### 3. `webserv/config/ConfigReader.hpp`

Visitor concreto que herda de `ParserVisitorBase` e percorre a AST para preencher um `std::vector<ConfigServerConfig>`.

**Fluxo de travessia:**
```
AST root (PT_MAIN)
  └─ Block PT_SERVER → cria ConfigServerConfig, visita filhos
        ├─ Directive PT_LISTEN                → listen[]
        ├─ Directive PT_SERVER_NAME           → server_names[]
        ├─ Directive PT_ROOT                  → root
        ├─ Directive PT_ERROR_PAGE            → error_pages[]
        ├─ Directive PT_CLIENT_MAX_BODY_SIZE  → client_max_body_size
        └─ Block PT_LOCATION                 → cria LocationConfig, visita filhos
              ├─ Directive PT_ROOT           → location.root
              ├─ Directive PT_INDEX          → location.index[]
              ├─ Directive PT_AUTOINDEX      → location.autoindex
              ├─ Directive PT_TRY_FILES      → location.try_files[]
              ├─ Directive PT_RETURN         → location.redirect
              └─ Directive PT_CLIENT_MAX_BODY_SIZE → location.client_max_body_size
```

- Usa `schema_string`, `schema_number`, `schema_bool` de `lib/schema/` para validar valores das diretivas
- Herda `has_errors` de `lib/segregation/has_errors.hpp` para acumular erros semânticos

**Única adição necessária em `parser/parser/ParserAst.hpp`:**
Método público `void walkWith(ParserVisitorBase&)` que chama `_root->accept(visitor)` — expõe a travessia sem vazar o ponteiro privado `_root`.

### 4. `webserv/Webserv.hpp` — o Singleton

```
Webserv (singleton C++98)
├── _instance: static Webserv*
├── _configs: std::vector<ConfigServerConfig>
├── _poll_fds: std::vector<struct pollfd>
├── _server_fds: std::map<int, size_t>   (fd → índice em _configs)
├── _running: bool
│
├── getInstance(): static Webserv&
├── destroy(): static void              ← para signal handler
├── loadConfig(path: string): void
│     → ParserBuilder::defaultBuilder().withFile(path).build()
│     → ast.validateStructure()
│     → ConfigReader lê ast → preenche _configs
│     → _setupSockets() cria/bind/listen por porta
├── run(): void                         ← loop poll()
├── stop(): void                        ← _running = false
└── [private]
      _setupSockets()
      _acceptClient(server_fd)
      _handleClient(client_fd)
```

**Padrão Singleton C++98:** construtor privado, `static Webserv* _instance` (inicializado NULL), `static void destroy()` para limpeza segura no signal handler.

---

## Integração com código existente

| Arquivo | O que muda |
|---|---|
| `parser/parser/ParserAst.hpp` | Adiciona `walkWith(ParserVisitorBase&)` público |
| `main.cpp` | Reescrito: lê `argv[1]`, getInstance, loadConfig, run |
| `lib/schema/*.hpp` | Usados no ConfigReader — **sem mudanças** |
| `lib/segregation/has_errors.hpp` | Herdado por ConfigReader — **sem mudanças** |
| `parser/parser/ParserBuilder.hpp` | **Sem mudanças** |

---

## Estrutura de diretórios proposta

```
webserv/
├── Webserv.hpp
└── config/
    ├── ConfigServerConfig.hpp
    ├── LocationConfig.hpp
    └── ConfigReader.hpp
```

---

## Verificação

1. Compilar com `c++ -std=c++98 -Wall -Wextra -Werror`
2. Executar `./webserv parser/lexer/example.config`
3. `ConfigReader` deve produzir 2 `ConfigServerConfig` (porta 80 e 443) com seus `LocationConfig` preenchidos
4. `Webserv::run()` deve iniciar o loop `poll()` ouvindo nas portas configuradas
5. Verificar com `ss -tlnp` ou `netstat` que as portas estão abertas
6. Conectar com `curl` e receber resposta
