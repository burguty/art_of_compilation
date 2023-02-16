#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

class Lexeme {
public:
    Lexeme() = default;
    ~Lexeme() = default;
    Lexeme(std::string text, int type, int num);
    std::string GetText();
    int GetType();
    int GetNum();
    void Show();
private:
    std::string text_;
    int type_;
    int num_;
};
