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
	//ofstream Fileoutput(Way, ios::out);
	ofstream Fileoutput("proga.s", ios::out);
	if (!Fileoutput)
	{
		Fileoutput.close();
		cerr << "Fileoutput open error" << endl;
		return;
	}

	Fileoutput << "\t.text"<< endl << endl;
	Fileoutput << "\t.global main" << endl;
	Fileoutput << "\t.type main, @function" << endl; 
	Fileoutput << "main:" << endl;

	//Пролог

	Fileoutput << "\tpushl %ebp" << endl;
	Fileoutput << "\tmovl %esp, %ebp" << endl;

	//Основной код
	while (index < VectorClass.size())
	{
		string NodeName = typeid(*VectorClass[index]).name();// Запись имени узла
		RedactionString(NodeName);//Строка приводится в годное состояние

		if (NodeName == "ASTClass")
		{
			if (!ClassPass(VectorClass[index], Fileoutput))
			{
				index = VectorClass.size();
			}
		}
		index++;
	}

	//leave
	//ret

	Fileoutput << "\tleave" << endl;
	Fileoutput << "\tret" << endl;

	//.data записываю из таблицы символов
	WriteData(Table, Fileoutput);

	Fileoutput.close();


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

bool GenerationCod::ClassPass(unique_ptr<AST> const &NodeClass, ofstream &Fileoutput)
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
				if (!FunBodyPass(NodeClass->GetChild()[index], Fileoutput))
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

void GenerationCod::WriteOperIf(ofstream &Fileoutput, string NodeNameOne, string NodeNameTwo, string op, string nameblock)
{
	Fileoutput << "\tcmp %ebx, %eax" << endl;
	if (op == "==")
	{
		Fileoutput << "\tjne " << nameblock << endl;
	}
	else if (op == "!=")
	{
		Fileoutput << "\tje " << nameblock << endl;
	}
	else if (op == "<")
	{
		Fileoutput << "\tjnl " << nameblock << endl;
	}
	else if (op == ">")
	{
		Fileoutput << "\tjng " << nameblock << endl;
	}
	else if (op == ">=")
	{
		Fileoutput << "\tjl " << nameblock << endl;
	}
	else if (op == "<=")
	{
		Fileoutput << "\tjg " << nameblock << endl;
	}
}

void GenerationCod::ExprIF(unique_ptr<AST> const &NodeIfExp, ofstream &Fileoutput, string nameblock)
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
		Fileoutput << "\txorl %eax, %eax" << endl;
		Fileoutput << "\txorl %ebx, %ebx" << endl;
		if (NodeNameOne != "ASTArray")
		{
			Fileoutput << "\tmovl "; 
			if (NodeNameOne == "ASTDigit") Fileoutput << "$";
			Fileoutput << NodeIfExp->GetExpressionL()->GetStrName() << ", %eax" << endl;// Помещение в аккум первый операнда
		}
		else 
		{
			string str = typeid(*NodeIfExp->GetExpressionL()->GetIndex()[0]).name();
			RedactionString(str);

			if (str == "ASTDigit")
			{	
				int i = atoi(NodeIfExp->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
				Fileoutput << "\tmovl " << NodeIfExp->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;
			}
			else
			{
				Fileoutput << "\tmovl " << NodeIfExp->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				Fileoutput << "\tmovl " << NodeIfExp->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			}
			/*else if (str == "ASTIdentifier")
			{
				str = NodeIfExp->GetExpressionL()->GetStrName();
				Fileoutput << "\tmovl str, %ecx" << endl;
				Fileoutput << "\tmovl " << str <<"(,%ecx,4), %eax" << endl;
				//movl array_start(,%ecx,4), %eax
			}*/
			
		}
		
		if (NodeNameTwo != "ASTArray")
		{
			Fileoutput << "\tmovl ";
			if (NodeNameTwo == "ASTDigit") Fileoutput << "$";
			Fileoutput << NodeIfExp->GetExpressionR()->GetStrName() << ", %ebx" << endl;// Помещение в базовый второй операнда
		}
		else 
		{
			string str = typeid(*NodeIfExp->GetExpressionR()->GetIndex()[0]).name();
			RedactionString(str);

			if (str == "ASTDigit")
			{
				int i = atoi(NodeIfExp->GetExpressionR()->GetIndex()[0]->GetStrName().c_str());
				Fileoutput << "\tmovl " << NodeIfExp->GetExpressionR()->GetStrName() << "+" << i * 4 << ", %ebx" << endl;
			}
			else
			{
				Fileoutput << "\tmovl " << NodeIfExp->GetExpressionR()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				Fileoutput << "\tmovl " << NodeIfExp->GetExpressionR()->GetStrName() << "(, %ecx, 4), %ebx" << endl;
			}
		}

		if (NodeNameOne != "ASTArray")
			WriteOperIf(Fileoutput, NodeIfExp->GetExpressionL()->GetStrName(), NodeIfExp->GetExpressionR()->GetStrName(), NodeIfExp->GetStrName(), nameblock);
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
			WriteOperIf(Fileoutput, str, NodeIfExp->GetExpressionR()->GetStrName(), NodeIfExp->GetStrName(), nameblock);
		}
	}
}

