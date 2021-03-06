#pragma once
#include <iostream>
#include <string>
#include <map>

using namespace std;

class Token {
public:
	enum class Words {
		Identifier,
		LeftParen, // (
		RightParen, // )
		LeftSquare, // [
		RightSquare, // ]
		LeftCurly, // {
		RightCurly, // }
		LessThan, // <
		GreaterThan, // >
		Equal, // ==
		Plus, // +
		Minus, // -
		Asterisk, // *
		Slash, // /
		Percent, // %
		Semicolon, // ;
		DoubleQuote, // ""
		Comma, // ,
		String,
		Double,
		Int,
		Float,
		Char,
		Bool,
		Var,
		Void,
		string,
		Library,
		End,
		Unexpected,
		Assignment, // =
		KeyWordIf, // if
		KeyWordElse,
		KeyWordWhile,
		KeyWordNew,
		Negation,
		Conjunction, // &&
		Disjunction, // ||
		False,
		True,
		NotEqual, // !=
		Increment, // ++
		Decrement, // --
		LessOrEqual, // <=
		MoreOrEqual, // >=
		AddAssign, // +=
		SubAssign, // -=
		MultAssign, // *=
		DivAssign, // /=
		Null, // null
		KeyWordUsing,
		KeyWordReturn,
		KeyWordStatic,
		Class,
		CallMethodId,
		Binary,
		Octal,
		Hexadecimal,
		DecimalInteger,
		Point,
		NotInteger,
		Public,
		Symbol,
	};

	Token()
	{
		BinopPrecedence["++"] = 5;
		BinopPrecedence["--"] = 5;
		BinopPrecedence["="] = 10;
		BinopPrecedence["+="] = 10;
		BinopPrecedence["-="] = 10;
		BinopPrecedence["*="] = 10;
		BinopPrecedence["/="] = 10;
		BinopPrecedence["<"] = 20;
		BinopPrecedence[">"] = 20;
		BinopPrecedence[">="] = 20;
		BinopPrecedence["<="] = 20;
		BinopPrecedence["=="] = 20;
		BinopPrecedence["&&"] = 20;
		BinopPrecedence["||"] = 20;
		BinopPrecedence["+"] = 30;
		BinopPrecedence["-"] = 30;
		BinopPrecedence["*"] = 40;
		BinopPrecedence["/"] = 40;
		BinopPrecedence["%"] = 40;
	}

	Token(Words Token) : TokenType(Token) 
	{
		BinopPrecedence["++"] = 5;
		BinopPrecedence["--"] = 5;
		BinopPrecedence["="] = 10;
		BinopPrecedence["+="] = 10;
		BinopPrecedence["-="] = 10;
		BinopPrecedence["*="] = 10;
		BinopPrecedence["/="] = 10;
		BinopPrecedence["<"] = 20;
		BinopPrecedence[">"] = 20;
		BinopPrecedence["=="] = 20;
		BinopPrecedence[">="] = 20;
		BinopPrecedence["<="] = 20;
		BinopPrecedence["&&"] = 20;
		BinopPrecedence["||"] = 20;
		BinopPrecedence["+"] = 30;
		BinopPrecedence["-"] = 30;
		BinopPrecedence["*"] = 40;
		BinopPrecedence["/"] = 40;
		BinopPrecedence["%"] = 40;
	}

	void SetTypeToken(Words Token) { TokenType = Token; }
	void SetString(string Str) { Line = Str; }
	void SetStrNumber(int i) { StrNum = i; }
	void SetColNumber(int i) { ColNum = i; }

	Words GetTypeToken() noexcept { return TokenType; }
	string GetString() { return Line; }
	int GetStrNum() { return StrNum; }
	int GetColNum() { return ColNum; }
	int GetBinopPrecedence(string str)
	{
		return BinopPrecedence[str];
	}

private:
	map<string, int> BinopPrecedence;
	Words TokenType{};
	string Line = "";
	int StrNum;
	int ColNum;
};