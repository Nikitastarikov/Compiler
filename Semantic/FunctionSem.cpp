#include "Semantica.h"

bool SemanticAnalysis::SemanticAnalysisStart(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table)
{
	int Index = 0;
	//Проход по дереву
	while (Index < VectorClass.size())
	{
		string NodeName = typeid(*VectorClass[Index]).name();// Запись имени узла
		RedactionString(NodeName);//Строка приводится в годное состояние

		if (NodeName == "ASTClass")
		{
			//Проверка семантики узла класса
			if (!CheckingClassSemantics(VectorClass[Index], Table))
			{
				Index = VectorClass.size();
				//cout << endl << "Semantic error: inside class: " + VectorClass[Index]->GetStrName() << endl;
				return false;
			}	
		}
		Index++;
	}
	return true;
}

bool SemanticAnalysis::CheckingClassSemantics(unique_ptr<AST> const &NodeClass, map<string, vector<ScopeVar>> &Table)
{
	int Index = 0;

	if (!NodeClass->GetChild().empty())
	{
		while (Index < NodeClass->GetChild().size())
		{
			string NodeName = typeid(*NodeClass->GetChild()[Index]).name();
			RedactionString(NodeName);//Строка приводится в годное состояние

 			if (NodeName == "ASTStatic")
			{
				//Обработка семантики статической функции
				if (!CheckingBodyFunSemantics(NodeClass->GetChild()[Index], Table, NodeClass->GetChild()[Index]->GetStrType()))
				{
					Index = NodeClass->GetChild().size();
					return false;
				}	
			}
			Index++;
		}
	}
	return true;
}

//Функция проверки семантики тела функции
bool SemanticAnalysis::CheckingBodyFunSemantics(unique_ptr<AST> const &NodeBodyFun, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	int Index = 0;

	if (!NodeBodyFun->GetChild().empty())
	{
		while (Index < NodeBodyFun->GetChild().size())
		{
			string NodeName = typeid(*NodeBodyFun->GetChild()[Index]).name();
			RedactionString(NodeName);//Строка приводится в годное состояние

			if (NodeName == "ASTArray" && !CheckSemanticArray(NodeBodyFun->GetChild()[Index], Table))// Проверка семантики массива
			{
				SemError("array");
				return false;
			}
			else if (NodeName == "ASTOperationBin" && !StartCheckSemanticOperation(NodeBodyFun->GetChild()[Index], Table))//Проверка семантики операций
			{
				SemError("inside operation block");
				return false;
			}
			else if (NodeName == "ASTReturn" && !CheckSemanticReturn(NodeBodyFun->GetChild()[Index], Table, ReturnType))//Проверка семантики блока return!!!
			{
				SemError("inside return block");
				return false;
			}
			else if (NodeName == "ASTSubBlock" && !CheckSemanticSubBlock(NodeBodyFun->GetChild()[Index], Table, ReturnType))//Проверка семантики подблока
			{
				SemError("inside subblock");
				return false;
			}
			else if (NodeName == "ASTWhile" && !CheckSemanticWhile(NodeBodyFun->GetChild()[Index], Table, ReturnType))//Проверка семантики блока while
			{
				SemError("inside while block");
				return false;
			}
			else if (NodeName == "ASTIf" && !CheckSemanticIf(NodeBodyFun->GetChild()[Index], Table, ReturnType))//Проверка семантики блока if
			{
				SemError("inside if block");
				return false;
			}
			else if (NodeName == "ASTLocal" && !CheckSemanticLocalFun(NodeBodyFun->GetChild()[Index], Table, NodeBodyFun->GetChild()[Index]->GetExpressionL()->GetStrType()))//Проверка семантики блока локальной фун-ии
			{
				SemError("inside LocalFun: " + NodeBodyFun->GetChild()[Index]->GetStrName());
				return false;
			}
			else if (NodeName == "ASTCallFun" && !CheckSemanticCallFun(NodeBodyFun->GetChild()[Index], Table))//Проверка семантики блока вызова функции
			{
				SemError("CallFun: " + NodeBodyFun->GetChild()[Index]->GetStrName());
				return false;
			}
			else if (NodeName == "ASTIdentifier" && !CheckSemanticId(NodeBodyFun->GetChild()[Index], Table))//Проверка семантики переменной
			{
				SemError("id:" + NodeBodyFun->GetChild()[Index]->GetStrName());
				return false;
			}
			Index++;
		}
	}
	return true;
}

