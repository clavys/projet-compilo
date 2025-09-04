#include "compilo/runtime.hpp"

static std::vector<std::string> currentParamNames;
static std::vector<Value> currentArgs;

Runtime::Runtime(Logger& logger) : logger(logger), pc(0), error(false) {}

bool Runtime::execute(const std::vector<Instruction>& instructions) {
    this->instructions = &instructions;
    reset();
    logger.info("=== Debut de l'execution ===");

    while (pc < instructions.size() && !error) {
        const Instruction& instr = instructions[pc];
        logger.debug("PC:", pc, " - ", (int)instr.opcode, " ", instr.operand ," ", instr.operand2);

        switch (instr.opcode) {
            case OpCode::PUSH: {
                const std::string& operand = instr.operand;
                if (!operand.empty() && operand.front() == '"' && operand.back() == '"') {
                    push(operand.substr(1, operand.length() - 2));
                } else {
                    try {
                        if (operand.find('.') != std::string::npos)
                            push(std::stod(operand));
                        else
                            push(std::stoi(operand));
                    } catch (...) {
                        setError("Erreur conversion: " + operand);
                        
                        return false;
                    }
                }
                break;
            }

        case OpCode::PUSH_BOOL: push(instr.operand == "1"); break;
        case OpCode::LOAD:  executeLoad(instr.operand); break;
        case OpCode::STORE: executeStore(instr.operand); break;
        case OpCode::ADD:   executeAdd(); break;
        case OpCode::SUB:   executeSub(); break;
        case OpCode::MUL:   executeMul(); break;
        case OpCode::DIV:   executeDiv(); break;
        case OpCode::MOD:   executeMod(); break;
        case OpCode::NEG:   executeNeg(); break;
        case OpCode::AND:   executeAnd(); break;
        case OpCode::OR:    executeOr(); break;
        case OpCode::NOT:   executeNot(); break;
        case OpCode::EQ:    executeEq(); break;
        case OpCode::NEQ:   executeNeq(); break;
        case OpCode::LT:    executeLt(); break;
        case OpCode::GT:    executeGt(); break;
        case OpCode::LE:    executeLe(); break;
        case OpCode::GE:    executeGe(); break;

        case OpCode::JUMP:
            pc = std::stoull(instr.operand);
            continue;

        case OpCode::JUMP_IF_FALSE:
            if (!valueToBool(pop())) {
                pc = std::stoull(instr.operand);
                continue;
            }
            break;

        case OpCode::MAKE_LIST: executeMakeList(std::stoi(instr.operand)); break;
        case OpCode::LIST_GET:  executeListGet(); break;
        case OpCode::LIST_SET:  executeListSet(); break;
        case OpCode::LIST_LEN:  executeListLen(); break;
        case OpCode::MAKE_EMPTY_LIST: executeMakeEmptyList(); break;

        case OpCode::PRINT_STRING:executePrintString(instr);break;
        case OpCode::PRINT_BOOL:executePrintBool(instr);break;
        case OpCode::PRINT:executePrintNumber(instr);break;

        case OpCode::LOAD_LOCAL:    executeLoadLocal(instr.operand); break;
        case OpCode::STORE_LOCAL:   executeStoreLocal(instr.operand); break;
        case OpCode::DECLARE_PARAM: executeDeclareParam(instr.operand); break;
        case OpCode::FUNC_DECL:     executeFuncDecl(instr.operand); break;
        case OpCode::PUSH_FRAME:    executePushFrame(); break;
        case OpCode::POP_FRAME:     executePopFrame(); break;
        case OpCode::ARG_COUNT:     executeArgCount(std::stoi(instr.operand)); break;
        case OpCode::CALL:          executeCall(instr.operand); break;
        case OpCode::RETURN:        executeReturn(); break;
        case OpCode::RETURN_VOID:   executeReturnVoid(); break;
        case OpCode::FUNC_END:      executeFuncEnd(); break;



        default:
            setError("Instruction inconnue !");
            return false;
    }

    pc++;
}

    if (error) {
        logger.error("Erreur d'execution:", lastError);
        return false;
    }

    logger.info("=== Fin de l'execution ===");
    return true;
}

void Runtime::reset() {
    while (!stack.empty()) stack.pop();
    while (!callStack.empty()) callStack.pop();  // AJOUTEZ
    variables.clear();
    functions.clear();  // AJOUTEZ
    currentParamNames.clear();  // AJOUTEZ  
    currentArgs.clear();  // AJOUTEZ
    pc = 0;
    error = false;
    lastError.clear();
}

