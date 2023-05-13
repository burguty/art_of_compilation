#pragma once
#include "Lexeme.h"
#include <stack>

struct Variable {
    std::string id;
    std::string type;
    bool is_init = false;
};

struct Function {
    std::string id;
    std::string type;
    std::vector<std::string> type_par;
};

struct NodeV {
    std::vector<Variable> vars;
    NodeV* next = nullptr;
    NodeV* prev = nullptr;
};

struct NodeF {
    Function function;
    NodeF* next = nullptr;
    NodeF* prev = nullptr;
};

struct TypeControl {
    std::string type;
    std::string id;
    int line;
};

class SemanticsAnalyzator {
public:
    SemanticsAnalyzator() = default;
    ~SemanticsAnalyzator() = default;
    void SetLexemes(std::vector<Lexeme> l);

    bool Check();
    bool FindVar(NodeV*& cur, 
        std::string type, std::string id,
        bool init, int line);
    bool FindFunc(NodeF*& cur,
        std::string type, std::string id,
        std::vector<std::string> p, int line);
    Variable SearchVar(NodeV*& cur, std::string id, int line);
    NodeF* SearchFunc(NodeF*& cur, std::string id, int line);
    void VarGo(NodeV*& cur);
    // void FuncGo(NodeF*& cur);


    void CheckBin(std::stack<TypeControl>& type_control);
    void CheckUno(std::stack<TypeControl>& type_control);

    void Priority0(std::stack<TypeControl>& type_control, int& i);
    void Priority1(std::stack<TypeControl>& type_control, int& i);
    void Priority2(std::stack<TypeControl>& type_control, int& i);
    void Priority3(std::stack<TypeControl>& type_control, int& i);
    void Priority4(std::stack<TypeControl>& type_control, int& i);
    void Priority5(std::stack<TypeControl>& type_control, int& i);
    void Priority6(std::stack<TypeControl>& type_control, int& i);
    void Priority7(std::stack<TypeControl>& type_control, int& i);
    void Priority8(std::stack<TypeControl>& type_control, int& i);
    void Priority9(std::stack<TypeControl>& type_control, int& i);
    void Priority10(std::stack<TypeControl>& type_control, int& i);
    void Operation1(std::stack<TypeControl>& type_control, int& i);
    void Operation2(std::stack<TypeControl>& type_control, int& i);
    void Operation3(std::stack<TypeControl>& type_control, int& i);
    void Operation4(std::stack<TypeControl>& type_control, int& i);
    void Expression(std::stack<TypeControl>& type_control, int& i);
    void StartAdding(std::stack<TypeControl>& type_control, int& i);
    void AddingFunctionParameters(std::stack<TypeControl>& type_control, 
        int& i, Function f, int& ind_par);
    void Adding(std::stack<TypeControl>& type_control, int& i);
private:
    std::vector<Lexeme> lexemes_;
};