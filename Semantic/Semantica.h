#pragma once

#include <map>
#include <typeinfo>
#include <stdlib.h>
#include "../Lexer/Lexer.h"
#include "../Parser/Parser.h"
#include "../Struct/AST.h"
#include "../Struct/Token.h"
#include "../Struct/ScopeVar.h"

using namespace std;

class SemanticAnalysis
{
	//map<string, vector<ScopeVar>> Table;
public:

	//void SetSymbolTable() {}
	bool SemanticAnalysisStart(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table);
	bool CheckingClassSemantics(unique_ptr<AST> const &NodeClass, map<string, vector<ScopeVar>> &Table);
	bool CheckingBodyFunSemantics(unique_ptr<AST> const &NodeBodyFun, map<string, vector<ScopeVar>> &Table, string ReturnType);
	bool CheckSemanticArray(unique_ptr<AST> const &NodeArray, map<string, vector<ScopeVar>> &Table);
	bool CallArrayElement(unique_ptr<AST> const &NodeArray, map<string, vector<ScopeVar>> &Table, string NodeName, string String);
	bool CallStringElement(unique_ptr<AST> const &NodeArray, map<string, vector<ScopeVar>> &Table);
	bool StartCheckSemanticOperation(unique_ptr<AST> const &NodeOper, map<string, vector<ScopeVar>> &Table);
	bool CheckSemanticOperation(unique_ptr<AST> const &NodeOperation, map<string, vector<ScopeVar>> &Table);
	bool CheckSemanticReturn(unique_ptr<AST> const &NodeReturn, map<string, vector<ScopeVar>> &Table, string ReturnType);
	bool CheckReturnElement(string TypeOne, string ReturnType, bool Element);//Перегруженный метод
	bool CheckReturnElement(string TypeOne, string ReturnType);
	bool CheckSemanticSubBlock(unique_ptr<AST> const &NodeSubBlock, map<string, vector<ScopeVar>> &Table, string ReturnType);
	bool CheckSemanticWhile(unique_ptr<AST> const &NodeWhile, map<string, vector<ScopeVar>> &Table, string ReturnType);
	bool CheckSemanticIf(unique_ptr<AST> const &NodeIf, map<string, vector<ScopeVar>> &Table, string ReturnType);
	bool CheckSemanticLocalFun(unique_ptr<AST> const &NodeLocalFun, map<string, vector<ScopeVar>> &Table, string ReturnType);
	bool CheckSemanticCallFun(unique_ptr<AST> const &NodeCallFun, map<string, vector<ScopeVar>> &Table);
	bool CheckCallFunArguments(unique_ptr<AST> const &NodeCallFun, map<string, vector<ScopeVar>> &Table, AST* const PtrNode, string NodeName);
	bool CheckSemanticId(unique_ptr<AST> const &NodeSubBlock, map<string, vector<ScopeVar>> &Table);
	bool CheckTwoTypes(string TypeOne, string TypeTwo);
	string CheckIdTableSymbol(string Name, string Scope, string Type, map<string, vector<ScopeVar>> &Table, string NodeType);
	string CheckIdName(vector<ScopeVar> &it, string Name, string NodeType);
	AST* const /*&*/SearchFunctionOrArrayDefinition(string Name, string Scope, string Type, map<string, vector<ScopeVar>> &Table, string NodeType);

	void SemError(string Error)
	{
		cout << "Semantic error: " << Error << endl;
	}
};