#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../Lexer/Lexer.h"

TEST_CASE("CheckLexer", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line = "Int";
	int Index = 0;

	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;

	Line = "int";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Int);
	Index++;

	Line = "фыва";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Unexpected);
	Index++;

	Line = "_13ggrg";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;

	Line = "Id";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;

	Line = "";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Unexpected);
	Index++;

	Line = "_I";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;

	Line = "char";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Char);
	Index++;
}

//Числа
TEST_CASE("CheckLexerDigit", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line = "10";
	int Index = 0;

	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::DecimalInteger);
	Index++;

	Line = "ч10";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Unexpected);
	Index++;

	Line = "\\423";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Unexpected);
	Index++;

	Line = "0x3";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Hexadecimal);
	Index++;

	Line = "0b3";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Binary);
	Index++;

	Line = "03";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Octal);
	Index++;
}

//Опеции
TEST_CASE("CheckLexerOperator", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line = "+";
	int Index = 0;

	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Plus);
	Index++;

	Line = "--";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Decrement);
	Index++;

	Line = "+=";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::AddAssign);
	Index++;

	Line = "<";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::LessThan);
	Index++;

	Line = " &&";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Conjunction);
	Index++;
}

//
TEST_CASE("CheckLexerMainConstructions", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line = "int i = 64;";//5
	int Index = 0;

	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Int);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Assignment);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::DecimalInteger);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Semicolon);
	Index++;


	Line = "while(id < id2)";//11
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::KeyWordWhile);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::LeftParen);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::LessThan);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::RightParen);
	Index++;

	Line = "int i, j = 0, u = (1 + 2)";//25
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Int);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Comma);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Assignment);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::DecimalInteger);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Comma);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Assignment);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::LeftParen);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::DecimalInteger);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Plus);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::DecimalInteger);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::RightParen);
	Index++;

	Line = "int i, j = 0//, u = (1 + 2)";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Int);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Comma);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::Assignment);
	Index++;
	REQUIRE(VectorToken[Index]->GetTypeToken() == Token::Words::DecimalInteger);
	Index++;

	REQUIRE(VectorToken.size() == Index);
}