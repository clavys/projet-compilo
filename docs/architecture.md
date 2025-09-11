# Architecture Technique Détaillée

##  Vue d'Ensemble

Ce système implémente un compilateur complet avec méta-compilateur utilisant une approche par **actions sémantiques codées**. L'architecture se distingue par son système d'actions numériques (`#100`, `#200`, etc.) intégrées directement dans les grammaires.

##  Architecture Globale 
> **[ UML 1 todo]**
```
┌─────────────────────────────────────────────────────────────┐
│                    MÉTA-NIVEAU                              │
├─────────────────────────────────────────────────────────────┤
│ Fichiers .gram → MetaLexer → MetaParser → Node AST          │
│                (méta-grammaire hardcodée)                   │
└─────────────────────────────────────────────────────────────┘
                               ↓ produit un AST
┌─────────────────────────────────────────────────────────────┐
│                  NIVEAU COMPILATION                         │
├─────────────────────────────────────────────────────────────┤
│ Fichiers .mylang → Lexer → Parser → PCodegen → P-Code       │
│                         ↗ utilise Node AST + actions        │
└─────────────────────────────────────────────────────────────┘
                               ↓ produit du P-Code
┌─────────────────────────────────────────────────────────────┐
│                  NIVEAU EXÉCUTION                           │
├─────────────────────────────────────────────────────────────┤
│ P-Code → Runtime → Résultat                                 │
└─────────────────────────────────────────────────────────────┘
```

### Séparation Méta vs Compilation

| **Méta-Compilateur** | **Compilateur Principal** |
|----------------------|---------------------------|
| Travaille sur `.gram` | Travaille sur `.mylang` |
| Produit un AST de grammaire | Produit du P-Code |
| Parse la structure des langages | Parse le code utilisateur |
| Actions intégrées dans tokens | Actions déclenchent génération |

##  Exemple Concret d'Actions Sémantiques

Voici un extrait de la grammaire avec les actions intégrées :

```
VarDecl -> 'var' . 'ID'#1 . '=' . ExprStmt#2
Assignment -> 'ID'#4 . '=' . ExprStmt#2
IfStmt -> 'if' . '(' . LogicalOr#100 . ')' . Block#101 . (/ Else /)
Else -> 'else'#102 . Block#103
AddExpr -> MulExpr . [ ( '+' . MulExpr#30 ) + ( '-' . MulExpr#31 ) ]
CallExpr -> 'IDFONCTION'#126 . [ '(' . [ ExprStmt . [ ',' . ExprStmt ] ] . ')' ]#125
ListLiteral -> '['#60 . [ LogicalOr#61 . [ ',' . LogicalOr#61 ] ] . ']'#62
WhileStmt -> 'while'#110 . '(' . LogicalOr#111 . ')' . Block#112
FuncDecl -> 'function' . 'IDFONCTION'#120 . '(' . [ 'ID'#121 . [ ',' . 'ID'#121 ] ] . ')' . FuncBlock#123
```

### Correspondance Actions → P-Code (Extrait de l'implémentation)

| Action | Description | P-Code Généré | Code Réel |
|--------|-------------|---------------|-----------|
| `#1` | Déclaration variable | `declareVariable(value)` | `pushVarName(value); declareVariable(value, !currentFunction.empty())` |
| `#2` | Assignation | `STORE variable` | `storeVariable(popVarName())` |
| `#30/#31` | Addition/Soustraction | `ADD/SUB` | `emit(OpCode::ADD/SUB); pushType("number")` |
| `#60-#62` | Création liste | `MAKE_LIST size` | `listSizeStack.push(0)` → `emit(MAKE_LIST, size)` |
| `#100-#104` | Structure IF/ELSE | `JUMP_IF_FALSE` + patch | Gestion `ifStack` avec `IfContext` |
| `#110-#112` | Boucle WHILE | `JUMP_IF_FALSE` + `JUMP` | Gestion `patchStack` pour sauts |
| `#120-#125` | Fonctions | `FUNC_DECL` + `CALL` | Gestion `functionTable` et `callStack` |

### Structures de Contrôle Complexes

**Structure IF :**
```cpp
struct IfContext {
    size_t jumpIfFalseIndex;
    size_t jumpIfFalseTarget;
    std::optional<size_t> jumpOverElseIndex;
    size_t jumpOverElseTarget;
};
```

Actions : `#100` → condition, `#101` → fin IF, `#102` → début ELSE, `#103` → fin ELSE, `#104` → patch final

##  Composants Clés (Signatures Essentielles)

> **[ UML  2 todo]** : Diagramme de classes montrant les relations entre MetaParser→ASTBuilder, Parser→PCodegen, Runtime→Value/Frame

### 1. Méta-Compilateur (Signatures Clés)

```cpp
namespace meta {
    class MetaLexer {
        int extract_action();  // Parse #100, #200...
        Token lex_terminal();  // Parse 'TERMINAL'
    };
    
    class MetaParser {
        bool analyse(Node& node, vector<Token>* tokens, int* index);
        bool generateGrammarAST();
    };
}
```

