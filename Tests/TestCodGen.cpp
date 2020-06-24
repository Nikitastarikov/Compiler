#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../Parser/Parser.h"
#include "../Semantic/Semantica.h"
#include "../GenerationCod/GenerationCod.h"

TEST_CASE("CheckGenerationCodOperation", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line, File = "TestCodGeneration.s";
	vector<string> LineAssem;
	int Index = 0;

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
 	ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());

 	GenerationCod ExampleGenCod(File);
 	ExampleGenCod.StartGenerationCod(ExamplePars.GetRootVector() , ExamplePars.GetTable(), 0, File);

	ifstream Fileinput(/*"ProgrammForTest/Dop/search.cs"*/ File, ios::in);
	if (!Fileinput) 
	{
		Fileinput.close();
		cerr << "FileinputTest open error" << endl;
		return;
	}
	//cout << "Assembler:" << endl << endl;

	LineAssem.push_back("\t.text");
	LineAssem.push_back("\t.global main");
	LineAssem.push_back("\t.type main, @function");
	LineAssem.push_back("main:");

	LineAssem.push_back("\tpushl %ebp");
	LineAssem.push_back("\tmovl %esp, %ebp");
	LineAssem.push_back("\txorl %eax, %eax");
	LineAssem.push_back("\txorl %ebx, %ebx");

	LineAssem.push_back("\txorl %eax, %eax");
	LineAssem.push_back("\txorl %ebx, %ebx");

	LineAssem.push_back("\tmovl $6, %eax");
	LineAssem.push_back("\tmovl $1, %ebx");
	LineAssem.push_back("\taddl %ebx, %eax");
	LineAssem.push_back("\tpushl %eax");
	LineAssem.push_back("\tpopl %ebx");
	LineAssem.push_back("\tmovl i, %eax");
	LineAssem.push_back("\tpushl %eax");
	LineAssem.push_back("\tmovl %ebx,i");

	getline(Fileinput, Line);//1
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//2

	getline(Fileinput, Line);//3
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//4
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//5
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//6
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//7
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//8
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//9
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//10
	
	getline(Fileinput, Line);//11
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//12
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//13

	getline(Fileinput, Line);//14
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//15
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//16
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//17
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//18
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//19
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//20
	REQUIRE(LineAssem[Index] == Line);
	Index++;
	
	Fileinput.close();

	//Index++;
}

TEST_CASE("CheckGenerationCodWhile", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line, File = "TestCodGenerationOne.s";
	vector<string> LineAssem;
	int Index = 0;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = 3;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "while (5 > i){";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "i++;";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "Console.WriteLine(i);";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 9, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 10, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	SemanticAnalysis ExampleSemantic;
 	ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());

 	GenerationCod ExampleGenCod(File);
 	ExampleGenCod.StartGenerationCod(ExamplePars.GetRootVector() , ExamplePars.GetTable(), 0, File);

	ifstream Fileinput(/*"ProgrammForTest/Dop/search.cs"*/ File, ios::in);
	if (!Fileinput) 
	{
		Fileinput.close();
		cerr << "FileinputTest open error" << endl;
		return;
	}
	//cout << "Assembler:" << endl << endl;

	LineAssem.push_back("\tmovl $5, %eax");//18
	LineAssem.push_back("\tmovl i, %ebx");
	LineAssem.push_back("\tcmp %ebx, %eax");
	LineAssem.push_back("\tjng else_blockWhile_1_1_2");
	LineAssem.push_back("\tcmp %ebx, %eax");
	LineAssem.push_back("\tjng else_blockWhile_1_1_2");
	LineAssem.push_back("if_blockWhile_1_1_2:");
	LineAssem.push_back("\tincl i");

	LineAssem.push_back("\tpushl i");//27
	LineAssem.push_back("\tpushl $format_l_n");
	LineAssem.push_back("\tcall printf");

	LineAssem.push_back("\tmovl $5, %eax");//31
	LineAssem.push_back("\tmovl i, %ebx");//32
	LineAssem.push_back("\tcmp %ebx, %eax");//33
	LineAssem.push_back("\tjng else_blockWhile_1_1_2");//34
	LineAssem.push_back("\tcmp %ebx, %eax");//35
	LineAssem.push_back("\tjng else_blockWhile_1_1_2");//36
	LineAssem.push_back("\tjmp if_blockWhile_1_1_2");//37
	LineAssem.push_back("else_blockWhile_1_1_2:");//38

	while (!Fileinput.eof() && Index < 17) 
	{
		getline(Fileinput, Line);
		Index++;
	}

	Index = 0;

	getline(Fileinput, Line);//18
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//19
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//20
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//21
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//22
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//23
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//24
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//25
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//26
	
	getline(Fileinput, Line);//27
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//28
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//29
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//30

	getline(Fileinput, Line);//31
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//32
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//33
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//34
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//35
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//36
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//37
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//38
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	Fileinput.close();
}

TEST_CASE("CheckGenerationCodIf", "[single-file]")
{
	Lexer Lex;
	vector<unique_ptr<Token>> VectorToken;
	string Line, File = "TestCodGenerationTwo.s";
	vector<string> LineAssem;
	int Index = 0;

	Line = "using System;";
	Lex.GetNextToken(Line, 1, 0, VectorToken);
	Line = "class Begin {";
	Lex.GetNextToken(Line, 2, 0, VectorToken);

	Line = "static int main() {";
	Lex.GetNextToken(Line, 3, 0, VectorToken);

	Line = "int i = 3;";
	Lex.GetNextToken(Line, 4, 0, VectorToken);

	Line = "if (5 > i){";
	Lex.GetNextToken(Line, 5, 0, VectorToken);

	Line = "Console.WriteLine(i);";
	Lex.GetNextToken(Line, 6, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 7, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 8, 0, VectorToken);

	Line = "}";
	Lex.GetNextToken(Line, 9, 0, VectorToken);

	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), 0);

	SemanticAnalysis ExampleSemantic;
 	ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());

 	GenerationCod ExampleGenCod(File);
 	ExampleGenCod.StartGenerationCod(ExamplePars.GetRootVector() , ExamplePars.GetTable(), 0, File);

	ifstream Fileinput(/*"ProgrammForTest/Dop/search.cs"*/ File, ios::in);
	if (!Fileinput) 
	{
		Fileinput.close();
		cerr << "FileinputTest open error" << endl;
		return;
	}
	//cout << "Assembler:" << endl << endl;

	LineAssem.push_back("\tmovl $5, %eax");//18
	LineAssem.push_back("\tmovl i, %ebx");
	LineAssem.push_back("\tcmp %ebx, %eax");
	LineAssem.push_back("\tjng blocif_1_1_2");
	LineAssem.push_back("\tcmp %ebx, %eax");
	LineAssem.push_back("\tjng blocif_1_1_2");

	LineAssem.push_back("\tpushl i");//25
	LineAssem.push_back("\tpushl $format_l_n");
	LineAssem.push_back("\tcall printf");

	LineAssem.push_back("\tblocif_1_1_2:");//29

	while (!Fileinput.eof() && Index < 17) 
	{
		getline(Fileinput, Line);
		Index++;
	}

	Index = 0;

	getline(Fileinput, Line);//18
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//19
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//20
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//21
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//22
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//23
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//24

	getline(Fileinput, Line);//25
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	getline(Fileinput, Line);//26
	REQUIRE(LineAssem[Index] == Line);
	Index++;
	
	getline(Fileinput, Line);//27
	REQUIRE(LineAssem[Index] == Line);
	Index++;

	Fileinput.close();
}