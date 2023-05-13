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
    semantics_analyzator_.SetLexemes(lexemes);
    poliz_.SetLexemes(lexemes);
    if (syntax_analyzator_.Check() == false) return false;
    if (semantics_analyzator_.Check() == false) return false;
    
    poliz_.MakePoliz();
    auto poliz = poliz_.GetPoliz();
    for (int i = 0; i < poliz.size(); ++i) {
        element e = poliz[i];
        std::cout << i << ' ' << e.text << ' ' << e.is_oper << ' ' << e.goto_index << ' ' << e.line << '\n';
    }
    return true;
}