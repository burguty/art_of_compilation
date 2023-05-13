#include "Poliz.h"

// Poliz 

Poliz::Poliz() {
    priority_table["unar+"] = 0, priority_table["unar-"] = 0, priority_table["++"] = 0, priority_table["--"] = 0;
    priority_table["*"] = 1, priority_table["/"] = 1, priority_table["%"] = 1;
    priority_table["+"] = 2, priority_table["-"] = 2;
    priority_table["<"] = 3, priority_table[">"] = 3, priority_table["<="] = 3, priority_table[">="] = 3;
    priority_table["=="] = 4, priority_table["<>"] = 4;
    priority_table["&"] = 5;
    priority_table["|"] = 6;
    priority_table["^"] = 7;
    priority_table["&&"] = 8;
    priority_table["||"] = 9;
    priority_table["="] = 10;
}

void Poliz::GetLexemes(std::string file_path) {
    std::ifstream is;
    is.open(file_path);
    std::string text;
    int type, num;
    char c;
    while (!is.eof()) {
        is >> text >> type >> num;
        lexemes_.push_back(Lexeme(text, type, num));
    }
    is.close();
}

element MakeOperand(std::string s, int line) {
    return { s, false, 0, line };
}

element MakeGoTo(int pos) {
    return { std::string("goto"), true, pos, 0 };
}

element MakeConditionGoTo(int pos) {
    return { std::string("condition_goto"), true, pos, 0 };
}

element MakeOperation(std::string s) {
    return { s, true, 0, 0 };
}

int Poliz::GetLastFree() {
    return poliz_.size() - 1;
}


void Poliz::TakeOperations(std::string oper) {
    while (!stack_oper_.empty() && stack_oper_.top() != "expr_start" &&
        (oper == "=" && priority_table[stack_oper_.top()] < priority_table[oper] ||
            oper != "=" && priority_table[stack_oper_.top()] <= priority_table[oper])) {
        poliz_.push_back(MakeOperation(stack_oper_.top()));
        stack_oper_.pop();
    }
}


std::vector<element> Poliz::GetPoliz() {
    return poliz_;
}

void Poliz::SetLexemes(std::vector<Lexeme> vec) {
    lexemes_ = vec;
}

void Poliz::Show() {
    for (auto tmp : lexemes_) {
        tmp.Show();
    }
}

long long ind1;

std::string Poliz::GetLex() {
    std::string str = "Lexeme was\'t found!";
    if (!(ind1 >= 0LL && ind1 < lexemes_.size())) {
        throw str;
        return "";
    }
    return lexemes_[ind1].GetText();
}

int Poliz::GetType() {
    std::string str = "Lexeme wasn\'t found!";
    if (!(ind1 >= 0LL && ind1 < lexemes_.size())) {
        throw str;
        return -1;
    }
    return lexemes_[ind1].GetType();
}

std::string Poliz::GetNum() {
    std::string str = "Lexeme wasn\'t found!";
    if (!(ind1 >= 0LL && ind1 < lexemes_.size())) {
        throw str;
        return "-1";
    }
    return std::to_string(lexemes_[ind1].GetNum());
}

void Poliz::CheckLexeme(std::string str1) {
    if (str1 != GetLex()) {
        std::string str = "Expected: ";
        str += "'";
        str += str1;
        str += "' ";
        str += "Got: ";
        str += "'";
        str += GetLex();
        str += "'";
        str += " Line: ";
        str += GetNum();
        throw str;
    }
}

void Poliz::Next() {
    ind1++;
}

void Poliz::Const() {
    poliz_.push_back(MakeOperand(GetLex(), lexemes_[ind1].GetNum()));
    Next();
}

void Poliz::String() {
    poliz_.push_back(MakeOperand(GetLex(), lexemes_[ind1].GetNum()));
    Next();
}

void Poliz::Sign() {
    if (GetLex() == "+") {
        TakeOperations("unar+");
        stack_oper_.push("unar+");
    }
    else if (GetLex() == "-") {
        TakeOperations("unar-");
        stack_oper_.push("unar-");
    }
    Next();
}

void Poliz::Bool() {
    poliz_.push_back(MakeOperand(GetLex(), lexemes_[ind1].GetNum()));
    Next();
}

