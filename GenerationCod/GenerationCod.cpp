#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "GenerationCod.h"

using namespace std;

void GenerationCod::StartGenerationCod(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table, int option)
{
	int index = 0;

	//cout << endl << "Way to out file: " << Way << endl;
	
	//Создание файла, в который будет записываться программа на ассемблер
	//ofstream fileoutput(Way, ios::out);
	ofstream fileoutput("proga.s", ios::out);
	if (!fileoutput)
	{
		fileoutput.close();
		cerr << "Fileoutput open error" << endl;
		return;
	}

	fileoutput << "\t.text"<< endl << endl;
	fileoutput << "\t.global main" << endl;
	fileoutput << "\t.type main, @function" << endl; 
	fileoutput << "main:" << endl;

	//Пролог

	fileoutput << "\tpushl %ebp" << endl;
	fileoutput << "\tmovl %esp, %ebp" << endl;

	//Основной код
	while (index < VectorClass.size())
	{
		string NodeName = typeid(*VectorClass[index]).name();// Запись имени узла
		RedactionString(NodeName);//Строка приводится в годное состояние

		if (NodeName == "ASTClass")
		{
			if (!ClassPass(VectorClass[index], fileoutput))
			{
				index = VectorClass.size();
			}
		}
		index++;
	}

	//leave
	//ret

	fileoutput << "\tleave" << endl;
	fileoutput << "\tret" << endl;

	//.data записываю из таблицы символов
	WriteData(Table, fileoutput);

	fileoutput.close();


	string Line;
	if (option)
	{
		ifstream fileinput(/*"ProgrammForTest/Dop/search.cs"*/ "proga.s", ios::in);
		if (!fileinput) 
		{
			fileinput.close();
			cerr << "Fileinput open error" << endl;
			return;
		}
		cout << "Assembler:" << endl << endl;
		while (!fileinput.eof()) 
		{
			getline(fileinput, Line);
			cout << Line << endl;
		}
		fileinput.close();
	}
}