bool SemanticAnalysis::CallArrayElement(unique_ptr<AST> const &NodeArray, map<string, vector<ScopeVar>> &Table, string NodeName, string SubNodeName)
{
	//Взимается указатель на узел
	auto NodePtr = SearchFunctionOrArrayDefinition(NodeArray->GetStrName(), NodeArray->GetScope(), NodeArray->GetStrType(), Table, NodeName);

	if (NodePtr != nullptr) {
		if (NodeArray->GetIndex().size() != NodePtr->GetSize().size())// не совпадает размерность массива
		{
			SemError("size array: " + NodePtr->GetStrName() + " " + to_string(NodeArray->GetIndex().size()) + " != " + to_string(NodePtr->GetSize().size()));
			return false;
		}

		//Проверка на выход из массива и целочисленность индекса
		for (int Index = 0; Index < NodeArray->GetIndex().size(); Index++)
		{
			SubNodeName = typeid(*NodeArray->GetIndex()[Index]).name();
			RedactionString(SubNodeName);//Строка приводится в годное состояние
			if (SubNodeName == "ASTIdentifier")
			{
				if (NodeArray->GetIndex()[Index]->GetSystemNumber() == "NotInteger")
				{
					SemError(NodeArray->GetIndex()[Index]->GetStrName() + " array size non-integer");
					return false;
				}
				//if (NodeArray->GetIndex()[Index]->GetStrName() >= NodeArray->GetSize()[Index]->) {}
			}
			else if (SubNodeName == "ASTDigit")
			{
				
				if (NodeArray->GetIndex()[Index]->GetStrType() == "NotInteger")
				{
					SemError(NodeArray->GetIndex()[Index]->GetStrName() + " array size non-integer");
					return false;
				}

				SubNodeName = typeid(*NodeArray->GetIndex()[Index]).name();
				RedactionString(SubNodeName);//Строка приводится в годное состояние
				
				if (SubNodeName == "ASTDigit" && atoi(NodeArray->GetIndex()[Index]->GetStrName().c_str()) >= atoi(NodePtr->GetSize()[Index]->GetStrName().c_str()) || atoi(NodeArray->GetIndex()[Index]->GetStrName().c_str()) < 0)
				{
					SemError("segmentaion fault(array): " + NodePtr->GetStrName());
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

bool SemanticAnalysis::CallStringElement(unique_ptr<AST> const &NodeArray, map<string, vector<ScopeVar>> &Table)
{
	if (NodeArray->GetIndex().size() != 1)
	{
		SemError(NodeArray->GetStrName() + " wrong number of Indexs");

		return false;
	}

	for (int Index = 0; Index < NodeArray->GetIndex().size(); Index++)
	{
		string NodeName = typeid(*NodeArray->GetIndex()[Index]).name();
		RedactionString(NodeName);//Строка приводится в годное состояние

		if (NodeName == "ASTIdentifier" || NodeName == "ASTDigit")
		{
			if (NodeArray->GetIndex()[Index]->GetStrType() == "NotInteger")
			{
				SemError(NodeArray->GetIndex()[Index]->GetStrName() + " Array size non-integer");

				return false;
			}

			if (NodeName == "ASTDigit" && atoi(NodeArray->GetIndex()[Index]->GetStrName().c_str()) < 0)
			{
				SemError(NodeArray->GetIndex()[Index]->GetStrName() + " Index < 0");

				return false;
			}
		}
	}
	return true;
}

//Функция проверки семантики массива
bool SemanticAnalysis::CheckSemanticArray(unique_ptr<AST> const &NodeArray, map<string, vector<ScopeVar>> &Table)
{
	string NodeName = typeid(*NodeArray).name();
	RedactionString(NodeName);//Строка приводится в годное состояние
	if (!NodeArray->GetFlag())//Проверка наличия оператора new
	{
		//Цикл для прохода по размерам массива	
		for (int Index = 0; Index < NodeArray->GetSize().size(); Index++)
		{
			string SubNodeName = typeid(*NodeArray->GetSize()[Index]).name();
			RedactionString(SubNodeName);//Строка приводится в годное состояние

			//размер массива должен указываться только целым числом или переменной
			if (SubNodeName == "ASTIdentifier" && NodeArray->GetSize()[Index]->GetSystemNumber() == "NotInteger")
			{	
				SemError(NodeArray->GetSize()[Index]->GetStrName() + " Array size non-integer");
				return false;
			}
			else if (SubNodeName == "ASTDigit" && NodeArray->GetSize()[Index]->GetStrType() == "NotInteger")
			{
				SemError(NodeArray->GetSize()[Index]->GetStrName() + " Array size non-integer");
				return false;
			}
		}
	}
	else//Если это не определение динамического массива
	{
		//Проверка, что это действительно массив
		string SubNodeName = CheckIdTableSymbol(NodeArray->GetStrName(), NodeArray->GetScope(), NodeArray->GetStrType(), Table, NodeName);
		if (SubNodeName == "")
		{
			SubNodeName = typeid(*NodeArray).name();
			RedactionString(SubNodeName);//Строка приводится в годное состояние
			SemError(SubNodeName + " - Wrong node type, expression is not an array");
			return false;
		}
		else if (SubNodeName != "string")//Проверка семантики вызова элемента массива
		{
			return CallArrayElement(NodeArray, Table, NodeName, SubNodeName);
		}
		else//Проверка семантики вызова элемента строки
		{
			return CallStringElement(NodeArray, Table);
		}
	}

	return true;
}

//Проверка семантики правой части(проверка семантики операций)
bool SemanticAnalysis::CheckSemanticOperation(unique_ptr<AST> const &NodeOperation, map<string, vector<ScopeVar>> &Table)
{
	if (NodeOperation->GetExpressionR() == nullptr && (NodeOperation->GetStrName() == "++" || NodeOperation->GetStrName() == "--")) 
	{
		return true;
	}

	string NodeName = typeid(*NodeOperation->GetExpressionR()).name();
	RedactionString(NodeName);//Строка приводится в годное состояние

	if (NodeName == "ASTOperationBin" && !CheckSemanticOperation(NodeOperation->GetExpressionR(), Table))// Если дерево операций продолжается, продолжается проверка
	{
		return false;
	}
	else if (NodeName == "ASTIdentifier" || NodeName == "ASTArray" || NodeName == "ASTString" || NodeName == "ASTDigit" || NodeName == "ASTCallFun")
	{
		string NodeRType = NodeOperation->GetExpressionR()->GetStrType();//Запись типа правого узла
		
		if (NodeName == "ASTArray" && !CheckSemanticArray(NodeOperation->GetExpressionR(), Table))//Проверка семантики массива
		{
			return false;
		}
		else if (NodeName == "ASTCallFun" && !CheckSemanticCallFun(NodeOperation->GetExpressionR(), Table))//Проверка семантики	вызова функции
		{
			return false;
		}
		else if (NodeName == "ASTIdentifier" && !CheckSemanticId(NodeOperation->GetExpressionR(), Table))//Проверка семантики переменной
		{
			return false;
		}

		//NodeRType = NodeOperation->GetExpressionR()->GetStrType();

		//NodeName = typeid(*NodeOperation->GetExpressionL()).name();
		//RedactionString(NodeName);//Строка приводится в годное состояние

		//cout << "op " << NodeName << endl;
		//if (NodeName == "ASTDigit" && NodeOperation->GetExpressionR()->GetStrType() != "NotInteger")
		//		NodeName = "int";
		//else
		NodeName = typeid(*NodeOperation->GetExpressionL()).name();
		RedactionString(NodeName);

		//cout << "NodeName = " << NodeName << endl;
		//cout << "Name = " << NodeOperation->GetStrName() << endl;

		if (NodeName == "ASTOperationBin")
		{
			NodeName = NodeOperation->GetExpressionL()->GetExpressionR()->GetStrType();
		}
		else if (NodeName == "AST")
		{
			NodeName = NodeOperation->GetExpressionR()->GetStrType();
			//cout << "NodeName = " << NodeName << endl;
		}
		else
		{
			NodeName = NodeOperation->GetExpressionL()->GetStrType();
		}

		if (NodeOperation->GetExpressionR()->GetStrName() == "Console" && NodeOperation->GetExpressionR()->GetExpressionL() != nullptr/* && NodeOperation->GetExpressionR()->GetExpressionL()->GetStrName() == "ReadLine"*/)
		{
			//cout << NodeOperation->GetExpressionR()->GetExpressionL()->GetStrName() << endl;
		}
		else if (!CheckTwoTypes(NodeName, NodeRType))//Проверка возможности привидения определенных типов
		{
			cout << "There" << endl;
			SemError(NodeOperation->GetExpressionL()->GetStrType() + " can't lead to " + NodeRType + ": " + NodeOperation->GetExpressionL()->GetStrName() + " " + NodeOperation->GetStrName() + " " + NodeOperation->GetExpressionR()->GetStrName());
			return false;
		}
	}
	return true;
}

//Функция проверки семантики операций
bool SemanticAnalysis::StartCheckSemanticOperation(unique_ptr<AST> const &NodeOper, map<string, vector<ScopeVar>> &Table)
{
	string NodeName = typeid(*NodeOper->GetExpressionL()).name();
	RedactionString(NodeName);//Строка приводится в годное состояние

	//cout << "StartCheckSemanticOperation: " << NodeName << endl;
	if (NodeName == "ASTIdentifier" && !CheckSemanticId(NodeOper->GetExpressionL(), Table))//Проверка семантики id
	{
		return false;
	}
	else if (NodeName == "ASTArray" && !CheckSemanticArray(NodeOper->GetExpressionL(), Table))//Проверка семантики массива
	{
		return false;
	}
	else if (NodeName == "ASTCallFun" && !CheckSemanticCallFun(NodeOper->GetExpressionL(), Table))//Проверка семантики вызова функции
	{
		return false;
	}

	if (!CheckSemanticOperation(NodeOper, Table))//Проверка семантики правой части операций
	{
		return false;
	}

	return true;
}

bool SemanticAnalysis::CheckReturnElement(string TypeOne, string ReturnType, bool Element)
{
	if (!Element) return false;

	return CheckReturnElement(TypeOne, ReturnType);
}

bool SemanticAnalysis::CheckReturnElement(string TypeOne, string ReturnType)
{
	if (!CheckTwoTypes(TypeOne, ReturnType))
	{
		SemError(TypeOne + " can't lead to " + ReturnType);

		return false;
	}
	return true;
}

//Функция проверки семантики блока return
bool SemanticAnalysis::CheckSemanticReturn(unique_ptr<AST> const &NodeReturn, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	string NameNode = typeid(*NodeReturn->GetExpressionL()).name();
	RedactionString(NameNode);//Строка приводится в годное состояние

	//cout << "CheckSemanticReturn " << NameNode << endl;
	if (NameNode == "ASTIdentifier" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType, CheckSemanticId(NodeReturn->GetExpressionL(), Table)))
	{
		return false;
	}
	else if (NameNode == "ASTString" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType))	
	{
		return false;
	}
	else if (NameNode == "ASTOperationBin" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType, CheckSemanticOperation(NodeReturn->GetExpressionL(), Table)))
	{
		return false;
	}
	else if (NameNode == "ASTArray" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType, CheckSemanticArray(NodeReturn->GetExpressionL(), Table)))
	{
		return false;
	}
	else if (NameNode == "ASTCallFun" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType, CheckSemanticCallFun(NodeReturn->GetExpressionL(), Table)))
	{
		return false;
	}
	else if (NameNode == "ASTDigit")
	{
		string NumberType;
		if (NodeReturn->GetExpressionL()->GetStrType() != "NotInteger")
		{
			NumberType = "int";
		}
		else
		{
			NumberType = "NotInteger";
		}

		if (!CheckTwoTypes(NumberType, ReturnType))
		{
			SemError(NodeReturn->GetExpressionL()->GetStrType() + " can't lead to " + ReturnType);

			return false;
		}
	}
	return true;
}