void Poliz::Name() {
    poliz_.push_back(MakeOperand(GetLex(), lexemes_[ind1].GetNum()));
    Next();
}

void Poliz::Type() {
    auto lex = GetLex();
    if (lex == "int" || lex == "double" || lex == "bool") {
        Next();
    }
}

void Poliz::FunctionParameters() {
    if (GetLex() == ")") {
        Next();
        return;
    }
    CheckLexeme(",");
    Next();
    if (GetLex() == "array") {
        Next();
    }
    // Type();
    Next();
    // Name();
    Next();
    FunctionParameters();
}

void Poliz::Function() {
    // тут надо запихивать функцию в реестр функций с соответсвующими параметрами
    if (GetLex() == "void") {
        Next();
        Name();
    }
    else {
        // Type();
        Next();
        Name();
    }
    CheckLexeme("(");
    Next();
    if (GetLex() == ")") {
        Next();
    }
    else {
        if (GetLex() == "array") {
            Next();
        }
        // Type();
        Next();
        //Name();
        Next();
        FunctionParameters();
    }
    CompoundOperator();
    poliz_.push_back(MakeOperation("end_function"));
}

void Poliz::Increment() {
    auto lex = GetLex();
    TakeOperations(lex);
    stack_oper_.push(lex);
    Next();
}

void Poliz::Variable() {
    auto type = GetLex();
    Next();
    Name();
    poliz_.push_back(MakeOperation(type));
    if (GetLex() == "=") {
        Next();
        Expression();
        poliz_.push_back(MakeOperation("="));
    }
}

void Poliz::InputParameters() {
    if (GetLex() == ">>") {
        Next();
        Name();
        if (GetLex() == "[") {
            Next();
            Expression();
            CheckLexeme("]");
            Next();
            poliz_.push_back(MakeOperation("take_element_array"));
        }
        InputParameters();
    }
}

void Poliz::InputOperator() {
    CheckLexeme("in");
    Next();
    poliz_.push_back(MakeOperation("in_start"));
    CheckLexeme(">>");
    Next();
    Name();
    if (GetLex() == "[") {
        Next();
        Expression();
        CheckLexeme("]");
        Next();
        poliz_.push_back(MakeOperation("take_element_array"));
    }
    InputParameters();
    CheckLexeme(";");
    Next();
    poliz_.push_back(MakeOperation("in"));
}

void Poliz::Array() {
    CheckLexeme("array");
    Next();
    auto type = GetLex();
    poliz_.push_back(MakeOperand(type, lexemes_[ind1].GetNum()));
    Next();
    Name();
    CheckLexeme("[");
    Next();
    Const();
    CheckLexeme("]");
    Next();
    poliz_.push_back(MakeOperation("array"));
}

void Poliz::Operation1() {
    auto lex = GetLex();
    TakeOperations(lex);
    stack_oper_.push(lex);
    Next();
}

void Poliz::Operation2() {
    auto lex = GetLex();
    TakeOperations(lex);
    stack_oper_.push(lex);
    Next();
}

void Poliz::Operation3() {
    auto lex = GetLex();
    TakeOperations(lex);
    stack_oper_.push(lex);
    Next();
}

void Poliz::Operation4() {
    auto lex = GetLex();
    TakeOperations(lex);
    stack_oper_.push(lex);
    Next();
}

void Poliz::Priority0() {
    std::string str = GetLex();
    if (GetLex() == "+" || GetLex() == "-") {
        Sign();
        Adding();
    }
    else if (GetLex() == "++" || GetLex() == "--") {
        Increment();
        Adding();
    }
    else {
        Adding();
    }
}

void Poliz::Priority1() {
    Priority0();
    if (GetLex() == "*" || GetLex() == "/" || GetLex() == "%") {
        Operation1();
        Priority1();
    }
}

void Poliz::Priority2() {
    Priority1();
    if (GetLex() == "+" || GetLex() == "-") {
        Operation2();
        Priority2();
    }
}

void Poliz::Priority3() {
    Priority2();
    if (GetLex() == "<" || GetLex() == ">" || GetLex() == "<=" || GetLex() == ">=") {
        Operation3();
        Priority3();
    }
}

