#include "../Semantic/Semantica.h"
#include <string>

class GenerationCod
{
	string Way;
public:
	void StartGenerationCod(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table, int option, string File);
	void ClassTraversal(unique_ptr<AST> const &NodeClass, ofstream &Fileoutput);
	void FunBodyPass(unique_ptr<AST> const &NodeBodyFun, ofstream &Fileoutput);
	void CallPrintf(unique_ptr<AST> const &CallPrn, ofstream &Fileoutput, int IndexPrintf);

	//Запись в поле .data
	void WriteData(map<string, vector<ScopeVar>> &Table, ofstream &Fileoutput);
	void WriteDataStringsClass(vector<unique_ptr<AST>> const &VectorClass, ofstream &Fileoutput);
	void WriteDataStringsFunction(unique_ptr<AST> const &NodeClass, ofstream &Fileoutput);
	void WriteDataStrings(unique_ptr<AST> const &NodeBodyFun, ofstream &Fileoutput);

	void Operation(ofstream &Fileoutput, string OperandIdL, string Operator);
	void Compare(ofstream &Fileoutput, string op, string NameBlock);
	
	//Арифметика
	void StartArithmeticGeneration(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);
	void Scanf(string NodeNameOne, unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);
	void UnaryOperations(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);
	void ArithmeticGeneration(unique_ptr<AST> const &NodeAST, ofstream &Fileoutput);
	void OperandProcessing(string OperandOne, string OperandTwo, unique_ptr<AST> const &PtrNodeOperandOne, unique_ptr<AST> const &PtrNodeOperandTwo, ofstream &Fileoutput);
	void PreparingTheOperand(string Register, unique_ptr<AST> const &NodeOperand, ofstream &Fileoutput);
	void OperationIdToId(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);

	//Блок If
	void IfTraversal(unique_ptr<AST> const &NodeIf, ofstream &Fileoutput, string Scope);
	void ExpressionIF(unique_ptr<AST> const &NodeIfExp, ofstream &Fileoutput, string NameBlock);
	void ProcessingOperandIf(string NodeName, string Register, unique_ptr<AST> const &NodeExp, ofstream &Fileoutput);

	//Блок While
	void WhileTraversal(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput, string Scope);

	GenerationCod(string FileWay) 
	{
		Way = FileWay;
		Way = Way.substr(0, Way.size() - 2);
		Way += "s";
	}
	~GenerationCod() = default;
};