//Функиция проверки семантики id
bool SemanticAnalysis::CheckSemanticId(unique_ptr<AST> const &NodeId, map<string, vector<ScopeVar>> &Table)
{
	string NodeName = typeid(*NodeId).name();
	RedactionString(NodeName);//Строка приводится в годное состояние

	if (NodeId->GetStrName() != "Console")
	{
		NodeName = CheckIdTableSymbol(NodeId->GetStrName(), NodeId->GetScope(), NodeId->GetStrType(), Table, NodeName);

		//cout << "CheckSemanticId: " << NodeName << endl;
		if (NodeName == "")
		{
			NodeName = typeid(*NodeId).name();
			RedactionString(NodeName);//Строка приводится в годное состояние
			SemError(NodeName + " - Wrong node type");

			return false;
		}
	}
	return true;
}

bool SemanticAnalysis::CheckSemanticSubBlock(unique_ptr<AST> const &NodeSubBlock, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	if (!CheckingBodyFunSemantics(NodeSubBlock, Table, ReturnType)) 
	{
		return false;
	}
	return true;
}

bool SemanticAnalysis::CheckSemanticWhile(unique_ptr<AST> const &NodeWhile, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	string NodeName = typeid(*NodeWhile->GetExpressionL()).name();
	RedactionString(NodeName);//Строка приводится в годное состояние

	if (NodeName == "ASTOperationBin" && !StartCheckSemanticOperation(NodeWhile->GetExpressionL(), Table))
		return false;
	else if (NodeName == "ASTArray" && !CheckSemanticArray(NodeWhile->GetExpressionL(), Table))
		return false;
	else if (NodeName == "ASTCallFun" && !CheckSemanticCallFun(NodeWhile->GetExpressionL(), Table))
		return false;

	if (!CheckingBodyFunSemantics(NodeWhile, Table, ReturnType)) return false;

	return true;
}

