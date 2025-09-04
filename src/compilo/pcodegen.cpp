#include "compilo/pcodegen.hpp"

void PCodegen::emit(OpCode opcode, const std::string& operand) {
    instructions.emplace_back(opcode, operand);
}

void PCodegen::dump() const {
    std::cout << "=== PCode ===" << std::endl;
    for (size_t i = 0; i < instructions.size(); i++) {
        std::cout << i << ": " << opCodeToString(instructions[i].opcode);
        if (!instructions[i].operand.empty())
            std::cout << " " << instructions[i].operand;
        std::cout << std::endl;
    }
}

// === Variables ===
void PCodegen::declareVariable(const std::string& name, bool isLocal) {
    
    std::string fullName = name;
    if (isLocal && !currentFunction.empty()) {
        fullName = currentFunction + "::" + name;
        localVariables.insert(name);
    }
    
    if (symbolTable.find(fullName) != symbolTable.end()) {
        std::cerr << "Erreur : variable déjà déclarée " << name << std::endl;
        hasError = true;
        return;
    }
    symbolTable[fullName] = "unknown";
}

void PCodegen::storeVariable(const std::string& name) {
    std::string t = popType();
    
    if (!currentFunction.empty() && localVariables.find(name) != localVariables.end()) {
        emit(OpCode::STORE_LOCAL, name);
        symbolTable[currentFunction + "::" + name] = t;
    } else {
        emit(OpCode::STORE, name);
        symbolTable[name] = t;
    }
}

void PCodegen::loadVariable(const std::string& name) {
    if (!currentFunction.empty() && localVariables.find(name) != localVariables.end()) {
        // Variable locale ou paramètre
        emit(OpCode::LOAD_LOCAL, name);
        std::string fullName = currentFunction + "::" + name;
        pushType(symbolTable.count(fullName) ? symbolTable[fullName] : "unknown");
    } else {
        // Variable globale
        if (symbolTable.find(name) == symbolTable.end()) {
            std::cerr << "Erreur : variable non déclarée " << name << std::endl;
            hasError = true;
            return;
        }
        emit(OpCode::LOAD, name);
        pushType(symbolTable[name]);
    }
}

void PCodegen::pushVarName(const std::string& name) { 
    varNameStack.push(name); 
}

std::string PCodegen::popVarName() { 
    if (varNameStack.empty()) return ""; 
    std::string n = varNameStack.top(); 
    varNameStack.pop(); 
    return n; 
}

// === Fonctions ===

void PCodegen::declareFunction(const std::string& name, int paramCount) {
    emit(   OpCode::FUNC_DECL, name);
}

void PCodegen::callFunction(const std::string& name) {
    emit(OpCode::CALL, name);
}


// === Types ===
void PCodegen::pushType(const std::string& type) {
    typeStack.push(type);
}

std::string PCodegen::popType() {
    if (typeStack.empty()) return "unknown";
    std::string t = typeStack.top();
    typeStack.pop();
    return t;
}

std::string PCodegen::topType() const {
    return typeStack.empty() ? "unknown" : typeStack.top();
}