void Runtime::setError(const std::string& message) {
    error = true;
    lastError = message;
    logger.error(message);
}

void Runtime::push(const Value& value) { stack.push(value); }

Value Runtime::pop() {
    if (stack.empty()) { setError("Pop sur pile vide"); return 0; }
    Value v = stack.top(); stack.pop(); return v;
}

Value Runtime::peek() const { return stack.empty() ? 0 : stack.top(); }
bool Runtime::isEmpty() const { return stack.empty(); }

// Conversions
std::string Runtime::valueToString(const Value& value) const {
    return std::visit([&](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) return v;
        else if constexpr (std::is_same_v<T, bool>) return v ? "true" : "false";
        else if constexpr (std::is_same_v<T, List>) {
            std::string res = "[";
            for (size_t i = 0; i < v->size(); i++) {
                if (i) res += ", ";
                res += valueToString((*v)[i]);
            }
            res += "]";
            return res;
        }
        else return std::to_string(v);
    }, value);
}

bool Runtime::valueToBool(const Value& value) const {
    return std::visit([](auto&& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) return v;
        else if constexpr (std::is_same_v<T, int>) return v != 0;
        else if constexpr (std::is_same_v<T, double>) return v != 0.0;
        else if constexpr (std::is_same_v<T, std::string>) return !v.empty();
        else if constexpr (std::is_same_v<T, List>) return !v->empty();
        else return true;
    }, value);
}

double Runtime::valueToNumber(const Value& value) const {
    return std::visit([&](auto&& v) -> double {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) return v;
        else if constexpr (std::is_same_v<T, double>) return v;
        else if constexpr (std::is_same_v<T, bool>) return v ? 1.0 : 0.0;
        else { const_cast<Runtime*>(this)->setError("Conversion vers nombre impossible"); return 0.0; }
    }, value);
}

// Arithmétiques
void Runtime::executeAdd(){ auto b=pop(),a=pop(); push(valueToNumber(a)+valueToNumber(b)); }
void Runtime::executeSub(){ auto b=pop(),a=pop(); push(valueToNumber(a)-valueToNumber(b)); }
void Runtime::executeMul(){ auto b=pop(),a=pop(); push(valueToNumber(a)*valueToNumber(b)); }
void Runtime::executeDiv(){ auto b=pop(),a=pop(); double d=valueToNumber(b); if(d==0){setError("Div/0");return;} push(valueToNumber(a)/d); }
void Runtime::executeMod(){ auto b=pop(),a=pop(); int d=(int)valueToNumber(b); if(d==0){setError("Mod/0");return;} push((int)valueToNumber(a)%d); }
void Runtime::executeNeg(){ push(-valueToNumber(pop())); }

// Logiques
void Runtime::executeAnd(){ auto b=pop(),a=pop(); push(valueToBool(a)&&valueToBool(b)); }
void Runtime::executeOr(){ auto b=pop(),a=pop(); push(valueToBool(a)||valueToBool(b)); }
void Runtime::executeNot(){ push(!valueToBool(pop())); }

// Comparaisons
void Runtime::executeEq(){ auto b=pop(),a=pop(); push(valueToNumber(a)==valueToNumber(b)); }
void Runtime::executeNeq(){ auto b=pop(),a=pop(); push(valueToNumber(a)!=valueToNumber(b)); }
void Runtime::executeLt(){ auto b=pop(),a=pop(); push(valueToNumber(a)<valueToNumber(b)); }
void Runtime::executeGt(){ auto b=pop(),a=pop(); push(valueToNumber(a)>valueToNumber(b)); }
void Runtime::executeLe(){ auto b=pop(),a=pop(); push(valueToNumber(a)<=valueToNumber(b)); }
void Runtime::executeGe(){ auto b=pop(),a=pop(); push(valueToNumber(a)>=valueToNumber(b)); }

// Variables
void Runtime::executeLoad(const std::string& n){ if(!variables.count(n)){ setError("Var non definie: "+n); return;} push(variables[n]); }
void Runtime::executeStore(const std::string& n){ variables[n]=pop(); }

// Listes
void Runtime::executeMakeList(int size) {
    auto list = std::make_shared<std::vector<Value>>();
    list->reserve(size);
    for (int i=0;i<size;i++) list->insert(list->begin(),pop());
    push(list);
}

