#include "Compilator.h"

bool Compilator::Check() {
    bool fl = false;
    auto lexemes = lexical_analyzator_.Analiz(fl);
    /*for (int i = 0; i < lexemes.size(); ++i) {
        lexemes[i].Show();
    }*/
    if (!fl) {
        return false;
    }
    syntax_analyzator_.SetLexemes(lexemes);
    return syntax_analyzator_.Check();
}