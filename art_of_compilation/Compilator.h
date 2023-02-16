#pragma once
#include "LexicalAnalyzator.h"
#include "SyntaxAnalyzator.h"

class Compilator {
public:
    Compilator() = default;
    ~Compilator() = default;

    bool Check();
private:
    LexicalAnalyzator lexical_analyzator_;
    SyntaxAnalyzator syntax_analyzator_;
};