void GenerationCod::ifPass(unique_ptr<AST> const &NodeIf, ofstream &Fileoutput, string scope)
{
	//Прочесть условие
	//cmpl op2, op1
	//jump если условие не выполняется, то перепрыгиываю код
	string NodeName = typeid(*NodeIf->GetExpressionL()).name();
	RedactionString(NodeName);

	Fileoutput << "\txorl %eax, %eax" << endl;
	Fileoutput << "\txorl %ebx, %ebx" << endl << endl;

	if (NodeName == "ASTDigit")
	{
		Fileoutput << "\tmovl $" << NodeIf->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng blocif" << scope << endl << endl;
	}
	else if (NodeName == "ASTIdentifier")
	{
		Fileoutput << "\tmovl " << NodeIf->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng blocif" << scope << endl << endl;
	}
	else if (NodeName == "ASTArray")
	{ 
		string str = typeid(*NodeIf->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);

		if (str == "ASTDigit")
		{
			int i = atoi(NodeIf->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl " << NodeIf->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;// op1
			Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			Fileoutput << "\tcmp %ebx, %eax" << endl;
			Fileoutput << "\tjng blocif" << scope << endl << endl;
		}
		else 
		{
			Fileoutput << "\tmovl " << NodeIf->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "movl " << NodeIf->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			Fileoutput << "\tcmp %ebx, %eax" << endl;
			Fileoutput << "\tjng blocif" << scope << endl << endl;	
		}
	}
	else if (NodeName == "ASTOperationBin")
	{
		string nameblock = "blocif" + scope;
		ExprIF(NodeIf->GetExpressionL(), Fileoutput, nameblock);
	}

	//cout << "Bodyif: size = " << NodeIf->GetChild().size() << endl;
	FunBodyPass(NodeIf, Fileoutput);
	
	Fileoutput << "blocif" << scope << ":" << endl;
}

void GenerationCod::whilePass(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput, string scope)
{
	string NodeName = typeid(*NodeOper->GetExpressionL()).name();
	RedactionString(NodeName);

	Fileoutput << "\txorl %eax, %eax" << endl;
	Fileoutput << "\txorl %ebx, %ebx" << endl << endl;

	if (NodeName == "ASTDigit")
	{
		Fileoutput << "\tmovl $" << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTIdentifier")
	{
		Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTArray")
	{
		string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);
		if (str == "ASTDigit")
		{
			int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;// op1
			Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			Fileoutput << "\tcmp %ebx, %eax" << endl;
			Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
		else 
		{
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			Fileoutput << "\tcmp %ebx, %eax" << endl;
			Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
	}
	else if (NodeName == "ASTOperationBin")
	{
		string nameblock = "else_blockWhile_" + scope;
		ExprIF(NodeOper->GetExpressionL(), Fileoutput, nameblock);
	}

	Fileoutput << "if_blockWhile_" << scope << ":" << endl;

	//cout << "Bodyif: size = " << NodeOper->GetChild().size() << endl;
	FunBodyPass(NodeOper, Fileoutput);

	if (NodeName == "ASTDigit")
	{
		Fileoutput << "\tmovl $" << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTIdentifier")
	{
		Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << ", %eax" << endl; // op1
		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
	}
	else if (NodeName == "ASTArray")
	{
		string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);
		if (str == "ASTDigit")
		{
			int i = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "+" << i * 4 << ", %eax" << endl;// op1
			Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			Fileoutput << "\tcmp %ebx, %eax" << endl;
			Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
		else 
		{
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetStrName() << "(, %ecx, 4), %eax" << endl;
			Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
			Fileoutput << "\tcmp %ebx, %eax" << endl;
			Fileoutput << "\tjng else_blockWhile_" << scope << endl << endl;
		}
	}
	else if (NodeName == "ASTOperationBin")
	{
		string nameblock = "else_blockWhile_" + scope;
		ExprIF(NodeOper->GetExpressionL(), Fileoutput, nameblock);
	}

	Fileoutput << "\tjmp if_blockWhile_" << scope << endl;
	Fileoutput << "else_blockWhile_" << scope << ":" << endl;
}

bool GenerationCod::FunBodyPass(unique_ptr<AST> const &NodeBodyFun, ofstream &Fileoutput)
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
				CallPrintf(NodeBodyFun->GetChild()[index]->GetExpressionL(), Fileoutput);//вывод
			}
			else if (NodeName == "ASTOperationBin")//Проверка семантики операций
			{
				StartArithmeticGeneration(NodeBodyFun->GetChild()[index], Fileoutput);//арифметика
			}
			else if (NodeName == "ASTWhile")//Проверка семантики блока while
			{
				whilePass(NodeBodyFun->GetChild()[index], Fileoutput, NodeBodyFun->GetChild()[index]->GetExpressionL()->GetScope());
			}
			else if (NodeName == "ASTIf")//Проверка семантики блока if
			{
				ifPass(NodeBodyFun->GetChild()[index], Fileoutput, NodeBodyFun->GetChild()[index]->GetExpressionL()->GetScope());
			}
			index++;
		}
	}
	return true;
}

void GenerationCod::CallPrintf(unique_ptr<AST> const &CallPrn, ofstream &Fileoutput)
{
	Fileoutput << endl;

	if (CallPrn->GetArgs()[0] != nullptr)
	{
		string name = typeid(*CallPrn->GetArgs()[0]).name();
		RedactionString(name);//Строка приводится в годное состояние

		//cout << "Callprintf: " << name << endl;
		if (name == "ASTIdentifier")
		{
			Fileoutput << "\tpushl " << CallPrn->GetArgs()[0]->GetStrName() << endl;
		}
		else if (name == "ASTArray")
		{
			//cout << CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName() << endl;
			string temp = typeid(CallPrn->GetArgs()[0]->GetIndex()[0]).name();
			RedactionString(temp);

			if (temp == "ASTDigit")
			{
				int i = atoi(CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName().c_str());
				Fileoutput << "\tmovl " << CallPrn->GetArgs()[0]->GetStrName() << "+" << i * 4 << ", %eax" << endl;
			}
			else 
			{
				Fileoutput << "\tmovl " << CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				Fileoutput << "\tmovl " << CallPrn->GetArgs()[0]->GetStrName() << "(, %ecx, 4), %eax" << endl;
			}
			Fileoutput << "\tpushl %eax" << endl;
		}
	}
	Fileoutput << "\tpushl " << "$format_l_n" << endl;
	Fileoutput << "\tcall printf" << endl << endl;

	//while (i < CallPrn->GetArgs().size())
	//{
	//	Fileoutput << "\tpushl " + CallPrn->GetArgs()[index] << endl;
	//	Fileoutput << "\tpushl " + "format_l" << endl;
	//}
	
}

//Запись блока .data
void GenerationCod::WriteData(map<string, vector<ScopeVar>> &Table, ofstream &Fileoutput)
{
	Fileoutput << endl;
	Fileoutput << ".data" << endl;
	Fileoutput << "\tformat_l:" << endl << "\t\t.string \"%d\"" << endl << endl;
	Fileoutput << "\tformat_l_n:" << endl << "\t\t.string \"%d\\n\"" << endl << endl;
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
					Fileoutput << "\t" << it->second[i].getNode()->GetStrName() << ":" << endl;
					if (name == "ASTIdentifier")
					{
						Fileoutput << "\t\t.long " << "0" << endl;
					}
					else if (name == "ASTArray")
					{ 
						//cout << "mas size :" << it->second[i].getNode()->GetArraySizes()[0]->GetStrName() << endl;
						int size = atoi(it->second[i].getNode()->GetArraySizes()[0]->GetStrName().c_str());
						Fileoutput << "\t\t.long ";
						for (int k = 0; k < size; k++)
						{
							if (k == size - 1)
								Fileoutput << "0" << endl;
							else
								Fileoutput << "0, ";
						}
					}
				}
			}
	}
}

