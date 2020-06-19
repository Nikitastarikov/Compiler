#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <memory>
#include "Token.h"

using namespace std;

class AST
{
	unique_ptr<AST> Expression = nullptr;
	vector<unique_ptr<AST>> Children;
	char flagforarray;
	string scope = "";

public:
	virtual void SetMethodLength() {}
	virtual bool GetMethodLength() { return false; }
	void space_print(int space) { for (int i = 0; i < space; i++) cout << " "; }
	virtual string GetStrName() { return "unssigned"; }
	virtual string GetStrType() { return "unssigned"; }
	virtual string GetScope() { return scope; }
	string GetSystemNumber() { return "This is't digit"; }
	virtual void SetStrType(string type) { cout << "SetStrType" << endl; }
	virtual vector<unique_ptr<AST>> const &GetChild() { return Children; }
	virtual vector<unique_ptr<AST>> const &GetArgs() { return Children; }
	virtual vector<unique_ptr<AST>> const &GetArraySizes() { return Children; }
	virtual vector<unique_ptr<AST>> const &GetIndex() { return Children; }
	virtual vector<unique_ptr<AST>> const &GetSize() { return Children; }
	virtual unique_ptr<AST> const &GetExpressionL() { return Expression; }
	virtual unique_ptr<AST> const &GetExpressionR() { return Expression; }
	virtual void print(int spase = 0) {}
	virtual char GetFlag() { return flagforarray; }
	virtual bool GetDeclar() { return false; }
	virtual ~AST() {}
	AST() {/*cout << "Darow" << endl;*/}
};

class ASTRoot : public AST
{
	vector<unique_ptr<AST>> Children;
	string scope = "";
public:
	ASTRoot(vector<unique_ptr<AST>> children) : Children(move(children)) {}

