#include "meta/token.hpp"
#include <cctype>
#include <iostream>
#include "meta/meta_parser.hpp"
#include <unordered_map>
#include "meta/meta_ast_builder.hpp"
#include <sstream>
#include <stdexcept>

bool MetaParser::analyse(const std::shared_ptr<Node>& node, std::vector<meta::Token>* tokens, int* index) {
    if (!node) {
        logger.error("Erreur : node est nullptr !");
        return false;
    }

    int stepId = stepCounter++;
    bool success = false;

    switch (node->kind) {
        case NodeKind::ATOM: {
            if (*index >= tokens->size()) return false;

            const meta::Token& current = (*tokens)[*index];

            if (node->atom.type == AtomType::Terminal) {
                logger.debug("ATOM (TERMINAL) : ", node->atom.code," token : ",current, " step ", stepId);

                if ((node->atom.code == current.value) ||
                    (node->atom.code == "IDNTER" && current.type == meta::TokenType::Identifier) ||
                    (node->atom.code == "ELTER" && current.type == meta::TokenType::Terminal)) {

                    if (node->atom.action != 0)
                        builder.applyAction(node->atom.action, current);

                    (*index)++;
                    
                    success = true;
                }
            } else {
                logger.debug("ATOM (NON_TERMINAL) : ", node->atom.code, " step ", stepId);
                const meta::Token s = current;
                
                auto itNode = metaGrammarRules.find(node->atom.code);
                if (itNode != metaGrammarRules.end() && analyse(itNode->second, tokens, index)) {
                    if (node->atom.action != 0)
                        builder.applyAction(node->atom.action, s);
                    success = true;
                }
            }
            break;
        }

        case NodeKind::CONCATENATION:
            logger.debug("CONCATENATION : " , stepId);
            if (analyse(node->left, tokens, index))
                success = analyse(node->right, tokens, index);
            break;

        case NodeKind::UNION_NODE:
            logger.debug("UNION_NODE : " , stepId);
            success = analyse(node->left, tokens, index) ||
                      analyse(node->right, tokens, index);
            break;

        case NodeKind::STAR:
            logger.debug("STAR : " , stepId);
            success = true;
            while (analyse(node->sub, tokens, index)) {}
            break;

        case NodeKind::UN:
            logger.debug("UN : " , stepId);
            analyse(node->sub, tokens, index);
            success = true;
            break;

        case NodeKind::GROUP:
            logger.debug("GROUP : " , stepId);
            success = analyse(node->sub, tokens, index);
            break;
    }

    logger.debug("Resultat step " , stepId , " : " , (success ? "SUCCES" : "ECHEC"));
    return success;
}

bool MetaParser::generateGrammarAST() {
    int index = 0;

    // Analyse avec la méta-grammaire
    auto rootNode = metaGrammarRules["S"];
    bool b = analyse(rootNode, &tokenList, &index);

    // Détection de la racine réelle dans le langage cible
    try {
        builder.determineRootRule(); // Erreur si plusieurs ou zéro racine
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return false;
    }

    if (b) {
        std::cout << "Analyse reussie !" << std::endl;
    } else {
        std::cout << "Analyse echoue" << std::endl;
    }

    return b;
}
