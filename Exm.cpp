#include <fstream>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantic/Semantica.h"
//#include "Token.h"

using namespace std;

int main(int argn, char **argv) {
	/*if (argn == 1)
	{
		cout << "no arguments" << endl;
	}
	else
	{
		for (int i = 1; i < argn; i++)
			cout << "argv[" << i << "] = " << argv[i] << endl;
	}
*/
	ifstream fileinput("ProgrammForTest/Dop/search.cs", ios::in);
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
		//cout << Line << endl;
		Lex.GetNextToken(Line, LineNumber, 0, VectorToken);
		LineNumber++;
	}
	Lex.GetNextToken(Line, LineNumber, 1, VectorToken);
	fileinput.close();
	cout << endl << "Vector:" << endl;
	for (int i = 0; i < VectorToken.size(); i++)
	{
		if (!(VectorToken[i]->GetTypeToken() == Token::Words::Unexpected))
			std::cout << "Loc<" << VectorToken[i]->GetStrNum() << ":" << VectorToken[i]->GetColNum() << "> " << VectorToken[i]->GetTypeToken() << " '" << VectorToken[i]->GetString() << "'" << endl;
		else
			std::cout << "Loc<" << VectorToken[i]->GetStrNum() << ":" << VectorToken[i]->GetColNum() << "> " << "UNEXPECTEDDDDDDDDDDDDD" << " '" << VectorToken[i]->GetString() << "'" << endl;
	}
	cout << endl;

	cout << "Parser:" << endl;
	//auto a = unique_ptr<AST>();
	//auto c = unique_ptr<AST>();
	//auto b = make_unique<ASTOperationBin>(move(a), move(c), "S", "ot Suka");
	//Parser
	Parser ExamplePars;
	ExamplePars.Parsing(VectorToken, VectorToken.size());

	//Semantica
	//SemanticAnalysis ExampleSemantic;
 	//ExampleSemantic.SemanticAnalysisStart(ExamplePars.GetRootVector() , ExamplePars.GetTable());

	return 0;
}