// === Actions déclenchées par la grammaire ===
void PCodegen::applyAction(int actionId, const std::string& value) {
    switch (actionId) {
        case 1: // déclaration variable
            pushVarName(value);
            declareVariable(value, !currentFunction.empty());
            break;

        case 2: // assignation
            storeVariable(popVarName());
            break;

        case 3: // chargement variable
            loadVariable(value);
            break;

        case 4: // assignation variable (Assignment)
            pushVarName(value);
            break;

        case 10: // NOT
            emit(OpCode::NOT);
            pushType("bool");
            break;

        case 11: // NEG
            emit(OpCode::NEG);
            pushType("number");
            break;

        case 20: // Number literal
            emit(OpCode::PUSH, value);
            pushType("number");
            break;

        case 21: // String literal
            emit(OpCode::PUSH, "\"" + value + "\"");
            pushType("string");
            break;

        case 22: // true
            emit(OpCode::PUSH_BOOL, "1");
            pushType("bool");
            break;

        case 23: // false
            emit(OpCode::PUSH_BOOL, "0");
            pushType("bool");
            break;

        // === Arithmétique ===
        case 30: emit(OpCode::ADD); pushType("number"); break;
        case 31: emit(OpCode::SUB); pushType("number"); break;
        case 32: emit(OpCode::MUL); pushType("number"); break;
        case 33: emit(OpCode::DIV); pushType("number"); break;
        case 34: emit(OpCode::MOD); pushType("number"); break;

        // === Logique ===
        case 40: emit(OpCode::OR);  pushType("bool"); break;
        case 41: emit(OpCode::AND); pushType("bool"); break;

        // === Comparaisons ===
        case 42: emit(OpCode::EQ);  pushType("bool"); break;
        case 43: emit(OpCode::NEQ); pushType("bool"); break;
        case 44: emit(OpCode::LT);  pushType("bool"); break;
        case 45: emit(OpCode::GT);  pushType("bool"); break;
        case 46: emit(OpCode::LE);  pushType("bool"); break;
        case 47: emit(OpCode::GE);  pushType("bool"); break;

        // === Listes ===

                // Création de listes 
        case 60: // Début '['
            listSizeStack.push(0);
            break;
            
        case 61: // Chaque élément
            if (!listSizeStack.empty()) {
                int currentSize = listSizeStack.top();
                listSizeStack.pop();
                listSizeStack.push(currentSize + 1);
            }
            break;
            
        case 62: // Fin ']' 
            {
                int size = 0;
                std::string elementType = "unknown";
                
                if (!listSizeStack.empty()) {
                    size = listSizeStack.top();
                    listSizeStack.pop();
                    
                    // Inférer le type des éléments
                    if (size > 0 && !typeStack.empty()) {
                        elementType = topType();
                    }
                }
                
                emit(OpCode::MAKE_LIST, std::to_string(size));
                pushType("list<" + elementType + ">");
            }
            break;

        case 63: // Accès liste||Assignation liste ?        
            rulesStack.push_back(63);       
            break;

        case 64: 
            if (!rulesStack.empty() && rulesStack.back() == 63) {
                // Assignation liste
                emit(OpCode::LIST_SET);
                popType(); // value
                popType(); // index  
                popType(); // liste
                pushType("list"); // liste modifiée

                } else {
                // Accès liste
                    emit(OpCode::LIST_GET);
                    popType(); // index
                    std::string listType = popType(); // liste
                                // Extraire le type de l'élément si possible
                    if (listType.find("list<") == 0 && listType.back() == '>') {
                        std::string elementType = listType.substr(5, listType.length() - 6);
                        pushType(elementType);
                    } else {
                        pushType("unknown");
                    }

                }
                if(!rulesStack.empty()){
                    rulesStack.pop_back();
                }else{
                    std::cerr << "Erreur: rulesStack vide lors du pop" << std::endl;
                }
            break;

        case 65: // size(list)
            emit(OpCode::LIST_LEN);
            popType();              
            pushType("number");
            break;

        case 66: // Liste static vide
            emit(OpCode::MAKE_EMPTY_LIST);
            pushType("list");
            break;




        // === I/O ===
        case 90:   // sans newline popType(); emit(OpCode::PRINT, "0");
        case 91:  // avec newline 
            {
                std::string t = popType();
                bool newline = (actionId == 91);
                
                if (t == "string") {
                    emit(OpCode::PRINT_STRING, newline ? "1" : "0");
                } else if (t == "bool") {
                    emit(OpCode::PRINT_BOOL, newline ? "1" : "0");
                } else {
                    emit(OpCode::PRINT, newline ? "1" : "0");
                }
            }
            break;

        // === IF/ELSE ===
        case 100: // après la condition IF
            emit(OpCode::JUMP_IF_FALSE, "");
            ifStack.push_back({instructions.size() - 1, 0, std::nullopt, 0});
            break;

        case 101: // fin du bloc IF avant else éventuel
            if (!ifStack.empty()) {
                IfContext& ctx = ifStack.back();            
                size_t target = instructions.size();
                ctx.jumpIfFalseTarget = target;
            }
            break;

        case 102: // début du ELSE
            emit(OpCode::JUMP, "");
            if (!ifStack.empty()) {
                IfContext& ctx = ifStack.back();
                ctx.jumpOverElseIndex = instructions.size() - 1;
            }
            break;

        case 103: // fin du bloc ELSE
            if (!ifStack.empty()) {
                IfContext& ctx = ifStack.back();
                size_t target = instructions.size();
                ctx.jumpOverElseTarget = target;
            }
            break;

        case 104: // ENDIF
            if (!ifStack.empty()) {
                IfContext ctx = ifStack.back();
                ifStack.pop_back();

                if (ctx.jumpOverElseIndex) {
                    instructions[ctx.jumpIfFalseIndex].operand = std::to_string(ctx.jumpIfFalseTarget + 1);
                    instructions[*ctx.jumpOverElseIndex].operand = std::to_string(ctx.jumpOverElseTarget);
                } else {
                    instructions[ctx.jumpIfFalseIndex].operand = std::to_string(ctx.jumpIfFalseTarget);
                }
            }
            break;

        // === WHILE ===
        case 110: // début WHILE
            patchStack.push_back(instructions.size());
            break;

        case 111: // après condition
            emit(OpCode::JUMP_IF_FALSE, "");
            patchStack.push_back(instructions.size() - 1);
            break;

        case 112: // fin WHILE
        {
            size_t jumpIfFalse = patchStack.back();
            patchStack.pop_back();

            size_t loopStart = patchStack.back();
            patchStack.pop_back();

            emit(OpCode::JUMP, std::to_string(loopStart));
            instructions[jumpIfFalse].operand = std::to_string(instructions.size());
        }
            break;

        // === FONCTIONS ===
        case 120: // déclaration fonction
            currentFunction = value;
            functionTable[value] = FunctionInfo{value, 0, {}, "unknown", false};
            break;

        case 121: {// paramètre de fonction
            if (!currentFunction.empty()) {
                auto& func = functionTable[currentFunction];
                func.paramCount++;
                func.paramNames.push_back(value);
                
                // Marquer comme variable locale
                localVariables.insert(value);
                
                // Déclarer dans la table des symboles locale
                symbolTable[currentFunction + "::" + value] = "param";
            }
            break;
        }

        case 122: {// début bloc fonction (après paramètres)
            if (!currentFunction.empty()) {
                auto& func = functionTable[currentFunction];
                
                // Émettre la déclaration avec paramètres
                for (const auto& param : func.paramNames) {
                    emit(OpCode::DECLARE_PARAM, param);
                }
                
                emit(OpCode::FUNC_DECL, func.name);
                emit(OpCode::PUSH_FRAME);
            }
            break;
        }

        case 123: // fin de fonction
            if (!currentFunction.empty()) {
                // Si pas de return explicite, ajouter return void
                if (instructions.empty() || 
                    (instructions.back().opcode != OpCode::RETURN && 
                     instructions.back().opcode != OpCode::RETURN_VOID)) {
                    emit(OpCode::RETURN_VOID);
                }
                
                emit(OpCode::POP_FRAME);
                emit(OpCode::FUNC_END);
                
                // Nettoyer
                localVariables.clear();
                currentFunction = "";
            }
            break;

        case 124: // return avec valeur
            if (!currentFunction.empty()) {
                std::string returnType = topType();
                functionTable[currentFunction].returnType = returnType;
                functionTable[currentFunction].typeInferred = true;
            }
            
            emit(OpCode::RETURN);
            popType();
            break;

        case 125:{ // appel de fonction
            // Vérifier que la fonction existe
            std::string funcName = callStack.empty() ? "" : callStack.top();
            if (!callStack.empty()) callStack.pop();
            if (functionTable.find(funcName) == functionTable.end()) {
                std::cerr << "Erreur: fonction non déclarée " << funcName << std::endl;
                hasError = true;
                break;
            }
            
            auto& func = functionTable[funcName];
            emit(OpCode::ARG_COUNT, std::to_string(func.paramCount));
            emit(OpCode::CALL, funcName);
            
            // Type de retour
            pushType(func.returnType);
            break;
        }
        
        case 126: // nom de fonction pour appel
            callStack.push(value);
            break;

        case 130: // import "fichier"
            {
            std::string filename = value; // nom du fichier à importer
            
            // Opcodes pour l'import
            emit(OpCode::IMPORT_MODULE, filename);

        }
        break;

        default:
            std::cerr << "Action inconnue: " << actionId << std::endl;
    }
}
