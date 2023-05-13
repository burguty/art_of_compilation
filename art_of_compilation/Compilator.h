#pragma once
#include "LexicalAnalyzator.h"
#include "SyntaxAnalyzator.h"
#include "SemanticsAnalyzator.h"
#include "Poliz.h"

class Compilator {
public:
    Compilator() = default;
    ~Compilator() = default;

    bool Check();

private:
    LexicalAnalyzator lexical_analyzator_;
    SyntaxAnalyzator syntax_analyzator_;
    SemanticsAnalyzator semantics_analyzator_;
    Poliz poliz_;
};