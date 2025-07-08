#ifndef META_AST_H
#define META_AST_H

#include <iostream>
#include <memory>
#include <string>

// Enumération pour différencier les types d'atomes
enum class AtomType { Terminal, Non_Terminal };

// Structure représentant un atome
struct Atom {
    std::string code;
    int action;
    AtomType type;
};

// Enumération pour identifier le type de nœud
enum class NodeKind {
    ATOM, CONCATENATION, UNION_NODE, STAR, UN
};

// Structure de l'arbre syntaxique utilisant des shared_ptr
struct Node {
    NodeKind kind;
    Atom atom; // Utilisé seulement si kind == ATOM
    std::shared_ptr<Node> left;   // Pour CONCATENATION et UNION_NODE
    std::shared_ptr<Node> right;  // Pour CONCATENATION et UNION_NODE
    std::shared_ptr<Node> sub;    // Pour STAR et UN

    // Constructeur pour ATOM
    Node(const Atom& a);

    // Constructeur pour CONCATENATION et UNION_NODE
    Node(NodeKind kind, std::shared_ptr<Node> l, std::shared_ptr<Node> r);

    // Constructeur pour STAR et UN
    Node(NodeKind kind, std::shared_ptr<Node> s);
};

// Fonction utilitaire pour convertir l'arbre en chaîne de caractères
std::string arbreToString(const Node* node);

extern std::shared_ptr<Node> S;

#endif // META_AST_H
