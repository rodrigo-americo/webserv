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


## Estrutura dos Testes

Os testes são organizados em diretórios com o sufixo `_test`.

### Exemplo

```text
tests/
├── @setup/
│   └── framework.cpp
│
├── parser_test/
│   ├── test_constructor.cpp
│   └── test_validation.cpp
│
└── utils_test/
    └── test_string.cpp
```

Cada arquivo `.cpp` dentro de um diretório `_test` representa um teste executável independente.

# Execução dos Testes

## Executar todos os testes

```bash
make tests
```

O sistema irá:

1. Localizar automaticamente todos os diretórios com sufixo `_test`.
2. Compilar cada arquivo de teste encontrado.
3. Executar cada teste individualmente.
4. Exibir um resumo ao final.

### Exemplo de saída

```text
module: parser

    testing... constructor
    testing... validation

──────────────────────────────────────────
12 tests passed.
0 test(s) failed.
```

## Modo Verbose

Para exibir também os testes aprovados:

```bash
make tests_verbose
```

Internamente, o Makefile adiciona o parâmetro:

```bash
--verbose
```

que é interpretado pelo framework.

# Criando Novos Testes

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

# Inicialização

Todo teste deve inicializar o sistema de mensagens:

```cpp
messages::settup(argc, argv);
```

Essa função:

* Limpa o estado interno.
* Detecta o parâmetro `--verbose`.
* Define se os testes aprovados serão exibidos.

# Sistema de Assertions

## assert

Compara dois valores.

### Sintaxe

```cpp
assert(expected, actual, test_name);
```

ou

```cpp
assert(expected, actual, test_name, LINE_DATA());
```

### Exemplo

```cpp
failures += assert(
    42,
    result,
    "Should return the expected value"
);
```

### Em caso de falha

```text
FAILED: Should return the expected value

EXPECTED:
42

GOT:
38
```

## assert_true

Atalho para validar expressões booleanas verdadeiras.

### Sintaxe

```cpp
assert_true(expression, test_name);
```

### Exemplo

```cpp
failures += assert_true(
    user.isValid(),
    "User should be valid"
);
```

Equivale a:

```cpp
assert(true, user.isValid(), "User should be valid");
```

## assert_false

Atalho para validar expressões booleanas falsas.

### Sintaxe

```cpp
assert_false(expression, test_name);
```

### Exemplo

```cpp
failures += assert_false(
    user.hasErrors(),
    "User should not contain errors"
);
```

Equivale a:

```cpp
assert(false, user.hasErrors(), "User should not contain errors");
```

# Rastreamento de Arquivo e Linha

O framework fornece a macro:

```cpp
LINE_DATA()
```

que captura automaticamente:

```text
arquivo:linha
```

onde a assertion foi executada.

### Exemplo

```cpp
failures += assert(
    expected,
    actual,
    "Result validation",
    LINE_DATA()
);
```

Saída:

```text
FAILED: Result validation src/parser.cpp:42
```

Isso facilita localizar rapidamente a origem do erro.

# Sistema de Mensagens

O framework armazena internamente:

* Testes aprovados.
* Testes falhos.

Ao final da execução:

```cpp
messages::print();
```

é exibido um relatório consolidado.

### Exemplo

```text
PASSED: Constructor test
PASSED: Empty input test

FAILED: Invalid token test
EXPECTED: ERROR
GOT: OK

──────────────────────────────────────────
2 tests passed.
1 test(s) failed.
```

# Recursos do Framework

## Saída Colorida

Os resultados são exibidos com estilos visuais para facilitar a leitura:

* Verde para sucessos.
* Vermelho para falhas.
* Amarelo para valores esperados.
* Informações de localização destacadas.

## Mensagens Detalhadas

Quando ocorre uma falha, o framework mostra:

* Nome do teste.
* Arquivo e linha (quando informado).
* Valor esperado.
* Valor obtido.

Exemplo:

```text
FAILED: Parse integer src/parser.cpp:87

EXPECTED:
123

GOT:
321
```

## Conversão Automática de Tipos

As mensagens utilizam:

```cpp
utils::to_string()
```

para converter valores para texto.

Isso melhora significativamente o debug de tipos personalizados e estruturas complexas.

## Descoberta Automática

O Makefile detecta automaticamente:

* Novos módulos de teste.
* Novos arquivos `.cpp`.
* Arquivos de setup compartilhados.

Não é necessário registrar testes manualmente.

# Exemplo Completo

```cpp
int main(int argc, char **argv)
{
    messages::settup(argc, argv);

    int failures = 0;

    failures += assert(
        10,
        5 + 5,
        "Simple addition",
        LINE_DATA()
    );

    failures += assert_true(
        10 > 5,
        "Comparison test",
        LINE_DATA()
    );

    failures += assert_false(
        5 > 10,
        "Negative comparison"
    );

    messages::print();

    return failures;
}
```

## Fluxo de Uso

1. Compilar o projeto normalmente.
2. Executar:

```bash
make tests
```

3. Para exibir também os testes aprovados:

```bash
make tests_verbose
```

4. Criar novos testes apenas adicionando:

   * uma pasta com sufixo `_test`;
   * arquivos `.cpp` dentro dela.

5. Utilizar `LINE_DATA()` para rastrear rapidamente a origem das falhas.

O framework cuidará automaticamente da compilação, execução, agregação dos resultados e geração dos relatórios.