void Runtime::executeListGet() {
    auto idx=pop(),listVal=pop();
    if(!std::holds_alternative<List>(listVal)){ setError("Acces index sur non-liste"); return;}
    auto list=std::get<List>(listVal);
    int i=(int)valueToNumber(idx);
    if(i<0||i>=list->size()){ setError("Index hors limites"); return;}
    push((*list)[i]);
}

void Runtime::executeListSet() {
    auto val=pop(),idx=pop(),listVal=pop();
    if(!std::holds_alternative<List>(listVal)){ setError("Assign index sur non-liste"); return;}
    auto list=std::get<List>(listVal);
    int i=(int)valueToNumber(idx);
    if(i<0||i>=list->size()){ setError("Index hors limites"); return;}
    (*list)[i]=val;
    push(list);
}

void Runtime::executeListLen() {
    Value listVal = pop();
    
    if (!std::holds_alternative<List>(listVal)) {
        setError("LIST_LEN sur non-liste");
        return;
    }
    
    auto list = std::get<List>(listVal);
    push((int)list->size());
}

void Runtime::executeMakeEmptyList() {
    // Récupérer la taille depuis la pile
    Value sizeVal = pop();
    int size = (int)valueToNumber(sizeVal);
    
    if (size < 0) {
        setError("Taille négative pour makeEmpty: " + std::to_string(size));
        return;
    }
    
    // Créer un tableau vide de la taille spécifiée, rempli de zéros
    auto list = std::make_shared<std::vector<Value>>(size, 0);
    push(list);
    
    logger.debug("Tableau vide créé, taille:", size);
}

// I/O
void Runtime::executePrintString(Instruction instr) {
    bool newline = (instr.operand == "1");
    auto v = pop();
    std::cout << valueToString(v);
    if (newline) std::cout << std::endl;
}

void Runtime::executePrintBool(Instruction instr) {
    bool newline = (instr.operand == "1");
    auto v = pop();
    std::cout << (valueToBool(v) ? "true" : "false");
    if (newline) std::cout << std::endl;
}

void Runtime::executePrintNumber(Instruction instr) {
    bool newline = (instr.operand == "1");
    auto v = pop();
    std::cout << valueToNumber(v);
    if (newline) std::cout << std::endl;
}

// Fonctions

void Runtime::executeLoadLocal(const std::string& name) {
    if (callStack.empty()) {
        setError("LOAD_LOCAL hors d'une fonction");
        return;
    }
    
    Frame& currentFrame = callStack.top();
    
    // Chercher d'abord dans les paramètres
    if (currentFrame.params.count(name)) {
        push(currentFrame.params[name]);
        return;
    }
    
    // Puis dans les variables locales
    if (currentFrame.localVars.count(name)) {
        push(currentFrame.localVars[name]);
        return;
    }
    
    setError("Variable locale non trouvée: " + name);
}

void Runtime::executeStoreLocal(const std::string& name) {
    if (callStack.empty()) {
        setError("STORE_LOCAL hors d'une fonction");
        return;
    }
    
    Frame& currentFrame = callStack.top();
    currentFrame.localVars[name] = pop();
}

void Runtime::executeDeclareParam(const std::string& name) {

    currentParamNames.push_back(name);
    logger.debug("Paramètre déclaré:", name);
}

void Runtime::executeFuncDecl(const std::string& name) {

    functions[name] = pc + 2; // +2 pour sauter PUSH_FRAME  
    logger.debug("Fonction déclarée:", name, "à l'adresse", pc + 2);
    
    // Sauter tout le corps de la fonction pour ne pas l'exécuter maintenant
    // On cherche le FUNC_END correspondant en tenant compte de l'imbrication
    int depth = 0;
    size_t originalPc = pc;
    size_t searchPc = pc + 1; 
    
    while (searchPc < instructions->size()) {
        const Instruction& instr = (*instructions)[searchPc];
        
        if (instr.opcode == OpCode::FUNC_DECL) {
            depth++;
        } else if (instr.opcode == OpCode::FUNC_END) {
            if (depth == 0) {
                // C'est notre FUNC_END
                pc = searchPc;
                logger.debug("Corps de fonction sauté, PC maintenant:", pc);
                return;
            } else {
                depth--;
            }
        }
        searchPc++;
    }
    
    // Si on arrive ici, FUNC_END n'a pas été trouvé
    setError("FUNC_END manquant pour la fonction " + name);
    pc = originalPc; // Restaurer en cas d'erreur
}

void Runtime::executePushFrame() {
    // Cette instruction est maintenant gérée dans executeCall()
    // Ne rien faire ici pour éviter la double création de frame
    logger.debug("PUSH_FRAME ignoré (géré par CALL)");
}

