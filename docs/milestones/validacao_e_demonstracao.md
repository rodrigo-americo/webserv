# Validação e Demonstração

Valida o servidor de ponta a ponta e o prepara para avaliação. Cobre **edge cases de conformidade com HTTP/1.1**, **suporte a cookies e gerenciamento de sessão**, **testes de stress e carga** (siege, wrk), **resiliência a timeouts e erros**, **auditoria de vazamentos de memória e file descriptors**. Inclui uma aplicação web de demonstração exibindo serving estático, múltiplos tipos de CGI e funcionalidades baseadas em sessão, construída exclusivamente sobre o servidor, servindo como prova final de corretude.

[README.md](../../README.md)

## Da Plataforma à Prova

Ao final deste milestone, o servidor é verificável. Cada camada construída até aqui é submetida à pressão: carga, borda, tempo, memória. O que não quebra, passa. O que passa, demonstra.

A aplicação de demonstração é um sistema real, rodando sobre o servidor, exibindo tudo que foi construído desde a fundação. Não há separação entre o que foi implementado e o que é mostrado.
