# Fundação

Estabelece a espinha dorsal arquitetural do servidor. Cobre utilitários core, abstração de rede (`sockets`, **multiplexing** com `epoll/poll/select`, **pool de conexões**), **engine HTTP** (**parser de requisições**, **headers**, **response builder**), **sistema hierárquico de configuração** e **logging multi-destino** com níveis de severidade. Sem funcionalidades voltadas ao usuário — infraestrutura pura sobre a qual tudo mais será construído.

[README.md](../../README.md)

## A importância de uma boa fundação

O valor de um software não está nas features que o usuário vê, mas nas decisões que moldam sua fundação. Desde as escolhas feitas antes da primeira linha até a identidade das utilidades que sustentam tudo que vem depois.

Decisões de fundação não geram mais código. Geram mais responsabilidade. Quem é dono da memória? Quais são as garantias de exceção? Como os módulos se comunicam? Essas perguntas podem demorar, mas depois que estão respondidas, implementar dezenas de funções vira trabalho mecânico.

> Uma decisão errada na fundação não custa apenas o tempo de corrigi-la. Ela multiplica esse custo por tudo que foi construído sobre ela.

## Identidade arquitetural

Um sistema não é a soma de seus módulos. É a forma como eles se relacionam e compartilham convenções.

Quando cada parte foi escrita com uma filosofia diferente, o custo não é apenas técnico, é cognitivo. O desenvolvedor passa a carregar na cabeça não só o que cada parte faz, mas como cada parte *pensa*. Isso se manifesta em debugging que dura horas por ambiguidade de ownership, e em onboarding impossível porque não há uma lógica unificada a ser aprendida, só exceções a serem memorizadas.

É por isso que estabelecer uma identidade arquitetural cedo importa: ela garante que decisões similares sejam tomadas de forma similar em todo o sistema.

## SOLID e padrões de projeto são ferramentas de fundação

Eles não existem para sofisticar o código, mas para criar uma linguagem comum de decisões.

Considere o **Dependency Inversion**: `ConnectionPool` depende de `IMultiplexer`, não de `epoll` ou `poll`. Isso significa que a decisão de qual mecanismo usar — `MultiplexerEpoll`, `MultiplexerPoll`, `MultiplexerSelect` é resolvida na configuração, não espalhada pela lógica de conexão. Adicionar suporte a `io_uring` no futuro seria implementar a interface, não refatorar o pool.

O padrão **Observer** aparece na relação entre `ConnectionPool` e os `Servers`: o pool não sabe como tratar uma requisição HTTP, só que alguém deve tratar. Essa separação mantém cada parte dentro dos seus limites.

Quando um time adota essas convenções de forma consistente, adicionar uma nova feature parece natural. O caminho já está traçado.

---

Fundações boas são invisíveis. Um sistema bem arquitetado é aquele onde tudo simplesmente funciona, e ninguém precisa perguntar por quê.

O que separa os projetos que duram dos que desmoronam raramente está nas features. Está no que foi construído antes delas.
