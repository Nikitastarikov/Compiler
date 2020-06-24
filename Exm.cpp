#include <fstream>
#include <string.h>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantic/Semantica.h"
#include "GenerationCod/GenerationCod.h"
//#include "Token.h"

using namespace std;

//Options:
//  --dump-tokens — вывести результат работы лексического анализатора
//  --dump-ast — вывести AST
//  --dump-asm — вывести ассемблер


int main(int argn, char **argv) {

	int Options[3] = {0, 0, 0};
	string Way;

	for (int i = 1; i < argn; i++)
	{
		cout << "argv[" << i << "] = " << argv[i] << endl;
		if (!strcmp("--dump-tokens", argv[i]))
			Options[0] = 1;
		else if (!strcmp("--dump-ast", argv[i]))
			Options[1] = 1;
		else if (!strcmp("--dump-asm", argv[i]))
			Options[2] = 1;
		else if (i == argn - 1)
			Way = argv[i];
	}

	if (argn == 1 || Way == "")
		Way = "testone.cs";

	cout << "Way: " << Way << endl;
	cout << "flags: " << endl;
	if (Options[0] == 1)
		cout << "--dump-tokens ";
	else if (Options[1] == 1)
		cout << "--dump-ast ";
	else if (Options[2] == 1)
		cout << "--dump-asm ";
	cout << endl;

	ifstream fileinput(Way, ios::in);
	if (!fileinput) 
	{
		fileinput.close();
		cerr << "Fileinput open error" << endl;
		return -1;
	}

	//Lexer
	vector<unique_ptr<Token>> VectorToken;
	Lexer Lex;
	string Line;
	int LineNumber = 1;

	while (!fileinput.eof()) 
	{
		getline(fileinput, Line);
		Lex.GetNextToken(Line, LineNumber, 0, VectorToken);
		LineNumber++;
	}

	Lex.GetNextToken(Line, LineNumber, 1, VectorToken);
	fileinput.close();

	if (Options[0])
	{
		cout << endl << "Lexer:" << endl;
		for (int i = 0; i < VectorToken.size(); i++)
		{
			if (!(VectorToken[i]->GetTypeToken() == Token::Words::Unexpected))
				std::cout << "Loc<" << VectorToken[i]->GetStrNum() << ":" << VectorToken[i]->GetColNum() << "> " << VectorToken[i]->GetTypeToken() << " '" << VectorToken[i]->GetString() << "'" << endl;
			else
				std::cout << "Loc<" << VectorToken[i]->GetStrNum() << ":" << VectorToken[i]->GetColNum() << "> " << "UNEXPECTEDDDDDDDDDDDDD" << " '" << VectorToken[i]->GetString() << "'" << endl;
		}
		cout << endl;
	}

	//Parser
	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size(), Options[1]);

	//Semantica
	SemanticAnalysis ExampleSemantic;
 	ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());

 	//GenerationCod
 	GenerationCod ExampleGenCod(Way);
 	ExampleGenCod.StartGenerationCod(ExamplePars.GetRootVector() , ExamplePars.GetTable(), Options[2]);

	return 0;
}