	void AddChild(unique_ptr<AST> child)
	{
		Children.push_back(move(child));
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	void print(int space)
	{
		cout << endl << "AST:" << endl;
		for (int i = 0; i < Children.size(); i++)
			Children[i]->print(space);
		cout << endl;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTUsing : public AST
{
	string Library = "";
	string scope = "";
	//unique_ptr<AST> Parent;
public:
	ASTUsing(string const &lib) : Library(lib) {}
	void print(int space)
	{
		cout << "Using(" << Library << ")" << endl;
	}

	string GetStrName() 
	{ 
		return Library; 
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTClass : public AST
{
	//unique_ptr<AST> Parent;
	vector<unique_ptr<AST>> Children;
	string Name;
	string scope = "";
public:
	ASTClass(vector<unique_ptr<AST>> children, string const &name) : Children(move(children)), Name(name) {}
	ASTClass(string const &name) : Name(name) {}
	void AddChild(unique_ptr<AST> child)
	{
		Children.push_back(move(child));
	}
	
	void AddVectorChildren(vector<unique_ptr<AST>> children)
	{
		Children = move(children);
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	void print(int space)
	{
		cout << "Class " << Name << "(" << endl;
		for (int i = 0; i < Children.size(); i++)
			Children[i]->print(space);

		cout << ")" << endl;
	}

	string GetStrName()
	{
		return Name;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTFun : public AST
{
	string Name = "";
	string Type = "unssigned";
	vector<unique_ptr<AST>> Args;
	string scope = "";

public:
	ASTFun(string const &name, string const &type, vector<unique_ptr<AST>> args, string s) : Name(name), Type(type), Args(move(args)), scope(s)/*, TypeArgs(move(typeargs))*/ {}
	ASTFun(string const &name, string const &type, string s) : Name(name), Type(type), scope(s) {}
	void AddArg(unique_ptr<AST> arg)
	{
		Args.push_back(move(arg));
	}

	void print(int space)
	{
		space_print(space);
		cout << Type << " " << Name << "( scope-" << scope << endl;
		for (int i = 0; i < Args.size(); i++)
			Args[i]->print(space + 2);
		space_print(space);
		cout << ")" << endl;
	}

	vector<unique_ptr<AST>> const &GetArgs()
	{
		return Args;
	}

	string GetStrName()
	{
		return Name;
	}

	string GetStrType() 
	{ 
		return Type; 
	}

	void SetStrType(string type) 
	{ 
		//cout << type << endl;
		Type = type;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTLocal : public AST
{
	unique_ptr<AST> ChildName;
	vector<unique_ptr<AST>> Children;
	string scope = "";
	string Type = "unssigned";
public:
	ASTLocal(unique_ptr<AST> childname, vector<unique_ptr<AST>> children, string s) : ChildName(move(childname)), Children(move(children)), scope(s)
	{
		Type = ChildName->GetStrType();
	}
	void AddChild(unique_ptr<AST> child)
	{
		Children.push_back(move(child));
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	string GetStrType()
	{
		return ChildName->GetStrType();
	}

	string GetStrName()
	{
		return ChildName->GetStrName();
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return ChildName;
	}

	void print(int space)
	{
		space_print(space);
		cout << "Local( scope-" << scope << endl;
		ChildName->print(space);
		for (int i = 0; i < Children.size(); i++)
			Children[i]->print(space + 2);
		space_print(space);
		cout << ")" << endl;
	}

	void SetStrType(string type)
	{
		//cout << type << endl;
		Type = type;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTStatic : public AST
{
	unique_ptr<AST> ChildName;
	vector<unique_ptr<AST>> Children;
	string scope = "";
	string Type = "unssigned";
public:
	ASTStatic(unique_ptr<AST> childname, vector<unique_ptr<AST>> children) : ChildName(move(childname)), Children(move(children)) 
	{
		Type = ChildName->GetStrType();
	}
	void AddChild(unique_ptr<AST> child)
	{
		Children.push_back(move(child));
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	string GetStrType()
	{
		return ChildName->GetStrType();
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return ChildName;
	}

	void print(int space)
	{
		space_print(space);
		cout << "Static(" << endl;
		ChildName->print(space);
		//space_print(space);
		//cout << ")";
		for (int i = 0; i < Children.size(); i++)
			Children[i]->print(space + 2);
		space_print(space);
		cout << ")" << endl;
	}

	void SetStrType(string type)
	{
		//cout << type << endl;
		Type = type;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTDigit : public AST
{
	//unique_ptr<AST> Parent;
	string SystemNumber;
	string Val;
	string scope = "";

public:
	ASTDigit(string const &systemnumber, string const &val, string s) : SystemNumber(systemnumber), Val(val), scope(s) {}
	ASTDigit(string const &val, string s) : Val(val), scope(s) {}
	void print(int space)
	{
		space_print(space);
		cout << "( scope-" << scope << " " << SystemNumber << " " << Val << ")" << endl;
		
	}

	string GetStrName()
	{ 
		return Val; 
	}

	string GetStrType() 
	{ 
		return SystemNumber;
	}

	string GetSystemNumber()
	{
		return SystemNumber;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTString : public AST
{
	string String;
	string scope = "";
	string Type = "string";
	unique_ptr<AST> Method = nullptr;

public:
	ASTString(string const &str, string s) : String(str), scope(s) {}
	ASTString(string const &str, string s, unique_ptr<AST> method) : String(str), scope(s), Method(move(method)) {}
	void print(int space)
	{
		space_print(space);
		cout << "String( scope-" << scope << " " << String << ")" << endl;
		if (Method != nullptr)
			Method->print(space + 2);
	}

	string GetStrName()
	{
		return String;
	}

	string GetStrType()
	{
		return Type;
	}

	string GetScope()
	{
		return scope;
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return Method;
	}
};

class ASTIdentifier : public AST
{
	string Name = "unssigned";
	string Type = "unssigned";
	unique_ptr<AST> Method = nullptr;
	bool Declar = false;
	string scope = "";
	bool Length = false;
	//unique_ptr<AST> Parent;

public:
	ASTIdentifier(string const &name, string const &type, string s, bool d = false) : Name(name), Type(type), scope(s), Declar(d) {}
	ASTIdentifier(string const &name, unique_ptr<AST> method, string s) : Name(name), Method(move(method)), scope(s)
	{
		if (Method->GetStrName() == "Write")
		{
			Type = "void";
		}
		else if (Method->GetStrName() == "ReadLine")
		{
			Type = "var";
		}
	}
	ASTIdentifier(string const &name, string s) : Name(name), scope(s) {}

	void SetMethodLength() { Length = true;}

	bool GetMethodLength() { return Length; }

	bool GetDeclar() { return Declar; }

	void print(int space)
	{
		space_print(space);
		cout << "( scope-" << scope << " " << Type << " " << Name << endl;
		if (Method != nullptr)
			Method->print(space + 2);
		space_print(space);
		cout << ")" << endl;
	}

	string GetStrType()
	{
		return Type;
	}

	string GetStrName()
	{
		return Name;
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return Method;
	}

	void SetStrType(string type)
	{
		//cout << type << endl;
		Type = type;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTArray : public AST
{
	//string Name;
	string Type = "unssigned";
	string scope = "";
	unique_ptr<AST> Array;
	vector<unique_ptr<AST>> Size;
	vector<unique_ptr<AST>> Index;
	char flagNew;

public:
	ASTArray(unique_ptr<AST> array_, vector<unique_ptr<AST>> index, string s, char flagn = 1) : Array(move(array_)), Index(move(index)), scope(s), flagNew(flagn) {}
	ASTArray(unique_ptr<AST> array_, string const &type, vector<unique_ptr<AST>> size, string s, char flagn = 0) : Array(move(array_)), Type(type), Size(move(size)), scope(s), flagNew(flagn) {}
	void print(int space)
	{
		space_print(space);
		if (!flagNew)
		{
			cout << "Array new( scope-" << scope << " " << Type << endl;
			Array->print(space + 2);
			
			space_print(space + 2);
			cout << "[" << endl;
			for (int i = 0; i < Size.size(); i++)
			{
				Size[i]->print(space + 4);
			}
			space_print(space + 2);
			cout << "]" << endl;

		}
		else
		{
			cout << "Array( scope-" << scope << endl;
			Array->print(space + 2);

			space_print(space + 2);
			cout << "[" << endl;
			for (int i = 0; i < Index.size(); i++)
			{
				Index[i]->print(space + 4);
			}
			space_print(space + 2);
			cout << "]" << endl;
		}
		space_print(space);
		cout << ")" << endl;
	}

	char GetFlag()
	{ 
		return flagNew;
	}

	vector<unique_ptr<AST>> const &GetArraySizes() 
	{
		return Size;
	}
	
	vector<unique_ptr<AST>> const &GetIndex() 
	{ 
		return Index; 
	}

	string GetStrType()
	{
		return Type;
	}

	string GetStrName()
	{
		return Array->GetStrName();
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return Array;
	}

	void SetStrType(string type)
	{
		//cout << type << endl;
		Type = type;
	}

	vector<unique_ptr<AST>> const &GetSize()
	{
		return Size;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTElse : public AST
{
	vector<unique_ptr<AST>> Children;
	string scope = "";
public:
	ASTElse(vector<unique_ptr<AST>> children, string s) : Children(move(children)), scope(s) {}
	void print(int space)
	{
		space_print(space);
		cout << "Else( scope-" << scope << endl;
		for (int i = 0; i < Children.size(); i++)
		{
			Children[i]->print(space + 2);
		}
		space_print(space);
		cout << ")" << endl;
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTIf : public AST
{
	string scope = "";
	//unique_ptr<AST> Parent;
	unique_ptr<AST> Expression;
	unique_ptr<AST> Else = nullptr;
	vector<unique_ptr<AST>> Children;
public:
	ASTIf(unique_ptr<AST> expression, vector<unique_ptr<AST>> children, string s) : Expression(move(expression)), Children(move(children)), scope(s) {}
	ASTIf(unique_ptr<AST> expression, vector<unique_ptr<AST>> children, unique_ptr<AST> else_, string s) : Expression(move(expression)), Children(move(children)), Else(move(else_)), scope(s) {}
	void print(int space)
	{
		space_print(space);
		cout << "If( scope-" << scope << endl;
		
		Expression->print(space + 2);
		
		space_print(space);
		cout << ")" << endl;
		
		space_print(space + 2);
		cout << "If body(" << endl;
		
		for (int i = 0; i < Children.size(); i++)
		{
			Children[i]->print(space + 2);
		}
		
		space_print(space + 2);
		cout << ")" << endl;

		if (Else != nullptr)
		{
			Else->print(space);
		}
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return Expression;
	}

	virtual unique_ptr<AST> const &GetExpressionR()
	{
		return Else; 
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTWhile : public AST
{
	//unique_ptr<AST> Parent;
	string scope = "";
	unique_ptr<AST> Expression;
	vector<unique_ptr<AST>> Children;
public:
	ASTWhile(unique_ptr<AST> expression, vector<unique_ptr<AST>> children, string s) : Expression(move(expression)), Children(move(children)), scope(s) {}
	ASTWhile(vector<unique_ptr<AST>> children, string s) : Children(move(children)), scope(s) {}
	void print(int space) {
		space_print(space);
		cout << "While( scope-" << scope << endl;

		Expression->print(space + 2);

		space_print(space);
		cout << ")" << endl;

		space_print(space);
		cout << "While body(" << endl;

		for (int i = 0; i < Children.size(); i++)
		{
			Children[i]->print(space + 2);
		}

		space_print(space);
		cout << ")" << endl;
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return Expression;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTSubBlock : public AST
{
	//unique_ptr<AST> Parent;
	vector<unique_ptr<AST>> Children;
	string scope = "";
public:
	ASTSubBlock(vector<unique_ptr<AST>> children, string s) : Children(move(children)), scope(s) {}
	void print(int space) {

		space_print(space);
		cout << "SubBlock body( scope-" << scope << endl;

		for (int i = 0; i < Children.size(); i++)
		{
			Children[i]->print(space + 2);
		}

		space_print(space);
		cout << ")" << endl;
	}

	vector<unique_ptr<AST>> const &GetChild()
	{
		return Children;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTOperationBin : public AST
{
	//unique_ptr<AST> Parent;
	unique_ptr<AST> Left = nullptr;
	unique_ptr<AST> Right = nullptr;
	string Op;
	string scope = "";
	
public:
	ASTOperationBin(unique_ptr<AST> left, unique_ptr<AST> right, string op, string s)// : Right(move(right)), Left(move(left)), Op(op), scope(scope) 
	{
		Op = op;
		scope = s;
		Left = move(left);
		Right = move(right);
	}
	//ASTOperationBin(unique_ptr<AST> left) : Left(move(left)) {}
	void print(int space)
	{
		space_print(space);
		cout << Op << " scope-" << scope << endl;
		if (Left != nullptr) Left->print(space + 2);
		if (Right != nullptr) Right->print(space + 2);
		//cout << endl;
	}

	string GetStrName()
	{
		return Op;
	}

	unique_ptr<AST> const &GetExpressionL() 
	{ 
		return Left; 
	}

	unique_ptr<AST> const &GetExpressionR()
	{
		return Right; 
	}

	string GetStrType()
	{
		return Left->GetStrType();
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTCallFun : public AST
{
	string Name;
	//unique_ptr<AST> Parent;
	vector<unique_ptr<AST>> Args;
	string scope = "";
	string Type = "unssigned";
public:
	ASTCallFun(string const &name, vector<unique_ptr<AST>> args, string s) : Name(name), Args(move(args)), scope(s) {}
	
	void print(int space)
	{
		space_print(space);
		cout << "scope - " << scope << " " << Type << " " << Name << "(" << endl;
		for (int i = 0; i < Args.size(); i++)
			Args[i]->print(space + 2);
		space_print(space);
		cout << ")" << endl;
	}

	vector<unique_ptr<AST>> const &GetArgs()
	{
		return Args;
	}

	string GetStrName()
	{
		return Name;
	}

	string GetStrType()
	{
		return Type;
	}

	void SetStrType(string type)
	{
		cout << type << endl;
		Type = type;
	}

	string GetScope()
	{
		return scope;
	}
};

class ASTReturn : public AST
{
	unique_ptr<AST> Id = nullptr;
	string scope = "";
public:
	ASTReturn(unique_ptr<AST> id, string s) : Id(move(id)), scope(s) {}
	ASTReturn(string s) : scope(s) {}
	void print(int space)
	{
		space_print(space);
		cout << "return( scope-" << scope << endl;
		if (Id != nullptr)
			Id->print(space + 2);
		space_print(space);
		cout << ")" << endl;
	}

	string GetScope()
	{
		return scope;
	}

	unique_ptr<AST> const &GetExpressionL()
	{
		return Id;
	}
};
