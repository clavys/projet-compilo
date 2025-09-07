# Guide des Grammaires - Format et Actions

## Introduction

Ce guide explique le système de grammaires à deux niveaux de votre compilateur. Chaque niveau a son rôle spécifique dans l'architecture de compilation :

| Niveau | Format | Exemple | Usage |
|--------|--------|---------|--------|
| **Méta-grammaire** | Hardcodée C++ + g0.gram | `NodeKind::UNION_NODE` | Système de parsing universel |
| **Grammaires GPL** | `.gram` utilisateur | `IfStmt → 'if' . '(' . Expr . ')' . Block` | Définition des langages cibles |

## Architecture du Système de Grammaires

```
Méta-grammaire hardcodée (C++) ←→ g0.gram (même système, format textuel)
                    ↓
            Parse les grammaires GPL (.gram)
                    ↓
            Génère un AST exploitable par le compilateur
```

## Niveau 1 : Méta-Grammaire (Système Fondamental)

### Nature de la Méta-Grammaire

La **méta-grammaire** est le système hardcodé dans votre classe AST (`ast/ast.h`) qui permet au méta-compilateur de parser **n'importe quelle grammaire**. C'est le niveau le plus fondamental qui définit les règles universelles de construction grammaticale.

```cpp
// Dans ast/ast.h - La méta-grammaire hardcodée
extern std::unordered_map<std::string, std::shared_ptr<Node>> metaGrammarRules;

enum class NodeKind {
    ATOM,           // Terminal ou non-terminal de base
    CONCATENATION,  // Séquence d'éléments  
    UNION_NODE,     // Alternatives
    STAR,           // Répétition
    UN,             // Optionnel
    GROUP           // Groupement
};

struct Node {
    NodeKind kind;
    Atom atom;                    // Si ATOM
    std::shared_ptr<Node> left;   // CONCATENATION, UNION_NODE
    std::shared_ptr<Node> right;
    std::shared_ptr<Node> sub;    // STAR, UN, GROUP
};
```

### Concepts Universels de la Méta-Grammaire

La méta-grammaire définit ces structures fondamentales pour toute grammaire :

- **ATOM** : Élément atomique (terminal/non-terminal)
- **CONCATENATION** : Séquence obligatoire d'éléments
- **UNION_NODE** : Choix entre alternatives
- **STAR** : Répétition (zéro ou plus)
- **UN** : Optionalité (zéro ou un)
- **GROUP** : Groupement pour la priorité

### Test d'Auto-Cohérence : g0.gram

Le fichier `g0.gram` contient la **représentation textuelle** de cette méta-grammaire hardcodée. C'est la même méta-grammaire, mais écrite selon son propre format :

```
S → [N."→".E.","].";" ,
N → "IDNTER" ,
E → T.["+".T] ,
T → F.[".".F] ,
F → "IDNTER" + "ELTER" + "(".E.")" + "[".E."]" + "(/".E."/)" ,
;
```

**Test fondamental** : Si le méta-compilateur peut parser `g0.gram` avec succès, cela prouve qu'il reconnaît correctement sa propre syntaxe. C'est un test d'**auto-cohérence**.

**Relation importante :**
- **AST hardcodé** (C++) ←→ **g0.gram** (textuel) = Même méta-grammaire, deux représentations
- **g0.gram** sert de validation du système, pas de niveau intermédiaire

### Règles de la Méta-Grammaire (définies dans g0.gram)

#### **Règle S (Start) - Structure Globale**
```
S → [N."→".E.","].";"
```
Une grammaire = suite de productions + point-virgule final

#### **Règle N (Non-terminal) - Noms de Règles**
```
N → "IDNTER"
```
Identifie le côté gauche d'une production (nom de règle)

#### **Règle E (Expression) - Alternatives**
```
E → T.["+".T]
```
Une expression = terme + zéro ou plusieurs alternatives séparées par `+`

