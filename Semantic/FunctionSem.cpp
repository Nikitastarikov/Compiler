#include "Semantica.h"

bool SemanticAnalysis::SemanticAnalysisStart(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table)
{
	int index = 0;
	//Проход по дереву
	while (index < VectorClass.size())
	{
		string str = typeid(*VectorClass[index]).name();// Запись имени узла
		RedactionString(str);//Строка приводится в годное состояние

		if (str == "ASTClass")
		{
			//Проверка семантики узла класса
			if (!CheckingClassSemantics(VectorClass[index], Table))
			{
				index = VectorClass.size();
				//cout << endl << "Semantic error: inside class: " + VectorClass[index]->GetStrName() << endl;
				return false;
			}	
		}
		index++;
	}
	//cout << endl << "Semantic GooD!!!" << endl;
	return true;

}

bool SemanticAnalysis::CheckingClassSemantics(unique_ptr<AST> const &NodeClass, map<string, vector<ScopeVar>> &Table)
{
	int index = 0;

	if (!NodeClass->GetChild().empty())
	{
		while (index < NodeClass->GetChild().size())
		{
			string str = typeid(*NodeClass->GetChild()[index]).name();
			RedactionString(str);//Строка приводится в годное состояние

			//cout << "Inside " << typeid(*NodeClass).name() << " CheckingClassSemantics: " << str << endl;
 			if (str == "ASTStatic")
			{
				//Обработка семантики статической функции
				if (!CheckingBodyFunSemantics(NodeClass->GetChild()[index], Table, NodeClass->GetChild()[index]->GetStrType()))
				{
					index = NodeClass->GetChild().size();
					return false;
				}	
			}
			index++;
		}
	}
	return true;
}

//Функция проверки семантики тела функции
bool SemanticAnalysis::CheckingBodyFunSemantics(unique_ptr<AST> const &NodeBodyFun, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	//cout << "returntype = " << ReturnType << endl;
	int index = 0;

	if (!NodeBodyFun->GetChild().empty())
	{
		while (index < NodeBodyFun->GetChild().size())
		{
			string str = typeid(*NodeBodyFun->GetChild()[index]).name();
			RedactionString(str);//Строка приводится в годное состояние
			//cout << "Inside " << typeid(*NodeBodyFun).name() << " CheckingBodyFunSemantics: " << str << endl;
			if (str == "ASTArray" && !CheckSemanticArray(NodeBodyFun->GetChild()[index], Table))// Проверка семантики массива
			{
				SemError("array");
				return false;
			}
			else if (str == "ASTOperationBin" && !StartCheckSemanticOperation(NodeBodyFun->GetChild()[index], Table))//Проверка семантики операций
			{
				SemError("inside operation block");
				return false;
			}
			else if (str == "ASTReturn" && !CheckSemanticReturn(NodeBodyFun->GetChild()[index], Table, ReturnType))//Проверка семантики блока return!!!
			{
				SemError("inside return block");
				return false;
			}
			else if (str == "ASTSubBlock" && !CheckSemanticSubBlock(NodeBodyFun->GetChild()[index], Table, ReturnType))//Проверка семантики подблока
			{
				SemError("inside subblock");
				return false;
			}
			else if (str == "ASTWhile" && !CheckSemanticWhile(NodeBodyFun->GetChild()[index], Table, ReturnType))//Проверка семантики блока while
			{
				SemError("inside while block");
				return false;
			}
			else if (str == "ASTIf" && !CheckSemanticIf(NodeBodyFun->GetChild()[index], Table, ReturnType))//Проверка семантики блока if
			{
				SemError("inside if block");
				return false;
			}
			else if (str == "ASTLocal" && !CheckSemanticLocalFun(NodeBodyFun->GetChild()[index], Table, NodeBodyFun->GetChild()[index]->GetExpressionL()->GetStrType()))//Проверка семантики блока локальной фун-ии
			{
				SemError("inside LocalFun: " + NodeBodyFun->GetChild()[index]->GetStrName());
				return false;
			}
			else if (str == "ASTCallFun" && !CheckSemanticCallFun(NodeBodyFun->GetChild()[index], Table))//Проверка семантики блока вызова функции
			{
				SemError("CallFun: " + NodeBodyFun->GetChild()[index]->GetStrName());
				return false;
			}
			else if (str == "ASTIdentifier" && !CheckSemanticId(NodeBodyFun->GetChild()[index], Table))//Проверка семантики переменной
			{
				SemError("id:" + NodeBodyFun->GetChild()[index]->GetStrName());
				return false;
			}
			index++;
		}
	}
	return true;
}

