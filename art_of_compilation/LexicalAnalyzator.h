#pragma once
#include "Lexeme.h"
#include <set>

class LexicalAnalyzator {
public:
	LexicalAnalyzator() = default;
	~LexicalAnalyzator() = default;

	bool Letter(const char& a);
	bool Number(const char& a);
	bool NumAndLetAndUnderscore(const char& a);
	bool Punctuation(const char& a);
	bool Operation(const char& a);
	bool DoubleOperation(const std::string& a);
	std::vector<Lexeme> Analiz(bool& fl);
};
