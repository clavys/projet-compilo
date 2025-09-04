#ifndef META_AST_H
#define META_AST_H

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Enumération pour différencier les types d'atomes
enum class AtomType { Terminal, Non_Terminal };

// Structure représentant un atome
struct Atom {
    std::string code;
    int action;
    AtomType type;
};

// Enumération pour identifier le type de noeud
enum class NodeKind {
    ATOM, CONCATENATION, UNION_NODE, STAR, UN, GROUP
};

// Structure de l'arbre syntaxique
struct Node {
    NodeKind kind;
    Atom atom; // Utilisé seulement si kind == ATOM
    std::shared_ptr<Node> left;   // Pour CONCATENATION et UNION_NODE
    std::shared_ptr<Node> right;  // Pour CONCATENATION et UNION_NODE
    std::shared_ptr<Node> sub;    // Pour STAR , UN et GROUP

    // Constructeur pour ATOM
    Node(const Atom& a);

    // Constructeur pour CONCATENATION et UNION_NODE
    Node(NodeKind kind, std::shared_ptr<Node> l, std::shared_ptr<Node> r);

    // Constructeur pour STAR , UN et GROUP
    Node(NodeKind kind, std::shared_ptr<Node> s);
};

// Fonction utilitaire pour convertir l'arbre en chaîne de caractères
std::string arbreToString(const Node* node);
void printAST(const std::shared_ptr<Node>& node, int indent = 0);


// Déclaration de la méta-grammaire
extern std::unordered_map<std::string, std::shared_ptr<Node>> metaGrammarRules;



#endif // META_AST_H
