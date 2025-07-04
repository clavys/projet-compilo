# projet-compilo
Un métacompilateur et compilateur modulaire en C++ basé sur des grammaires formelles.

## Objectif

- Définir une grammaire personnalisée
- Générer un compilateur en C++ (analyse lexicale, syntaxique, AST, codegen)
- Créer un langage jouet entièrement compilable

## Structure

- `src/` : code source du méta-compilo
- `grammars/` : grammaires des langages à compiler
- `generated/` : compilateurs générés automatiquement
- `examples/` : exemples de code dans le langage compilé
- `tests/` : tests unitaires
- `docs/` : documentation technique

## TODO

- [ ] Support EBNF dans les grammaires
- [ ] Génération automatique d'un parser C++
- [ ] AST + interpréteur simple
