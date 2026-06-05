# Visão Geral da Arquitetura

Esse módulo implementa um pipeline completo de análise de configuração inspirado no funcionamento do Nginx, transformando um arquivo de texto em uma estrutura navegável e validada.

O fluxo geral é:

```text
Arquivo de Configuração
          │
          ▼
       File
          │
          ▼
       Lexer
          │
          ▼
   LexerIterator
          │
          ▼
       Parser
          │
          ▼
      ParserAst
          │
          ▼
        Visitors
     ┌───────────────┐
     │ Printer       │
     │ Validator     │
     └───────────────┘
```

---

# Camada de Leitura

## File

A classe `File` abstrai o acesso ao arquivo.

Responsabilidades:

* carregar o conteúdo;
* armazenar linhas;
* controlar posição atual;
* rastrear linha e coluna;
* permitir navegação para frente e para trás.

Ela funciona como uma espécie de iterador enriquecido sobre um texto.

---

# Camada Léxica

## Lexer

O lexer converte texto bruto em tokens.

Exemplo:

```nginx
server {
    listen 8080;
}
```

vira algo semelhante a:

```text
WORD(server)
LBRACE
WORD(listen)
WORD(8080)
SEMICOLON
RBRACE
```

Responsabilidades:

* identificar delimitadores;
* identificar comentários;
* identificar strings;
* identificar palavras;
* gerar tokens.

---

## LexerBuilder

Permite configurar o lexer.

Exemplo:

```cpp
LexerBuilder()
	.withDefaultTokens()
	.withComment("#")
	.withQuote("\"");
```

Isso evita que o lexer fique cheio de configurações fixas.

---

## LexerIterator

Fornece navegação segura sobre os tokens produzidos pelo lexer.

Responsabilidades:

* avançar;
* retroceder;
* acessar token atual;
* expor erros léxicos.

---

# Camada Sintática

## Parser

O parser recebe tokens e produz uma árvore.

Transforma:

```nginx
http {
	server {
		listen 8080;
	}
}
```

em algo conceitualmente parecido com:

```text
MAIN
└── HTTP
    └── SERVER
        └── LISTEN 8080
```

Responsabilidades:

* reconhecer blocos;
* reconhecer diretivas;
* construir nós;
* detectar erros sintáticos.

---

## ParserComposite

Implementa o padrão Composite.

### ParserNode

Classe base.

### Directive

Nó folha.

Exemplo:

```nginx
listen 8080;
```

### Block

Nó composto.

Exemplo:

```nginx
server {
}
```

Pode possuir filhos.

---

# Camada de AST

## ParserAst

Representa a árvore sintática completa.

Responsabilidades:

* armazenar raiz;
* armazenar erros;
* executar visitors;
* servir como ponto de entrada da AST.

---

# Camada de Visitors

A árvore não contém regras de impressão nem validação.

Essas responsabilidades são externas.

---

## ParserVisitorPrinter

Percorre a árvore e imprime sua estrutura.

Exemplo:

```text
[http]
    [server]
        listen -> 8080
```

---

## ParserVisitorValidator

Percorre a árvore validando:

* escopos;
* quantidade de argumentos.

Funciona como coordenador dos validadores.

---

# Camada de Validação

## ScopeValidator

Valida contexto.

Exemplos:

✔

```nginx
http {
	server {}
}
```

✘

```nginx
events {
	server {}
}
```

---

## ArgAmountValidator

Valida quantidade de argumentos.

Exemplo:

✔

```nginx
root /var/www;
```

✘

```nginx
root;
```

---

# Builders

O projeto utiliza builders em vários pontos:

* LexerBuilder
* ParserBuilder
* ScopeValidatorBuilder
* ArgAmountValidatorBuilder

Isso permite criar configurações declarativas:

```cpp
ParserBuilder::defaultBuilder()
	.withFile("nginx.conf")
	.build();
```

---

# Padrões de Projeto Utilizados

## Builder

Construção incremental de objetos complexos.

Exemplos:

* ParserBuilder
* LexerBuilder
* ScopeValidatorBuilder
* ArgAmountValidatorBuilder

---

## Composite

Representação hierárquica da AST.

Exemplos:

* ParserNode
* Directive
* Block

Permite tratar folhas e nós compostos de maneira uniforme.

---

## Visitor

Operações externas sobre a AST.

Exemplos:

* ParserVisitorPrinter
* ParserVisitorValidator

Permite adicionar funcionalidades sem alterar os nós da árvore.

---

# O que foi praticado construindo isso

Esse parser exercita muito mais do que apenas parsing.

## Estruturas de Dados

Você praticou:

* árvores;
* pilhas implícitas de contexto;
* mapas;
* vetores;
* iteração customizada.

---

## Programação Orientada a Objetos

Você praticou:

* herança;
* polimorfismo;
* interfaces;
* encapsulamento;
* composição.

---

## Design Patterns

Poucos projetos da 42 permitem praticar tantos padrões ao mesmo tempo:

* Builder
* Composite
* Visitor

e todos em um contexto real.

---

## Arquitetura de Software

Talvez o ponto mais valioso.

Você separou claramente:

```text
Leitura
↓
Tokenização
↓
Parsing
↓
AST
↓
Validação
↓
Impressão
```

Cada etapa possui uma responsabilidade única.

Isso segue fortemente o princípio de responsabilidade única (SRP).

---

## Desenvolvimento de Linguagens

Mesmo sem ser um compilador completo, você praticou conceitos encontrados em:

* compiladores;
* interpretadores;
* engines de script;
* parsers de configuração;
* bancos de dados;
* navegadores.

A arquitetura é muito parecida com a de ferramentas reais.

---

# Valeu a pena fazer dessa forma?

Sim.

Provavelmente mais do que seria necessário para apenas passar no Webserv.

Você poderia ter feito algo muito mais simples:

```text
Arquivo
↓
Parser gigante
↓
Configuração
```

e o projeto continuaria funcionando.

Mas o que você construiu tem vantagens importantes:

### Extensibilidade

Adicionar uma nova diretiva exige apenas:

```cpp
.withDirective(...)
.addValidation(...)
.withDirectiveOnScope(...)
```

Sem alterar o parser.

---

### Manutenibilidade

Cada módulo tem responsabilidade clara.

Um bug no lexer não afeta o validador.

Um bug no validador não afeta a AST.

---

### Testabilidade

Cada componente pode ser testado isoladamente.

Você consegue testar:

* lexer;
* parser;
* escopos;
* quantidade de argumentos;

sem depender do servidor inteiro.

---

### Reutilização

Esse parser poderia ser reutilizado em outros projetos.

Na prática, ele já é quase uma pequena biblioteca de parsing configurável.

---

# O ganho mais importante

O maior ganho não é o parser em si.

É ter praticado a construção de um sistema onde novas funcionalidades são adicionadas principalmente por configuração e composição, e não por modificações em código existente.

Essa é uma característica presente em software profissional de longo prazo.

Você saiu de algo como:

```cpp
if (directive == "root")
	...
else if (directive == "listen")
	...
else if (directive == "server_name")
	...
```

para algo muito mais declarativo:

```cpp
.withDirective(...)
.addValidation(...)
.withDirectiveOnScope(...)
```

Essa mudança de mentalidade — sair de código procedural cheio de condicionais para um sistema dirigido por dados e regras — é provavelmente a habilidade arquitetural mais valiosa exercitada nesse parser.