void Poliz::Priority4() {
    Priority3();
    if (GetLex() == "==" || GetLex() == "<>") {
        Operation4();
        Priority4();
    }
}

void Poliz::Priority5() {
    Priority4();
    if (GetLex() == "&") {
        TakeOperations("&");
        stack_oper_.push("&");
        Next();
        Priority5();
    }
}

void Poliz::Priority6() {
    Priority5();
    if (GetLex() == "|") {
        TakeOperations("|");
        stack_oper_.push("|");
        Next();
        Priority6();
    }
}

void Poliz::Priority7() {
    Priority6();
    if (GetLex() == "^") {
        TakeOperations("^");
        stack_oper_.push("^");
        Next();
        Priority7();
    }
}

void Poliz::Priority8() {
    Priority7();
    if (GetLex() == "&&") {
        TakeOperations("&&");
        stack_oper_.push("&&");
        Next();
        Priority8();
    }
}

void Poliz::Priority9() {
    Priority8();
    if (GetLex() == "||") {
        TakeOperations("||");
        stack_oper_.push("||");
        Next();
        Priority9();
    }
}

void Poliz::Priority10() {
    Priority9();
    if (GetLex() == "=") {
        TakeOperations("=");
        stack_oper_.push("=");
        Next();
        Priority10();
    }
}

void Poliz::Expression() {
    stack_oper_.push("expr_start");
    Priority10();
    while (!stack_oper_.empty() && stack_oper_.top() != "expr_start") {
        poliz_.push_back(MakeOperation(stack_oper_.top()));
        stack_oper_.pop();
    }
    stack_oper_.pop();
}

void Poliz::StartAdding() {
    if (GetType() == 3) {
        Const();
    }
    else if (GetLex() == "true" || GetLex() == "false") {
        Bool();
    }
    else if (GetLex() == "!") {
        stack_oper_.push("!");
        Next();
        Adding();
    }
}

void Poliz::AddingFunctionParameters() {
    if (GetLex() == ")") {
        Next();
        return;
    }
    CheckLexeme(",");
    Next();
    Expression();
    AddingFunctionParameters();
}

void Poliz::Adding() {
    std::string str = GetLex();
    if (GetType() == 3 || GetLex() == "true" || GetLex() == "false" || GetLex() == "!") {
        StartAdding();
    }
    else if (GetType() == 2) {
        auto name = GetLex();
        auto line = lexemes_[ind1].GetNum();
        Next();
        if (GetLex() == "[") {
            Next();
            poliz_.push_back(MakeOperand(name, line));
            Expression();
            CheckLexeme("]");
            Next();
            poliz_.push_back(MakeOperation("take_element_array"));
        }
        else if (GetLex() == "(") {
            Next();
            if (GetLex() != ")") {
                Expression();
                AddingFunctionParameters();
            }
            else {
                Next();
            }
            poliz_.push_back(MakeOperation(name));
        }
        else
        {
            poliz_.push_back(MakeOperand(name, line));
        }
    }
    else if (GetLex() == "(") {
        Next();
        Expression();
        CheckLexeme(")");
        Next();
    }
}

void Poliz::DefinitionParameters() {
    if (GetLex() == ",") {
        Next();
        if (GetLex() == "array") {
            Array();
        }
        else {
            Variable();
        }
        DefinitionParameters();
    }
}

void Poliz::Definition() {
    if (GetLex() == "array") {
        Array();
    }
    else {
        Variable();
    }
    DefinitionParameters();
}

void Poliz::DefinitionOperator() {
    Definition();
    CheckLexeme(";");
    Next();
}

void Poliz::OutputOperatorParameters() {
    if (GetLex() == "<<") {
        Next();
        if (GetType() == 8) {
            String();
        }
        else {
            Expression();
        }
        OutputOperatorParameters();
    }
}

void Poliz::OutputOperator() {
    CheckLexeme("out");
    Next();
    poliz_.push_back(MakeOperation("out_start"));
    CheckLexeme("<<");
    Next();
    if (GetType() == 8) {
        String();
    }
    else {
        Expression();
    }
    OutputOperatorParameters();
    CheckLexeme(";");
    Next();
    poliz_.push_back(MakeOperation("out"));
}

