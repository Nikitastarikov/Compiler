#pragma once
#include <typeinfo>
#include "../Lexer/Lexer.h"
#include "../Struct/AST.h"
#include "../Struct/Token.h"
#include "../Struct/ScopeVar.h"

using namespace std;


bool GetNextToken(int &index, int size, vector<unique_ptr<Token>> const &VectorToken);
bool GetPredToken(int &index, int size, vector<unique_ptr<Token>> const &VectorToken);
void RedactionString(string &str);

template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&&... args)
{ return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }

// Use make_unique for arrays
template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(size_t n)
{ return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]()); }

class Parser
{
	map<string, vector<ScopeVar>> Table;
	unique_ptr<AST> Root;
	int FlagEqual = 0;
	char FlagError = 0;
	string FlagDefenition = "";

public:
	//friend class SemanticAnalysis;
	Parser() {}
	map<string, vector<ScopeVar>> const &GetTableSymbol() { return Table; }
	void Parsing(vector<unique_ptr<Token>> const &VectorToken, int size);
	int GetTokPrecedence(vector<unique_ptr<Token>> const &VectorToken, int &index, int size);
	bool CheckDigit(vector<unique_ptr<Token>> const &VectorToken, int index);
	bool CheckType(vector<unique_ptr<Token>> const &VectorToken, int index);
	bool CheckThisIsBlock(string str);
	unique_ptr<AST> ArrayDefenition(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, vector<ScopeVar> &SubTable, string scope, int indexFun, string idType);
	unique_ptr<AST> ParseParenAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseTypeIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, vector<ScopeVar> &SubTable, string scope, int indexFun);
	unique_ptr<AST> ParseIdentifierASTArray(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string const &IdName, unique_ptr<AST> id, string scope, int indexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseIdentifierASTFun(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string const &IdName, string scope, int indexFun, vector<ScopeVar> &SubTable, int flagMethod);
	unique_ptr<AST> ParseIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseNumberStringAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, vector<ScopeVar> &SubTable, string scope, int flag);
	unique_ptr<AST> ParsePrimary(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> KeyWordNew(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseBinOpRHS(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ReadIf(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int &indexFun);
	unique_ptr<AST> ReadFunLocal(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTableFun);
	unique_ptr<AST> ReadWhile(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun);
	unique_ptr<AST> ReadElse(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun);
	unique_ptr<AST> ReadReturn(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ReadSubBlock(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun);
	unique_ptr<AST> ReadFun(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope);
	vector<unique_ptr<AST>> ReadFunBody(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, vector<ScopeVar> &SubTable);
	vector<unique_ptr<AST>> ReadBodyClass(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexClass);
	unique_ptr<AST> ReadClass(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexClass);
	unique_ptr<AST> ReadUsing(vector<unique_ptr<Token>> const &VectorToken, int &index, int size);
	vector<unique_ptr<AST>> BuildAST(vector<unique_ptr<Token>> const &VectorToken, int size);
	string CheckIdName(vector<ScopeVar> &it, string name);
	string CheckIdTableSymbol(string name, string scope, string type);
	string CheckDeclaretion(string scope, string name, string type, unique_ptr<AST> const &tree, vector<ScopeVar> &SubTable);
	string CheckDeclaretion(string scope, string name, string type, unique_ptr<ASTIdentifier> const &tree, vector<ScopeVar> &SubTable);
	void PrintTable();

	vector<unique_ptr<AST>> const &GetRootVector()
	{
		return Root->GetChild();
	}

	map<string, vector<ScopeVar>> &GetTable() { return Table; }
	unique_ptr<AST> ReadExpression(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable)
	{
		auto LHS = ParsePrimary(VectorToken, index, size, scope, indexFun, SubTable);
		if (!LHS) return nullptr;

		return ParseBinOpRHS(0, move(LHS), VectorToken, index, size, scope, indexFun, SubTable);
	}
	
	unique_ptr<AST> LogError(int NumStr, int NumCol, string const Str)
	{
		FlagError = 1;
		if (Str == "") return nullptr;

		if (NumStr == 0 && NumCol == 0)
			cout << " Parsing error:" << Str << endl;
		else
			cout << NumStr << ":" << NumCol << " Parsing error:" << Str << endl;
		return nullptr;
	}
	unique_ptr<ASTCallFun> LogErrorP(int NumStr, int NumCol, string const Str)
	{
		FlagError = 1;
		LogError(NumStr, NumCol, Str);
		return nullptr;
	}
	unique_ptr<ASTFun> LogErrorOpr(int NumStr, int NumCol, string const Str)
	{
		FlagError = 1;
		LogError(NumStr, NumCol, Str);
		return nullptr;
	}
	vector<unique_ptr<AST>> LogErrorVector(int NumStr, int NumCol, string const Str)
	{
		vector<unique_ptr<AST>> ZeroVector;
		FlagError = 1;
		if (Str == "") return move(ZeroVector);

		if (NumStr == 0 && NumCol == 0)
			cout << " Parsing error:" << Str << endl;
		else
			cout << NumStr << ":" << NumCol << " Parsing error:" << Str << endl;
		return move(ZeroVector);
	}
	
	unique_ptr<AST> Correctness(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string Error)
	{
		cout << VectorToken[index]->GetStrNum() << ":" << VectorToken[index]->GetColNum();
		index = size - 1;
		return LogError(0, 0, Error);
	}
	unique_ptr<ASTFun> CorrectnessOpr(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string Error)
	{
		cout << VectorToken[index]->GetStrNum() << ":" << VectorToken[index]->GetColNum();
		index = size - 1;
		return LogErrorOpr(0, 0, Error);
	}
	vector<unique_ptr<AST>> CorrectnessVect(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string Error)
	{
		cout << VectorToken[index]->GetStrNum() << ":" << VectorToken[index]->GetColNum();
		index = size - 1;
		return LogErrorVector(0, 0, Error);
	}
	unique_ptr<AST> CorrectnessDefenition(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, unique_ptr<AST> id)
	{
		if (id == nullptr) Correctness(VectorToken, index, size, " incorrect defenition ");
		return move(id);
	}

	unique_ptr<AST> CheckOb(vector<unique_ptr<Token>> const &VectorToken, int &index, string scope, int indexFun, vector<ScopeVar> &SubTable, string Error, unique_ptr<AST> ob)
	{
		//cout << "CheckOB " << ob->GetStrType() << " " << ob->GetStrName() << endl;
		if (ob != nullptr)
		{
			//cout << "!!!!CheckOb!!!!-> " << ob->GetStrName() << endl;
			if (ob->GetStrName() != "Console")
			{		
				//cout << "!!!!CheckOb!!!! " << ob->GetStrName() << endl;
				string str = CheckDeclaretion(scope, ob->GetStrName(), ob->GetStrType(), ob, SubTable);
				
				if (str == ob->GetStrName() + " without definition" || str == ob->GetStrName() + " has definition at this lvl")
					return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " " + str);
				ob->SetStrType(str);
			}
			return move(ob);
		}
		return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), Error);
	}

	unique_ptr<AST> CallMethod(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable, string IdName)
	{
		GetNextToken(index, size, VectorToken);//Пропуск .

		if (VectorToken[index]->GetTypeToken() == Token::Words::Identifier)
		{
			//auto Method = ParseIdentifierAST(VectorToken, index, size, scope, indexFun, SubTable);
			//if (Method != nullptr) return make_unique<ASTIdentifier>(IdName, move(Method), scope);

			if (VectorToken[index]->GetString() == "Write" || VectorToken[index]->GetString() == "WriteLine")
			{
				GetNextToken(index, size, VectorToken);
				auto Method = ParseIdentifierASTFun(VectorToken, index, size, VectorToken[index]->GetString(), scope, indexFun, SubTable, 0);
				if (Method != nullptr) return make_unique<ASTIdentifier>(IdName, move(Method), scope);
			}
			else if (VectorToken[index]->GetString() == "ReadLine")
			{
				GetNextToken(index, size, VectorToken);
				auto Method = ParseIdentifierASTFun(VectorToken, index, size, VectorToken[index]->GetString(), scope, indexFun, SubTable, 1);
				if (Method != nullptr) return make_unique<ASTIdentifier>(IdName, move(Method), scope);
			}
			else if (IdName != "Console" && VectorToken[index]->GetString() == "Length")
			{
				//GetNextToken(index, size, VectorToken);
				auto Method = ParseIdentifierAST(VectorToken, index, size, scope, indexFun, SubTable);
				if (Method != nullptr)
				{
					Method->SetStrType("Int");
					return make_unique<ASTString>(IdName, scope, move(Method));
				}
			}

			return Correctness(VectorToken, index, size, " the " + IdName + " does't have a " + VectorToken[index]->GetString() + " method");
		}

		return Correctness(VectorToken, index, size, " expected id after 'Console.' or 'string.'");
	}
};
