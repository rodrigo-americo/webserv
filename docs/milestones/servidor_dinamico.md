# Servidor Dinâmico

Estende o servidor com capacidades dinâmicas. Cobre **execução de CGI** (PHP, Python, etc.), **upload de arquivos**, **redirecionamentos HTTP** e **repasse de variáveis de ambiente** para processos CGI. Este milestone introduz gerenciamento de processos e a complexidade dos ciclos dinâmicos de requisição/resposta construídos sobre a fundação estática.

[READEME.md](../../README.md)

## Do Estático ao Vivo

O Servidor Estático estabeleceu o protocolo. O Servidor Dinâmico é onde o sistema começa a computar.

Ao final deste milestone, o servidor é um executor de lógica. Requisições disparam processos. Processos produzem respostas. O servidor passa a ser um intermediário entre o cliente e o código.