void Poliz::ReturnOperator() {
    CheckLexeme("return");
    Next();
    auto lex = GetLex();
    if (!lex.empty() && (lex == "+" || lex == "-" ||
        GetType() == 3 || lex == "true" || lex == "false" ||
        lex == "!" || lex == "++" || lex == "--" || lex == "(" || lex == "array" || GetType() == 2)) {
        Expression();
        poliz_.push_back(MakeOperation("return"));
        CheckLexeme(";");
        Next();
    }
    else {
        CheckLexeme(";");
        Next();
        poliz_.push_back(MakeOperation("return_void"));
    }
}

void Poliz::ExpressionOperator() {
    Expression();
    CheckLexeme(";");
    Next();
    poliz_.push_back(MakeOperation(";"));
}

std::vector<int> positions_for_goto;

void Poliz::ElseIf() {
    if (GetLex() == "else") {
        Next();
        if (GetLex() != "if") {
            CompoundOperator();
            poliz_.push_back({});
            positions_for_goto.push_back(GetLastFree());

            for (int pos : positions_for_goto) {
                poliz_[pos] = MakeGoTo(GetLastFree() + 1);
            }
            positions_for_goto.clear();
            return;
        }
        Next();
        CheckLexeme("(");
        Next();
        Expression();
        CheckLexeme(")");
        Next();
        poliz_.push_back({});
        int pos_goto = GetLastFree();
        CompoundOperator();
        poliz_.push_back({});
        positions_for_goto.push_back(GetLastFree());
        poliz_[pos_goto] = MakeConditionGoTo(GetLastFree() + 1);
        ElseIf();
    }
    for (int pos : positions_for_goto) {
        poliz_[pos] = MakeGoTo(GetLastFree() + 1);
    }
    positions_for_goto.clear();
}

void Poliz::IfOperator() {
    positions_for_goto.clear();
    CheckLexeme("if");
    Next();
    CheckLexeme("(");
    Next();
    Expression();
    CheckLexeme(")");
    Next();
    poliz_.push_back({});
    int pos_goto = GetLastFree();
    CompoundOperator();
    poliz_.push_back({});
    positions_for_goto.push_back(GetLastFree());
    poliz_[pos_goto] = MakeConditionGoTo(GetLastFree() + 1);
    ElseIf();
}

void Poliz::FunctionCallOperatorParameters() {
    if (GetLex() == ",") {
        Next();
        Expression();
        FunctionCallOperatorParameters();
    }
}

void Poliz::FunctionCallOperator() {
    //Name();
    auto name = GetLex();
    Next();
    CheckLexeme("(");
    Next();
    if (GetLex() != ")") {
        Expression();
        FunctionCallOperatorParameters();
        CheckLexeme(")");
        Next();
    }
    poliz_.push_back(MakeOperation(name));
    CheckLexeme(";");
    Next();
}

void Poliz::ForOperator() {
    CheckLexeme("for");
    Next();
    CheckLexeme("(");
    Next();
    if (GetLex() == "int" || GetLex() == "double" || GetLex() == "bool") {
        auto type = GetLex();
        Next();
        Name();
        poliz_.push_back(MakeOperation(type));
        CheckLexeme("=");
        Next();
        Expression();
        poliz_.push_back(MakeOperation("="));
    }
    else if (GetLex() == ";") {

    }
    else {
        Expression();
        poliz_.push_back(MakeOperation(";"));
    }
    CheckLexeme(";");
    Next();
    int pos_e2 = GetLastFree() + 1;
    if (GetLex() == ";") {
        poliz_.push_back(MakeOperand("true", lexemes_[ind1].GetNum()));
    }
    else {
        Expression();
    }
    CheckLexeme(";");
    Next();
    poliz_.push_back({});
    int pos_condition_goto = GetLastFree();
    poliz_.push_back({});
    int pos_goto_1 = GetLastFree();
    int pos_e3 = GetLastFree() + 1;
    if (GetLex() == ")") {

    }
    else {
        Expression();
        poliz_.push_back(MakeOperation(";"));
    }
    poliz_.push_back(MakeGoTo(pos_e2));
    CheckLexeme(")");
    Next();
    int pos_s = GetLastFree() + 1;
    CompoundOperator();
    poliz_.push_back(MakeGoTo(pos_e3));
    poliz_[pos_goto_1] = MakeGoTo(pos_s);
    poliz_[pos_condition_goto] = MakeConditionGoTo(GetLastFree() + 1);
}