bool SemanticAnalysis::CheckSemanticIf(unique_ptr<AST> const &NodeIf, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	string NodeName = typeid(*NodeIf->GetExpressionL()).name();
	RedactionString(NodeName);//Строка приводится в годное состояние

	if (NodeName == "ASTOperationBin" && !StartCheckSemanticOperation(NodeIf->GetExpressionL(), Table))
	{
		return false;
	}
	else if(NodeName == "ASTArray" && !CheckSemanticArray(NodeIf->GetExpressionL(), Table))
	{
		return false;
	}
	else if (NodeName == "ASTCallFun" && !CheckSemanticCallFun(NodeIf->GetExpressionL(), Table))
	{
		return false;
	}

	if (!CheckingBodyFunSemantics(NodeIf, Table, ReturnType)) 
	{
		return false;
	}

	if (NodeIf->GetExpressionR() != nullptr && !CheckingBodyFunSemantics(NodeIf->GetExpressionR(), Table, ReturnType))
	{
		SemError("inside else block");

		return false;
	}

	return true;
}

bool SemanticAnalysis::CheckSemanticLocalFun(unique_ptr<AST> const &NodeLocalFun, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	if (!CheckingBodyFunSemantics(NodeLocalFun, Table, ReturnType))	
	{
		return false;
	}
	return true;
}