#### **Règle T (Terme) - Séquences**
```
T → F.[".".F]
```
Un terme = facteur + zéro ou plusieurs facteurs en séquence

#### **Règle F (Facteur) - Éléments Atomiques**
```
F → "IDNTER" + "ELTER" + "(".E.")" + "[".E."]" + "(/".E."/)"
```

Les 5 types de facteurs :
1. **"IDNTER"** : Référence à une autre règle
2. **"ELTER"** : Terminal du langage
3. **"(" E ")"** : Groupement 
4. **"[" E "]"** : Répétition (zéro ou plus)
5. **"(/" E "/)"** : Optionnel (zéro ou un)

### Notation et Symboles

| Symbole | Signification | Équivalent EBNF |
|---------|---------------|-----------------|
| `"..."` | Terminal littéral | `"..."` |
| `[ X ]` | Répétition (zéro ou plus) | `X*` |
| `(/ X /)` | Optionnel (zéro ou un) | `X?` |
| `.` | Séquence/concaténation | espace |
| `IDNTER` | Non-terminal | Nom de règle |
| `ELTER` | Terminal | Token du langage |

 **Attention : Ambiguïté du symbole `+`**

Le symbole `+` a **deux significations différentes** selon le contexte :

| Contexte | Signification | Exemple |
|----------|---------------|---------|
| **Dans les règles E et T** | Terminal littéral "+" | `T.["+".T]` = terme, puis "+", puis terme |
| **Dans la règle F** | Alternative/union | `"IDNTER" + "ELTER"` = soit IDNTER soit ELTER |

**Exemple concret :**
```
AddExpr → Term . [ '+' . Term ]     # Le '+' ici est le terminal "+"
Literal → 'ID' + 'NUMBER'           # Le '+' ici signifie "ou" (alternative)
```

## Niveau 2 : Grammaires GPL (Langages Utilisateur)

### Syntaxe des Grammaires GPL

Toute grammaire GPL doit respecter la syntaxe définie par la méta-grammaire :

```
<rule_name> → <production1> + <production2> + <production3> ,
<another_rule> → <terminal> . <non_terminal> ,
;
```

### Éléments de Base

#### 1. **Structure Obligatoire**
```
Program → [ Statement . ';' ] . 'EOF' ,
Statement → PrintStmt + VarDecl + Assignment + FuncDecl ,
;
```

#### 2. **Terminaux et Non-Terminaux**
- **Non-terminaux** : Noms de règles (PascalCase recommandé)
- **Terminaux** : Entre guillemets simples `'var'`, `'if'`, `'function'`
- **Tokens spéciaux** : `'ID'`, `'NUMBER'`, `'STRING'`, `'IDFONCTION'`, `'IDLIST'`

#### 3. **Actions Sémantiques**
```
VarDecl → 'var' . 'ID'#1 . '=' . ExprStmt#2 ,
```
L'action `#1` sera exécutée quand le token `'ID'` est reconnu et L'action `#2` sera exécutée quand le parseur recursif remonte dans l'AST et valide ExprStmt,

## Système d'Actions Sémantiques

### Actions Variables (1-10)
```
'ID'#1    # Déclaration variable → declareVariable()
'ID'#2    # Assignation → storeVariable()
'ID'#3    # Chargement variable → loadVariable()
'ID'#4    # Nom variable pour assignment → pushVarName()
```

### Actions Littéraux (20-29)
```
'NUMBER'#20    # Nombre → PUSH number
'STRING'#21    # Chaîne → PUSH "string"
'true'#22      # Booléen vrai → PUSH_BOOL 1
'false'#23     # Booléen faux → PUSH_BOOL 0
```

### Actions Opérateurs (30-49)
```
# Arithmétique
'+'#30, '-'#31, '*'#32, '/'#33, '%'#34

# Logique  
'||'#40, '&&'#41

# Comparaisons
'=='#42, '!='#43, '<'#44, '>'#45, '<='#46, '>='#47
```

