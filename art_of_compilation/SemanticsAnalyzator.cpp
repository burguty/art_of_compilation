#include "SemanticsAnalyzator.h"

NodeV* var_tid = new NodeV;
NodeF* func_tid = nullptr;

void NewVarScope() {
    NodeV* new_scope = new NodeV;
    var_tid->next = new_scope;
    new_scope->prev = var_tid;
    var_tid = new_scope;
}

void DeleteVarScope() {
    auto tmp = var_tid;
    var_tid = var_tid->prev;
    if (var_tid != nullptr) var_tid->next = nullptr;
    delete tmp;
}

void AddVar(std::string id, std::string type, bool is_init) {
    var_tid->vars.push_back({ id, type, is_init });
}

void DeleteFunc() {
    auto tmp = func_tid;
    func_tid = func_tid->prev;
    if (func_tid != nullptr) func_tid->next = nullptr;
    delete tmp;
}

void AddFunc(std::string id, std::string type, std::vector<std::string> pars) {
    Function tmp;
    tmp.id = id;
    tmp.type = type;
    tmp.type_par = pars;
    if (func_tid == nullptr) {
        NodeF* q = new NodeF;
        q->function = tmp;
        func_tid = q;
        return;
    }
    NodeF* q = new NodeF;
    q->function = tmp;
    func_tid->next = q;
    q->prev = func_tid;
    func_tid = q;
}

void SemanticsAnalyzator::SetLexemes(std::vector<Lexeme> l) {
    lexemes_ = l;
}

bool SemanticsAnalyzator::FindVar(NodeV*& cur, std::string type, 
        std::string id, bool init, int line) {

    std::string error;

    if (cur == nullptr) {
        if (type == "none") {
            error += "Variable doesn't exist: ";
            error += id;
            error += " Line: ";
            error += std::to_string(line);
            throw error;
            return false;
        }
        return true;
    }
    for (int i = 0; i < cur->vars.size(); ++i) {
        auto tmp = cur->vars[i];
        if (id != tmp.id) continue;

        if (type == "array") {
            if (tmp.type.substr(0, 5) == "array") {
                return true;
            }
            else {
                error = "Isn't array: ";
                error += id;
                error += " Line: ";
                error += std::to_string(line);
                throw error;
                return false;
            }
        }

        if (type == tmp.type) {
            error = "Variable already exists: ";
            error += id;
            error += " Line: ";
            error += std::to_string(line);
            throw error;
            return false;
        }
        if (type != "none" && type != tmp.type) { 
            if (cur == var_tid) {
                error = "Variable already exists with a different type: ";
                error += id;
                error += " Line: ";
                error += std::to_string(line);
                throw error;
                return false;
            }
            return true;
        }

        if (type == "none" && !init && !tmp.is_init) {
            error = "Variable isn't initialized: ";
            error += id;
            error += " Line: ";
            error += std::to_string(line);
            throw error;
            return false; // используем как rvalue
        }
        if (type == "none" && init && tmp.is_init) return true; // повторно инициализируем
        if (type == "none" && !init && tmp.is_init) return true; // используем как rvalue
        if (type == "none" && init && !tmp.is_init) {
            cur->vars[i].is_init = true;
            return true; // инициализируем сущ. переменную
        }
    }
    return FindVar(cur->prev, type, id, init, line);
}

bool CheckEqualFunctions(NodeF*& cur, std::string id, std::vector<std::string> par) {
    if (cur->function.id != id) return false;
    if (cur->function.type_par.size() != par.size()) return false;
    for (int i = 0; i < par.size(); ++i) {
        if (cur->function.type_par[i] != par[i]) {
            return false;
        }
    }
    return true;
}

bool SemanticsAnalyzator::FindFunc(NodeF*& cur, std::string type, 
    std::string id, std::vector<std::string> p, int line) {
    
    std::string error;

    if (cur == nullptr) {
        if (type == "none") {
            error = "Fuction doesn't exist: ";
            error += id;
            error += " Line: ";
            error += std::to_string(line);
            throw error;
            return false;
        }
        return true;
    }

    if (id != cur->function.id) {
        return FindFunc(cur->prev, type, id, p, line);
    }

    if (type != "none" && type == cur->function.type) {
        error = "Function already exists: ";
        error += id;
        error += " Line: ";
        error += std::to_string(line);
        throw error;
        return false;
    }
    if (type != "none" && type != cur->function.type) {
        error = "Function already exists with a different type: ";
        error += id;
        error += " Line: ";
        error += std::to_string(line);
        throw error;
        return false;
    }

    if (type == "none") {
        if (CheckEqualFunctions(cur, id, p)) {
            return true;
        }
    }
    return FindFunc(cur->prev, type, id, p, line);
}

