#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../Parser/Parser.h"

TEST_CASE("CheckParser", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = 6 + 1;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	
	REQUIRE(ExamplePars.GetFlagError() == 0);
	//Index++;
}

TEST_CASE("CheckParserWhile", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = 6 + 1;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "while(i < 10){";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "i++;";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 9, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	
	REQUIRE(ExamplePars.GetFlagError() == 0);
	//Index++;
}

TEST_CASE("CheckParserIf", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = 6 + 1;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "if(i < 10){";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "i++;";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 9, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	
	REQUIRE(ExamplePars.GetFlagError() == 0);
	//Index++;
}

TEST_CASE("CheckParserErrorLocalFun", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int fun(int i){";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "return i * 2; }";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "int i = fun(2);";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	
	REQUIRE(ExamplePars.GetFlagError() == 0);
	//Index++;
}

TEST_CASE("CheckParserErrorOne", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = 6";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	
	REQUIRE(ExamplePars.GetFlagError() == 1);
	//Index++;
}

TEST_CASE("CheckParserErrorTwo", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = j;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	
	REQUIRE(ExamplePars.GetFlagError() == 1);
	//Index++;
}

TEST_CASE("CheckParserErrorLocalFunError", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = fun(2);";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "int fun(int i){";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "return i * 2; }";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	
	REQUIRE(ExamplePars.GetFlagError() == 1);
	//Index++;
}