void GenerationCod::WriteOper(ofstream &Fileoutput, string OperandIdL, string Operator)
{

	if (Operator == "=")
	{	
		Fileoutput << "\tmovl %ebx," << OperandIdL << endl;
	}
	else if (Operator == "+")
	{
		Fileoutput << "\taddl %ebx, %eax" << endl;
	}
	else if (Operator == "-")
	{
		Fileoutput << "\tsub %ebx, %eax" << endl;
	}
	else if (Operator == "*")
	{
		Fileoutput << "\tmull %ebx" << endl;
	}
	else if (Operator == "/")
	{
		Fileoutput << "\tidivl %ebx" << endl;
	}
	//результат остался в аккуме
}

void GenerationCod::StartArithmeticGeneration(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput)
{
	string OperationName = NodeOper->GetStrName();
	string NodeNameOne = typeid(*NodeOper->GetExpressionL()).name();
	string NodeNameTwo;
	RedactionString(NodeNameOne);//Строка приводится в годное состояние

	if (NodeOper->GetExpressionR() != nullptr)
	{
		NodeNameTwo = typeid(*NodeOper->GetExpressionR()).name();
		RedactionString(NodeNameTwo);//Строка приводится в годное состояние
	}

	if ((NodeNameOne == "ASTIdentifier" || NodeNameOne == "ASTArray") && NodeNameTwo == "ASTIdentifier" && NodeOper->GetExpressionR()->GetExpressionL() != nullptr)
	{
		Scanf(NodeNameOne, NodeOper, Fileoutput);// Генерация ввода
	}
	else if (NodeOper->GetExpressionR() == nullptr)
	{
		UnaryOperations(NodeOper, Fileoutput); //Генерация унарных операций
	}
	else 
	{
		ArithmeticGeneration(NodeOper, Fileoutput);// Генерация бинарных операций
	}
}