bool SemanticAnalysis::CheckCallFunArguments(unique_ptr<AST> const &NodeCallFun, map<string, vector<ScopeVar>> &Table, AST* const PtrNode, string NodeName)
{
	if (PtrNode != nullptr)
	{
		if (PtrNode->GetArgs().size() != NodeCallFun->GetArgs().size())
		{
			SemError("CallFun: " + NodeCallFun->GetStrName() + " invalid number of arguments");
			return false;
		}

		for (int Index = 0; Index < PtrNode->GetArgs().size(); Index++)
		{
			NodeName = typeid(*NodeCallFun->GetArgs()[Index]).name();
			RedactionString(NodeName);//Строка приводится в годное состояние
			if (NodeName == "ASTDigit")
			{
				if (NodeCallFun->GetArgs()[Index]->GetStrType() != "NotInteger")
					NodeName = "int";
				else
					NodeName = "NotInteger";
			}
			else
			{
				NodeName = NodeCallFun->GetArgs()[Index]->GetStrType();
			}

			//cout << NodeName << " " << PtrNode->GetArgs()[Index]->GetNodeNameType() << endl;
			if (PtrNode->GetArgs()[Index]->GetStrType() != NodeName)
			{
				SemError("CallFun: " + NodeCallFun->GetStrName() + " wrong type of arguments");
				return false;
			}
		}
		return true;
	}
	return false;
}