bool GenerationCod::ClassPass(unique_ptr<AST> const &NodeClass, ofstream &fileoutput)
{
	int index;
	if (!NodeClass->GetChild().empty())
	{
		while (index < NodeClass->GetChild().size())
		{
			string NodeName = typeid(*NodeClass->GetChild()[index]).name();
			RedactionString(NodeName);//Строка приводится в годное состояние

			//cout << "Inside " << typeid(*NodeClass).name() << " ClassPass: " << NodeName << endl;
 			if (NodeName == "ASTStatic")
			{
				//Генерация кода для функции
				if (!FunBodyPass(NodeClass->GetChild()[index], fileoutput))
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

void GenerationCod::WriteOperIf(ofstream &fileoutput, string NodeNameOne, string NodeNameTwo, string op, string nameblock)
{
	fileoutput << "\tcmp %ebx, %eax" << endl;
	if (op == "==")
	{
		fileoutput << "\tjne " << nameblock << endl;
	}
	else if (op == "!=")
	{
		fileoutput << "\tje " << nameblock << endl;
	}
	else if (op == "<")
	{
		fileoutput << "\tjnl " << nameblock << endl;
	}
	else if (op == ">")
	{
		fileoutput << "\tjng " << nameblock << endl;
	}
	else if (op == ">=")
	{
		fileoutput << "\tjl " << nameblock << endl;
	}
	else if (op == "<=")
	{
		fileoutput << "\tjg " << nameblock << endl;
	}
}

void GenerationCod::ExprIF(unique_ptr<AST> const &NodeIfExp, ofstream &fileoutput, string nameblock)
{
	string NodeNameOne = typeid(*NodeIfExp->GetExpressionL()).name();
	string NodeNameTwo;
	RedactionString(NodeNameOne);

	if (NodeIfExp->GetExpressionR() != nullptr)
	{
		NodeNameTwo = typeid(*NodeIfExp->GetExpressionR()).name();
		RedactionString(NodeNameTwo);
	}

	if (NodeNameTwo != "ASTOperationBin" && NodeNameTwo != "ASTOperationBin")
	{
		fileoutput << "\txorl %eax, %eax" << endl;
		fileoutput << "\txorl %ebx, %ebx" << endl;
		if (NodeNameOne != "ASTArray")
		{
			fileoutput << "\tmovl "; 
			if (NodeNameOne == "ASTDigit") fileoutput << "$";
			fileoutput << NodeIfExp->GetExpressionL()->GetStrName() << ", %eax" << endl;// Помещение в аккум первый операнда
		}
		else 
		{
			string str = typeid(*NodeIfExp->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);

			if (str == "ASTDigit")
			{	
				int i = atoi(NodeIfExp->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl " << NodeIfExp->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;
			}
			else
			{
				fileoutput << "\tmovl " << NodeIfExp->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeIfExp->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			}
			/*else if (str == "ASTIdentifier")
			{
				str = NodeIfExp->GetExpressionL()->GetStrName();
				fileoutput << "\tmovl str, %ecx" << endl;
				fileoutput << "\tmovl " << str <<"(,%ecx,4), %eax" << endl;
				//movl array_start(,%ecx,4), %eax
			}*/
			
		}
		
		if (NodeNameTwo != "ASTArray")
		{
			fileoutput << "\tmovl ";
			if (NodeNameTwo == "ASTDigit") fileoutput << "$";
			fileoutput << NodeIfExp->GetExpressionR()->GetStrName() << ", %ebx" << endl;// Помещение в базовый второй операнда
		}
		else 
		{
			string str = typeid(*NodeIfExp->GetExpressionR()->GetIndex()[0]).name();
			RedactionString(str);

			if (str == "ASTDigit")
			{
				int i = atoi(NodeIfExp->GetExpressionR()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl " << NodeIfExp->GetExpressionR()->GetStrName() << "+" << i * 4 << ", %ebx" << endl;
			}
			else
			{
				fileoutput << "\tmovl " << NodeIfExp->GetExpressionR()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeIfExp->GetExpressionR()->GetStrName() << "(, %ecx, 4), %ebx" << endl;
			}
		}

		if (NodeNameOne != "ASTArray")
			WriteOperIf(fileoutput, NodeIfExp->GetExpressionL()->GetStrName(), NodeIfExp->GetExpressionR()->GetStrName(), NodeIfExp->GetStrName(), nameblock);
		else 
		{
			string str = typeid(*NodeIfExp->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);

			if (str == "ASTDigit")
			{
				int i = atoi(NodeIfExp->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				str = NodeIfExp->GetExpressionL()->GetStrName() + "+" + to_string(i * 4);
			}
			else 
			{
				str = NodeIfExp->GetExpressionL()->GetStrName() + "(, %ecx, 4)";
			}
			//cout << "str = " << str << endl;
			WriteOperIf(fileoutput, str, NodeIfExp->GetExpressionR()->GetStrName(), NodeIfExp->GetStrName(), nameblock);
		}
	}
}

void GenerationCod::ifPass(unique_ptr<AST> const &NodeIf, ofstream &fileoutput, string scope)
{
	//Прочесть условие
	//cmpl op2, op1
	//jump если условие не выполняется, то перепрыгиываю код
	string NodeName = typeid(*NodeIf->GetExpressionL()).name();
	RedactionString(NodeName);

	fileoutput << "\txorl %eax, %eax" << endl;
	fileoutput << "\txorl %ebx, %ebx" << endl << endl;

	if (NodeName == "ASTDigit")
	{
		fileoutput << "\tmovl $" << NodeIf->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		fileoutput << "\tcmp %ebx, %eax" << endl;
		fileoutput << "\tjng blocif" << scope << endl << endl;
	}
	else if (NodeName == "ASTIdentifier")
	{
		fileoutput << "\tmovl " << NodeIf->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		fileoutput << "\tcmp %ebx, %eax" << endl;
		fileoutput << "\tjng blocif" << scope << endl << endl;
	}
	else if (NodeName == "ASTArray")
	{ 
		string str = typeid(*NodeIf->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);

		if (str == "ASTDigit")
		{
			int i = atoi(NodeIf->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			fileoutput << "\tmovl " << NodeIf->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;// op1
			fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			fileoutput << "\tcmp %ebx, %eax" << endl;
			fileoutput << "\tjng blocif" << scope << endl << endl;
		}
		else 
		{
			fileoutput << "\tmovl " << NodeIf->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			fileoutput << "movl " << NodeIf->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			fileoutput << "\tcmp %ebx, %eax" << endl;
			fileoutput << "\tjng blocif" << scope << endl << endl;	
		}
	}
	else if (NodeName == "ASTOperationBin")
	{
		string nameblock = "blocif" + scope;
		ExprIF(NodeIf->GetExpressionL(), fileoutput, nameblock);
	}

	//cout << "Bodyif: size = " << NodeIf->GetChild().size() << endl;
	FunBodyPass(NodeIf, fileoutput);
	
	fileoutput << "blocif" << scope << ":" << endl;
}

void GenerationCod::whilePass(unique_ptr<AST> const &NodeOper, ofstream &fileoutput, string scope)
{
	string NodeName = typeid(*NodeOper->GetExpressionL()).name();
	RedactionString(NodeName);

	fileoutput << "\txorl %eax, %eax" << endl;
	fileoutput << "\txorl %ebx, %ebx" << endl << endl;

	if (NodeName == "ASTDigit")
	{
		fileoutput << "\tmovl $" << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		fileoutput << "\tcmp %ebx, %eax" << endl;
		fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTIdentifier")
	{
		fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		fileoutput << "\tcmp %ebx, %eax" << endl;
		fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTArray")
	{
		string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);
		if (str == "ASTDigit")
		{
			int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;// op1
			fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			fileoutput << "\tcmp %ebx, %eax" << endl;
			fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
		else 
		{
			fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			fileoutput << "movl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			fileoutput << "\tcmp %ebx, %eax" << endl;
			fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
	}
	else if (NodeName == "ASTOperationBin")
	{
		string nameblock = "else_blockWhile_" + scope;
		ExprIF(NodeOper->GetExpressionL(), fileoutput, nameblock);
	}

	fileoutput << "if_blockWhile_" << scope << ":" << endl;

	//cout << "Bodyif: size = " << NodeOper->GetChild().size() << endl;
	FunBodyPass(NodeOper, fileoutput);

	if (NodeName == "ASTDigit")
	{
		fileoutput << "\tmovl $" << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		fileoutput << "\tcmp %ebx, %eax" << endl;
		fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTIdentifier")
	{
		fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		fileoutput << "\tcmp %ebx, %eax" << endl;
		fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTArray")
	{
		string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);
		if (str == "ASTDigit")
		{
			int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;// op1
			fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			fileoutput << "\tcmp %ebx, %eax" << endl;
			fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
		else 
		{
			fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			fileoutput << "movl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			fileoutput << "\tcmp %ebx, %eax" << endl;
			fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
	}
	else if (NodeName == "ASTOperationBin")
	{
		string nameblock = "else_blockWhile_" + scope;
		ExprIF(NodeOper->GetExpressionL(), fileoutput, nameblock);
	}

	fileoutput << "\tjmp if_blockWhile_" << scope << endl;
	fileoutput << "else_blockWhile_" << scope << ":" << endl;
}

bool GenerationCod::FunBodyPass(unique_ptr<AST> const &NodeBodyFun, ofstream &fileoutput)
{
	int index = 0;

	if (!NodeBodyFun->GetChild().empty())
	{
		while (index < NodeBodyFun->GetChild().size())
		{
			string NodeName = typeid(*NodeBodyFun->GetChild()[index]).name();
			RedactionString(NodeName);//Строка приводится в годное состояние
			//cout << "Inside " << NodeName << " FunBodyPass: " << NodeName << endl;
			if (NodeName == "ASTIdentifier" && NodeBodyFun->GetChild()[index]->GetExpressionL() != nullptr && (NodeBodyFun->GetChild()[index]->GetExpressionL()->GetStrName() == "Write" 
				|| NodeBodyFun->GetChild()[index]->GetExpressionL()->GetStrName() == "WriteLine"))
			{
				CallPrintf(NodeBodyFun->GetChild()[index]->GetExpressionL(), fileoutput);//вывод
			}
			else if (NodeName == "ASTOperationBin")//Проверка семантики операций
			{
				CalculationsPass(NodeBodyFun->GetChild()[index], fileoutput);//арифметика
			}
			else if (NodeName == "ASTWhile")//Проверка семантики блока while
			{
				whilePass(NodeBodyFun->GetChild()[index], fileoutput, NodeBodyFun->GetChild()[index]->GetExpressionL()->GetScope());
			}
			else if (NodeName == "ASTIf")//Проверка семантики блока if
			{
				ifPass(NodeBodyFun->GetChild()[index], fileoutput, NodeBodyFun->GetChild()[index]->GetExpressionL()->GetScope());
			}
			index++;
		}
	}
	return true;
}

void GenerationCod::WriteOper(ofstream &fileoutput, string one, string two, string NodeNameOne, string NodeNameTwo, string op)
{

	if (op == "=")
	{	
		fileoutput << "\tmovl %ebx," << one << endl;
	}
	else if (op == "+")
	{
		fileoutput << "\taddl %ebx, %eax" << endl;
	}
	else if (op == "-")
	{
		fileoutput << "\tsub %ebx, %eax" << endl;
	}
	else if (op == "*")
	{
		fileoutput << "\tmull %ebx" << endl;
	}
	else if (op == "/")
	{
		fileoutput << "\tidivl %ebx" << endl;
	}
	//результат остался в аккуме
}

void GenerationCod::RecursPass(unique_ptr<AST> const &NodeOper, ofstream &fileoutput)
{
	string NodeNameOne = typeid(*NodeOper->GetExpressionL()).name();
	string NodeNameTwo;
	RedactionString(NodeNameOne);//Строка приводится в годное состояние

	//cout << "one = " << NodeNameOne << " = " << NodeOper->GetExpressionL()->GetStrName() << endl;
	if (NodeOper->GetExpressionR() != nullptr)
	{
		NodeNameTwo = typeid(*NodeOper->GetExpressionR()).name();
		RedactionString(NodeNameTwo);//Строка приводится в годное состояние
		//cout << "two = " << NodeNameTwo << " = " << NodeOper->GetExpressionR()->GetStrName() << endl;
	}

	if (NodeNameTwo == "ASTOperationBin" && (NodeNameOne == "ASTIdentifier" || NodeNameOne == "ASTArray"))
	{
		//запуск по кругу
		//cout << "input" << endl;
		RecursPass(NodeOper->GetExpressionR(), fileoutput);
		if (NodeNameOne == "ASTIdentifier")
			fileoutput << "\tmovl %eax, " << NodeOper->GetExpressionL()->GetStrName() << endl << endl;
		else
		{
			string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);
			//cout << "CalculationsPass1: str = " << str << " " << NodeOper->GetExpressionL()->GetStrName() << endl;
			if (str == "ASTDigit")
			{
				int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl %eax, " << NodeOper->GetExpressionL()->GetStrName() << "+" << 4 * i << endl << endl;
			}
			else 
			{
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			}
		} 
	}
	
	if (NodeNameTwo != "ASTOperationBin" && NodeNameTwo != "ASTOperationBin")
	{
		fileoutput << "\txorl %eax, %eax" << endl;
		fileoutput << "\txorl %ebx, %ebx" << endl;
		if (NodeNameOne != "ASTArray")
		{
			fileoutput << "\tmovl "; 
			if (NodeNameOne == "ASTDigit") fileoutput << "$";
			fileoutput << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl;// Помещение в аккум первый операнда
		}
		else 
		{
			string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);
			//cout << "CalculationsPass1: str = " << str << " " << NodeOper->GetExpressionL()->GetStrName() << endl;
			if (str == "ASTDigit")
			{
				int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;
			}
			else 
			{
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			}
		}
		
		if (NodeNameTwo != "ASTArray")
		{
			fileoutput << "\tmovl ";
			if (NodeNameTwo == "ASTDigit") fileoutput << "$";
			fileoutput << NodeOper->GetExpressionR()->GetStrName() << ", %ebx" << endl;// Помещение в базовый второй операнда
		}
		else 
		{
			string str = typeid(*NodeOper->GetExpressionR()->GetIndex()[0]).name();
			RedactionString(str);
			//cout << "CalculationsPass1: str = " << str << " " << NodeOper->GetExpressionL()->GetStrName() << endl;
			if (str == "ASTDigit")
			{
				int i = atoi(NodeOper->GetExpressionR()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl " << NodeOper->GetExpressionR()->GetStrName() << "+" << i * 4 << ", %ebx" << endl;
			}
			else 
			{
				fileoutput << "\tmovl " << NodeOper->GetExpressionR()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionR()->GetStrName() << "(, %ecx, 4), %ebx" << endl;
			}
		}

		if (NodeNameOne != "ASTArray")
			WriteOper(fileoutput, NodeOper->GetExpressionL()->GetStrName(), NodeOper->GetExpressionR()->GetStrName(), NodeNameOne, NodeNameTwo, NodeOper->GetStrName());
		else 
		{
			string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);
			//cout << "CalculationsPass1: str = " << str << " " << NodeOper->GetExpressionL()->GetStrName() << endl;
			if (str == "ASTDigit")
			{
				int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				str = NodeOper->GetExpressionL()->GetStrName() + "+" + to_string(i * 4);
				//cout << "str = " << str << endl;
			}
			else 
			{
				//fileoutput << "\tmovl " << NodeOper->GetExpressionL()GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				str = NodeOper->GetExpressionL()->GetStrName() + "(, %ecx, 4)"; 
			}
			WriteOper(fileoutput, str, NodeOper->GetExpressionR()->GetStrName(), NodeNameOne, NodeNameTwo, NodeOper->GetStrName());
		}
	}
}

void GenerationCod::CalculationsPass(unique_ptr<AST> const &NodeOper, ofstream &fileoutput)
{
	//куда сохранять данные
	//i = console.readline(); - ввод
	//pushl i
	//pushl $format_l
	//call scanf
	string NodeNameOne = typeid(*NodeOper->GetExpressionL()).name();
	string NodeNameTwo;
	RedactionString(NodeNameOne);//Строка приводится в годное состояние

	if (NodeOper->GetExpressionR() != nullptr)
	{
		NodeNameTwo = typeid(*NodeOper->GetExpressionR()).name();
		RedactionString(NodeNameTwo);//Строка приводится в годное состояние
	}

	if ((NodeNameOne == "ASTIdentifier" || NodeNameOne == "ASTArray") && NodeNameTwo == "ASTIdentifier" && NodeOper->GetExpressionR()->GetExpressionL() != nullptr 
		&& NodeOper->GetStrName() == "=")
	{
		if (NodeNameOne == "ASTIdentifier")
		{
			fileoutput << "\tpushl $" << NodeOper->GetExpressionL()->GetStrName() << endl;
		}
		else
		{
			string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);
			//cout << "CalculationsPass1: str = " << str << " " << NodeOper->GetExpressionL()->GetStrName() << endl;
			if (str == "ASTDigit")
			{
				int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl $" << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;
				fileoutput << "\tpushl %eax" << endl;
			} 
			else
			{
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
				fileoutput << "\tpushl %eax" << endl;	
			}
		}

		fileoutput << "\tpushl " << "$format_l" << endl;
		fileoutput << "\tcall scanf" << endl;
	}// ввод
	else if (NodeOper->GetStrName() == "++")
	{
		if (NodeNameOne == "ASTIdentifier")
		{
			fileoutput << "\tincl " << NodeOper->GetExpressionL()->GetStrName() << endl;
		}
		else 
		{
			string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);
			//cout << "CalculationsPass1: str = " << str << " " << NodeOper->GetExpressionL()->GetStrName() << endl;
			if (str == "ASTDigit")
			{
				int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;
				fileoutput << "\tdecl %eax" << endl;
				fileoutput << "\tmovl %eax, " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << endl;
			}
			else
			{
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
				fileoutput << "\tdecl %eax" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl %eax, " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4)" << endl;
			}
		}
	}
	else if (NodeOper->GetStrName() == "--")
	{
		//cout << "TUT" << endl;
		//cout << NodeNameOne << endl;
		if (NodeNameOne == "ASTIdentifier")
		{
			fileoutput << "\tdecl " << NodeOper->GetExpressionL()->GetStrName() << endl;
		}
		else 
		{
			string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);
			//cout << "CalculationsPass1: str = " << str << " " << NodeOper->GetExpressionL()->GetStrName() << endl;

			if (str == "ASTDigit")
			{
				int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;
				fileoutput << "\tdecl %eax" << endl;
				fileoutput << "\tmovl %eax, " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << endl;
			}
			else
			{
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
				fileoutput << "\tdecl %eax" << endl;
				fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl %eax, " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4)" << endl;
			}
		}
	}
	else 
	{
		RecursPass(NodeOper, fileoutput);
	}

	//i = k + 1 * 4;
	//xorl %eax, %eax
	//movl $1, %eax
	//mull $4
	//addl k, %eax
	//movl %eax, i

}

void GenerationCod::CallPrintf(unique_ptr<AST> const &CallPrn, ofstream &fileoutput)
{
	fileoutput << endl;

	if (CallPrn->GetArgs()[0] != nullptr)
	{
		string name = typeid(*CallPrn->GetArgs()[0]).name();
		RedactionString(name);//Строка приводится в годное состояние

		//cout << "Callprintf: " << name << endl;
		if (name == "ASTIdentifier")
		{
			fileoutput << "\tpushl " << CallPrn->GetArgs()[0]->GetStrName() << endl;
		}
		else if (name == "ASTArray")
		{
			//cout << CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName() << endl;
			string temp = typeid(CallPrn->GetArgs()[0]->GetIndex()[0]).name();
			RedactionString(temp);

			if (temp == "ASTDigit")
			{
				int i = atoi(CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName().c_str());
				fileoutput << "\tmovl " << CallPrn->GetArgs()[0]->GetStrName() << "+" << i * 4 << ", %eax" << endl;
			}
			else 
			{
				fileoutput << "\tmovl " << CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				fileoutput << "\tmovl " << CallPrn->GetArgs()[0]->GetStrName() << "(, %ecx, 4), %eax" << endl;
			}
			fileoutput << "\tpushl %eax" << endl;
		}
	}
	fileoutput << "\tpushl " << "$format_l_n" << endl;
	fileoutput << "\tcall printf" << endl << endl;

	//while (i < CallPrn->GetArgs().size())
	//{
	//	fileoutput << "\tpushl " + CallPrn->GetArgs()[index] << endl;
	//	fileoutput << "\tpushl " + "format_l" << endl;
	//}
	
}

//Запись блока .data
void GenerationCod::WriteData(map<string, vector<ScopeVar>> &Table, ofstream &fileoutput)
{
	fileoutput << endl;
	fileoutput << ".data" << endl;
	fileoutput << "\tformat_l:" << endl << "\t\t.string \"%d\"" << endl << endl;
	fileoutput << "\tformat_l_n:" << endl << "\t\t.string \"%d\\n\"" << endl << endl;
	string name;

	map<string, vector<ScopeVar>>::iterator it = Table.begin(); 
	it++;
	for (; it != Table.end(); it++)
	{
		if (!it->second.empty())
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i].getNode() != nullptr)
				{
					name = typeid(*it->second[i].getNode()).name();
					RedactionString(name);//Строка приводится в годное состояние	
					//cout << name << endl;
					fileoutput << "\t" << it->second[i].getNode()->GetStrName() << ":" << endl;
					if (name == "ASTIdentifier")
					{
						fileoutput << "\t\t.long " << "0" << endl;
					}
					else if (name == "ASTArray")
					{ 
						//cout << "mas size :" << it->second[i].getNode()->GetArraySizes()[0]->GetStrName() << endl;
						int size = atoi(it->second[i].getNode()->GetArraySizes()[0]->GetStrName().c_str());
						fileoutput << "\t\t.long ";
						for (int k = 0; k < size; k++)
						{
							if (k == size - 1)
								fileoutput << "0" << endl;
							else
								fileoutput << "0, ";
						}
					}
				}
			}
	}
}