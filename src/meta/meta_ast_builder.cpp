#include "meta/meta_ast_builder.hpp"

void ASTBuilder::applyAction(int action, const meta::Token& token) {
    std::shared_ptr<Node> t1 = nullptr;
    std::shared_ptr<Node> t2 = nullptr;

    switch (action) {
        case 0:
            break;

        case 1: // Lier une regle à un arbre
            logger.debug("action 1 : liaison dans l'AST");
            if (!stack.isEmpty()) { t1 = stack.top(); stack.pop(); }
            if (!stack.isEmpty()) { t2 = stack.top(); stack.pop(); }
            if (t1 && t2) {
                if (!t2->atom.code.empty()) {
                    if (astMap.count(t2->atom.code)) {
                        logger.error("Règle dupliquée : " + t2->atom.code);
                        throw std::runtime_error("Erreur : règle dupliquée '" + t2->atom.code + "'");
                    }
                    astMap[t2->atom.code] = t1;
                    if (startRule.empty()) startRule = t2->atom.code;
                } else {
                    logger.warn("Atom.code vide dans action 1");
                }
            }
            break;

        case 2: // Empile un ATOM
            logger.debug("action 2 : empile un noeud Atom");
            stack.push(std::make_shared<Node>(Atom{
                token.value,
                token.action,
                token.type == meta::TokenType::Terminal ? AtomType::Terminal : AtomType::Non_Terminal
            }));
            break;

        case 3: // UNION_NODE
            logger.debug("action 3 : construit UNION_NODE");
            if (!stack.isEmpty()) { t1 = stack.top(); stack.pop(); }
            if (!stack.isEmpty()) { t2 = stack.top(); stack.pop(); }
            if (t1 && t2) {
                stack.push(std::make_shared<Node>(NodeKind::UNION_NODE, t2, t1));
            }
            break;

        case 4: // CONCATENATION
            logger.debug("action 4 : construit CONCATENATION");
            if (!stack.isEmpty()) { t1 = stack.top(); stack.pop(); }
            if (!stack.isEmpty()) { t2 = stack.top(); stack.pop(); }
            if (t1 && t2) {
                stack.push(std::make_shared<Node>(NodeKind::CONCATENATION, t2, t1));
            }
            break;

        case 5: // Push Terminal ou Non-terminal
            logger.debug("action 5 : push terminal ou non-terminal");
            stack.push(std::make_shared<Node>(Atom{
                token.value,
                token.action,
                token.type == meta::TokenType::Terminal ? AtomType::Terminal : AtomType::Non_Terminal
            }));
            break;

        case 6: // STAR
            logger.debug("action 6 : étoile (STAR)");
            if (!stack.isEmpty()) { t1 = stack.top(); stack.pop(); }
            if (t1) stack.push(std::make_shared<Node>(NodeKind::STAR, t1));
            break;

        case 7: // UN (optionnel)
            logger.debug("action 7 : UN (optionnel)");
            if (!stack.isEmpty()) { t1 = stack.top(); stack.pop(); }
            if (t1) stack.push(std::make_shared<Node>(NodeKind::UN, t1));
            break;

        case 8: // GROUP
            logger.debug("action 8 : groupe (GROUP)");
            if (!stack.isEmpty()) { t1 = stack.top(); stack.pop(); }
            if (t1) stack.push(std::make_shared<Node>(NodeKind::GROUP, t1));
            break;    

        default:
            std::cerr << "Erreur: code d'action inconnu : " << action << std::endl;
            break;
    }
}

void ASTBuilder::printAST(const std::shared_ptr<Node>& node, int indent) const {
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

        case NodeKind::GROUP:
            std::cout << "GROUP\n";
            printAST(node->sub, indent + 1);
            break;
    }
}

void ASTBuilder::printAllASTs() const {
    for (const auto& [ruleName, rootNode] : astMap) {
        std::cout << "Règle: " << ruleName << "\n";
        printAST(rootNode, 1);
        std::cout << "\n";
    }
}

void ASTBuilder::collectNonTerminals(
    const std::shared_ptr<Node>& node,
    std::unordered_set<std::string>& referenced) const
{
    if (!node) return;
    if (node->kind == NodeKind::ATOM && node->atom.type == AtomType::Non_Terminal) {
        referenced.insert(node->atom.code);
        return;
    }
    if (node->left)  collectNonTerminals(node->left, referenced);
    if (node->right) collectNonTerminals(node->right, referenced);
    if (node->sub)   collectNonTerminals(node->sub, referenced);
}

void ASTBuilder::determineRootRule() {
    if (astMap.empty()) {
        logger.error("AST vide : aucune règle construite.");
        throw std::runtime_error("Aucune règle construite.");
    }

    std::unordered_set<std::string> referenced;
    for (const auto& kv : astMap) {
        collectNonTerminals(kv.second, referenced);
    }

    std::vector<std::string> roots;
    for (const auto& kv : astMap) {
        if (!referenced.count(kv.first)) roots.push_back(kv.first);
    }

    if (roots.empty()) {
        logger.error("Aucune règle racine détectée.");
        throw std::runtime_error("Aucune règle racine détectée.");
    }

    if (roots.size() > 1) {
        std::ostringstream oss;
        oss << "Plusieurs racines détectées : ";
        for (size_t i = 0; i < roots.size(); ++i) {
            if (i) oss << ", ";
            oss << roots[i];
        }
        logger.error(oss.str());
        throw std::runtime_error(oss.str());
    }

    startRule = roots[0];
    logger.debug("Règle racine détectée : " + startRule);
}

