#include "LexicalAnalyzator.h"
#include <fstream>
#include <set>
#include <vector>

bool LexicalAnalyzator::Letter(const char& a) {
	if ((a - 'a' >= 0 && a - 'a' < 26) || (a - 'A' >= 0 && a - 'A' < 26)) {
		return true;
	}
	return false;
}

bool LexicalAnalyzator::Number(const char& a) {
	if (a - '0' >= 0 && a - '0' <= 9) {
		return true;
	}
	return false;
}

bool LexicalAnalyzator::NumAndLetAndUnderscore(const char& a) {
	if (a == '_') {
		return true;
	}
	return (Letter(a) || Number(a));
}

bool LexicalAnalyzator::Punctuation(const char& a) {
	if (a == ':' || a == ';' || a == ',' || a == '{' || a == '}') {
		return true;
	}
	return false;
}

bool LexicalAnalyzator::Operation(const char& a) {
	if (a == '+' || a == '-' || a == '*' || a == '/' || a == '%' ||
		a == '&' || a == '|' || a == '!' || a == '~' || a == '=' ||
		a == '>' || a == '<') {
		return true;
	}
	return false;
}

bool LexicalAnalyzator::DoubleOperation(const std::string& a) {
	if (a == "+=" || a == "-=" || a == "*=" || a == "/=" || a == "%=" || a == "&=" ||
		a == "!=" || a == "~=" || a == "==" || a == ">=" || a == "<=" ||
		a == "&&" || a == "||" || a == "<<" || a == ">>" || a == "++" || a == "--") {
		return true;
	}
	return false;
}

std::vector<Lexeme> LexicalAnalyzator::Analiz(bool& fl) {
	fl = 1;
	std::ifstream fin("officialWords.txt");
	std::string line;
	std::set<std::string> ofWords;
	while (fin >> line) {
		ofWords.insert(line);
	}
	fin.close();
	fin.open("inputLexical.txt");
	std::string program;
	while (getline(fin, line)) {
		program += line;
		program += '\n';
	}
	fin.close();
	std::vector<Lexeme> vec;
	int number = 1;
	for (int i = 0; i < program.size(); i++) {
		if (program[i] == '\n') {
			number++;
		}
		if (i + 1 < program.size() && program[i] == '/' && program[i + 1] == '/') {
			while (i < program.size() && program[i] != '\n') {
				i++;
			}
			i--;
			continue;
		}
		if (Letter(program[i])) {
			std::string str;
			while (i < program.size() && NumAndLetAndUnderscore(program[i])) {
				str += program[i];
				i++;
			}
			if (ofWords.find(str) != ofWords.end()) {
				vec.push_back({ str, 1, number });
			}
			else {
				vec.push_back({ str, 2, number });
			}
			i--;
			continue;
		}
		if (Number(program[i])) {
			std::string str;
			bool flDot = true;
			while (i < program.size() && (Number(program[i]) || (program[i] == '.' && flDot))) {
				if (program[i] == '.') {
					flDot = false;
				}
				str += program[i];
				i++;
			}
			if (i < program.size()) {
				if (Letter(program[i])) {
					fl = 0;
				}
			}
			if (flDot) {
				vec.push_back({ str, 3, number });
			}
			else {
				vec.push_back({ str, 3, number });
			}
			i--;
			continue;
		}
		if (Operation(program[i])) {
			if (i + 1 < program.size() && Operation(program[i + 1])) {
				std::string str;
				str += program[i], str += program[i + 1];
				if (DoubleOperation(str)) {
					vec.push_back({ str, 4, number });
					i++;
					continue;
				}
			}
			vec.push_back({ std::string(1, program[i]), 4, number });
			continue;
		}
		if (Punctuation(program[i])) {
			vec.push_back({ std::string(1, program[i]), 5, number });
			continue;
		}
		if (program[i] == '(' || program[i] == ')') {
			vec.push_back({ std::string(1, program[i]), 6, number });
			continue;
		}
		if (program[i] == '[' || program[i] == ']') {
			vec.push_back({ std::string(1, program[i]), 7, number });
			continue;
		}
		if (program[i] == '"') {
			std::string str;
			i++;
			while (i < program.size() && (program[i] != '"')) {
				str += program[i];
				i++;
			}
			vec.push_back({ str, 8, number });
			continue;
		}
		if (program[i] == ' ' || program[i] == '\t' || program[i] == '\n') {
			continue;
		}
		fl = 0;
		std::cout << "A forbidden symbol was enetered: " << program[i] << " Number of string: " << number;
		return { };
	}
	return vec;
}