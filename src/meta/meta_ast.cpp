#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cctype>
#include "meta/meta_ast.h"
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
                make_shared<Node>(Atom{")", 0, AtomType::Terminal})),
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
