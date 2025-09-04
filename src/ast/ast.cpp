#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cctype>
#include "ast/ast.h"
#include <map>
using namespace std;

Node::Node(const Atom& a)
    : kind(NodeKind::ATOM), atom(a), left(nullptr), right(nullptr), sub(nullptr) {}

Node::Node(NodeKind kind, std::shared_ptr<Node> l, std::shared_ptr<Node> r)
    : kind(kind), left(l), right(r), sub(nullptr) {}

Node::Node(NodeKind kind, std::shared_ptr<Node> s)
    : kind(kind), left(nullptr), right(nullptr), sub(s) {}

string arbreToString(const Node* node) {
    if (!node) {
        return "_";
    }

    switch (node->kind) {
        case NodeKind::ATOM: {
            if(node->atom.type == AtomType::Terminal) {
                return "'" + node->atom.code + "'";
            } else {
                return node->atom.code;
            }
        }
        case NodeKind::CONCATENATION: {
            string left = arbreToString(node->left.get());
            string right = arbreToString(node->right.get());
            return left + "." + right;
        }
        case NodeKind::UNION_NODE: {
            string left = arbreToString(node->left.get());
            string right = arbreToString(node->right.get());
            return left + "+" + right;
        }
        case NodeKind::STAR: {
            return "[" + arbreToString(node->sub.get()) + "]";
        }
        case NodeKind::UN: {
            return "(/" + arbreToString(node->sub.get()) + "/)";
        }
    }
    return "";
}

void printAST(const std::shared_ptr<Node>& node, int indent)  {
    if (!node) return;
    for (int i = 0; i < indent; ++i) std::cout << "  ";

    switch (node->kind) {
        case NodeKind::ATOM:
            std::cout << "ATOM: " 
                      << (node->atom.type == AtomType::Terminal ? "'" + node->atom.code + "'" : node->atom.code)
                      << " (action=" << node->atom.action << ")\n";
            break;

        case NodeKind::CONCATENATION:
            std::cout << "CONCATENATION\n";
            printAST(node->left, indent + 1);
            printAST(node->right, indent + 1);
            break;

        case NodeKind::UNION_NODE:
            std::cout << "UNION\n";
            printAST(node->left, indent + 1);
            printAST(node->right, indent + 1);
            break;

        case NodeKind::STAR:
            std::cout << "STAR\n";
            printAST(node->sub, indent + 1);
            break;

        case NodeKind::UN:
            std::cout << "OPTIONAL\n";
            printAST(node->sub, indent + 1);
            break;
    }
}


/*méta-grammaire hard-codé*/
shared_ptr<Node> S = make_shared<Node>(NodeKind::CONCATENATION,
    make_shared<Node>(NodeKind::STAR,
        make_shared<Node>(NodeKind::CONCATENATION,
            make_shared<Node>(NodeKind::CONCATENATION,
                make_shared<Node>(NodeKind::CONCATENATION,
                    make_shared<Node>(Atom{"N", 0, AtomType::Non_Terminal}),
                    make_shared<Node>(Atom{"->", 0, AtomType::Terminal}) 
                ),
                make_shared<Node>(Atom{"E", 0, AtomType::Non_Terminal})
            ),
            make_shared<Node>(Atom{",", 1, AtomType::Terminal})
        )
    ),
    make_shared<Node>(Atom{";", 0, AtomType::Terminal})
);

shared_ptr<Node> N = make_shared<Node>(Atom{"IDNTER", 2, AtomType::Terminal});

shared_ptr<Node> E = make_shared<Node>(NodeKind::CONCATENATION,
    make_shared<Node>(Atom{"T", 0, AtomType::Non_Terminal}), 
    make_shared<Node>(NodeKind::STAR,
        make_shared<Node>(NodeKind::CONCATENATION,
            make_shared<Node>(Atom{"+", 0, AtomType::Terminal}),
            make_shared<Node>(Atom{"T", 3, AtomType::Non_Terminal}))));

shared_ptr<Node> T = make_shared<Node>(NodeKind::CONCATENATION,
    make_shared<Node>(Atom{"F", 0, AtomType::Non_Terminal}),
    make_shared<Node>(NodeKind::STAR,
        make_shared<Node>(NodeKind::CONCATENATION,
            make_shared<Node>(Atom{".", 0, AtomType::Terminal}),
            make_shared<Node>(Atom{"F", 4, AtomType::Non_Terminal}))));

shared_ptr<Node> F = make_shared<Node>(NodeKind::UNION_NODE, 
    make_shared<Node>(Atom{"IDNTER", 5, AtomType::Terminal}), 
    make_shared<Node>(NodeKind::UNION_NODE, 
        make_shared<Node>(Atom{"ELTER", 5, AtomType::Terminal}), 
        make_shared<Node>(NodeKind::UNION_NODE,
            make_shared<Node>(NodeKind::CONCATENATION, 
                make_shared<Node>(NodeKind::CONCATENATION, 
                    make_shared<Node>(Atom{"(", 0, AtomType::Terminal}), 
                    make_shared<Node>(Atom{"E", 0, AtomType::Non_Terminal})), 
                make_shared<Node>(Atom{")", 8, AtomType::Terminal})),
            make_shared<Node>(NodeKind::UNION_NODE, 
                make_shared<Node>(NodeKind::CONCATENATION, 
                    make_shared<Node>(NodeKind::CONCATENATION,
                        make_shared<Node>(Atom{"[", 0, AtomType::Terminal}), 
                        make_shared<Node>(Atom{"E", 0, AtomType::Non_Terminal})),
                    make_shared<Node>(Atom{"]", 6, AtomType::Terminal})),
                make_shared<Node>(NodeKind::CONCATENATION, 
                    make_shared<Node>(NodeKind::CONCATENATION, 
                        make_shared<Node>(Atom{"(/", 0, AtomType::Terminal}), 
                        make_shared<Node>(Atom{"E", 0, AtomType::Non_Terminal})),
                    make_shared<Node>(Atom{"/)", 7, AtomType::Terminal}))))));

unordered_map<string, shared_ptr<Node>> metaGrammarRules = {
    {"S", S},
    {"N", N},
    {"E", E},
    {"T", T},
    {"F", F}
};
