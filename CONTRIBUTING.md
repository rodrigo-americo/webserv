# Guia de Contribuição — Webserver

## Branches

### Estrutura

```
main        ← estável, protegida
feature/    ← novas funcionalidades
fix/        ← correções
```

### Regras

- Nunca commitar diretamente na `main`
- Nomear branches em kebab-case: `feature/http-parser`, `fix/header-parsing`

---

## Proteção da main

As seguintes regras estão configuradas no GitHub e não podem ser contornadas:

- Pull request obrigatório antes de mergear
- **2 aprovações obrigatórias**
- Aprovações caem se novos commits forem feitos após a aprovação (dismiss stale reviews)
- A aprovação mais recente deve ser de alguém que não fez o último push
- Todas as conversas abertas no PR devem ser resolvidas antes do merge
- O build de CI deve passar antes do merge

---

## CI — Build e Testes

O GitHub Actions roda automaticamente em todo PR para `main` ou `develop`:

- **Build** — compila o projeto com `make`. PR bloqueado se falhar.
- **Tests** — roda `make tests`. PR bloqueado se algum teste falhar.

Para rodar localmente antes de abrir o PR:

```bash
make
make tests
```

---

## Padrão de Commits

Seguimos [Conventional Commits](https://www.conventionalcommits.org/).

### Formato

```
<tipo>: <descrição curta no imperativo>
```

### Tipos

| Tipo | Quando usar |
|------|-------------|
| `feat` | nova funcionalidade |
| `fix` | correção de bug |
| `docs` | documentação |
| `test` | adição ou correção de testes |
| `refactor` | refatoração sem mudança de comportamento |
| `chore` | configuração, Makefile, CI |

### Exemplos

```
feat: adiciona parsing de headers HTTP
fix: corrige leitura de body com content-length zero
test: adiciona testes para schema_string minLength
docs: atualiza README com instruções de build
chore: adiciona workflow de CI para build e testes
```

### Regras

- Descrição em minúsculas
- Sem ponto final
- No imperativo: "adiciona", "corrige", "remove" — não "adicionado" ou "adicionando"
- Commits pequenos e atômicos — um commit por mudança lógica

---

## Template de Pull Request

```markdown
## O que foi feito
-

## Motivação
<!-- Por que essa mudança é necessária? -->

## Como testar
-

## Checklist
- [ ] Compila sem erros ou warnings
- [ ] Testes passando (`make tests`)
- [ ] Sem prints de debug ou código comentado
- [ ] Documentação atualizada se necessário
```

---

## Fluxo do dia a dia

```bash
# 1. atualiza develop local
git checkout develop
git pull origin develop

# 2. cria branch para a feature
git checkout -b feature/nome-da-feature

# 3. desenvolve e commita
git add .
git commit -m "feat: descrição da mudança"

# 4. abre PR para develop no GitHub
# preenche o template, aguarda 2 aprovações e CI passar
```