void GenerationCod::UnaryOperations(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput)
{
	string OperationName = NodeOper->GetStrName();
	string NodeNameOne = typeid(*NodeOper->GetExpressionL()).name();
	string OperandIdL = NodeOper->GetExpressionL()->GetStrName();
	RedactionString(NodeNameOne);
	cout << "UnaryOperations" << endl;

	if (NodeNameOne == "ASTIdentifier")
	{
		if (OperationName == "++")
			Fileoutput << "\tincl " << OperandIdL << endl;
		else if (OperationName == "--")
			Fileoutput << "\tdecl " << OperandIdL << endl;
	}
	else 
	{
		string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);
		//cout << "CalculationsPass1: str = " << str << " " << OperandIdL << endl;
		if (str == "ASTDigit")
		{
			int ArrayIndexNumber = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl " << OperandIdL << "+" << ArrayIndexNumber * 4 << ", %eax" << endl;

			if (OperationName == "++")
				Fileoutput << "\tdecl %eax" << endl;
			else if (OperationName == "--")
				Fileoutput << "\tincl %eax" << endl;

			Fileoutput << "\tmovl %eax, " << OperandIdL << "+" << ArrayIndexNumber * 4 << endl;
		}
		else
		{
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "\tmovl " << OperandIdL << "(, %ecx, 4), %eax" << endl;

			if (OperationName == "++")
				Fileoutput << "\tdecl %eax" << endl;
			else if (OperationName == "--")
				Fileoutput << "\tincl %eax" << endl;

			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "\tmovl %eax, " << OperandIdL << "(, %ecx, 4)" << endl;
		}
	}
}