### Actions Unaires (10-19)
```
'!'#10    # NOT logique → emit(NOT)
'-'#11    # NEG arithmétique → emit(NEG)
```

### Actions Listes (60-69)
```
'['#60           # Début création liste → listSizeStack.push(0)
LogicalOr#61     # Chaque élément → increment taille
']'#62           # Fin création → emit(MAKE_LIST, size)
'['#63           # Contexte assignation → rulesStack.push(63)
']'#64           # Accès/assignation → LIST_GET ou LIST_SET
'size'#65        # Taille liste → emit(LIST_LEN)
'makeEmpty'#66   # Liste vide → emit(MAKE_EMPTY_LIST)
```

### Actions I/O (90-99)
```
'print'#90       # Print sans newline → emit(PRINT, "0")
'println'#91     # Print avec newline → emit(PRINT, "1")
```

### Actions Structures de Contrôle (100-119)
```
# IF/ELSE - Gestion complexe avec IfContext
')'#100          # Après condition → emit(JUMP_IF_FALSE)
'}'#101          # Fin bloc IF → marquer target
'else'#102       # Début ELSE → emit(JUMP)
'}'#103          # Fin bloc ELSE → marquer target
IfStmt#104       # Fin complète → patch tous les sauts

# WHILE - Gestion avec patchStack
'while'#110      # Début boucle → marquer position loop
')'#111          # Après condition → emit(JUMP_IF_FALSE)
Block#112        # Fin boucle → emit(JUMP loop) + patch
```

### Actions Fonctions (120-129)
```
'IDFONCTION'#120     # Nom fonction → functionTable[name]
'ID'#121             # Paramètre → emit(DECLARE_PARAM)
'{'#122              # Début corps → emit(PUSH_FRAME)
FuncBlock#123        # Fin fonction → emit(POP_FRAME, FUNC_END)
'return'#124         # Return avec valeur → emit(RETURN)
')'#125              # Fin appel → emit(ARG_COUNT, CALL)
'IDFONCTION'#126     # Nom pour appel → callStack.push(name)
```

## Grammaire GPL Complète - Votre Système

```
Program → [ Statement . ';' ] . 'EOF' ,

Statement → PrintStmt + VarDecl + Assignment + FuncDecl + IfStmt#104 + WhileStmt + ExprStmt ,

ExprStmt → CallExpr#125 + LogicalOr ,

VarDecl → 'var' . 'ID'#1 . '=' . ExprStmt#2 ,

Assignment → 'ID'#4 . '=' . ExprStmt#2 ,

Primary → ( '(' . LogicalOr . ')' ) + Literal ,

CallExpr → 'IDFONCTION'#126 . [ '(' . [ ExprStmt . [ ',' . ExprStmt ] ] . ')' ] ,

LogicalOr → LogicalAnd . [ '||' . LogicalAnd#40 ] ,

LogicalAnd → Equality . [ '&&' . Equality#41 ] ,

Equality → Relational . [ ( '==' . Relational#42 ) + ( '!=' . Relational#43 ) ] ,

Relational → AddExpr . [ ( '<' . AddExpr#44 ) + ( '>' . AddExpr#45 ) + 
                          ( '<=' . AddExpr#46 ) + ( '>=' . AddExpr#47 ) ] ,

AddExpr → MulExpr . [ ( '+' . MulExpr#30 ) + ( '-' . MulExpr#31 ) ] ,

MulExpr → Unary . [ ( '*' . Unary#32 ) + ( '/' . Unary#33) + ( '%' . Unary#34 ) ] ,

Unary → ( '!' . Primary#10 ) + ( '-' . Primary#11 ) + Primary ,

Literal → 'ID'#3 + ListStatement + ListOp#65 + NumberLiteral + StringLiteral + 
           Bool + ListLiteral + CallExpr#125 ,

NumberLiteral → 'NUMBER'#20 ,

StringLiteral → 'STRING'#21 ,

Bool → 'true'#22 + 'false'#23 ,

IfStmt → 'if' . '(' . LogicalOr#100 . ')' . Block#101 . (/ Else /) ,

Else → 'else'#102 . Block#103 ,

WhileStmt → 'while'#110 . '(' . LogicalOr#111 . ')' . Block#112 ,

Block → '{' . [ Statement . ';' ] . '}' ,

FuncDecl → 'function' . 'IDFONCTION'#120 . '(' . [ 'ID'#121 . [ ',' . 'ID'#121 ] ] . ')' . FuncBlock#123 ,

FuncBlock → '{'#122 . [ Statement . ';' ] . (/ ReturnStmt /) .'}' ,

ReturnStmt → 'return' . LogicalOr#124 . ';' ,

PrintStmt → ('print' . LogicalOr#90 ) + ('println' . LogicalOr#91 ) ,

ListLiteral → ( 'makeEmpty' . '(' . ExprStmt#66 . ')' ) + 
               ( '['#60 . [ LogicalOr#61 . [ ',' . LogicalOr#61 ] ] . ']'#62 ) ,

ListStatement → 'IDLIST'#3 . (/ ListIndex#64 /) ,

ListOp → 'size' . '(' . Literal . ')' ,

ListIndex → '[' . LogicalOr . ']' . (/ '=' . LogicalOr#63 /) ,

;
```

