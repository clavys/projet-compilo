# Référence Complète GPL

## Variables et Types

### Déclaration et Assignation
```
var x = 10;
var name = "hello world";
var flag = true;
```

### Types Supportés
```
var number = 42;
var decimal = 3.14;
var text = "chaîne de caractères";
var boolean_true = true;
var boolean_false = false;
```

### Assignation
```
var x = 0;
x = 100;
x = x + 50;
```

## Expressions Arithmétiques

### Opérateurs de Base
```
var a = 10;
var b = 3;
var addition = a + b;
var soustraction = a - b;
var multiplication = a * b;
var division = a / b;
var modulo = a % b;
```

### Expressions Unaires
```
var x = 5;
var negatif = -x;
var positif = x;
```

### Expressions Complexes
```
var resultat = (10 + 5) * 3 - 2 / 4;
var calcul = ((a + b) * c) / (d - e);
```

## Expressions Logiques

### Opérateurs de Comparaison
```
var x = 10;
var y = 20;
var egal = x == y;
var different = x != y;
var inferieur = x < y;
var superieur = x > y;
var inf_egal = x <= y;
var sup_egal = x >= y;
```

### Opérateurs Logiques
```
var a = true;
var b = false;
var et_logique = a && b;
var ou_logique = a || b;
var non_logique = !a;
```

### Expressions Complexes
```
var condition = (x > 5) && (y < 20) || (z == 0);
var test = !(a && b) || (c != d);
```

## Structures de Contrôle

### Instruction IF
```
var x = 10;
if (x > 5) {
    print "x est supérieur à 5";
};
```

### Instruction IF/ELSE
```
var age = 18;
if (age >= 18) {
    println "Majeur";
} else {
    println "Mineur";
};
```

### IF/ELSE Imbriqués
```
var note = 85;
if (note >= 90) {
    println "Excellent";
} else {
    if (note >= 70) {
        println "Bien";
    } else {
        println "Peut mieux faire";
    };
};
```

### Boucle WHILE
```
var i = 0;
while (i < 5) {
    println i;
    i = i + 1;
};
```

### Boucle WHILE avec Condition Complexe
```
var x = 10;
var y = 20;
while ((x < y) && (x > 0)) {
    x = x - 1;
    print x;
};
```

## Fonctions

### Déclaration de Fonction Simple
```
function saluer() {
    println "Bonjour!";
};
```


### Fonction avec Plusieurs Paramètres
```
function calculer(x, y, z) {
    var somme = x + y + z;
    var moyenne = somme / 3;
    return moyenne;
};
```


### Appel de Fonction
```
var somme = additionner(10, 20);
var fact = factorielle(5);
saluer();
```

## Listes et Tableaux

### Création de Listes
```
var nombres = [1, 2, 3, 4, 5];
var noms = ["Alice", "Bob", "Charlie"];
```

### Liste Vide
```
var vide = makeEmpty();
```

### Accès aux Éléments
```
var liste = [10, 20, 30];
var premier = liste[0];
var deuxieme = liste[1];
var dernier = liste[2];
```

### Modification d'Éléments
```
var arr = [1, 2, 3];
arr[0] = 100;
arr[1] = arr[0] + 50;
```

### Opérations sur les Listes
```
var ma_liste = [1, 2, 3, 4];
var taille = size(ma_liste);
println taille;
```

### Listes dans des Expressions
```
var liste1 = [1, 2, 3];
var liste2 = [4, 5, 6];
var element = liste1[0] + liste2[1];
```

## Entrées/Sorties

### Affichage Simple
```
print "Hello";
print 42;
print true;
```

### Affichage avec Nouvelle Ligne
```
println "Première ligne";
println "Deuxième ligne";
```

### Affichage de Variables
```
var nom = "Alice";
var age = 25;
print nom;
println age;
```

### Affichage d'Expressions
```
var x = 10;
var y = 20;
println x + y;
print (x * y) / 2;
```

## Programmes Complets

### Exemple 1: Calcul de Moyenne
```
function moyenne(a, b, c) {
    var somme = a + b + c;
    return somme / 3;
};

var note1 = 15;
var note2 = 17;
var note3 = 12;
var moy = moyenne(note1, note2, note3);
println "Moyenne: ";
println moy;
```

### Exemple 2: Recherche dans une Liste
```
function chercher(liste, valeur) {
    var i = 0;
    var taille = size(liste);
    while (i < taille) {
        if (liste[i] == valeur) {
            return i;
        };
        i = i + 1;
    };
    return -1;
};

var nombres = [10, 20, 30, 40, 50];
var position = chercher(nombres, 30);
if (position >= 0) {
    println "Trouvé à la position: ";
    println position;
} else {
    println "Non trouvé";
};
```

### Exemple 3: Tri par Insertion
```
function tri_insertion(arr) {
    var i = 1;
    var taille = size(arr);
    while (i < taille) {
        var cle = arr[i];
        var j = i - 1;
        while ((j >= 0) && (arr[j] > cle)) {
            arr[j + 1] = arr[j];
            j = j - 1;
        };
        arr[j + 1] = cle;
        i = i + 1;
    };
};

var liste = [64, 34, 25, 12, 22, 11, 90];
println "Avant tri:";
var k = 0;
while (k < size(liste)) {
    print liste[k];
    print " ";
    k = k + 1;
};
println "";

tri_insertion(liste);

println "Après tri:";
k = 0;
while (k < size(liste)) {
    print liste[k];
    print " ";
    k = k + 1;
};
println "";
```

### Exemple 4: Calculatrice Simple
```
function calculer(operation, a, b) {
    if (operation == 1) {
        return a + b;
    } else {
        if (operation == 2) {
            return a - b;
        } else {
            if (operation == 3) {
                return a * b;
            } else {
                if (operation == 4) {
                    return a / b;
                } else {
                    return 0;
                };
            };
        };
    };
};

var x = 10;
var y = 3;
var addition = calculer(1, x, y);
var soustraction = calculer(2, x, y);
var multiplication = calculer(3, x, y);
var division = calculer(4, x, y);

println "Addition: ";
println addition;
println "Soustraction: ";
println soustraction;
println "Multiplication: ";
println multiplication;
println "Division: ";
println division;
```


## Règles de Syntaxe

### Points-Virgules Obligatoires
Chaque instruction doit se terminer par un point-virgule, y compris les blocs :
```
var x = 10;                    // Variable
if (condition) { ... };        // Bloc if
while (condition) { ... };     // Bloc while  
function nom() { ... };        // Fonction
```

### Commentaires
Les commentaires ne sont pas supportés dans cette version du langage.

### Portée des Variables
- Variables globales : déclarées au niveau principal
- Variables locales : déclarées dans les fonctions
- Les paramètres de fonction sont locaux à la fonction

### Conventions de Nommage
- Variables : `snake_case` recommandé
- Fonctions : `snake_case` recommandé
- Pas de mots-clés réservés comme noms de variables

## Limitations

### Déclarations

- Assignation obligatoire : var x; sans valeur n'est pas autorisé
- Toute variable doit avoir une valeur initiale

### Types
- Pas de déclaration de type explicite
- Inférence de type automatique
- Types supportés : number, string, boolean, list

### Structures de Données
- Listes static seulement (pas d'objets ou de structures)

### Contrôle de Flux
- Pas de boucle `for`
- Pas de `switch/case`
- Pas de `break/continue`

### Fonctions
- Pas de surcharge de fonctions
- Pas de fonctions anonymes
- Récursion pas supportée
