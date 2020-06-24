#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../Parser/Parser.h"
#include "../Semantic/Semantica.h"

TEST_CASE("CheckSemantic", "[single-file]")
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

	SemanticAnalysis ExampleSemantic;
 	bool SemCheck = ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());
	
	REQUIRE(SemCheck == true);
	//Index++;
}

TEST_CASE("CheckSemanticOne", "[single-file]")
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

	Line = "int i = 3;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "int j = i + 3;";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	SemanticAnalysis ExampleSemantic;
 	bool SemCheck = ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());
	
	REQUIRE(SemCheck == true);
	//Index++;
}

TEST_CASE("CheckSemanticTwo", "[single-file]")
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

	Line = "int i = 3;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "char j = i + 3;";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	SemanticAnalysis ExampleSemantic;
 	bool SemCheck = ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());
	
	REQUIRE(SemCheck == true);
	//Index++;
}

TEST_CASE("CheckSemanticLocalFunOne", "[single-file]")
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

	Line = "return i * 2;}";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "int i = fun(5);";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	SemanticAnalysis ExampleSemantic;
 	bool SemCheck = ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());
	
	REQUIRE(SemCheck == true);
	//Index++;
}

TEST_CASE("CheckSemanticLocalFunTwo", "[single-file]")
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

	Line = "return i * 2;}";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "int i = fun(5.5);";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	SemanticAnalysis ExampleSemantic;
 	bool SemCheck = ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());
	
	REQUIRE(SemCheck == false);
	//Index++;
}

TEST_CASE("CheckSemanticLocalFunThree", "[single-file]")
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

	Line = "i = 6.3;";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "return i * 2;}";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	SemanticAnalysis ExampleSemantic;
 	bool SemCheck = ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());
	
	REQUIRE(SemCheck == false);
	//Index++;
}