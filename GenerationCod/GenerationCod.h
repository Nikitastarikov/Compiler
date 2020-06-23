#include "../Semantic/Semantica.h"

class GenerationCod
{
	string Way;
public:
	void StartGenerationCod(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table, int option);
	bool ClassPass(unique_ptr<AST> const &NodeClass, ofstream &Fileoutput);
	bool FunBodyPass(unique_ptr<AST> const &NodeBodyFun, ofstream &Fileoutput);
	void CallPrintf(unique_ptr<AST> const &CallPrn, ofstream &Fileoutput);
	void WriteData(map<string, vector<ScopeVar>> &Table, ofstream &Fileoutput);
	void WriteOper(ofstream &Fileoutput, string OperandIdL, string Operator);
	void ifPass(unique_ptr<AST> const &NodeIf, ofstream &Fileoutput, string scope);
	void ExprIF(unique_ptr<AST> const &NodeIfExp, ofstream &Fileoutput, string nameblock);
	void WriteOperIf(ofstream &Fileoutput, string NodeNameOne, string NodeNameTwo, string op, string nameblock);
	void whilePass(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput, string scope);
	
	void StartArithmeticGeneration(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);
	void Scanf(string NodeNameOne, unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);
	void UnaryOperations(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);
	void ArithmeticGeneration(unique_ptr<AST> const &NodeAST, ofstream &Fileoutput);
	void OperandProcessing(string OperandOne, string OperandTwo, unique_ptr<AST> const &PtrNodeOperandOne, unique_ptr<AST> const &PtrNodeOperandTwo, ofstream &Fileoutput);
	void PreparingTheOperand(string Register, unique_ptr<AST> const &NodeOperand, ofstream &Fileoutput);
	void OperationIdToId(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput);

	GenerationCod(string FileWay) 
	{
		Way = FileWay;
		Way = Way.substr(0, Way.size() - 2);
		Way += "s";
	}
	~GenerationCod() = default;
};