bool SemanticAnalysis::CheckSemanticCallFun(unique_ptr<AST> const &NodeCallFun, map<string, vector<ScopeVar>> &Table)
{
	//cout << "CheckSemanticCallFun " << typeid(*NodeCallFun).name() << endl;
	string NodeName = typeid(*NodeCallFun).name();
	RedactionString(NodeName);//Строка приводится в годное состояние

	NodeName = CheckIdTableSymbol(NodeCallFun->GetStrName(), NodeCallFun->GetScope(), NodeCallFun->GetStrType(), Table, NodeName);
	RedactionString(NodeName);//Строка приводится в годное состояние

	if (NodeName == "")
	{
		NodeName = typeid(*NodeCallFun).name();
		RedactionString(NodeName);//Строка приводится в годное состояние
		SemError(NodeName + " - wrong node type");

		return false;
	}

	NodeName = typeid(*NodeCallFun).name();
	RedactionString(NodeName);//Строка приводится в годное состояние
	auto PtrNode = SearchFunctionOrArrayDefinition(NodeCallFun->GetStrName(), NodeCallFun->GetScope(), NodeCallFun->GetStrType(), Table, NodeName);

	return CheckCallFunArguments(NodeCallFun, Table, PtrNode, NodeName);
}

string SemanticAnalysis::CheckIdName(vector<ScopeVar> &it, string name/*, map<string, vector<ScopeVar>> const &Table*/, string NodeType)
{
	//cout << "CheckIdName: " << NodeType << " " << name << endl;
	string NodeName;
	if (!it.empty())
	{
		for (int Index = 0; Index < it.size(); Index++)
		{
			if (it[Index].getNode() != nullptr && it[Index].getNode()->GetStrName() == name)
			{
				NodeName = typeid(*it[Index].getNode()).name();
				RedactionString(NodeName);//Строка приводится в годное состояние
				if (NodeType == "ASTCallFun" && NodeName == "ASTFun" || NodeName == NodeType)
					return it[Index].getNode()->GetStrType();
				//cout << NodeName << endl;
			}	
		}
	}
	return "";
}

string SemanticAnalysis::CheckIdTableSymbol(string Name, string Scope, string Type, map<string, vector<ScopeVar>> &Table, string NodeType)
{
	if (Type == "string" && NodeType == "ASTArray")
	{
		NodeType = "ASTIdentifier";
	}

	//cout << "CheckIdTableSymbol: " << Type << " " << Name << " " << NodeType << " " << Scope << endl;
	map<string, vector<ScopeVar>>::iterator it = Table.begin();
	
	for (; it != Table.end(); it++)
	{
		int limit = it->first.size();
		//cout << Scope << " " << it->first << endl;
		if (Scope.substr(0, limit) == it->first.substr(0, limit))
		{
			string NameId;
			
			NameId = CheckIdName(it->second, Name, NodeType);
			if (NameId != "") 
			{
				return NameId;
			}
		}
	}

	return "";
}

AST* const /*&*/SemanticAnalysis::SearchFunctionOrArrayDefinition(string Name, string Scope, string Type, map<string, vector<ScopeVar>> &Table, string NodeType)
{
	//cout << "SearchFunctionDefinition " << Name << " " << Type << endl;
	if (Type == "string" && NodeType == "ASTArray")
	{
		NodeType = "ASTIdentifier";
	}

	map<string, vector<ScopeVar>>::iterator it = Table.begin();
	AST *PtrNode = nullptr;
	
	for (; it != Table.end(); it++)
	{
		int limit = it->first.size();
		//cout << Scope << " " << it->first << endl;
		if (Scope.substr(0, limit) == it->first.substr(0, limit))
		{
			if (!it->second.empty())
			{
				for (int Index = 0; Index < it->second.size(); Index++)
				{
					if (it->second[Index].getNode() != nullptr && it->second[Index].getNode()->GetStrName() == Name)
					{
						string NodeName = typeid(*it->second[Index].getNode()).name();
						RedactionString(NodeName);//Строка приводится в годное состояние

						if (NodeName == "ASTFun" && NodeType == "ASTCallFun" || NodeName == NodeType)
						{
							PtrNode = it->second[Index].getNode();
						}
					}
				}
			}
		}
	}
	
	return PtrNode;
}

bool SemanticAnalysis::CheckTwoTypes(string TypeOne, string TypeTwo)
{
	//cout << "Type: " << TypeOne << ":" << TypeTwo << endl;
	if (TypeOne == "var" || TypeTwo == "var")
	{
		return true;
	}
	else if (TypeOne == "int" || TypeOne == "double" || TypeOne == "DecimalInteger" || TypeOne == "NotAnInteger")
	{
		if (TypeTwo == "int" || TypeTwo == "double" || TypeTwo == "DecimalInteger" || TypeTwo == "NotAnInteger")
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (TypeOne == "string" || TypeOne == "char")
	{
		if (TypeTwo == "string" || TypeTwo == "char")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}