Variable SemanticsAnalyzator::SearchVar(NodeV*& cur, std::string id, int line) {
    std::string error;
    if (cur == nullptr) {
        error = "Variable doesn't exist: ";
        error += id;
        error += " Line: ";
        error += std::to_string(line);
        throw error;
        return { };
    }
    for (auto p : cur->vars) {
        if (p.id == id) {
            return p;
        }
    }
    return SearchVar(cur->prev, id, line);
}

NodeF* SemanticsAnalyzator::SearchFunc(NodeF*& cur, std::string id, int line) {
    std::string error;
    if (cur == nullptr) {
        return nullptr;
    }
    if (cur->function.id == id) return cur;
    return SearchFunc(cur->prev, id, line);
}

void SemanticsAnalyzator::VarGo(NodeV*& cur) {
    if (cur == nullptr) return;
    for (auto p : cur->vars) {
        std::cout << p.id << ' ' << p.type << '\n';
    }
    VarGo(cur->prev);
}

bool IntOperation(std::string s) {
    return s == "+" || s == "-" || s == "*"
        || s == "/" || s == "%" || s == "=" || s == "&" ||
        s == "|" || s == "^" || s == "<" || s == ">" ||
        s == "<=" || s == ">=" || s == "==" || s == "<>";
}

bool DoubleOperation(std::string s) {
    return s == "+" || s == "-" || s == "*"
        || s == "/" || s == "=" || s == "<" || s == ">" ||
        s == "<=" || s == ">=" || s == "==" || s == "<>";
}

bool LogicalOperation(std::string s) {
    return s == "==" || s == "<>" ||
        s == "&&" || s == "||" || s == "=";
}

bool CompareOperation(std::string s) {
    return s == "==" || s == "<" || s == ">" ||
        s == "<=" || s == ">=" || s == "<>";
}

void SemanticsAnalyzator::CheckBin(std::stack<TypeControl>& type_control) {
    auto c = type_control.top();
    type_control.pop();
    auto b = type_control.top();
    type_control.pop();
    auto a = type_control.top();
    type_control.pop();

    std::string error;
    if (a.type == "void") {
        error = "Value mustn't be \'void' in expression: ";
        error += a.id;
        error += " Line: ";
        error += std::to_string(a.line);
        throw error;
        return;
    }
    if (c.type == "void") {
        error = "Value mustn't be \'void': ";
        error += c.id;
        error += " Line: ";
        error += std::to_string(c.line);
        throw error;
        return;
    }
    if (a.type == "int" && c.type == "int" && IntOperation(b.id)) {
        if (CompareOperation(b.id)) {
            type_control.push({ "bool", "", b.line });
        }
        else type_control.push({ "int", "", a.line });
        return;
    }
    if (a.type == "int" && c.type == "double" && DoubleOperation(b.id)) {
        if (b.id == "=") {
            error = "Initialization error: int = double Variable: " + a.id + " Line: ";
            error += std::to_string(b.line);
            throw error;
            return;
        }
        else if (CompareOperation(b.id)) {
            type_control.push({ "bool", "", b.line });
        }
        else type_control.push({ "double", "", a.line });
        return;
    }
    if (a.type == "int" && c.type == "bool" && IntOperation(b.id)) {
        if (b.id == "=") {
            error = "Initialization error: int = bool Variable: ";
            error += a.id;
            error += " Line: ";
            error += std::to_string(b.line);
            throw error;
            return;
        }
        error = "Type conversion error: int and bool Line: ";
        error += std::to_string(b.line);
        throw error;
        return;
    }
    if (a.type == "double" && c.type == "int" && DoubleOperation(b.id)) {
        if (CompareOperation(b.id)) {
            type_control.push({ "bool", "", b.line });
        }
        else type_control.push({ "double", "", a.line });
        return;
    }
    if (a.type == "double" && c.type == "double" && DoubleOperation(b.id)) {
        if (CompareOperation(b.id)) {
            type_control.push({ "bool", "", b.line });
        }
        else type_control.push({ "double", "", a.line });
        return;
    }
    if (a.type == "double" && c.type == "bool" && DoubleOperation(b.id)) {
        error = "Type conversion error: double and bool Line: ";
        error += std::to_string(b.line);
        throw error;
        return;
    }
    if (a.type == "bool" && c.type == "bool" && LogicalOperation(b.id)) {
        type_control.push({ "bool", "", a.line });
        return;
    }
    if (a.type == "bool" && c.type == "int" && LogicalOperation(b.id)) {
        error = "Type conversion error: bool and int Line: ";
        error += std::to_string(b.line);
        throw error;
        return;
    }
    if (a.type == "bool" && c.type == "double" && LogicalOperation(b.id)) {
        error = "Type conversion error: bool and double Line: ";
        error += std::to_string(b.line);
        throw error;
        return;
    }
}

