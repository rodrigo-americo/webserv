
# Framework de Testes

Este projeto inclui um framework de testes leve, integrado ao sistema de build e projetado para facilitar a criação, execução e manutenção de testes automatizados.

[READEME.md](../README.md)

## Principais Recursos

* Descoberta automática de testes através da estrutura de diretórios.
* Compilação e execução automatizadas via Makefile.
* Saída colorida e legível para facilitar a identificação de sucessos e falhas.
* Mensagens detalhadas com valores **EXPECTED** e **GOT**.
* Rastreamento automático da origem dos erros através de arquivo e linha.
* Suporte a assertions genéricas e booleanas.
* Integração com `utils::to_string()` para melhor visualização de valores durante o debug.
* Filtragem de testes por módulo ou nome (`TEST_FILTER`).
* Execução de testes em modo verbose.
* Suporte a coverage automatizado via `gcovr`.
* Ambiente virtual Python (`.venv`) criado automaticamente para tooling.
* Lib e setup de testes pré-compilados para otimização de performance.
* Execução isolada de cada teste (um binário por teste).
* Sistema de limpeza completo incluindo artefatos de coverage e ambiente Python.

## Estrutura dos Testes

Os testes são organizados em diretórios com o sufixo `_test`.

### Exemplo

```text
tests/
├── @setup/
│   ├── framework.cpp
│   └── *.cpp (setup compartilhado)
│
├── parser_test/
│   ├── test_constructor.cpp
│   └── test_validation.cpp
│
└── utils_test/
    └── test_string.cpp
````

### Componentes adicionais do framework

```text
tests/
├── test/        → biblioteca de testes pré-compilada (.ar)
├── lib/         → biblioteca do projeto usada nos testes (.ar)
```

Cada arquivo `.cpp` dentro de um diretório `_test` representa um teste executável independente.


## Executar todos os testes

```bash
make tests
```

O sistema irá:

1. Localizar automaticamente todos os diretórios com sufixo `_test`.
2. Compilar a lib e setup (pré-compilados quando possível).
3. Compilar cada arquivo de teste individualmente.
4. Executar cada teste isoladamente.
5. Exibir um resumo ao final.

## Modo Verbose

```bash
make tests_verbose
```

Exibe também informações detalhadas de execução de cada teste.

Internamente ativa:

```bash
--verbose
```

## Filtragem de testes

Permite executar apenas partes específicas do framework.

```bash
make tests TEST_FILTER="<nome_1>, <nome_2>, ... <nome_n>"
```

### Exemplos

A separação dos nomes pode ser através de `,` ou `<espaço>`.

```bash
# executa apenas o teste file do módulo parser
make tests TEST_FILTER=file
# executa apenas o teste str do módulo utils, apos isso executa todos os testes do módulo schema
make tests TEST_FILTER="str, schema"
# executa apenas o teste file do módulo parser, apos isso executa apenas o teste logger do módulo tools
make tests TEST_FILTER="file logger"
```

📌 O filtro pode ser aplicado tanto em módulos quanto em arquivos de teste.

## Coverage

```bash
make tests_coverage
```

Este modo:

1. Cria automaticamente o ambiente `.venv` (se necessário)
2. Instala dependências via `requirements.py_txt`
3. Compila com flag `--coverage`
4. Executa os testes
5. Gera relatório com `gcovr`

## Limpeza

### Limpeza parcial

```bash
make clean
```

Remove objetos e arquivos intermediários:

* objetos de build
* bibliotecas `.ar`
* artefatos de coverage (`.gcda`, `.gcno`, `.gcov`)

### Limpeza total

```bash
make fclean
```

Remove:

* binário principal
* `.venv`

## Criando Novos Testes

Para adicionar um novo conjunto de testes:

1. Criar uma pasta terminada em `_test`.
2. Adicionar arquivos `.cpp` dentro dela.
3. Implementar um `main()` utilizando as funções do framework.

### Exemplo

```text
tests/math_test/test_sum.cpp
```

```cpp
int main(int argc, char **argv)
{
    messages::settup(argc, argv);

    int failures = 0;

    failures += assert(4, 2 + 2, "Basic sum");

    messages::print();

    return failures;
}
```

Nenhuma alteração no Makefile é necessária.

## Inicialização

Todo teste deve inicializar o sistema de mensagens:

```cpp
messages::settup(argc, argv);
```

Essa função:

* Limpa o estado interno.
* Detecta parâmetros como `--verbose`.
* Configura o modo de exibição de resultados.

## Sistema de Assertions

### assert

Compara dois valores.

```cpp
assert(expected, actual, test_name, LINE_DATA());
```

### assert_true

Valida expressão verdadeira.

```cpp
assert_true(expression, test_name, LINE_DATA());
```

### assert_false

Valida expressão falsa.

```cpp
assert_false(expression, test_name, LINE_DATA());
```

## Rastreamento de Arquivo e Linha

```cpp
LINE_DATA()
```

Captura automaticamente:

```text
arquivo:linha
```

Exemplo:

```cpp
FAILED: Test name src/file.cpp:42
```

## Sistema de Mensagens

```cpp
messages::print();
```

Exibe resumo final:

* testes aprovados
* testes falhos
* detalhes de erro

## Recursos do Framework

### Saída colorida

* Verde → sucesso
* Vermelho → falha
* Destaques para EXPECTED / GOT

### Mensagens detalhadas

Quando um teste falha:

* nome do teste
* localização (arquivo/linha)
* valor esperado
* valor obtido

---

### Conversão automática

```cpp
utils::to_string()
```

Permite imprimir tipos complexos automaticamente.

---

### Descoberta automática

O Makefile detecta automaticamente:

* novos módulos `_test`
* novos arquivos `.cpp`
* setup compartilhado

---

## Fluxo de uso recomendado

### Desenvolvimento

```bash
make tests TEST_FILTER=<feature>
```

### Validação completa

```bash
make tests_coverage
make fclean
```

---

## Observações importantes

* Cada teste é compilado e executado isoladamente
* O framework é otimizado para feedback rápido durante desenvolvimento
* Coverage e tooling Python são opcionais e isolados via `.venv`

---
