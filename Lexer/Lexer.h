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
	void CheckOb(unique_ptr<Token> TokenObject, string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size);
};

unique_ptr<Token> IsSymbol(string &Line, int &LineNumber, int &Identifier, int LineSize);
int IsSpace(char Symbol);
int IsLetter(char Symbol);
int IsDigit(char Symbol);
bool IsBinary(string const &Line, int &LineIndex, int &SizeSubLine, int LineSize);
bool IsOctal(string &Line, int &LineIndex, int &SizeSubLine, int LineSize);
bool IsHexadecimal(string &Line, int &LineIndex, int &SizeSubLine, int LineSize);
bool IsDecimalInteger(string &Line, int &LineIndex, int &SizeSubLine, int LineSize, bool &NotInteger);
unique_ptr<Token>  ReadNumber(string &Line, int &LineNumber, int &Identifier, int LineSize);
void ReadWord(int TemporaryIdentifier, unique_ptr<Token> const &TokenObject, string &Line, int SizeSubLine, int &LineSize, int &LineIndex);
unique_ptr<Token> ReadString(string &Line, int &LineNumber, int &LineSize, int &LineIndex);
void CheckOperEqual(unique_ptr<Token> const &TokenObject, string &Line, int &LineSize, int &LineIndex, Token::Words TokenAssign, Token::Words TokenOperand);
unique_ptr<Token> ReadOperator(string &Line, int &LineNumber, int &LineSize, int &LineIndex);
std::ostream& operator << (std::ostream& os, const Token::Words& kind);