### 2. Parser Dirigé par Actions

```cpp
class Parser {
    bool analyse(Node& node, vector<Token>* tokens, int* index);
    bool generatePcode();
};

class PCodegen {
    // === Structures complexes pour gestion état ===
    struct FunctionInfo {
        std::string name;
        int paramCount;
        std::vector<std::string> paramNames;
        std::string returnType;
        bool typeInferred;
    };
    
    struct IfContext {
        size_t jumpIfFalseIndex;
        size_t jumpIfFalseTarget;
        std::optional<size_t> jumpOverElseIndex;
        size_t jumpOverElseTarget;
    };
    
    // === Piles de gestion complexes ===
    std::stack<int> listSizeStack;                    // Taille listes #60-#62
    std::vector<int> rulesStack;                      // Règles actives #63-#64
    std::stack<std::string> typeStack;                // Inférence types
    std::stack<std::string> varNameStack;             // Variables temp
    std::stack<std::string> callStack;                // Noms fonctions
    std::vector<size_t> patchStack;                   // Sauts while #110-#112
    std::vector<IfContext> ifStack;                   // IF/ELSE #100-#104
    
    void applyAction(int actionId, const string& value);  // 130+ actions
    void emit(OpCode opcode, const string& operand = "");
};
```


### 3. Runtime Multi-Types

```cpp
// === Système de valeurs polymorphes ===
struct Value : public std::variant<int, double, bool, std::string, List> {
    using variant::variant;
};
using List = std::shared_ptr<std::vector<Value>>;

// === Frame de fonction complète ===
struct Frame {
    std::unordered_map<std::string, Value> localVars;
    std::unordered_map<std::string, Value> params;
    std::vector<std::string> paramNames;
    size_t returnAddress;
};

class Runtime {
    stack<Value> stack;
    map<string, Value> variables;
    stack<Frame> callStack;
    
    // === Méthodes spécialisées par opcode ===
    void executeAdd();                     // Action #30
    void executeCall();                    // Action #125
    void executeMakeList(int size);        // Action #62
    void executeJumpIfFalse();             // Actions #100, #111
    void executeFuncDecl();                // Action #120
    void executeListGet();                 // Action #64 (accès)
    void executeListSet();                 // Action #64 (assignation)
};
```



##  Flux de Données Détaillé

> **[ UML 3 todo]** : Diagramme de séquence montrant l'interaction temporelle MetaLexer→MetaParser→ASTBuilder→Parser→PCodegen→Runtime

### Phase 1 : Méta-Compilation avec Actions
```
fichier.gram → MetaLexer → meta::Token[action] → MetaParser → ASTBuilder → Node AST
                                    ↓
                            applyAction(code, token) pour chaque action
```

### Phase 2 : Compilation Dirigée par Actions  
```
fichier.mylang → Lexer → compilo::Token[] → Parser(Node AST) → PCodegen
                                                    ↓
                                        applyAction(actionId, value) → Instruction[]
```

**Exemple concret avec actions :**
```
Code: if (x < 5) { y = 10; }
Tokens: [if] [(] [x] [<] [5] [)] [{] [y] [=] [10] [}]
Actions: #3  #44 #20 #100   #4   #20  #2  #101 #104
P-Code: LOAD x → PUSH 5 → LT → JUMP_IF_FALSE 7 → STORE y → PUSH 10 → STORE y
```

### Phase 3 : Exécution Multi-Types
```
Instruction[] → Runtime → {stack<Value>, callStack<Frame>, variables<Value>}
```

##  Caractéristiques Techniques

### 1. **Système d'Actions Intégrées **
Les actions sémantiques ne sont pas dans un fichier séparé mais **codées directement dans les grammaires**. l'implémentation gère :

**Variables et Types :**
- Actions #1-#4 : Déclaration, assignation, chargement
- Actions #20-#23 : Littéraux (nombres, strings, booléens)

**Expressions :**
- Actions #30-#34 : Arithmétique (ADD, SUB, MUL, DIV, MOD)
- Actions #40-#47 : Logique et comparaisons (OR, AND, EQ, NEQ, LT, GT, LE, GE)

**Structures de Contrôle :**
- Actions #100-#104 : IF/ELSE avec gestion complexe `IfContext`
- Actions #110-#112 : WHILE avec `patchStack` pour les sauts

**Listes/Tableaux :**
- Actions #60-#66 : Création, accès, assignation, opérations (size, makeEmpty)

**Fonctions :**
- Actions #120-#126 : Déclaration, paramètres, appels, return

**I/O :**
- Actions #90-#91 : Print avec/sans newline, gestion types

### 2. **Gestion État Complexe dans PCodegen**
PCodegen maintient plusieurs piles simultanément :
```cpp
std::stack<int> listSizeStack;        // Pour #60-#62 (listes)
std::vector<int> rulesStack;          // Pour #63-#64 (contexte)
std::vector<size_t> patchStack;       // Pour #110-#112 (while)
std::vector<IfContext> ifStack;       // Pour #100-#104 (if/else)
```

