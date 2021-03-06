#include <fstream>
#include "LexerMain.h"
#include "Parser.h"
#include "AST.h"
#include "Semantica.h"

using namespace std;

int main(int argn, char **argv) {
	if (argn == 1)
	{
		cout << "no arguments" << endl;
	}
	else
	{
		for (int i = 1; i < argn; i++)
			cout << "argv[" << i << "] = " << argv[i] << endl;
	}

	ifstream fileinput("STR.cs", ios::in || ios::binary);
	if (!fileinput) {
		fileinput.close();
		cerr << "Fileinput open error" << endl;
		return -1;
	}

	//Lexer
	vector<unique_ptr<Token>> VectorToken;
	Lexer Lex;
	string Line;
	int LineNumber = 1;
	while (!fileinput.eof()) {
		getline(fileinput, Line);
		Lex.GetNextToken(Line, LineNumber, 0, VectorToken);
		LineNumber++;
	}
	Lex.GetNextToken(Line, LineNumber, 1, VectorToken);
	fileinput.close();
	cout << "Vector:" << endl;
	for (int i = 0; i < VectorToken.size(); i++)
	{
		cout << "Loc<" << VectorToken[i]->GetStrNum() << ":" << VectorToken[i]->GetColNum() << "> " << VectorToken[i]->GetTypeToken() << " '" << VectorToken[i]->GetString() << "'" << endl; // << " " << i << endl;
	}
	cout << endl;
	
	//Parser
	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size());

	//Semantica
	SemanticAnalysis ExampleSemantic;
	ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());

	system("PAUSE");
	return 0;
}