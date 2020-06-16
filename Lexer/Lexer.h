#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <string.h>
#include "../Struct/Token.h"

using namespace std;

class Lexer {

public:
	int CharacterReadMode = 0;
	Lexer() = default;
	~Lexer() = default;
	void UnexpectedToken(string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size);
	void CharacterReadModeTwo(string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size);
	void GetNextToken(string line, int stroka, int last, vector<unique_ptr<Token>> &VectorToken);
	void CheckOb(unique_ptr<Token> ob, string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size);
};

unique_ptr<Token> isSymbol(string &Line, int &LineNumber, int &Identifier, int LineSize);
int isletter(char a);
bool IsBinary(string const &Line, int &LineIndex, int &SizeSubLine, int LineSize);
bool IsOctal(string &Line, int &LineIndex, int &SizeSubLine, int LineSize);
bool IsHexadecimal(string &Line, int &LineIndex, int &SizeSubLine, int LineSize);
bool IsDecimalInteger(string &Line, int &LineIndex, int &SizeSubLine, int LineSize, bool &real);
unique_ptr<Token>  WordDigit(string &Line, int &LineNumber, int &Identifier, int LineSize);
void ReadWord(int TemporaryIdentifier, unique_ptr<Token> const &ob, string &Line, int SizeSubLine, int &LineSize, int &LineIndex);
unique_ptr<Token> WordString(string &Line, int &LineNumber, int &LineSize, int &LineIndex);
void checkOperEqual(unique_ptr<Token> const &ob, string &Line, int &LineSize, int &LineIndex, Token::Words tAs, Token::Words t);
unique_ptr<Token> WordZnak(string &Line, int &LineNumber, int &LineSize, int &LineIndex);
std::ostream& operator << (std::ostream& os, const Token::Words& kind);
