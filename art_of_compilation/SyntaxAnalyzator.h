#pragma once
#include "Lexeme.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

class SyntaxAnalyzator {
public:
    SyntaxAnalyzator() = default;
    ~SyntaxAnalyzator() = default;
    void GetLexemes(std::string file_path);
    void SetLexemes(std::vector<Lexeme> vec);
    void Show();
    bool Check();
    void CheckLexeme(std::string str);
    std::string GetLex();
    int GetType();
    std::string GetNum();
    void Next();
    bool CheckOperator();

    void ProgramParameters(); //
    void Program(); //
    void DefinitionOperator(); //
    void DefinitionParameters(); //
    void Definition(); // описание
    void FunctionParameters(); //
    void Function(); //
    void CompoundOperatorParameters(); //
    void CompoundOperator(); //
    void Operator();
    void InputOperator(); //
    void OutputOperator(); //
    void OutputOperatorParameters(); //
    void ReturnOperator(); //
    void ExpressionOperator(); //
    void CycleOperator(); //
    void ElseIf(); //
    void IfOperator(); //
    void FunctionCallOperatorParameters(); //
    void FunctionCallOperator(); //
    void Expression(); //
    void Const(); //
    void Sign(); //
    void Bool(); //
    void StartAdding(); // нач объ€вление
    void AddingFunctionParameters(); //
    void Adding(); // объ€вление
    void Name(); // 
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
    void Operation3(); // 
    void Operation4(); //
    void Variable(); //
    void Array(); //
    void ForOperator(); //
    void WhileOperator(); //
    void Type(); // 
    void Increment(); //
    void String(); //
    void InputParameters(); //
private:
    std::vector<Lexeme> lexemes_;
};

