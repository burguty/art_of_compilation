#include "SyntaxAnalyzator.h"

// SyntaxAnalyzator 
void SyntaxAnalyzator::GetLexemes(std::string file_path) {
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

void SyntaxAnalyzator::SetLexemes(std::vector<Lexeme> vec) {
    lexemes_ = vec;
}

void SyntaxAnalyzator::Show() {
    for (auto tmp : lexemes_) {
        tmp.Show();
    }
}

long long ind;

std::string SyntaxAnalyzator::GetLex() {
    std::string str = "Lexeme was\'t found!";
    if (!(ind >= 0LL && ind < lexemes_.size())) {
        throw str;
        return "";
    }
    return lexemes_[ind].GetText();
}

int SyntaxAnalyzator::GetType() {
    std::string str = "Lexeme wasn\'t found!";
    if (!(ind >= 0LL && ind < lexemes_.size())) {
        throw str;
        return -1;
    }
    return lexemes_[ind].GetType();
}

std::string SyntaxAnalyzator::GetNum() {
    std::string str = "Lexeme wasn\'t found!";
    if (!(ind >= 0LL && ind < lexemes_.size())) {
        throw str;
        return "-1";
    }
    return std::to_string(lexemes_[ind].GetNum());
}

void SyntaxAnalyzator::CheckLexeme(std::string str1) {
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

void SyntaxAnalyzator::Next() {
    ind++;
}

void SyntaxAnalyzator::Const() {
    if (GetType() == 3) {
        Next();
    }
    else {
        std::string error = "Isn\'t const: ";
        error += GetLex();
        error += " Line: ";
        error += GetNum();
        throw error;
    }
}

void SyntaxAnalyzator::String() {
    if (GetType() == 8) {
        Next();
        return;
    }
    std::string str = "Isn\'t string: ";
    str += GetLex();
    str += " Line: ";
    str += GetNum();
    throw str;
}

void SyntaxAnalyzator::Sign() {
    if (GetLex() != "+" && GetLex() != "-") {
        std::string str = "Expected: '+' or '-' Got: ";
        str += GetLex();
        str += " Line: ";
        str += GetNum();
        throw str;
    }
    Next();
}

void SyntaxAnalyzator::Bool() {
    auto lex = GetLex();
    if (lex == "true" || lex == "false") {
        Next();
    }
    else {
        std::string str = "Expected: \"true\" or \"false\" Got: ";
        str += GetLex();
        str += " Line: ";
        str += GetNum();
        throw str;
    }
}

void SyntaxAnalyzator::Name() {
    if (GetType() == 2) {
        Next();
    }
    else {
        std::string str = "Isn\'t name: ";
        str += GetLex();
        str += " Line: ";
        str += GetNum();
        throw str;
    }
}

void SyntaxAnalyzator::Type() {
    auto lex = GetLex();
    if (lex == "int" || lex == "double" || lex == "bool") {
        Next();
    }
    else {
        std::string str = "Expected: \"int\", \"double\" or \"bool\" Got: ";
        str += GetLex();
        str += " Line: ";
        str += GetNum();
        throw str;
    }
}

void SyntaxAnalyzator::FunctionParameters() {
    if (GetLex() == ")") {
        Next();
        return;
    }
    CheckLexeme(",");
    Next();
    if (GetLex() == "array") {
        Next();
    }
    Type();
    Name();
    FunctionParameters();
}

void SyntaxAnalyzator::Function() {
    if (GetLex() == "void") {
        Next();
        Name();
    }
    else {
        Type();
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
        Type();
        Name();
        FunctionParameters();
    }
    CompoundOperator();
}

void SyntaxAnalyzator::Increment() {
    auto lex = GetLex();
    if (lex == "++" || lex == "--") {
        Next();
    }
    else {
        std::string error = "Expected: \'++\' or \'--\' Got: ";
        error += GetLex();
        error += " Line: ";
        error += GetNum();
        throw error;
    }
}

void SyntaxAnalyzator::Variable() {
    Type();
    Name();
    if (GetLex() == "=") {
        Next();
        Expression();
    }
}

void SyntaxAnalyzator::InputParameters() {
    if (GetLex() == ">>") {
        Next();
        Name();
        if (GetLex() == "[") {
            Next();
            Expression();
            CheckLexeme("]");
            Next();
        }
        InputParameters();
    }
}

void SyntaxAnalyzator::InputOperator() {
    CheckLexeme("in");
    Next();
    CheckLexeme(">>");
    Next();
    Name();
    if (GetLex() == "[") {
        Next();
        Expression();
        CheckLexeme("]");
        Next();
    }
    InputParameters();
    CheckLexeme(";");
    Next();
}

void SyntaxAnalyzator::Array() {
    CheckLexeme("array");
    Next();
    Type();
    Name();
    CheckLexeme("[");
    Next();
    Const();
    CheckLexeme("]");
    Next();
}

void SyntaxAnalyzator::Operation1() {
    auto lex = GetLex();
    if (lex != "*" && lex != "/" && lex != "%") {
        std::string error = "Expected: \'*\', \'/\' or \'%\' Got: ";
        error += lex;
        error += " Line: ";
        error += GetNum();
        throw error;
    }
    Next();
}

void SyntaxAnalyzator::Operation2() {
    auto lex = GetLex();
    if (lex != "+" && lex != "-") {
        std::string error = "Expected: \'+\' or \'-\' Got: ";
        error += lex;
        error += " Line: ";
        error += GetNum();
        throw error;
    }
    Next();
}

void SyntaxAnalyzator::Operation3() {
    auto lex = GetLex();
    if (lex != "<" && lex != ">" && lex != "<=" && lex != ">=") {
        std::string error = "Expected: \'<\', \'>\', \'<=\' or \'>=\' Got: ";
        error += lex;
        error += " Line: ";
        error += GetNum();
        throw error;
    }
    Next();
}

void SyntaxAnalyzator::Operation4() {
    auto lex = GetLex();
    if (lex != "==" && lex != "<>") {
        std::string str = "Expected: \'==\' or \'<>\' Got: ";
        str += GetLex();
        str += " Line: ";
        str += GetNum();
        throw str;
    }
    Next();
}

void SyntaxAnalyzator::Priority0() {
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

void SyntaxAnalyzator::Priority1() {
    Priority0();
    if (GetLex() == "*" || GetLex() == "/" || GetLex() == "%") {
        Operation1();
        Priority1();
    }
}

void SyntaxAnalyzator::Priority2() {
    Priority1();
    if (GetLex() == "+" || GetLex() == "-") {
        Operation2();
        Priority2();
    }
}

void SyntaxAnalyzator::Priority3() {
    Priority2();
    if (GetLex() == "<" || GetLex() == ">" || GetLex() == "<=" || GetLex() == ">=") {
        Operation3();
        Priority3();
    }
}

void SyntaxAnalyzator::Priority4() {
    Priority3();
    if (GetLex() == "==" || GetLex() == "<>") {
        Operation4();
        Priority4();
    }
}

void SyntaxAnalyzator::Priority5() {
    Priority4();
    if (GetLex() == "&") {
        Next();
        Priority5();
    }
}

void SyntaxAnalyzator::Priority6() {
    Priority5();
    if (GetLex() == "|") {
        Next();
        Priority6();
    }
}

void SyntaxAnalyzator::Priority7() {
    Priority6();
    if (GetLex() == "^") {
        Next();
        Priority7();
    }
}

void SyntaxAnalyzator::Priority8() {
    Priority7();
    if (GetLex() == "&&") {
        Next();
        Priority8();
    }
}

void SyntaxAnalyzator::Priority9() {
    Priority8();
    if (GetLex() == "||") {
        Next();
        Priority9();
    }
}

void SyntaxAnalyzator::Priority10() {
    Priority9();
    if (GetLex() == "=") {
        Next();
        Priority10();
    }
}

void SyntaxAnalyzator::Expression() {
    Priority10();
}

void SyntaxAnalyzator::StartAdding() {
    if (GetType() == 3) {
        Const();
    }
    else if (GetLex() == "true" || GetLex() == "false") {
        Bool();
    }
    else if (GetLex() == "!") {
        Next();
        Adding();
    }
    else {
        std::string str = "Empty adding!";
        str += " Line: ";
        str += GetNum();
        throw str;
    }
}

void SyntaxAnalyzator::AddingFunctionParameters() {
    if (GetLex() == ")") {
        Next();
        return;
    }
    CheckLexeme(",");
    Next();
    Expression();
    AddingFunctionParameters();
}

void SyntaxAnalyzator::Adding() {
    std::string str = GetLex();
    if (GetType() == 3 || GetLex() == "true" || GetLex() == "false" || GetLex() == "!") {
        StartAdding();
    }
    else if (GetType() == 2) {
        Name();
        if (GetLex() == "[") {
            Next();
            Expression();
            CheckLexeme("]");
            Next();
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
        }
    }
    else if (GetLex() == "(") {
        Next();
        Expression();
        CheckLexeme(")");
        Next();
    }
    else {
        std::string error = "Error in adding! Got: ";
        error += GetLex();
        error += " Line: ";
        error += GetNum();
        throw error;
    }
}

void SyntaxAnalyzator::DefinitionParameters() {
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

void SyntaxAnalyzator::Definition() {
    if (GetLex() == "array") {
        Array();
    }
    else {
        Variable();
    }
    DefinitionParameters();
}

void SyntaxAnalyzator::DefinitionOperator() {
    Definition();
    CheckLexeme(";");
    Next();
}

void SyntaxAnalyzator::OutputOperatorParameters() {
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

void SyntaxAnalyzator::OutputOperator() {
    CheckLexeme("out");
    Next();
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
}

void SyntaxAnalyzator::ReturnOperator() {
    CheckLexeme("return");
    Next();
    auto lex = GetLex();
    if (!lex.empty() && (lex == "+" || lex == "-" ||
        lex >= "0" && lex <= "9" || lex == "true" || lex == "false" ||
        lex == "!" || lex == "++" || lex == "--" || lex == "(" || lex == "array")) {
        ExpressionOperator();
    }
    else if (GetType() == 2) {
        Next();
        if (ind >= lexemes_.size()) {
            --ind;
            ExpressionOperator();
        }
        else if (GetLex() == "(") {
            --ind;
            FunctionCallOperator();
        }
        else {
            --ind;
            ExpressionOperator();
        }
    }
    else {
        CheckLexeme(";");
        Next();
    }
}

void SyntaxAnalyzator::ExpressionOperator() {
    Expression();
    CheckLexeme(";");
    Next();
}

void SyntaxAnalyzator::ElseIf() {
    if (GetLex() == "else") {
        Next();
        if (GetLex() != "if") {
            CompoundOperator();
            return;
        }
        Next();
        CheckLexeme("(");
        Next();
        Expression();
        CheckLexeme(")");
        Next();
        CompoundOperator();
        ElseIf();
    }
}

void SyntaxAnalyzator::IfOperator() {
    CheckLexeme("if");
    Next();
    CheckLexeme("(");
    Next();
    Expression();
    CheckLexeme(")");
    Next();
    CompoundOperator();
    ElseIf();
}

void SyntaxAnalyzator::FunctionCallOperatorParameters() {
    if (GetLex() == ",") {
        Next();
        Expression();
        FunctionCallOperatorParameters();
    }
}

void SyntaxAnalyzator::FunctionCallOperator() {
    Name();
    CheckLexeme("(");
    Next();
    if (GetLex() != ")") {
        Expression();
        FunctionCallOperatorParameters();
        CheckLexeme(")");
        Next();
    }
    CheckLexeme(";");
    Next();
}

void SyntaxAnalyzator::ForOperator() {
    CheckLexeme("for");
    Next();
    CheckLexeme("(");
    Next();
    if (GetLex() == "int" || GetLex() == "double" || GetLex() == "bool") {
        Type();
        Name();
        CheckLexeme("=");
        Next();
        Expression();
    }
    else if (GetLex() == ";") {

    }
    else {
        Expression();
    }
    CheckLexeme(";");
    Next();
    if (GetLex() == ";") {

    }
    else {
        Expression();
    }
    CheckLexeme(";");
    Next();
    if (GetLex() == ")") {

    }
    else {
        Expression();
    }
    CheckLexeme(")");
    Next();
    CompoundOperator();
}

void SyntaxAnalyzator::WhileOperator() {
    CheckLexeme("while");
    Next();
    CheckLexeme("(");
    Next();
    Expression();
    CheckLexeme(")");
    Next();
    CompoundOperator();
}

void SyntaxAnalyzator::CycleOperator() {
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

void SyntaxAnalyzator::Operator() {
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
        lex >= "0" && lex <= "9" || lex == "true" || lex == "false" ||
        lex == "!" || lex == "++" || lex == "--" || lex == "(" || lex == "array")) {
        ExpressionOperator();
    }
    else if (GetType() == 2) {
        Next();
        if (ind >= lexemes_.size()) {
            --ind;
            ExpressionOperator();
        }
        else if (GetLex() == "(") {
            --ind;
            FunctionCallOperator();
        }
        else {
            --ind;
            ExpressionOperator();
        }
    }
}

bool SyntaxAnalyzator::CheckOperator() {
    auto lex = GetLex();
    return GetType() == 2 || !lex.empty() && (lex == "+" || lex == "-" ||
        lex >= "0" && lex <= "9" || lex == "true" || lex == "false" ||
        lex == "!" || lex == "++" || lex == "--"  || lex == "_" || lex == "(" || 
        lex == "array" || lex == "for" || lex == "while" || lex == "int" || 
        lex == "double" || lex == "bool" ||
        lex == "return" || lex == "in" || lex == "out" || lex == "if");
}

void SyntaxAnalyzator::CompoundOperatorParameters() {
    if (CheckOperator()) {
        Operator();
        CompoundOperatorParameters();
    }
}

void SyntaxAnalyzator::CompoundOperator() {
    CheckLexeme("{");
    Next();
    CompoundOperatorParameters();
    CheckLexeme("}");
    Next();
}

void SyntaxAnalyzator::ProgramParameters() {
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
                ++ind;
                if (GetLex() == "main") {
                    return;
                }
                --ind;
            }
            Type();
            Name();
            --ind;
            if (GetLex() == "main") {
                --ind;
                CheckLexeme("int");
                ++ind;
            }
            ++ind;

            if (GetLex() == "(") {
                ind -= 2;
                Function();
                ProgramParameters();
            }
            else if (GetLex() == "[" || GetLex() == "," || GetLex() == ";" || GetLex() == "=") {
                if (GetLex() == "[") {
                    ind -= 3;
                }
                else {
                    ind -= 2;
                }
                Definition();
                CheckLexeme(";");
                Next();
                ProgramParameters();
            }
        }
    }
}

void SyntaxAnalyzator::Program() {
    ProgramParameters();
    if (GetLex() == "main") {
        Next();
        CheckLexeme("(");
        Next();
        CheckLexeme(")");
        Next();
        CompoundOperator();
    }
    else {
        std::string str = "There is no \"main\" function!";
        str += " Line: ";
        str += GetNum();
        throw str;
    }
}

bool SyntaxAnalyzator::Check() {
    ind = 0;
    try {
        if (lexemes_.empty()) {
            std::string error = "Empty program!";
            throw error;
        }
        else {
            Program();
            if (ind != lexemes_.size()) {
                std::string error = "Extra symbols: ";
                error += GetLex();
                error += " Line: ";
                error += GetNum();
                throw error;
            }
        }
    }
    catch (std::string str) {
        std::cout << "ERROR: " << str << '\n';
        return false;
    }
    return true;
}