void GenerationCod::Scanf(string NodeNameOne, unique_ptr<AST> const &NodeOper, ofstream &Fileoutput)
{
	string OperandIdL = NodeOper->GetExpressionL()->GetStrName();

	if (NodeNameOne == "ASTIdentifier")
	{
		Fileoutput << "\tpushl $" << OperandIdL << endl;
	}
	else
	{
		string NodeArrayIndex = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(NodeArrayIndex);
		//cout << "CalculationsPass1: str = " << str << " " << OperandIdL << endl;
		if (NodeArrayIndex == "ASTDigit")
		{
			int ArrayIndexNumber = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl $" << OperandIdL << "+" << ArrayIndexNumber * 4 << ", %eax" << endl;
			Fileoutput << "\tpushl %eax" << endl;
		} 
		else
		{
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "\tmovl " << OperandIdL << "(, %ecx, 4), %eax" << endl;
			Fileoutput << "\tpushl %eax" << endl;	
		}
	}

	Fileoutput << "\tpushl " << "$format_l" << endl;
	Fileoutput << "\tcall scanf" << endl;
}

void GenerationCod::ArithmeticGeneration(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput)
{
	string OperationName = NodeOper->GetStrName();
	string OperandIdL = NodeOper->GetExpressionL()->GetStrName();// Имя левого Id
	string OperandIdR = NodeOper->GetExpressionR()->GetStrName();// Имя правого Id
	string NodeNameOne = typeid(*NodeOper->GetExpressionL()).name();// Имя левого узла AST
	string NodeNameTwo = "";
	string OperandOne;
	string OperandTwo;
	RedactionString(NodeNameOne);

	if (NodeOper->GetExpressionR() != nullptr)
	{
		NodeNameTwo = typeid(*NodeOper->GetExpressionR()).name();
		RedactionString(NodeNameTwo);
	}
	else 
	{
		return;
	}

	Fileoutput << "\txorl %eax, %eax" << endl;
	Fileoutput << "\txorl %ebx, %ebx" << endl;

	// Если достигли узла, где слева и справа узлы не являются узлами операций, генерируем ассемблерный код
	if (NodeNameTwo != "ASTOperationBin" && NodeNameOne != "ASTOperationBin")
	{

		cout << "OperationIdToId: " << OperandIdL << OperationName << OperandIdR << endl;

		OperationIdToId(NodeOper, Fileoutput);
		//OperationGeneration(NodeNameOne, NodeNameTwo, Operator);
		Fileoutput << "\tpush %eax" << endl; // Результат сохраняется в стек

		return;
	}

	// Спускаемся сначала в самый правый узел дерева операций
	if (NodeNameTwo == "ASTOperationBin")
	{
		OperandOne = "%ebx";
		OperandTwo = "%eax";

		OperandProcessing(OperandOne, OperandTwo, NodeOper->GetExpressionR(), NodeOper->GetExpressionL(), Fileoutput);

		//Выполнение операций
		Fileoutput << "\tpushl %eax" << endl;
	}// Спуск влево
	else if (NodeNameOne == "ASTOperationBin")
	{
		OperandOne = "%eax";
		OperandTwo = "%ebx";

		OperandProcessing(OperandOne, OperandTwo, NodeOper->GetExpressionL(), NodeOper->GetExpressionR(), Fileoutput);

		//Выполнение операций
		Fileoutput << "\tpushl %eax" << endl; // Результат сохраняется в стек
	}
	
	// Выполнение операций между ac и bas
	
	if (NodeNameOne != "ASTArray")
		WriteOper(Fileoutput, OperandIdL, OperationName);
	else 
	{
		string NodeArrayIndex = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(NodeArrayIndex);
		
		if (NodeArrayIndex == "ASTDigit")
		{
			int ArrayIndexNumber = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			OperandIdL = OperandIdL + "+" + to_string(ArrayIndexNumber * 4);
		}
		else 
		{
			OperandIdL = OperandIdL + "(, %ecx, 4)"; 
		}
		WriteOper(Fileoutput, OperandIdL,  OperationName);
	}

	Fileoutput << "\tpushl %eax" << endl;
}