void SemanticsAnalyzator::CheckUno(std::stack<TypeControl>& type_control) {
    auto b = type_control.top();
    type_control.pop();
    auto a = type_control.top();
    type_control.pop();
    
    std::string error;
    if (b.type == "void") {
        error = "Value mustn't be \'void' in expression: ";
        error += b.id;
        error += " Line: ";
        error += std::to_string(b.line);
        throw error;
        return;
    }
    if (a.id == "++" || a.id == "--") {
        if (b.type == "bool") {
            error = "Increment with bool Line: ";
            error += std::to_string(a.line);
            throw error;
            return;
        }
        return;
    }
    if (a.id == "+" || a.id == "-") {
        if (b.type == "bool") {
            error = "Sign with bool, Line: ";
            error += std::to_string(a.line);
            throw error;
            return;
        }
        return;
    }
    if (a.id == "!") {
        if (b.type == "int" || b.type == "double") {
            error = "Denial with int or double, Line: ";
            error += std::to_string(a.line);
            throw error;
            return;
        }
        return;
    }
}

void SemanticsAnalyzator::Priority0(
    std::stack<TypeControl>& type_control, int& i) {
    std::string str = lexemes_[i].GetText();
    if (str == "+" || str == "-") {
        type_control.push({ "", str, lexemes_[i].GetNum() });
        Adding(type_control, i);
        CheckUno(type_control);
    }
    else if (str == "++" || str == "--") {
        type_control.push({ "", str, lexemes_[i].GetNum() });
        Adding(type_control, i);
        CheckUno(type_control);
    }
    else {
        Adding(type_control, i);
    }
}

