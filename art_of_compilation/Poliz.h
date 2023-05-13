#pragma once
#include "Lexeme.h"
#include <stack>
#include <map>

struct element {
    std::string text;
    bool is_oper;
    int goto_index;
    int line;
};

class Poliz {
public:
    Poliz();
    ~Poliz() = default;
    void GetLexemes(std::string file_path);
    void SetLexemes(std::vector<Lexeme> vec);
    std::vector<element> GetPoliz();
    void Show();
    void MakePoliz();
    void CheckLexeme(std::string str);
    std::string GetLex();
    int GetType();
    std::string GetNum();
    void Next();
    bool CheckOperator();
    int GetLastFree();
    void TakeOperations(std::string oper);

    void ProgramParameters(); 
    void Program(); //
    void DefinitionOperator(); //
    void DefinitionParameters(); //
    void Definition(); //
    void FunctionParameters(); //
    void Function(); //
    void CompoundOperatorParameters(); //
    void CompoundOperator(); //
    void Operator(); //
    void InputOperator(); //
    void OutputOperator(); //
    void OutputOperatorParameters(); //
    void ReturnOperator(); //
    void ExpressionOperator(); //
    void CycleOperator(); 
    void ElseIf(); //
    void IfOperator(); //
    void FunctionCallOperatorParameters(); 
    void FunctionCallOperator(); 
    void Expression(); //
    void Const(); //
    void Sign(); //
    void Bool(); //
    void StartAdding(); //
    void AddingFunctionParameters(); //
    void Adding(); //
    void Name();  //
    void Priority0(); //
    void Priority1(); //
    void Priority2(); //
    void Priority3(); //
    void Priority4(); //
    void Priority5(); //
    void Priority6(); //
    void Priority7(); //
    void Priority8(); //
    void Priority9(); //
    void Priority10(); //
    void Operation1(); //
    void Operation2(); //
    void Operation3();  //
    void Operation4(); //
    void Variable(); //
    void Array(); //
    void ForOperator(); 
    void WhileOperator(); 
    void Type();  //
    void Increment(); //
    void String(); //
    void InputParameters(); //
private:
    std::vector<Lexeme> lexemes_;
    std::vector<element> poliz_;
    std::stack<std::string> stack_oper_;
    std::map<std::string, int> priority_table;
};