bool SemanticAnalysis::CallArrayElement(unique_ptr<AST> const &NodeArray, map<string, vector<ScopeVar>> &Table, string NodeName, string str)
{
	//Взимается указатель на узел
	auto ptr = SearchFunctionOrArrayDefinition(NodeArray->GetStrName(), NodeArray->GetScope(), NodeArray->GetStrType(), Table, NodeName);

	if (ptr != nullptr) {
		if (NodeArray->GetIndex().size() != ptr->GetSize().size())// не совпадает размерность массива
		{
			SemError("size array: " + ptr->GetStrName() + " " + to_string(NodeArray->GetIndex().size()) + " != " + to_string(ptr->GetSize().size()));
			return false;
		}

		//Проверка на выход из массива и целочисленность индекса
		for (int index = 0; index < NodeArray->GetIndex().size(); index++)
		{
			str = typeid(*NodeArray->GetIndex()[index]).name();
			RedactionString(str);//Строка приводится в годное состояние
			if (str == "ASTIdentifier")
			{
				if (NodeArray->GetIndex()[index]->GetSystemNumber() == "NotInteger")
				{
					SemError(NodeArray->GetIndex()[index]->GetStrName() + " array size non-integer");
					return false;
				}
				//if (NodeArray->GetIndex()[index]->GetStrName() >= NodeArray->GetSize()[index]->) {}
			}
			else if (str == "ASTDigit")
			{
				
				if (NodeArray->GetIndex()[index]->GetStrType() == "NotInteger")
				{
					SemError(NodeArray->GetIndex()[index]->GetStrName() + " array size non-integer");
					return false;
				}

				str = typeid(*NodeArray->GetIndex()[index]).name();
				RedactionString(str);//Строка приводится в годное состояние
				
				if (str == "ASTDigit" && atoi(NodeArray->GetIndex()[index]->GetStrName().c_str()) >= atoi(ptr->GetSize()[index]->GetStrName().c_str()) || atoi(NodeArray->GetIndex()[index]->GetStrName().c_str()) < 0)
				{
					SemError("segmentaion fault(array): " + ptr->GetStrName());
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
		SemError(NodeArray->GetStrName() + " wrong number of indexs");
		return false;
	}

	for (int index = 0; index < NodeArray->GetIndex().size(); index++)
	{
		string str = typeid(*NodeArray->GetIndex()[index]).name();
		RedactionString(str);//Строка приводится в годное состояние

		if (str == "ASTIdentifier" || str == "ASTDigit")
		{
			if (NodeArray->GetIndex()[index]->GetStrType() == "NotInteger")
			{
				SemError(NodeArray->GetIndex()[index]->GetStrName() + " Array size non-integer");
				return false;
			}

			if (str == "ASTDigit" && atoi(NodeArray->GetIndex()[index]->GetStrName().c_str()) < 0)
			{
				SemError(NodeArray->GetIndex()[index]->GetStrName() + " index < 0");
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
		for (int index = 0; index < NodeArray->GetSize().size(); index++)
		{
			string str = typeid(*NodeArray->GetSize()[index]).name();
			RedactionString(str);//Строка приводится в годное состояние

			//размер массива должен указываться только целым числом или переменной
			if (str == "ASTIdentifier" && NodeArray->GetSize()[index]->GetSystemNumber() == "NotInteger")
			{	
				SemError(NodeArray->GetSize()[index]->GetStrName() + " Array size non-integer");
				return false;
			}
			else if (str == "ASTDigit" && NodeArray->GetSize()[index]->GetStrType() == "NotInteger")
			{
				SemError(NodeArray->GetSize()[index]->GetStrName() + " Array size non-integer");
				return false;
			}
		}
	}
	else//Если это не определение динамического массива
	{
		//Проверка, что это действительно массив
		string str = CheckIdTableSymbol(NodeArray->GetStrName(), NodeArray->GetScope(), NodeArray->GetStrType(), Table, NodeName);
		if (str == "")
		{
			str = typeid(*NodeArray).name();
			RedactionString(str);//Строка приводится в годное состояние
			SemError(str + " - Wrong node type, expression is not an array");
			return false;
		}
		else if (str != "string")//Проверка семантики вызова элемента массива
		{
			return CallArrayElement(NodeArray, Table, NodeName, str);
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
	if (NodeOperation->GetExpressionR() == nullptr && (NodeOperation->GetStrName() == "++" || NodeOperation->GetStrName() == "--")) return true;

	string NodeName = typeid(*NodeOperation->GetExpressionR()).name();
	RedactionString(NodeName);//Строка приводится в годное состояние

	//cout << "CheckSemanticOperation: " << NodeName << endl;
	//cout << NodeOperation->GetStrName() << endl;

	if (NodeName == "ASTOperationBin" && !CheckSemanticOperation(NodeOperation->GetExpressionR(), Table))// Если дерево операций продолжается, продолжается проверка
	{
		return false;
	}
	else if (NodeName == "ASTIdentifier" || NodeName == "ASTArray" || NodeName == "ASTString" || NodeName == "ASTDigit" || NodeName == "ASTCallFun")
	{
		string temp = NodeOperation->GetExpressionR()->GetStrType();//Запись типа правого узла
		
		if (NodeName == "ASTArray" && !CheckSemanticArray(NodeOperation->GetExpressionR(), Table))//Проверка семантики массива
			return false;
		else if (NodeName == "ASTCallFun" && !CheckSemanticCallFun(NodeOperation->GetExpressionR(), Table))//Проверка семантики	вызова функции
			return false;
		else if (NodeName == "ASTIdentifier" && !CheckSemanticId(NodeOperation->GetExpressionR(), Table))//Проверка семантики переменной
			return false;

		temp = NodeOperation->GetExpressionR()->GetStrType();

		NodeName = typeid(*NodeOperation->GetExpressionL()).name();
		RedactionString(NodeName);//Строка приводится в годное состояние
		//cout << "op " << NodeName << endl;
		//if (NodeName == "ASTDigit" && NodeOperation->GetExpressionR()->GetStrType() != "NotInteger")
		//		NodeName = "int";
		//else
			NodeName = NodeOperation->GetExpressionL()->GetStrType();
			//cout << NodeOperation->GetExpressionR()->GetStrName() << endl;

		if (NodeOperation->GetExpressionR()->GetStrName() == "Console" && NodeOperation->GetExpressionR()->GetExpressionL() != nullptr/* && NodeOperation->GetExpressionR()->GetExpressionL()->GetStrName() == "ReadLine"*/)
		{
			//cout << NodeOperation->GetExpressionR()->GetExpressionL()->GetStrName() << endl;
		}
		else if (!CheckTwoTypes(NodeName, temp))//Проверка возможности привидения определенных типов
		{
			SemError(NodeOperation->GetExpressionL()->GetStrType() + " can't lead to " + temp + ": " + NodeOperation->GetExpressionL()->GetStrName() + " " + NodeOperation->GetStrName() + " " + NodeOperation->GetExpressionR()->GetStrName());
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
		return false;
	else if (NameNode == "ASTString" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType))	
		return false;
	else if (NameNode == "ASTOperationBin" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType, CheckSemanticOperation(NodeReturn->GetExpressionL(), Table)))
		return false;
	else if (NameNode == "ASTArray" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType, CheckSemanticArray(NodeReturn->GetExpressionL(), Table)))
		return false;
	else if (NameNode == "ASTCallFun" && !CheckReturnElement(NodeReturn->GetExpressionL()->GetStrType(), ReturnType, CheckSemanticCallFun(NodeReturn->GetExpressionL(), Table)))
		return false;
	else if (NameNode == "ASTDigit")
	{
		string typedigit;
		if (NodeReturn->GetExpressionL()->GetStrType() != "NotInteger")
			typedigit = "int";
		else
			typedigit = "NotInteger";

		if (!CheckTwoTypes(typedigit, ReturnType))
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
		//cout << "!!!!" << NodeName << endl;
		//cout << NodeId->GetStrName() << endl;
		//cout << NodeId->GetScope() << endl;
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
	if (!CheckingBodyFunSemantics(NodeSubBlock, Table, ReturnType)) return false;
	return true;
}

bool SemanticAnalysis::CheckSemanticWhile(unique_ptr<AST> const &NodeWhile, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	string str = typeid(*NodeWhile->GetExpressionL()).name();
	RedactionString(str);//Строка приводится в годное состояние

	if (str == "ASTOperationBin" && !StartCheckSemanticOperation(NodeWhile->GetExpressionL(), Table))
		return false;
	else if (str == "ASTArray" && !CheckSemanticArray(NodeWhile->GetExpressionL(), Table))
		return false;
	else if (str == "ASTCallFun" && !CheckSemanticCallFun(NodeWhile->GetExpressionL(), Table))
		return false;

	if (!CheckingBodyFunSemantics(NodeWhile, Table, ReturnType)) return false;

	return true;
}

bool SemanticAnalysis::CheckSemanticIf(unique_ptr<AST> const &NodeIf, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	string str = typeid(*NodeIf->GetExpressionL()).name();
	RedactionString(str);//Строка приводится в годное состояние

	if (str == "ASTOperationBin" && !StartCheckSemanticOperation(NodeIf->GetExpressionL(), Table))
		return false;
	else if(str == "ASTArray" && !CheckSemanticArray(NodeIf->GetExpressionL(), Table))
		return false;
	else if (str == "ASTCallFun" && !CheckSemanticCallFun(NodeIf->GetExpressionL(), Table))
		return false;

	if (!CheckingBodyFunSemantics(NodeIf, Table, ReturnType)) return false;

	if (NodeIf->GetExpressionR() != nullptr && !CheckingBodyFunSemantics(NodeIf->GetExpressionR(), Table, ReturnType))
	{
		SemError("inside else block");
		return false;
	}

	return true;
}

bool SemanticAnalysis::CheckSemanticLocalFun(unique_ptr<AST> const &NodeLocalFun, map<string, vector<ScopeVar>> &Table, string ReturnType)
{
	if (!CheckingBodyFunSemantics(NodeLocalFun, Table, ReturnType))	return false;
	return true;
}

bool SemanticAnalysis::CheckCallFunArguments(unique_ptr<AST> const &NodeCallFun, map<string, vector<ScopeVar>> &Table, AST* const ptr, string str)
{
	if (ptr != nullptr)
	{
		if (ptr->GetArgs().size() != NodeCallFun->GetArgs().size())
		{
			SemError("CallFun: " + NodeCallFun->GetStrName() + " invalid number of arguments");
			return false;
		}

		for (int index = 0; index < ptr->GetArgs().size(); index++)
		{
			str = typeid(*NodeCallFun->GetArgs()[index]).name();
			RedactionString(str);//Строка приводится в годное состояние
			if (str == "ASTDigit")
			{
				if (NodeCallFun->GetArgs()[index]->GetStrType() != "NotInteger")
					str = "int";
				else
					str = "NotInteger";
			}
			else
			{
				str = NodeCallFun->GetArgs()[index]->GetStrType();
			}

			//cout << str << " " << ptr->GetArgs()[index]->GetStrType() << endl;
			if (ptr->GetArgs()[index]->GetStrType() != str)
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
	string str = typeid(*NodeCallFun).name();
	RedactionString(str);//Строка приводится в годное состояние
	str = CheckIdTableSymbol(NodeCallFun->GetStrName(), NodeCallFun->GetScope(), NodeCallFun->GetStrType(), Table, str);
	RedactionString(str);//Строка приводится в годное состояние

	if (str == "")
	{
		str = typeid(*NodeCallFun).name();
		RedactionString(str);//Строка приводится в годное состояние
		SemError(str + " - Wrong node type");
		return false;
	}

	str = typeid(*NodeCallFun).name();
	RedactionString(str);//Строка приводится в годное состояние
	auto ptr = SearchFunctionOrArrayDefinition(NodeCallFun->GetStrName(), NodeCallFun->GetScope(), NodeCallFun->GetStrType(), Table, str);

	return CheckCallFunArguments(NodeCallFun, Table, ptr, str);
}

string SemanticAnalysis::CheckIdName(vector<ScopeVar> &it, string name/*, map<string, vector<ScopeVar>> const &Table*/, string NodeType)
{
	//cout << "CheckIdName: " << NodeType << " " << name << endl;
	string str;
	if (!it.empty())
	{
		for (int index = 0; index < it.size(); index++)
		{
			if (it[index].getNode() != nullptr && it[index].getNode()->GetStrName() == name)
			{
				str = typeid(*it[index].getNode()).name();
				RedactionString(str);//Строка приводится в годное состояние
				if (NodeType == "ASTCallFun" && str == "ASTFun" || str == NodeType)
					return it[index].getNode()->GetStrType();
				//cout << str << endl;
			}	
		}
	}
	return "";
}

string SemanticAnalysis::CheckIdTableSymbol(string name, string scope, string type, map<string, vector<ScopeVar>> &Table, string NodeType)
{
	if (type == "string" && NodeType == "ASTArray")
		NodeType = "ASTIdentifier";

	//cout << "CheckIdTableSymbol: " << type << " " << name << " " << NodeType << " " << scope << endl;
	map<string, vector<ScopeVar>>::iterator it = Table.begin();
	
	for (; it != Table.end(); it++)
	{
		int limit = it->first.size();
		//cout << scope << " " << it->first << endl;
		if (scope.substr(0, limit) == it->first.substr(0, limit))
		{
			string temp;
			
			temp = CheckIdName(it->second, name, NodeType);
			if (temp != "") return temp;
		}
	}

	return "";
}

AST* const /*&*/SemanticAnalysis::SearchFunctionOrArrayDefinition(string name, string scope, string type, map<string, vector<ScopeVar>> &Table, string NodeType)
{
	//cout << "SearchFunctionDefinition " << name << " " << type << endl;
	if (type == "string" && NodeType == "ASTArray")
		NodeType = "ASTIdentifier";

	map<string, vector<ScopeVar>>::iterator it = Table.begin();
	AST *ptr = nullptr;
	
	for (; it != Table.end(); it++)
	{
		int limit = it->first.size();
		//cout << scope << " " << it->first << endl;
		if (scope.substr(0, limit) == it->first.substr(0, limit))
		{
			if (!it->second.empty())
			{
				for (int index = 0; index < it->second.size(); index++)
				{
					if (it->second[index].getNode() != nullptr && it->second[index].getNode()->GetStrName() == name)
					{
						string str = typeid(*it->second[index].getNode()).name();
						RedactionString(str);//Строка приводится в годное состояние
						if (str == "ASTFun" && NodeType == "ASTCallFun" || str == NodeType)
							ptr = it->second[index].getNode();
					}
				}
			}
		}
	}
	
	return ptr;
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
			return true;
		else
			return false;
	}
	else if (TypeOne == "string" || TypeOne == "char")
	{
		if (TypeTwo == "string" || TypeTwo == "char")
			return true;
		else
			return false;
	}

	return false;
}