void SemanticsAnalyzator::Priority1(
    std::stack<TypeControl>& type_control, int& i) {
    Priority0(type_control, i);
    if (lexemes_[i].GetText() == "*" || lexemes_[i].GetText() == "/" || 
        lexemes_[i].GetText() == "%") {
        Operation1(type_control, i);
        Priority1(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority2(
    std::stack<TypeControl>& type_control, int& i) {
    Priority1(type_control, i);
    if (lexemes_[i].GetText() == "+" || lexemes_[i].GetText() == "-") {
        Operation2(type_control, i);
        Priority2(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority3(
    std::stack<TypeControl>& type_control, int& i) {
    Priority2(type_control, i);
    if (lexemes_[i].GetText() == "<" || lexemes_[i].GetText() == ">" || 
        lexemes_[i].GetText() == "<=" || lexemes_[i].GetText() == ">=") {
        Operation3(type_control, i);
        Priority3(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority4(
    std::stack<TypeControl>& type_control, int& i) {
    Priority3(type_control, i);
    if (lexemes_[i].GetText() == "==" || lexemes_[i].GetText() == "<>") {
        Operation4(type_control, i);
        Priority4(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority5(
    std::stack<TypeControl>& type_control, int& i) {
    Priority4(type_control, i);
    if (lexemes_[i].GetText() == "&") {
        type_control.push({ "", "&", lexemes_[i].GetNum() });
        ++i;
        Priority5(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority6(
    std::stack<TypeControl>& type_control, int& i) {
    Priority5(type_control, i);
    if (lexemes_[i].GetText() == "|") {
        type_control.push({ "", "|", lexemes_[i].GetNum() });
        ++i;
        Priority6(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority7(
    std::stack<TypeControl>& type_control, int& i) {
    Priority6(type_control, i);
    if (lexemes_[i].GetText() == "^") {
        type_control.push({ "", "^", lexemes_[i].GetNum() });
        ++i;
        Priority7(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority8(
    std::stack<TypeControl>& type_control, int& i) {
    Priority7(type_control, i);
    if (lexemes_[i].GetText() == "&&") {
        type_control.push({ "", "&&", lexemes_[i].GetNum() });
        ++i;
        Priority8(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority9(
    std::stack<TypeControl>& type_control, int& i) {
    Priority8(type_control, i);
    if (lexemes_[i].GetText() == "||") {
        type_control.push({ "", "||", lexemes_[i].GetNum() });
        ++i;
        Priority9(type_control, i);
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Priority10(
    std::stack<TypeControl>& type_control, int& i) {
    Priority9(type_control, i);
    if (lexemes_[i].GetText() == "=") {
        type_control.push({ "", "=", lexemes_[i].GetNum() });
        ++i;
        Priority10(type_control, i); // надо заменить на priority9
        CheckBin(type_control);
    }
}

void SemanticsAnalyzator::Operation1(
    std::stack<TypeControl>& type_control, int& i) {
    auto lex = lexemes_[i].GetText();
    if (lex != "*" && lex != "/" && lex != "%") {
        return;
    }
    type_control.push({ "", lex, lexemes_[i].GetNum() });
    ++i;
}

void SemanticsAnalyzator::Operation2(
    std::stack<TypeControl>& type_control, int& i) {
    auto lex = lexemes_[i].GetText();
    if (lex != "+" && lex != "-") {
        return;
    }
    type_control.push({ "", lex, lexemes_[i].GetNum() });
    ++i;
}

void SemanticsAnalyzator::Operation3(
    std::stack<TypeControl>& type_control, int& i) {
    auto lex = lexemes_[i].GetText();
    if (lex != "<" && lex != ">" && lex != "<=" && lex != ">=") {
        return;
    }
    type_control.push({"", lex, lexemes_[i].GetNum()});
    ++i;
}

void SemanticsAnalyzator::Operation4(
    std::stack<TypeControl>& type_control, int& i) {
    auto lex = lexemes_[i].GetText();
    if (lex != "==" && lex != "<>") {
        return;
    }
    type_control.push({ "", lex, lexemes_[i].GetNum() });
    ++i;
}

void SemanticsAnalyzator::Expression(
    std::stack<TypeControl>& type_control, int& i) {
    Priority10(type_control, i);
}

void SemanticsAnalyzator::StartAdding(
    std::stack<TypeControl>& type_control, int& i) {
    if (lexemes_[i].GetType() == 3) {
        auto p = lexemes_[i].GetText().find('.');
        if (0 <= p && p < lexemes_[i].GetText().size()) {
            // проверка на вещественное число
            type_control.push({ "double", "", lexemes_[i].GetNum() });
            return;
        }
        else {
            // целое число
            type_control.push({ "int", "", lexemes_[i].GetNum() });
        }
        ++i;
    }
    else if (lexemes_[i].GetText() == "true" || lexemes_[i].GetText() == "false") {
        // буловское значение
        type_control.push({ "bool", "", lexemes_[i].GetNum() });
        ++i;
    }
    else if (lexemes_[i].GetText() == "!") {
        // отрицание
        type_control.push({ "", "!", lexemes_[i].GetNum() });
        ++i;
        Adding(type_control, i);
        CheckUno(type_control);
    }
}

void SemanticsAnalyzator::AddingFunctionParameters(
    std::stack<TypeControl>& type_control, int& i, Function f, int& ind_par) {
    std::string error;
    if (lexemes_[i].GetText() == ")") {
        ++i;
        ++ind_par;
        if (f.type_par.size() != ind_par) {
            error = "Wrong amount of parameters! Function: ";
            error += f.id;
            error += " Line: ";
            error += std::to_string(lexemes_[i].GetNum());
            throw error;
            return;
        }
        return;
    }
    ++i;
    ++ind_par;
    std::stack<TypeControl> type_control_parameter;
    Expression(type_control_parameter, i);
    std::cout << '-' << lexemes_[i].GetText() << '\n';
    if (!(ind_par >= 0 && ind_par < f.type_par.size())) {
        error = "Wrong amount of parameters! Function: ";
        error += f.id;
        error += " Line: ";
        error += std::to_string(lexemes_[i].GetNum());
        throw error;
        return;
    }
    if (f.type_par[ind_par] != type_control_parameter.top().type && 
        (f.type_par[ind_par] != "double" || type_control_parameter.top().type != "int")) {
        error = "Parameter type conversion error! Function: ";
        error += f.id;
        error += " Line: ";
        error += std::to_string(type_control_parameter.top().line);
        throw error;
        return;
    }
    AddingFunctionParameters(type_control, i, f, ind_par);
}

void SemanticsAnalyzator::Adding(
    std::stack<TypeControl>& type_control, int& i) {
    std::string str = lexemes_[i].GetText();
    std::string error;
    if (lexemes_[i].GetType() == 3 || str == "true" || str == "false" || str == "!") {
        StartAdding(type_control, i);
    }
    else if (lexemes_[i].GetType() == 2) {
        if (lexemes_[i + 1].GetText() == "[") {
            std::string name = lexemes_[i].GetText();
            int line = lexemes_[i].GetNum();
            auto var = SearchVar(var_tid, name, line);
            std::string type = var.type;
            if (type.substr(0, 5) != "array") {
                error = name;
                error += " isn't array! Line: ";
                error += std::to_string(lexemes_[i].GetNum());
                throw error;
                return;
            }
            i += 2;
            std::stack<TypeControl> type_control_index;
            Expression(type_control_index, i);
            if (type_control_index.top().type != "int") {
                error = "Not integer value! Line";
                error += std::to_string(line);
                throw error;
                return;
            }
            type = type.substr(6);
            type_control.push({ type, name, line });
            ++i; // сдвиг потому что ]
        }
        else if (lexemes_[i + 1].GetText() == "(") {
            std::string name = lexemes_[i].GetText();
            int line = lexemes_[i].GetNum();
            auto func_ptr = SearchFunc(func_tid, name, line);
            if (func_ptr == nullptr) {
                error = "Function doesn't exist: ";
                error += name;
                error += " Line: ";
                error += std::to_string(line);
                throw error;
            }
            auto func = func_ptr->function;
            std::string type = func.type;
            i += 2;

            int ind_par = 0;
            if (lexemes_[i].GetText() != ")") {
                std::stack<TypeControl> type_control_parameter;
                Expression(type_control_parameter, i);
                if (!(ind_par >= 0 && ind_par < func.type_par.size())) {
                    error = "Wrong amount of parameters! Function: ";
                    error += func.id;
                    error += " Line: ";
                    error += std::to_string(line);
                    throw error;
                    return;
                }
                if (func.type_par[ind_par] != type_control_parameter.top().type && 
                    (func.type_par[ind_par] != "double" || type_control_parameter.top().type != "int")) {
                    error = "Parameter type conversion error! Function: ";
                    error += name;
                    error += " Line: ";
                    error += std::to_string(type_control_parameter.top().line);
                    throw error;
                    return;
                }
                AddingFunctionParameters(type_control, i, func, ind_par);
            }
            else {
                if (func.type_par.size() != 0) {
                    error = "Error in entering function parameters! Name: ";
                    error += func.id;
                    error += " Line: ";
                    error += std::to_string(line);
                    throw error;
                    return;
                }
                ++i;
            }
            type_control.push({ type, name, line });
        }
        else {
            auto p = SearchVar(var_tid, lexemes_[i].GetText(), lexemes_[i].GetNum());
            if (p.is_init == false) {
                error = "Variable isn't initialized: ";
                error += p.id;
                error += " Line: ";
                error += std::to_string(lexemes_[i].GetNum());
                throw error;
                return;
            }
            auto type = p.type;
            ++i;
            type_control.push({ type, lexemes_[i].GetText(), lexemes_[i].GetNum() });
        }
    }
    else if (lexemes_[i].GetText() == "(") {
        ++i; 
        Expression(type_control, i);
        ++i;
    }
}

bool SemanticsAnalyzator::Check() {
    try {
        bool is_return_exist = false;
        std::string current_function_type = "";
        std::string current_function_name = "";
        int gap_cnt = 0;

        for (int i = 0; i < lexemes_.size(); ++i) {
            auto lex = lexemes_[i].GetText();
            auto type = lexemes_[i].GetType();
            std::string error;
            if (lex == "return") {
                is_return_exist = true;
                ++i;

                auto line = lexemes_[i].GetNum();

                // проверка на пустое выражение
                if (lexemes_[i].GetText() == ";") {
                    if (current_function_type == "void") continue;
                    error = "Return mustn't be empty! ";
                    error += " Line: ";
                    error += std::to_string(line);
                    throw error;
                    break;
                }

                std::stack<TypeControl> type_control;
                Expression(type_control, i);
                auto expr_type = type_control.top().type;
                if (expr_type != current_function_type &&
                    (current_function_type != "double" || expr_type != "int")) {
                    error = "Return expression type is not equal functional type! ";
                    error += current_function_type;
                    error += " != ";
                    error += expr_type;
                    error += " Line: ";
                    error += std::to_string(line);
                    throw error;
                    break;
                }
            }
            else if (lex == "out") {
                int line = lexemes_[i].GetNum();
                i += 2;
                if (lexemes_[i].GetType() != 8) {
                    std::stack<TypeControl> type_control;
                    Expression(type_control, i);
                    if (type_control.top().type == "void") {
                        error = "Expected: int, double or bool, Got: void Line: ";
                        error += std::to_string(type_control.top().line);
                        throw error;
                        break;
                    }
                }
                else {
                    i++;
                }
                while (lexemes_[i].GetText() == "<<") {
                    i++;
                    if (lexemes_[i].GetType() != 8) {
                        std::stack<TypeControl> type_control;
                        Expression(type_control, i);
                        if (type_control.top().type == "void") {
                            error = "Expected: int, double or bool, Got: void Line: ";
                            error += std::to_string(type_control.top().line);
                            throw error;
                            break;
                        }
                    }
                    else {
                        i++;
                    }
                }
            }
            else if (lex == "in") {
                i += 2;
                auto name = lexemes_[i].GetText();
                auto line = lexemes_[i].GetNum();
                if (lexemes_[i + 1].GetText() != "[") {
                    if (!FindVar(var_tid, "none", name, true, line)) break;
                    i++;
                }
                else {
                    if (!FindVar(var_tid, "array", name, true, line)) break;
                    i += 2;
                    std::stack<TypeControl> type_control;
                    Expression(type_control, i);
                    if (type_control.empty() || type_control.top().type != "int") {
                        error = "Entered wrong array size! Array: ";
                        error += name;
                        error += " Line: ";
                        error += std::to_string(line);
                        throw error;
                        break;
                    }
                    i++;
                }
                while (lexemes_[i].GetText() == ">>") {
                    i++;
                    name = lexemes_[i].GetText();
                    line = lexemes_[i].GetNum();
                    if (lexemes_[i + 1].GetText() != "[") {
                        if (!FindVar(var_tid, "none", name, true, line)) break;
                        i++;
                    }
                    else {
                        if (!FindVar(var_tid, "array", name, true, line)) break;
                        i += 2;
                        std::stack<TypeControl> type_control;
                        Expression(type_control, i);
                        if (type_control.empty() || type_control.top().type != "int") {
                            error = "Entered wrong array size! Array: ";
                            error += name;
                            error += " Line: ";
                            error += std::to_string(line);
                            throw error;
                            break;
                        }
                        i++;
                    }
                }
            }
            else if (lex == "for") {
                gap_cnt++;
                NewVarScope();
                i += 2; // сдвиг на первое выражение
                std::stack<TypeControl> type_control;
                Expression(type_control, i);
                ++i; // сдвиг потому что ;
                auto line = lexemes_[i].GetNum();
                if (!type_control.empty()) type_control.pop();
                Expression(type_control, i);
                if (!type_control.empty() && type_control.top().type != "bool") {
                    error = "Expression must be bool! Line: ";
                    error += std::to_string(line);
                    throw error;
                    break;
                }
            }
            else if (lex == "while") {
                gap_cnt++;
                NewVarScope();
                i += 2;
                auto line = lexemes_[i].GetNum();
                std::stack<TypeControl> type_control;
                Expression(type_control, i);
                if (type_control.empty()) {
                    error = "Expression is empty! Line: ";
                    error += std::to_string(line);
                    throw error;
                    break;
                }
                if (type_control.top().type != "bool") {
                    error = "Expression must be bool! Line: ";
                    error += std::to_string(line);
                    throw error;
                    break;
                }
            }
            else if (lex == "if") {
                gap_cnt++;
                NewVarScope();
                i += 2;
                auto line = lexemes_[i].GetNum();
                std::stack<TypeControl> type_control;
                Expression(type_control, i);
                if (type_control.empty()) {
                    error = "Expression is empty! Line: ";
                    error += std::to_string(line);
                    throw error;
                    break;
                }
                if (type_control.top().type != "bool") {
                    error = "Expression must be bool! Line: ";
                    error += std::to_string(line);
                    throw error;
                    break;
                }
            }
            else if (lex == "array") {
                std::string cur_type = "array ";
                i++;
                cur_type += lexemes_[i].GetText();
                i++;
                std::string id = lexemes_[i].GetText();
                if (!FindVar(var_tid, cur_type, id, false, lexemes_[i].GetNum())) break;
                i++;
                if (lexemes_[i].GetText() == "[") {
                    i++;
                    auto array_size = lexemes_[i].GetText();
                    auto p = array_size.find('.');
                    if (p >= 0 && p < array_size.size() || std::stoi(array_size) <= 0) {
                        error = "Entered wrong array size! Array: ";
                        error += lexemes_[i].GetText();
                        error += " Line: ";
                        error += std::to_string(lexemes_[i].GetNum());
                        throw error;
                        break;
                    }
                }
                
                AddVar(id, cur_type, true);
            }
            else if (lex == "void") {
                is_return_exist = false;
                gap_cnt++;
                current_function_type = "void";

                std::string type = lex; // lex = "void"
                i++;
                lex = lexemes_[i].GetText();
                current_function_name = lex;

                // проверка на Ќ≈существование функции
                auto func = SearchFunc(func_tid, lex, lexemes_[i].GetNum());
                if (func != nullptr) {
                    error = "Function already exists: ";
                    error += lex;
                    error += " Line: ";
                    error += std::to_string(lexemes_[i].GetNum());
                    throw error;
                    break;
                }

                NewVarScope();
                i += 2;
                std::vector<std::string> pars;

                // ввод параметров функции
                if (lexemes_[i].GetText() == ")") {
                    AddFunc(lex, type, { });
                }
                else {
                    std::string param_type, param_id;
                    if (lexemes_[i].GetText() == "array") {
                        i++;
                        param_type += "array ";
                    }
                    param_type += lexemes_[i].GetText();
                    i++;
                    param_id += lexemes_[i].GetText();
                    if (!FindVar(var_tid, param_type, param_id, false, lexemes_[i].GetNum())) break;
                    AddVar(param_id, param_type, true);
                    pars.push_back(param_type);

                    i++;
                    param_type.clear();
                    param_id.clear();
                    while (lexemes_[i].GetText() != ")") {
                        i++; // сдвиг потому что зап€та€
                        if (lexemes_[i].GetText() == "array") {
                            i++;
                            param_type += "array ";
                        }
                        param_type += lexemes_[i].GetText();
                        i++;
                        param_id += lexemes_[i].GetText();
                        if (!FindVar(var_tid, param_type, param_id, false, lexemes_[i].GetNum())) break;
                        AddVar(param_id, param_type, true);
                        pars.push_back(param_type);

                        i++;
                        param_type.clear();
                        param_id.clear();
                    }
                }

                AddFunc(lex, type, pars);
            }
            else if (lex == "int" || lex == "double" || lex == "bool") {
                std::string type = lex;

                i++;
                lex = lexemes_[i].GetText();
                if (lex == "main") {
                    current_function_name = "main";
                    is_return_exist = false;
                    gap_cnt++;
                    current_function_type = type;
                    NewVarScope();
                }
                // если создание функции
                else if (lexemes_[i].GetType() == 2 && lexemes_[i + 1].GetText() == "(") {
                    current_function_name = lex;
                    is_return_exist = false;
                    gap_cnt++;
                    current_function_type = type;

                    auto func = SearchFunc(func_tid, lex, lexemes_[i].GetNum());
                    if (func != nullptr) {
                        error = "Function already exists: ";
                        error += lex;
                        error += " Line: ";
                        error += std::to_string(lexemes_[i].GetNum());
                        throw error;
                        break;
                    }

                    NewVarScope();
                    i += 2;
                    std::vector<std::string> pars;

                    // ввод параметров функции
                    if (lexemes_[i].GetText() == ")") {
                        AddFunc(lex, type, { });
                    }
                    else {
                        std::string param_type, param_id;
                        if (lexemes_[i].GetText() == "array") {
                            i++;
                            param_type += "array ";
                        }
                        param_type += lexemes_[i].GetText();
                        i++;
                        param_id += lexemes_[i].GetText();
                        if (!FindVar(var_tid, param_type, param_id, false, lexemes_[i].GetNum())) break;
                        AddVar(param_id, param_type, true);
                        pars.push_back(param_type);

                        i++;
                        param_type.clear();
                        param_id.clear();
                        while (lexemes_[i].GetText() != ")") {
                            i++; // сдвиг потому что зап€та€
                            if (lexemes_[i].GetText() == "array") {
                                i++;
                                param_type += "array ";
                            }
                            param_type += lexemes_[i].GetText();
                            i++;
                            param_id += lexemes_[i].GetText();
                            if (!FindVar(var_tid, param_type, param_id, false, lexemes_[i].GetNum())) break;
                            AddVar(param_id, param_type, true);
                            pars.push_back(param_type);

                            i++;
                            param_type.clear();
                            param_id.clear();
                        }
                        AddFunc(lex, type, pars);
                    }
                }
                // если переменна€
                else if (lexemes_[i].GetType() == 2) {
                    // инициализаци€
                    int line = lexemes_[i].GetNum();
                    if (lexemes_[i + 1].GetText() == "=") {
                        // произведено ли повторное создание переменной
                        
                        if (!FindVar(var_tid, type, lex, true, lexemes_[i].GetNum())) break;
                        if (!FindFunc(func_tid, type, lex, {}, lexemes_[i].GetNum())) break;
                        // обработка правильности выражени€ после равно
                        i += 2;
                        std::stack<TypeControl> type_control;
                        Expression(type_control, i);
                        if (type_control.top().type != type && 
                            (type != "double" || type_control.top().type != "int")) {
                            error = "Type of lvalue doesn't equals type of rvalue: ";
                            error += lex;
                            error += " != ";
                            error += type_control.top().type;
                            error += " Line: ";
                            error += std::to_string(line);
                            throw error;
                            break;
                        }
                        // добавление переменной в дерево
                        AddVar(lex, type, true);
                    }
                    // без инициализации 
                    else {
                        // произведено ли повторное создание переменной
                        if (!FindVar(var_tid, type, lex, false, lexemes_[i].GetNum())) break;
                        // добавление переменной в дерево
                        AddVar(lex, type, false);
                    }
                }
            }
            // если функци€ или переменна€ уже существуют
            else if (type == 2) {
                // если вызов функции
                if (lexemes_[i + 1].GetText() == "(") {
                    auto line = lexemes_[i].GetNum();

                    auto func_ptr = SearchFunc(func_tid, lex, line);
                    if (func_ptr == nullptr) {
                        error = "Function doesn't exist: ";
                        error += lex;
                        error += " Line: ";
                        error += std::to_string(line);
                        throw error;
                    }
                    auto func = func_ptr->function;

                    int ind_par = 0;
                    i += 2;
                    if (lexemes_[i].GetText() == ")") {
                        if (func.type_par.size() != 0) {
                            error = "Wrong amount of parameters! Name: ";
                            error += lex;
                            error += " Line: ";
                            error += std::to_string(line);
                            throw error;
                            break;
                        }
                        continue;
                    }
                    std::stack<TypeControl> type_control;
                    Expression(type_control, i);
                    if (func.type_par[ind_par] != type_control.top().type && 
                        (func.type_par[ind_par] != "double" || type_control.top().type != "int")) {
                        error = "Error in entering function parameters: ";
                        error += func.type_par[ind_par];
                        error += " != ";
                        error += type_control.top().type;
                        error += " Name: ";
                        error += lex;
                        error += " Line: ";
                        error += std::to_string(line);
                        throw error;
                        break;
                    }
                    while (lexemes_[i].GetText() != ")") {
                        ++i; // сдвиг потому что ,
                        ++ind_par;
                        type_control.pop();
                        Expression(type_control, i);
                        if (!(ind_par >= 0 && ind_par < func.type_par.size())) {
                            error = "Wrong amount of parameters! Function: ";
                            error += func.id;
                            error += " Line: ";
                            error += std::to_string(line);
                            throw error;
                            break;
                        }
                        if (func.type_par[ind_par] != type_control.top().type && 
                            (func.type_par[ind_par] != "double" || type_control.top().type != "int")) {
                            error = "Error in entering function parameters: ";
                            error += func.type_par[ind_par];
                            error += " != ";
                            error += type_control.top().type;
                            error += " Name: ";
                            error += lex;
                            error += " Line: ";
                            error += std::to_string(line);
                            throw error;
                            break;
                        }
                    }
                }
                // если элемент массива
                else if (lexemes_[i + 1].GetText() == "[") {
                    if (!FindVar(var_tid, "none", lex, false, lexemes_[i].GetNum())) break;
                    auto arr = SearchVar(var_tid, lex, lexemes_[i].GetNum());
                    i += 2;
                    int line = lexemes_[i].GetNum();
                    std::stack<TypeControl> type_control;
                    Expression(type_control, i);
                    if (type_control.empty() || type_control.top().type != arr.type) {
                        error = "Error in entering index! Line: ";
                        error = std::to_string(line);
                        throw error;
                        break;
                    }
                    i++;
                    if (lexemes_[i].GetText() == "=") {
                        i++;
                        type_control.pop();
                        Expression(type_control, i);
                        if (type_control.empty() || type_control.top().type != arr.type) {
                            error = "Type of lvalue doesn't equals type of rvalue: ";
                            error += lex;
                            error += " != ";
                            error += type_control.top().type;
                            error += " Line: ";
                            error += std::to_string(line);
                            throw error;
                            break;
                        }
                        
                    }
                }
                // если вызов переменной
                else {
                    // если приравнивание
                    if (lexemes_[i + 1].GetText() == "=") {
                        auto name = lex;
                        int line = lexemes_[i + 1].GetNum();
                        if (!FindVar(var_tid, "none", name, true, lexemes_[i].GetNum())) break;
                        // обработка выражени€ 
                        auto p = SearchVar(var_tid, name, line);
                        auto var_type = p.type;
                        i += 2;
                        std::stack<TypeControl> type_control;
                        Expression(type_control, i);
                        if (var_type != type_control.top().type && 
                            (var_type != "double" || type_control.top().type != "int")) {
                            error = "Type of lvalue doesn't equals type of rvalue: ";
                            error += lex;
                            error += " != ";
                            error += type_control.top().type;
                            error += " Line: ";
                            error += std::to_string(line);
                            throw error;
                            break;
                        }

                    }
                    // если использование как rvalue
                    else {
                        if (!FindVar(var_tid, "none", lex, false, lexemes_[i].GetNum())) break;
                    }
                }
            }
            else if (lex == "}") {
                gap_cnt--;
                if (gap_cnt == 0 && current_function_type != "void" && !is_return_exist) {
                    error = "Function must return value! Function: ";
                    error += current_function_name;
                    throw error;
                    break;
                }
                DeleteVarScope();
            }
        }
    }
    catch (std::string error) {
        std::cout << "ERROR: " << error << '\n';
        return false;
    }
    return true;
}