void Poliz::WhileOperator() {
    CheckLexeme("while");
    Next();
    CheckLexeme("(");
    Next();
    int pos_start_expression = GetLastFree() + 1;
    Expression();
    poliz_.push_back({});
    int pos_goto = GetLastFree();
    CheckLexeme(")");
    Next();
    CompoundOperator();
    poliz_.push_back(MakeGoTo(pos_start_expression));
    poliz_[pos_goto] = MakeConditionGoTo(GetLastFree() + 1);
}

void Poliz::CycleOperator() {
    if (GetLex() == "for") {
        ForOperator();
    }
    else if (GetLex() == "while") {
        WhileOperator();
    }
    else {
        std::string str = "Error in cycle: ";
        str += GetLex();
        str += " Line: ";
        str += GetNum();
        throw str;
    }
}

void Poliz::Operator() {
    auto lex = GetLex();
    if (lex == "for" || lex == "while") {
        CycleOperator();
    }
    else if (lex == "if") {
        IfOperator();
    }
    else if (lex == "in") {
        InputOperator();
    }
    else if (lex == "out") {
        OutputOperator();
    }
    else if (lex == "return") {
        ReturnOperator();
    }
    else if (lex == "array" || lex == "int" || lex == "double" || lex == "bool") {
        DefinitionOperator();
    }
    else if (!lex.empty() && (lex == "+" || lex == "-" ||
        GetType() == 3 || lex == "true" || lex == "false" ||
        lex == "!" || lex == "++" || lex == "--" || lex == "(" || lex == "array")) {
        ExpressionOperator();
    }
    else if (GetType() == 2) {
        Next();
        if (ind1 >= lexemes_.size()) {
            --ind1;
            ExpressionOperator();
        }
        else if (GetLex() == "(") {
            --ind1;
            FunctionCallOperator();
        }
        else {
            --ind1;
            ExpressionOperator();
        }
    }
}

bool Poliz::CheckOperator() {
    auto lex = GetLex();
    return GetType() == 2 || !lex.empty() && (lex == "+" || lex == "-" ||
        GetType() == 3 || lex == "true" || lex == "false" ||
        lex == "!" || lex == "++" || lex == "--" || lex == "_" || lex == "(" ||
        lex == "array" || lex == "for" || lex == "while" || lex == "int" ||
        lex == "double" || lex == "bool" ||
        lex == "return" || lex == "in" || lex == "out" || lex == "if");
}

void Poliz::CompoundOperatorParameters() {
    if (CheckOperator()) {
        Operator();
        CompoundOperatorParameters();
    }
}

void Poliz::CompoundOperator() {
    CheckLexeme("{");
    Next();
    CompoundOperatorParameters();
    CheckLexeme("}");
    Next();
}

void Poliz::ProgramParameters() {
    if (GetLex() == "void") {
        Function();
        ProgramParameters();
    }
    else {
        if (GetLex() == "array") {
            Definition();
            ProgramParameters();
        }
        else {
            if (GetLex() == "int") {
                ++ind1;
                if (GetLex() == "main") {
                    return;
                }
                --ind1;
            }
            Next();
            Next();

            if (GetLex() == "(") {
                ind1 -= 2;
                Function();
                ProgramParameters();
            }
            else if (GetLex() == "[" || GetLex() == "," || GetLex() == ";" || GetLex() == "=") {
                if (GetLex() == "[") {
                    ind1 -= 3;
                }
                else {
                    ind1 -= 2;
                }
                Definition();
                CheckLexeme(";");
                Next();
                ProgramParameters();
            }
        }
    }
}

void Poliz::Program() {
    ProgramParameters();
    if (GetLex() == "main") {
        poliz_.push_back(MakeOperand(std::string("main"), lexemes_[ind1].GetNum()));
        Next();
        CheckLexeme("(");
        Next();
        CheckLexeme(")");
        Next();
        CompoundOperator();
        poliz_.push_back(MakeOperand(std::string("end_main"), lexemes_[ind1 - 1].GetNum()));
    }
}

void Poliz::MakePoliz() {
    ind1 = 0;
    Program();
}