## Exemples Pratiques

### Déclaration de Variable
```
Grammaire: VarDecl → 'var' . 'ID'#1 . '=' . ExprStmt#2 ,
Code:      var x = 10;
Actions:   #1 (déclare x) → #20 (push 10) → #2 (store x)
P-Code:    DECLARE_VAR x, PUSH 10, STORE x
```

### Structure IF/ELSE avec Gestion Complexe
```
Code:      if (x < 5) { y = 1; } else { y = 2; }
Actions:   #3→#20→#44→#100→#4→#20→#2→#101→#102→#4→#20→#2→#103→#104
P-Code:    LOAD x, PUSH 5, LT, JUMP_IF_FALSE 8, STORE y, PUSH 1, STORE y, 
           JUMP 11, STORE y, PUSH 2, STORE y
```

### Liste avec Actions Contextuelles
```
Code:      var arr = [1, 2, 3]; arr[0] = 5;
Actions:   #1→#60→#20→#61→#20→#61→#20→#61→#62→#2→#4→#20→#63→#20→#64
P-Code:    DECLARE_VAR arr, PUSH 1, PUSH 2, PUSH 3, MAKE_LIST 3, STORE arr,
           LOAD arr, PUSH 0, PUSH 5, LIST_SET
```

## Contraintes et Bonnes Pratiques

### Contraintes de la Méta-Grammaire
- **Syntaxe stricte** : Respect obligatoire des règles S, N, E, T, F
- **Séparateurs obligatoires** : `.` pour séquences, `+` pour alternatives, `,` après règles
- **Terminaison** : Point-virgule `;` obligatoire
- **LL(1) requis** : Pas de récursion à gauche, pas d'ambiguïté

### Transformation EBNF → Format GPL

| EBNF Standard | Format GPL |
|---------------|------------|
| `A ::= B C` | `A → B . C ,` |
| `A ::= B \| C` | `A → B + C ,` |
| `A ::= B*` | `A → [ B ] ,` |
| `A ::= B?` | `A → (/ B /) ,` |

### Erreurs Courantes

```
#  Syntaxe incorrecte
AddExpr -> AddExpr '+' MulExpr    # Manque '.' et ','
Statement -> if | while           # Manque guillemets

#  Syntaxe correcte  
AddExpr → MulExpr . [ '+' . MulExpr#30 ] ,
Statement → 'if' + 'while' ,
```

## Extension du Système

### Ajouter de Nouvelles Constructions

1. **Définir les tokens** dans le lexer compilo
2. **Ajouter la règle GPL** selon la méta-grammaire
3. **Implémenter l'action** dans `PCodegen::applyAction()`
4. **Tester** avec le méta-compilateur

