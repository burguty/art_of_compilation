#include "Lexeme.h"

// Lexeme
Lexeme::Lexeme(std::string text, int type, int num) :
    text_(text), type_(type), num_(num) {}

void Lexeme::Show() {
    std::cout << text_ << ' ' << type_ << ' ' << num_ << '\n';
}

std::string Lexeme::GetText() {
    return text_;
}

int Lexeme::GetType() {
    return type_;
}

int Lexeme::GetNum() {
    return num_;
}