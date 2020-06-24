#pragma once
#include <typeinfo>
#include "../Lexer/Lexer.h"
#include "../Struct/AST.h"
#include "../Struct/Token.h"
#include "../Struct/ScopeVar.h"

using namespace std;


bool GetNextToken(int &Index, int size, vector<unique_ptr<Token>> const &VectorToken);
bool GetPredToken(int &Index, int size, vector<unique_ptr<Token>> const &VectorToken);
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
	int FlagError = 0;
	string FlagDefenition = "";

public:
	//friend class SemanticAnalysis;
	Parser() {}
	map<string, vector<ScopeVar>> const &GetTableSymbol() { return Table; }
	void Parsing(vector<unique_ptr<Token>> const &VectorToken, int size, int Option);
	int GetTokPrecedence(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size);
	bool CheckDigit(vector<unique_ptr<Token>> const &VectorToken, int Index);
	bool CheckType(vector<unique_ptr<Token>> const &VectorToken, int Index);
	bool CheckThisIsBlock(string NodeName);
	unique_ptr<AST> ArrayDefenition(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, vector<ScopeVar> &SubTable, string Scope, int IndexFun, string IdType);
	unique_ptr<AST> ParseParenAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseTypeIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, vector<ScopeVar> &SubTable, string Scope, int IndexFun);
	unique_ptr<AST> ParseIdentifierASTArray(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string const &IdName, unique_ptr<AST> id, string Scope, int IndexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseIdentifierASTFun(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string const &IdName, string Scope, int IndexFun, vector<ScopeVar> &SubTable, int FlagMethod);
	unique_ptr<AST> ParseIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseNumberStringAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, vector<ScopeVar> &SubTable, string Scope, int Flag);
	unique_ptr<AST> ParsePrimary(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> KeyWordNew(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ParseBinOpRHS(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ReadIf(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int &IndexFun);
	unique_ptr<AST> ReadFunLocal(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTableFun);
	unique_ptr<AST> ReadWhile(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun);
	unique_ptr<AST> ReadElse(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun);
	unique_ptr<AST> ReadReturn(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable);
	unique_ptr<AST> ReadSubBlock(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun);
	unique_ptr<AST> ReadFun(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope);
	vector<unique_ptr<AST>> ReadFunBody(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, vector<ScopeVar> &SubTable);
	vector<unique_ptr<AST>> ReadBodyClass(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexClass);
	unique_ptr<AST> ReadClass(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexClass);
	unique_ptr<AST> ReadUsing(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size);
	vector<unique_ptr<AST>> BuildAST(vector<unique_ptr<Token>> const &VectorToken, int size);
	string CheckIdName(vector<ScopeVar> &Iterator, string Name);
	string CheckIdTableSymbol(string Name, string Scope, string Type);
	string CheckDeclaretion(string Scope, string Name, string Type, unique_ptr<AST> const &Tree, vector<ScopeVar> &SubTable);
	string CheckDeclaretion(string Scope, string Name, string Type, unique_ptr<ASTIdentifier> const &Tree, vector<ScopeVar> &SubTable);
	void PrintTable();

	char GetFlagError() {return FlagError;}

	vector<unique_ptr<AST>> const &GetRootVector()
	{
		return Root->GetChild();
	}

	map<string, vector<ScopeVar>> &GetTable() { return Table; }
	unique_ptr<AST> ReadExpression(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable)
	{
		auto LHS = ParsePrimary(VectorToken, Index, size, Scope, IndexFun, SubTable);
		if (!LHS) 
		{
			return nullptr;
		}

		return ParseBinOpRHS(0, move(LHS), VectorToken, Index, size, Scope, IndexFun, SubTable);
	}
	
	unique_ptr<AST> LogError(int NumStr, int NumCol, string const StringError)
	{
		FlagError = 1;
		if (StringError == "") 
		{
			return nullptr;
		}

		if (NumStr == 0 && NumCol == 0)
		{
			cout << " Parsing error:" << StringError << endl;
		}
		else
		{
			cout << NumStr << ":" << NumCol << " Parsing error:" << StringError << endl;
		}

		return nullptr;
	}
	unique_ptr<ASTCallFun> LogErrorP(int NumStr, int NumCol, string const StringError)
	{
		FlagError = 1;
		LogError(NumStr, NumCol, StringError);
		return nullptr;
	}
	unique_ptr<ASTFun> LogErrorOpr(int NumStr, int NumCol, string const StringError)
	{
		FlagError = 1;
		LogError(NumStr, NumCol, StringError);
		return nullptr;
	}
	vector<unique_ptr<AST>> LogErrorVector(int NumStr, int NumCol, string const StringError)
	{
		vector<unique_ptr<AST>> ZeroVector;
		FlagError = 1;
		if (StringError == "") 
		{
			return move(ZeroVector);
		}

		if (NumStr == 0 && NumCol == 0)
		{
			cout << " Parsing error:" << StringError << endl;
		}
		else
		{
			cout << NumStr << ":" << NumCol << " Parsing error:" << StringError << endl;
		}

		return move(ZeroVector);
	}
	
	unique_ptr<AST> Correctness(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Error)
	{
		cout << VectorToken[Index]->GetStrNum() << ":" << VectorToken[Index]->GetColNum();
		Index = size - 1;

		return LogError(0, 0, Error);
	}
	unique_ptr<ASTFun> CorrectnessOpr(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Error)
	{
		cout << VectorToken[Index]->GetStrNum() << ":" << VectorToken[Index]->GetColNum();
		Index = size - 1;

		return LogErrorOpr(0, 0, Error);
	}
	vector<unique_ptr<AST>> CorrectnessVect(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Error)
	{
		cout << VectorToken[Index]->GetStrNum() << ":" << VectorToken[Index]->GetColNum();
		Index = size - 1;

		return LogErrorVector(0, 0, Error);
	}
	unique_ptr<AST> CorrectnessDefenition(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, unique_ptr<AST> id)
	{
		if (id == nullptr) 
		{
			Correctness(VectorToken, Index, size, " incorrect defenition ");
		}

		return move(id);
	}

	unique_ptr<AST> CheckOb(vector<unique_ptr<Token>> const &VectorToken, int &Index, string Scope, int IndexFun, vector<ScopeVar> &SubTable, string Error, unique_ptr<AST> NodeObject)
	{
		if (NodeObject != nullptr)
		{
			if (NodeObject->GetStrName() != "Console")
			{		
				string str = CheckDeclaretion(Scope, NodeObject->GetStrName(), NodeObject->GetStrType(), NodeObject, SubTable);
				
				//cout << "CheckOb " << NodeObject->GetStrName() << " " << NodeObject->GetStrType() << endl;
				if (str == NodeObject->GetStrName() + " without definition" || str == NodeObject->GetStrName() + " has definition at this lvl")
				{
					return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " " + str);
				}

				NodeObject->SetStrType(str);
			}
			return move(NodeObject);
		}
		return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), Error);
	}

	unique_ptr<AST> CallMethod(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable, string IdName)
	{
		GetNextToken(Index, size, VectorToken);//Пропуск .

		if (VectorToken[Index]->GetTypeToken() == Token::Words::Identifier)
		{
			//auto Method = ParseIdentifierAST(VectorToken, Index, size, Scope, IndexFun, SubTable);
			//if (Method != nullptr) return make_unique<ASTIdentifier>(IdName, move(Method), Scope);

			if (VectorToken[Index]->GetString() == "Write" || VectorToken[Index]->GetString() == "WriteLine")
			{
				GetNextToken(Index, size, VectorToken);
				auto Method = ParseIdentifierASTFun(VectorToken, Index, size, VectorToken[Index - 1]->GetString(), Scope, IndexFun, SubTable, 0);
				
				if (Method != nullptr) 
				{
					return make_unique<ASTIdentifier>(IdName, move(Method), Scope);
				}
			}
			
			if (VectorToken[Index]->GetString() == "ReadLine")
			{
				GetNextToken(Index, size, VectorToken);
				auto Method = ParseIdentifierASTFun(VectorToken, Index, size, VectorToken[Index - 1]->GetString(), Scope, IndexFun, SubTable, 1);
				
				if (Method != nullptr) 
				{
					return make_unique<ASTIdentifier>(IdName, move(Method), Scope);
				}
			}
			/*else if (IdName != "Console" && VectorToken[Index]->GetString() == "Length")
			{
				//GetNextToken(Index, size, VectorToken);
				auto Method = ParseIdentifierAST(VectorToken, Index, size, Scope, IndexFun, SubTable);
				if (Method != nullptr)
				{
					Method->SetStrType("int");
					Method->SetMethodLength();
					return move(Method);
					//return make_unique<ASTString>(IdName, Scope, move(Method));
				}
			}*/

			return Correctness(VectorToken, Index, size, " the " + IdName + " does't have a " + VectorToken[Index]->GetString() + " method");
		}

		return Correctness(VectorToken, Index, size, " expected id after 'Console.' or 'string.'");
	}
};
