# Projet de Compilation avec Méta-Compilateur

Ce projet a été réalisé dans le cadre d'un **Master 1 pour une UE de Compilation**. Il implémente un système de compilation complet en C++ avec une approche originale utilisant un méta-compilateur pour interpréter les grammaires.

##  Concept Clé

**Le méta-compilateur ne génère pas de code** (contrairement à ANTLR ou yacc), mais il **interprète la grammaire** et construit un **AST de grammaire** exploitable. Cette approche permet une grande flexibilité  : l'AST(arbre syntaxique abstrait) de grammaire sert de "mode d'emploi" que le parser principal utilise pour analyser le code source.

##  Architecture du Projet

### 1. **Méta-Compilateur** (`meta/`) - Interpréteur de Grammaires
- **Objectif** : Parse les grammaires (fichiers `.gram`) et construit un AST de grammaire exploitable
- **Approche** : Contrairement aux générateurs de parsers classiques, il n'écrit pas de code mais produit une structure de données
- **Composants** :
  - `MetaLexer` : Tokenisation des grammaires selon la méta-grammaire
  - `MetaParser` : Parsing des grammaires et construction de l'AST de grammaire
  - `MetaASTBuilder` : Construction des structures AST interprétables

### 2. **Compilateur Principal** (`compilo/`) - Utilise l'AST de Grammaire  
- **Objectif** : Compile le code source en P-code en exploitant l'AST de grammaire fourni par le méta-compilateur
- **Approche** : Le parser utilise l'AST de grammaire comme "guide" pour analyser le code source dynamiquement
- **Composants** :
  - `Lexer` : Tokenisation du code source
  - `Parser` : Parsing du code source guidé par l'AST de grammaire
  - `PCodeGen` : Génération de P-code

### 3. **Runtime** (`compilo/runtime.cpp`) - Machine Virtuelle
- Interprète le P-code avec une pile d'exécution et gestion des variables

##  Flux d'Exécution

```
meta/
Grammaire (.gram) → [Méta-Lexer] → Méta-Tokens → ([Méta-Parser] + méta-AST) → AST de Grammaire

compilo/                                                      ↓
Code Source (.mylang) → [Lexer] → Tokens → ([Parser] + AST de Grammaire) → P-code → [Runtime] → Exécution
```

##  Exemple Concret

### Code Source (`example.mylang`)
```javascript
var a = 0;
while (a < 5) {
    a = a + 1;
};
print a;
```

### P-Code Généré
```
0:  PUSH 0
1:  STORE a
2:  LOAD a
3:  PUSH 5
4:  LT
5:  JMZ 11
6:  LOAD a
7:  PUSH 1
8:  ADD
9:  STORE a
10: JMP 2
11: LOAD a
12: PRINT
13: HALT
```

### Résultat d'Exécution
```
5
Execution terminee sans erreur
```

##  Démarrage Rapide

### Build
```bash
mkdir build && cd build
cmake ..
make
```

### Exécution
```bash
./projet-compilo
```

##  Fonctionnalités Principales

### **Spécification de Grammaires LL(1)**
- Format `.gram` personnalisé pour définir des langages
- Contraintes : grammaires non-récursives à gauche, compatibles LL(1)
- **Test d'auto-cohérence** : `g0.gram` (méta-grammaire) / `gpl.gram` (langage complet)

### **Parser Dynamique par Descente Récursive**
- Construit à l'exécution à partir de l'AST de grammaire
- Validation stricte contre les règles grammaticales

### **P-Code et Machine Virtuelle**
- Jeu d'instructions fixe : arithmétique, contrôle de flux, variables, fonctions
- Exécution stack-based avec gestion d'erreurs fail-fast

##  Extensibilité
### **Extensibilité des Grammaires**
Les utilisateurs peuvent définir de nouveaux langages en écrivant des fichiers `.gram`. Le méta-compilateur génère automatiquement un parser à partir de ces grammaires.

### **Extensibilité du Jeu d'Instructions**
Le jeu d'instructions P-Code est fixe par défaut (arithmétique, contrôle de flux, fonctions, gestion des variables). Cependant, les développeurs peuvent l'étendre en ajoutant de nouveaux opcodes. Cela nécessite des ajouts coordonnés dans le lexer, parser, générateur P-Code, runtime, définitions de tokens et grammaire, mais ces changements sont additifs et ne nécessitent pas de modifier la logique existante.

##  Structure du Projet

```
projet-compilo/
├── grammars/             # g0.gram (méta-grammaire), gpl.gram
├── examples/             # example.mylang, tri_fusion.mylang  
├── include/compilo/      # lexer, parser, runtime, pcodegen
├── include/meta/         # meta_lexer, meta_parser
├── src/                  # Implémentations C++
├── tests/                # Test d'intégration
└── CMakeLists.txt
```

##  Exemples

### Programmes d'Exemple
- `example.mylang` : Variables, boucles, expressions
- `tri_fusion.mylang` : Algorithme de tri avec fonctions

### Configuration Debug
```cpp
// Dans main.cpp - pour tracer l'exécution
Logger logger(Logger::Level::Debug);
```

##  Concepts Académiques Illustrés

- **Méta-compilation** : Interprétation de grammaires vs génération de code
- **Auto-reconnaissance** : Test de cohérence avec la méta-grammaire  
- **Parsing dirigé par données** : AST de grammaire comme structure de contrôle
- **Architecture modulaire** : Séparation méta-compilation/compilation/runtime

##  Utilisation Personnalisée

1. **Définir une grammaire** dans `grammars/ma_grammaire.gram`
2. **Écrire un programme** dans `examples/mon_programme.mylang`  
3. **Modifier les chemins** dans `main.cpp`
4. **Compiler et exécuter**

##  Contribution

1. Fork le repository
2. Créer une branche pour votre fonctionnalité
3. Implémenter avec tests
4. Soumettre une pull request

##  Auteur

Projet de Master 1 - UE Compilation  
REPPERT William 
reppertwilliam@gmail.com 
william.reppert@etu.univ-nantes.fr 