void Runtime::executePopFrame() {
    if (callStack.empty()) {
        setError("POP_FRAME sans frame");
        return;
    }
    
    callStack.pop();
    logger.debug("Frame détruite");
}

void Runtime::executeArgCount(int count) {
    // Récupérer les arguments depuis la pile 
    currentArgs.clear();
    for (int i = 0; i < count; i++) {
        currentArgs.push_back(pop());
    }
    
    // Les remettre dans l'ordre correct (inverser)
    std::reverse(currentArgs.begin(), currentArgs.end());
    
    logger.debug("Arguments récupérés:", count);
}

void Runtime::executeCall(const std::string& funcName) {
    if (functions.find(funcName) == functions.end()) {
        setError("Fonction non trouvée: " + funcName);
        return;
    }
    
    size_t returnAddr = pc + 1;
    
    // Créer une nouvelle frame avec les arguments déjà récupérés
    callStack.push(Frame(returnAddr));
    
    // Mapper les arguments aux paramètres
    if (!currentArgs.empty() && !currentParamNames.empty()) {
        Frame& newFrame = callStack.top();
        size_t minSize = std::min(currentArgs.size(), currentParamNames.size());
        
        for (size_t i = 0; i < minSize; i++) {
            newFrame.params[currentParamNames[i]] = currentArgs[i];
            logger.debug("Paramètre", currentParamNames[i], "=", valueToString(currentArgs[i]));
        }
        
        currentArgs.clear();
    }
    
    // Sauter à la fonction
    pc = functions[funcName];
    logger.debug("Appel de", funcName, "saut à", pc);
    

    pc--; // Compensera le pc++ dans la boucle principale
}

void Runtime::executeReturn() {
    if (callStack.empty()) {
        setError("RETURN hors d'une fonction");
        return;
    }
    
    // La valeur de retour est déjà sur la pile
    Value returnValue = pop();
    size_t returnAddr = callStack.top().returnAddress;
    callStack.pop();
    
    // Remettre la valeur de retour sur la pile
    push(returnValue);
    
    // Retourner à l'appelant
    pc = returnAddr - 1; // -1 car pc++ sera fait dans la boucle
    logger.debug("Retour avec valeur à:", returnAddr);
}

void Runtime::executeReturnVoid() {
    if (callStack.empty()) {
        setError("RETURN_VOID hors d'une fonction");
        return;
    }
    
    size_t returnAddr = callStack.top().returnAddress;
    callStack.pop();
    
    // Retourner à l'appelant
    pc = returnAddr - 1; // -1 car pc++ sera fait dans la boucle
    logger.debug("Retour void à:", returnAddr);
}

void Runtime::executeFuncEnd() {
    // Nettoyer les noms de paramètres pour la prochaine fonction
    currentParamNames.clear();
    logger.debug("Fin de fonction atteinte");
}

// Debug

void Runtime::dumpStack() const {
    std::cout << "=== Stack ===\n";
    if(stack.empty()){ std::cout << "(vide)\n"; return;}
    auto temp = stack; 
    int idx = temp.size() - 1;
    while(!temp.empty()){ 
        std::cout << idx-- << ": " << valueToString(temp.top()) << "\n"; 
        temp.pop(); 
    }
}

void Runtime::dumpVariables() const {
    std::cout << "=== Variables Globales ===\n";
    if(variables.empty()){ std::cout << "(aucune)\n"; }
    else {
        for(auto&[k,v]:variables) 
            std::cout << k << " = " << valueToString(v) << "\n";
    }
    
    std::cout << "=== Call Stack ===\n";
    if(callStack.empty()) { std::cout << "(vide)\n"; return; }
    
    auto temp = callStack;
    int level = 0;
    while(!temp.empty()) {
        const Frame& frame = temp.top();
        std::cout << "Frame " << level++ << " (ret: " << frame.returnAddress << "):\n";
        
        if (!frame.params.empty()) {
            std::cout << "  Paramètres:\n";
            for(auto&[k,v] : frame.params) {
                std::cout << "    " << k << " = " << valueToString(v) << "\n";
            }
        }
        
        if (!frame.localVars.empty()) {
            std::cout << "  Variables locales:\n";
            for(auto&[k,v] : frame.localVars) {
                std::cout << "    " << k << " = " << valueToString(v) << "\n";
            }
        }
        
        temp.pop();
    }
}

void Runtime::dumpCallStack() const {
    dumpVariables(); // Inclut déjà l'affichage de la call stack
}


