#include "compilo/token.hpp"
#include <cctype>
#include <iostream>
#include "compilo/parser.hpp"
#include "compilo/lexer.hpp"
#include <unordered_map>
#include "compilo/pcodegen.hpp"
#include <sstream>
#include <stdexcept>
#include <fstream>

bool Parser::analyse(const std::shared_ptr<Node>& node, std::vector<compilo::Token>* tokens, int* index) {
    if (!node) {
        logger.error("Erreur : node est nullptr !");
        return false;
    }

    int stepId = stepCounter++;
    bool success = false;
    

    switch (node->kind) {
        case NodeKind::ATOM: {
            
             
            
            if (*index >= tokens->size()) return false;
            
            const compilo::Token& current = (*tokens)[*index];
            
            if (node->atom.type == AtomType::Terminal) {
                logger.debug("ATOM (TERMINAL) : ", node->atom.code," token : ",current, " step ", stepId);

                if ((node->atom.code == current.value) ||
                    (node->atom.code == "ID" && current.type == compilo::TokenType::Identifier) ||
                    (node->atom.code == "NUMBER" && current.type == compilo::TokenType::Number)||
                    (node->atom.code == "STRING" && current.type == compilo::TokenType::String)||
                    (node->atom.code == "IDLIST" && current.type == compilo::TokenType::IdentifierList)||
                    (node->atom.code == "IDFONCTION" && current.type == compilo::TokenType::IdentifierFonction)
                    
                    ) {
                    /**  */
                    if (node->atom.action != 0){
                        builder.applyAction(node->atom.action, current.value);
                    }
                    (*index)++;
                    
                        
                    success = true;
                }
            } else {
                logger.debug("ATOM (NON_TERMINAL) : ", node->atom.code, " step ", stepId);
                const compilo::Token s = current;
                
                auto itNode = grammarRules.find(node->atom.code);
                if (itNode != grammarRules.end() && analyse(itNode->second, tokens, index)) {
                    if (node->atom.action != 0){
                        builder.applyAction(node->atom.action, s.value);
                    }
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



bool Parser::generatePcode() {
    logger.info("Génération du PCode...");
    int index = 0;
    bool b = analyse(grammarRules[rootRule], &tokenList, &index);
    if (b) {
        std::cout << "Analyse reussie !" << std::endl;
        builder.dump();
    } else {
        std::cout << "Analyse echoue" << std::endl;
    }
    return b;
}