### 3. **Parser Récursif Universel**
Un seul algorithme de parsing pour toutes les grammaires avec support des structures complexes de la grammaire (répétitions, optionnels, groupements).

### 4. **Support Complet des Fonctions avec Frames**
```cpp
// Actions #120-#125 génèrent :
DECLARE_PARAM "n"           // #121
FUNC_DECL "factorial"       // #122
PUSH_FRAME                  // #122
// ... code fonction
RETURN                      // #124
POP_FRAME                   // #123
FUNC_END                    // #123
```

##  Diagramme UML - Composants Runtime 



```
┌─────────────────────────────────────────────────────────────┐
│                        Runtime                              │
├─────────────────────────────────────────────────────────────┤
│ - stack: Stack<Value>                                       │
│ - variables: Map<string, Value>                             │
│ - callStack: Stack<Frame>                                   │
│ - functions: Map<string, size_t>                            │
│ - pc: size_t                                                │
├─────────────────────────────────────────────────────────────┤
│ + execute(instructions): bool                               │
│ + executeLoad(name): void               // Action #3        │
│ + executeAdd(): void                    // Action #30       │
│ + executeCall(funcName): void           // Action #125      │
│ + executeMakeList(size): void           // Action #62       │
│ + executeJumpIfFalse(): void            // Actions #100,#111│
│ + executeListGet(): void                // Action #64       │
│ + executeListSet(): void                // Action #64       │
│ + executeFuncDecl(): void               // Action #120      │
│ + executePrintString(): void            // Action #91       │
└─────────────────────────────────────────────────────────────┘
                    │
                    │ uses
                    ▼
┌─────────────────────────────────────────────────────────────┐
│                       Value                                 │
├─────────────────────────────────────────────────────────────┤
│ variant<int, double, bool, string, List>                    │
│ + valueToString(): string                                   │
│ + valueToBool(): bool                                       │
│ + valueToNumber(): double                                   │
└─────────────────────────────────────────────────────────────┘
                    │
                    │ composition
                    ▼
┌─────────────────────────────────────────────────────────────┐
│                       Frame                                 │
├─────────────────────────────────────────────────────────────┤
│ - localVars: Map<string, Value>                             │
│ - params: Map<string, Value>                                │
│ - paramNames: vector<string>                                │
│ - returnAddress: size_t                                     │
├─────────────────────────────────────────────────────────────┤
│ + Frame(retAddr): constructor                               │
└─────────────────────────────────────────────────────────────┘
```

##  Points d'Extension

### Ajout de Nouveaux Opcodes
1. **Enum** : Ajouter dans `OpCode`
2. **String conversion** : Ajouter dans `opCodeToString()`
3. **Exécution** : Implémenter `executeNewOpcode()` dans `Runtime`
4. **Génération** : Ajouter l'action dans `PCodegen::applyAction()`

### Support de Nouvelles Constructions
1. **Lexer** : Ajouter le `TokenType`
2. **Grammaire** : Définir les règles avec actions
3. **Actions** : Implémenter la génération P-Code
4. **Runtime** : Supporter l'exécution si nécessaire

##  Limitations et Performance

### **Contraintes Architecturales**
- **Grammaires LL(1) uniquement** : Pas de récursion à gauche, ambiguïtés interdites
- **Fail-fast** : Aucune récupération d'erreur, arrêt à la première erreur
- **Actions liées au parsing** : Modification d'une action nécessite de toucher plusieurs fichiers

### **Complexité et Performance**
| Étape        | Complexité réaliste | Justification |
| ------------ | ----------------- | ------------- |
| Méta-parsing | O(g)              | Chaque token de la grammaire est lu une seule fois par le méta-parser LL(1), comme pour le parsing classique. |
| Parsing      | O(n)              | Chaque token du code source est analysé exactement une fois par le parser LL(1). |
| Exécution    | O(i)              | Le runtime exécute séquentiellement chaque instruction P-Code. Les boucles et jumps augmentent le nombre d’instructions exécutées, mais la complexité reste proportionnelle au flux réel d’exécution. |


### **Extensibilité**
- **Ajout simple** : Nouveaux opcodes, nouvelles actions
- **Coordination requise** : Lexer → Parser → PCodegen → Runtime
- **Impact limité** : Changements additifs sans casser l'existant

---

##  Annexes Détaillées

Pour les détails complets d'implémentation, consulter :
- **Opcodes complets** : `include/compilo/opcode.hpp` (30+ instructions)
- **Actions sémantiques** : `PCodegen::applyAction()` 
- **Tokens étendus** : `include/compilo/token.hpp` et `include/meta/token.hpp`
- **Runtime spécialisé** : `include/compilo/runtime.hpp` (+20 méthodes d'exécution)