void GenerationCod::OperandProcessing(string OperandOne, string OperandTwo, unique_ptr<AST> const &PtrNodeOperandOne, unique_ptr<AST> const &PtrNodeOperandTwo, ofstream &Fileoutput)
{
	ArithmeticGeneration(PtrNodeOperandOne, Fileoutput);
		
	string NodeNameTwo = typeid(*PtrNodeOperandTwo).name();	
	string OperandId = PtrNodeOperandTwo->GetStrName();
	RedactionString(NodeNameTwo);

	if (NodeNameTwo != "ASTOperationBin")
	{
		Fileoutput << "\tpopl " << OperandOne << endl;//pop bas(R)

		//ac(L)
		PreparingTheOperand(OperandTwo, PtrNodeOperandTwo, Fileoutput);
	}
	else 
	{
		ArithmeticGeneration(PtrNodeOperandTwo, Fileoutput);
		Fileoutput << "\tpopl " << OperandTwo << endl;//pop ac(L)
		Fileoutput << "\tpopl " << OperandOne << endl;//pop bas(R)
	}
}

void GenerationCod::PreparingTheOperand(string Register, unique_ptr<AST> const &NodeOperand, ofstream &Fileoutput)
{
	string OperandId = NodeOperand->GetStrName();
	string NodeName = typeid(*NodeOperand).name();
	RedactionString(NodeName);

	if (NodeName != "ASTArray")
	{
		Fileoutput << "\tmovl ";
		if (NodeName == "ASTDigit") Fileoutput << "$";
		Fileoutput << OperandId << ", " << Register << endl;

	}
	else if (NodeName == "ASTIdentifier")
	{
		string NodeArrayIndex = typeid(*NodeOperand->GetIndex()[0]).name();
		RedactionString(NodeArrayIndex);

		if (NodeArrayIndex == "ASTDigit")
		{
			int ArrayIndexNumber = atoi(NodeOperand->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl " << OperandId << "+" << ArrayIndexNumber * 4 << ", " << Register << endl;
		}
		else 
		{
			string ArrayIndexId = NodeOperand->GetIndex()[0]->GetStrName();
			Fileoutput << "\tmovl " << ArrayIndexId << ", %ecx" << endl;
			Fileoutput << "\tmovl " << OperandId << "(, %ecx, 4), " << Register << endl;
		}
	}
}

void GenerationCod::OperationIdToId(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput)
{
	string RegisterAcum = "%eax";
	string RegisterBas = "%ebx";
	string OperationName = NodeOper->GetStrName();
	string OperandIdL = NodeOper->GetExpressionL()->GetStrName();// Имя левого Id
	string NodeNameOne = typeid(*NodeOper->GetExpressionL()).name(); // Имя левого узла AST
	RedactionString(NodeNameOne);

	PreparingTheOperand(RegisterAcum, NodeOper->GetExpressionL(), Fileoutput);//Запись левого операнда в %eax
	PreparingTheOperand(RegisterBas, NodeOper->GetExpressionR(), Fileoutput);//Запись правого операнда в %ebx

	if (NodeNameOne != "ASTArray")
		WriteOper(Fileoutput, OperandIdL, OperationName);
	else 
	{
		string NodeArrayIndex = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(NodeArrayIndex);
		
		if (NodeArrayIndex == "ASTDigit")
		{
			int ArrayIndexNumber = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			OperandIdL = OperandIdL + "+" + to_string(ArrayIndexNumber * 4);
		}
		else 
		{
			OperandIdL = OperandIdL + "(, %ecx, 4)"; 
		}
		WriteOper(Fileoutput, OperandIdL, OperationName);
	}
}