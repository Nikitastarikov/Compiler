#include "../Semantic/Semantica.h"

class GenerationCod
{
	string Way;
public:
	void StartGenerationCod(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table, int option);
	bool ClassPass(unique_ptr<AST> const &NodeClass, ofstream &fileoutput);
	bool FunBodyPass(unique_ptr<AST> const &NodeBodyFun, ofstream &fileoutput);
	void CallPrintf(unique_ptr<AST> const &CallPrn, ofstream &fileoutput);
	void WriteData(map<string, vector<ScopeVar>> &Table, ofstream &fileoutput);
	void CalculationsPass(unique_ptr<AST> const &NodeOper, ofstream &fileoutput);
	void RecursPass(unique_ptr<AST> const &NodeOper, ofstream &fileoutput);
	void WriteOper(ofstream &fileoutput, string one, string two, string NodeNameOne, string NodeNameTwo, string op);
	void ifPass(unique_ptr<AST> const &NodeIf, ofstream &fileoutput, string scope);
	void ExprIF(unique_ptr<AST> const &NodeIfExp, ofstream &fileoutput, string nameblock);
	void WriteOperIf(ofstream &fileoutput, string NodeNameOne, string NodeNameTwo, string op, string nameblock);
	void whilePass(unique_ptr<AST> const &NodeOper, ofstream &fileoutput, string scope);

	GenerationCod(string FileWay) 
	{
		Way = FileWay;
		Way = Way.substr(0, Way.size() - 2);
		Way += "s";
	}
	~GenerationCod() = default;
};