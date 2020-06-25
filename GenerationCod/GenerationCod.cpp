#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "GenerationCod.h"

using namespace std;

void GenerationCod::StartGenerationCod(vector<unique_ptr<AST>> const &VectorClass, map<string, vector<ScopeVar>> &Table, int option, string File)
{
	int IndexChildAST = 0;

	//cout << endl << "Way to out file: " << Way << endl;
	
	//Создание файла, в который будет записываться программа на ассемблер
	//ofstream Fileoutput(Way, ios::out);
	ofstream Fileoutput(File, ios::out);
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
	while (IndexChildAST < VectorClass.size())
	{
		string NodeName = typeid(*VectorClass[IndexChildAST]).name();// Запись имени узла
		RedactionString(NodeName);//Строка приводится в годное состояние

		if (NodeName == "ASTClass")
		{
			ClassTraversal(VectorClass[IndexChildAST], Fileoutput);
		}
		IndexChildAST++;
	}

	//leave
	//ret

	Fileoutput << "\tleave" << endl;
	Fileoutput << "\tret" << endl;

	//.data записываю из таблицы символов
	WriteData(Table, Fileoutput);
	WriteDataStringsClass(VectorClass, Fileoutput);

	Fileoutput.close();


	string Line;
	if (option)
	{
		ifstream Fileinput(/*"ProgrammForTest/Dop/search.cs"*/ File, ios::in);
		if (!Fileinput) 
		{
			Fileinput.close();
			cerr << "Fileinput open error" << endl;
			return;
		}
		cout << "Assembler:" << endl << endl;
		while (!Fileinput.eof()) 
		{
			getline(Fileinput, Line);
			cout << Line << endl;
		}
		Fileinput.close();
	}
}

void GenerationCod::ClassTraversal(unique_ptr<AST> const &NodeClass, ofstream &Fileoutput)
{
	int IndexChildAST;
	if (!NodeClass->GetChild().empty())
	{
		while (IndexChildAST < NodeClass->GetChild().size())
		{
			string NodeName = typeid(*NodeClass->GetChild()[IndexChildAST]).name();
			RedactionString(NodeName);//Строка приводится в годное состояние

			//cout << "Inside " << typeid(*NodeClass).name() << " ClassTraversal: " << NodeName << endl;
 			if (NodeName == "ASTStatic")
			{
				//Генерация кода для функции
				FunBodyTraversal(NodeClass->GetChild()[IndexChildAST], Fileoutput);
			}
			IndexChildAST++;
		}
	}
}

void GenerationCod::FunBodyTraversal(unique_ptr<AST> const &NodeBodyFun, ofstream &Fileoutput)
{
	int IndexChildAST = 0;
	int IndexPrintf = 1;

	if (!NodeBodyFun->GetChild().empty())
	{
		while (IndexChildAST < NodeBodyFun->GetChild().size())
		{
			string NodeName = typeid(*NodeBodyFun->GetChild()[IndexChildAST]).name();
			string NameOfTheLeftSubnode = "";
			string ScopeOfTheLeftSubnode = "";

			RedactionString(NodeName);//Строка приводится в годное состояние

			//cout << "NodeName = " << NodeName << endl;
			if (NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL() != nullptr)
			{

				NameOfTheLeftSubnode = NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetStrName();
				ScopeOfTheLeftSubnode = NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetScope();
				
				//cout << "NodeName(L) = " << NameOfTheLeftSubnode << endl;
			}

			if (NodeName == "ASTIdentifier" && NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL() != nullptr && (NameOfTheLeftSubnode == "Write" || NameOfTheLeftSubnode == "WriteLine"))
			{
				CallPrintf(NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL(), Fileoutput, IndexPrintf);//Генерация Вывода
				//IndexPrintf++;
			}
			else if (NodeName == "ASTOperationBin")
			{
				StartArithmeticGeneration(NodeBodyFun->GetChild()[IndexChildAST], Fileoutput);//Генерация Арифметика
			}
			else if (NodeName == "ASTIf")
			{
				IfTraversal(NodeBodyFun->GetChild()[IndexChildAST], Fileoutput, ScopeOfTheLeftSubnode);//Генерация блока if
			}
			else if (NodeName == "ASTWhile")
			{
				WhileTraversal(NodeBodyFun->GetChild()[IndexChildAST], Fileoutput, ScopeOfTheLeftSubnode);//Генерация блока while
			}
			

			IndexChildAST++;
		}
	}
}

void GenerationCod::Compare(ofstream &Fileoutput, string op, string NameBlock, string Type)
{
	if (Type != "string")
	{
		Fileoutput << "\tcmpl %ebx, %eax" << endl;
	}
	else
	{
		Fileoutput << "\tcmpb %bl, %al" << endl;
	}

	if (op == "==")
	{
		Fileoutput << "\tjne " << NameBlock << endl << endl;
	}
	else if (op == "!=")
	{
		Fileoutput << "\tje " << NameBlock << endl << endl;
	}
	else if (op == "<")
	{
		Fileoutput << "\tjnl " << NameBlock << endl << endl;
	}
	else if (op == ">")
	{
		Fileoutput << "\tjng " << NameBlock << endl << endl;
	}
	else if (op == ">=")
	{
		Fileoutput << "\tjl " << NameBlock << endl << endl;
	}
	else if (op == "<=")
	{
		Fileoutput << "\tjg " << NameBlock << endl << endl;
	}
}

void GenerationCod::ProcessingOperandIf(string NodeName, string Register, unique_ptr<AST> const &NodeExp, ofstream &Fileoutput)
{
	if (NodeName != "ASTOperationBin")
	{
		PreparingTheOperand(Register, NodeExp, Fileoutput);
	}
	else 
	{
		StartArithmeticGeneration(NodeExp, Fileoutput);
	}
}

void GenerationCod::ExpressionIF(unique_ptr<AST> const &NodeIfExp, ofstream &Fileoutput, string NameBlock)
{
	string NodeNameOne = typeid(*NodeIfExp->GetExpressionL()).name();
	string NodeNameTwo;
	string RegisterAcum = "%eax";
	string RegisterBas = "%ebx";
	string Type = NodeIfExp->GetExpressionL()->GetStrType();

	if (Type == "string")
	{
		RegisterAcum = "%al";
		RegisterBas = "%bl";
	}

	RedactionString(NodeNameOne);

	if (NodeIfExp->GetExpressionR() != nullptr)
	{
		NodeNameTwo = typeid(*NodeIfExp->GetExpressionR()).name();
		RedactionString(NodeNameTwo);
	}
	else 
	{
		return;
	}

	if (NodeNameOne != "ASTOperationBin" && NodeNameTwo != "ASTOperationBin")
	{

		PreparingTheOperand(RegisterAcum, NodeIfExp->GetExpressionL(), Fileoutput);
		PreparingTheOperand(RegisterBas, NodeIfExp->GetExpressionR(), Fileoutput);

		//Compare(Fileoutput, NodeIfExp->GetStrName(), NameBlock);
	}

	if (NodeNameTwo == "ASTOperationBin")
	{
		StartArithmeticGeneration(NodeIfExp->GetExpressionR(), Fileoutput);
		Fileoutput << "\tpushl %eax" << endl;

		ProcessingOperandIf(NodeNameOne, RegisterAcum, NodeIfExp->GetExpressionL(), Fileoutput);
		Fileoutput << "\tpopl %eax" << endl;
		Fileoutput << "\tpopl %ebx" << endl << endl;
	}
	else if (NodeNameOne == "ASTOperationBin")
	{
		ProcessingOperandIf(NodeNameTwo, RegisterBas, NodeIfExp->GetExpressionR(), Fileoutput);
		Fileoutput << "\tpushl %ebx" << endl;

		StartArithmeticGeneration(NodeIfExp->GetExpressionL(), Fileoutput);
		Fileoutput << "\tpopl %eax" << endl;
		Fileoutput << "\tpopl %ebx" << endl << endl;
	}

	Compare(Fileoutput, NodeIfExp->GetStrName(), NameBlock, Type);
}

void GenerationCod::IfTraversal(unique_ptr<AST> const &NodeIf, ofstream &Fileoutput, string Scope)
{
	//Прочесть условие
	//cmpl op2, op1
	//jump если условие не выполняется, то перепрыгиываю код
	string NodeName = typeid(*NodeIf->GetExpressionL()).name();
	RedactionString(NodeName);

	//Зануление регистров перед использованием
	Fileoutput << "\txorl %eax, %eax" << endl;
	Fileoutput << "\txorl %ebx, %ebx" << endl << endl;

	//Проверка условия If
	if (NodeName != "ASTOperationBin")
	{ 
		string RegisterAcum = "%eax";

		PreparingTheOperand(RegisterAcum, NodeIf->GetExpressionL(), Fileoutput);

		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;

		if(NodeIf->GetExpressionR() != nullptr)
		{
			Fileoutput << "\tjng else_" << Scope << endl << endl;
		}
		else
		{
			Fileoutput << "\tjng blocif_" << Scope << endl << endl;
		}
	}
	else
	{
		string NameBlock = "blocif_" + Scope;

		if(NodeIf->GetExpressionR() != nullptr)
		{
			NameBlock = "else_" + Scope;
		}

		ExpressionIF(NodeIf->GetExpressionL(), Fileoutput, NameBlock);
	}

	FunBodyTraversal(NodeIf, Fileoutput);

	if(NodeIf->GetExpressionR() != nullptr)
	{
		Fileoutput << "\tjmp blocif_" << Scope << endl << endl;
		Fileoutput << "else_" << Scope << ":" << endl;
		FunBodyTraversal(NodeIf->GetExpressionR(), Fileoutput);
	}
	
	Fileoutput << "blocif_" << Scope << ":" << endl;
}

void GenerationCod::WhileTraversal(unique_ptr<AST> const &NodeWhile, ofstream &Fileoutput, string Scope)
{
	string NodeName = typeid(*NodeWhile->GetExpressionL()).name();
	RedactionString(NodeName);

	Fileoutput << "\txorl %eax, %eax" << endl;
	Fileoutput << "\txorl %ebx, %ebx" << endl << endl;

	if (NodeName != "ASTOperationBin")
	{
		string RegisterAcum = "%eax";

		PreparingTheOperand(RegisterAcum, NodeWhile->GetExpressionL(), Fileoutput);

		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng else_blockWhile_" << Scope << endl << endl;
	}
	else
	{
		string NameBlock = "else_blockWhile_" + Scope;
		ExpressionIF(NodeWhile->GetExpressionL(), Fileoutput, NameBlock);
	}

	Fileoutput << "if_blockWhile_" << Scope << ":" << endl;

	FunBodyTraversal(NodeWhile, Fileoutput);

	if (NodeName != "ASTOperationBin")
	{
		string RegisterAcum = "%eax";

		PreparingTheOperand(RegisterAcum, NodeWhile->GetExpressionL(), Fileoutput);

		Fileoutput << "\tmovl $0, %ebx" << endl << endl; //op2
		Fileoutput << "\tcmp %ebx, %eax" << endl;
		Fileoutput << "\tjng else_blockWhile_" << Scope << endl << endl;
	}
	else
	{
		string NameBlock = "else_blockWhile_" + Scope;
		ExpressionIF(NodeWhile->GetExpressionL(), Fileoutput, NameBlock);
	}

	Fileoutput << "\tjmp if_blockWhile_" << Scope << endl;
	Fileoutput << "else_blockWhile_" << Scope << ":" << endl;
}

void GenerationCod::CallPrintf(unique_ptr<AST> const &CallPrn, ofstream &Fileoutput, int &IndexPrintf)
{
	Fileoutput << endl;

	if (CallPrn->GetArgs()[0] != nullptr)
	{
		string Name = typeid(*CallPrn->GetArgs()[0]).name();
		string Type = CallPrn->GetArgs()[0]->GetStrType();
		string NameId = CallPrn->GetArgs()[0]->GetStrName();
		RedactionString(Name);//Строка приводится в годное состояние

		//cout << "Callprintf: " << Name << endl;
		if (Name == "ASTIdentifier")
		{
			if (Type != "string")
			{
				Fileoutput << "\tpushl " << NameId << endl;
				Fileoutput << "\tpushl " << "$format_l_n" << endl;
			}
			else
			{
				Fileoutput << "\tpushl $" << NameId << endl;
			}
		}
		else if (Name == "ASTArray")
		{
			//cout << CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName() << endl;
			string temp = typeid(CallPrn->GetArgs()[0]->GetIndex()[0]).name();
			RedactionString(temp);

			if (temp == "ASTDigit")
			{
				int ArrayIndexNumber = atoi(CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName().c_str());
				Fileoutput << "\tmovl " << NameId << "+" << ArrayIndexNumber * 4 << ", %eax" << endl;
			}
			else 
			{
				Fileoutput << "\tmovl " << CallPrn->GetArgs()[0]->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
				Fileoutput << "\tmovl " << NameId << "(, %ecx, 4), %eax" << endl;
			}
			Fileoutput << "\tpushl %eax" << endl;
			Fileoutput << "\tpushl " << "$format_l_n" << endl;
		}
		else if (Name == "ASTString")
		{
			Fileoutput << "\tpushl $String_" <<  CallPrn->GetArgs()[0]->GetScope() << "_" << IndexPrintf << endl;
			IndexPrintf++;
		}
		else if (Name == "ASTDigit")
		{
			int Number = atoi(CallPrn->GetArgs()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl $" << NameId << ", %eax" << endl;
			Fileoutput << "\tpushl %eax" << endl;
			Fileoutput << "\tpushl " << "$format_l_n" << endl;
		}

	}
	
	Fileoutput << "\tcall printf" << endl << endl;

	//while (i < CallPrn->GetArgs().size())
	//{
	//	Fileoutput << "\tpushl " + CallPrn->GetArgs()[IndexChildAST] << endl;
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
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i].getNode() != nullptr)
				{
					string Type = it->second[i].getNode()->GetStrType();
					name = typeid(*it->second[i].getNode()).name();
					RedactionString(name);//Строка приводится в годное состояние	
					//cout << name << endl;

					if (Type != "string")
					{		
						Fileoutput << "\t" << it->second[i].getNode()->GetStrName() << ":" << endl;
					}

					if (name == "ASTIdentifier")
					{
						if (Type != "string")
						{
							Fileoutput << "\t\t.long " << "0" << endl;
						}

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
	Fileoutput << "\tTemp:" << endl;
	Fileoutput << "\t\t.long 0" << endl;
}

void GenerationCod::WriteDataStringsClass(vector<unique_ptr<AST>> const &VectorClass, ofstream &Fileoutput)
{
	int IndexChildAST = 0;

	while (IndexChildAST < VectorClass.size())
	{
		string NodeName = typeid(*VectorClass[IndexChildAST]).name();// Запись имени узла
		RedactionString(NodeName);//Строка приводится в годное состояние

		if (NodeName == "ASTClass")
		{
			WriteDataStringsFunction(VectorClass[IndexChildAST], Fileoutput);
		}
		IndexChildAST++;
	}
}

void GenerationCod::WriteDataStringsFunction(unique_ptr<AST> const &NodeClass, ofstream &Fileoutput)
{
	int IndexChildAST = 0;

	if (!NodeClass->GetChild().empty())
	{
		while (IndexChildAST < NodeClass->GetChild().size())
		{
			string NodeName = typeid(*NodeClass->GetChild()[IndexChildAST]).name();
			RedactionString(NodeName);//Строка приводится в годное состояние

			//cout << "Inside " << typeid(*NodeClass).name() << " ClassTraversal: " << NodeName << endl;
 			if (NodeName == "ASTStatic")
			{
				//Генерация кода для функции
				WriteDataStrings(NodeClass->GetChild()[IndexChildAST], Fileoutput);
			}
			IndexChildAST++;
		}
	}
}

void GenerationCod::WriteDataStrings(unique_ptr<AST> const &NodeBodyFun, ofstream &Fileoutput)
{
	int IndexChildAST = 0;
	int IndexPrintf = 1;

	if (!NodeBodyFun->GetChild().empty())
	{
		while (IndexChildAST < NodeBodyFun->GetChild().size())
		{
			string NodeName = typeid(*NodeBodyFun->GetChild()[IndexChildAST]).name();
			string NameOfTheLeftSubnode = "";
			string ScopeOfTheLeftSubnode = "";
			RedactionString(NodeName);//Строка приводится в годное состояние

			if (NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL() != nullptr)
			{
				NameOfTheLeftSubnode = NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetStrName();
				ScopeOfTheLeftSubnode = NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetScope();
			}
			
			if (NodeName == "ASTIdentifier" && NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL() != nullptr && (NameOfTheLeftSubnode == "Write" || NameOfTheLeftSubnode == "WriteLine"))
			{
				if (NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetArgs()[0] != nullptr)
				{

					string SubnodeName = typeid(*NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetArgs()[0]).name();
					RedactionString(SubnodeName);
					//cout << SubnodeName << endl;
					if (SubnodeName == "ASTString")
					{
						string Stroka = NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetArgs()[0]->GetStrName();
						cout << "NameOfTheLeftSubnode = " << NameOfTheLeftSubnode << endl;
						if (NameOfTheLeftSubnode == "WriteLine")
						{
							Stroka = Stroka + "\n";	
						}
						Fileoutput << "\tString_" << NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetArgs()[0]->GetScope() << "_" << IndexPrintf << ":" << endl;
						Fileoutput << "\t\t.string " << Stroka << endl;
						IndexPrintf++;
					}
				}
			}
			else if (NodeName == "ASTIf")
			{
				WriteDataStrings(NodeBodyFun->GetChild()[IndexChildAST], Fileoutput);
			}
			else if (NodeName == "ASTWhile")
			{
				
				WriteDataStrings(NodeBodyFun->GetChild()[IndexChildAST], Fileoutput);
			}
			else if (NodeName == "ASTOperationBin" && NodeBodyFun->GetChild()[IndexChildAST]->GetStrName() == "=")
			{	
				string OperationName = NodeBodyFun->GetChild()[IndexChildAST]->GetStrName();
				string Type = NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetStrType();
				string NodeType = typeid(*NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionR()).name();
				RedactionString(NodeType);

				cout << "OperationName = " << OperationName << endl;
				cout << "Type = " << Type << endl;
				cout << "NodeType = " << NodeType << endl;

				if (Type == "string" && OperationName == "=" && Type == "string" && NodeType == "ASTString")
				{
					string Stroka = NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionR()->GetStrName();
					Stroka[Stroka.size() - 1] = '\\';
					Stroka += "n\"";  
					Fileoutput << "\t" << NodeBodyFun->GetChild()[IndexChildAST]->GetExpressionL()->GetStrName() << ":" << endl;
					Fileoutput << "\t\t.string " << Stroka << endl;
				}
			}

			IndexChildAST++;
		}
	}
}

void GenerationCod::Operation(ofstream &Fileoutput, string OperandIdL, string Operator)
{

	if (Operator == "=")
	{	
		Fileoutput << "\tmovl %ebx, " << OperandIdL << endl << endl;
	}
	else if (Operator == "+")
	{
		Fileoutput << "\taddl %ebx, %eax" << endl << endl;
	}
	else if (Operator == "-")
	{
		Fileoutput << "\tsubl %ebx, %eax" << endl << endl;
	}
	else if (Operator == "*")
	{
		Fileoutput << "\tmull %ebx" << endl << endl;
	}
	else if (Operator == "/")
	{
		Fileoutput << "\tcdq" << endl;
		Fileoutput << "\tidivl %ebx" << endl << endl;
	}
	else if (Operator == "%")
	{
		Fileoutput << "\txorl %edx, %edx" << endl;
		Fileoutput << "\txorl %ecx, %ecx" << endl;

		Fileoutput << "\tmovl %ebx, %ecx" << endl;
		Fileoutput << "\tmovl %eax, Temp" << endl;

		//Fileoutput << "\tmovl %eax, %ecx" << endl;

		Fileoutput << "\tcdq" << endl;
		Fileoutput << "\tidivl %ecx" << endl;//Получили только целую часть деления в %eax

		//Fileoutput << "\tmovl %eax, Temp" << endl;
		Fileoutput << "\tmull %ecx" << endl;//Домножили целую часть на делитель
		Fileoutput << "\tmovl %eax, %ebx" << endl;

		//Fileoutput << "\tmovl %ecx, %eax" << endl;

		Fileoutput << "\tmovl Temp, %eax" << endl;
		Fileoutput << "\tsubl %ebx, %eax" << endl << endl;
	}
	//результат остался в аккуме
}

void GenerationCod::StartArithmeticGeneration(unique_ptr<AST> const &NodeOper, ofstream &Fileoutput)
{
	string OperationName = NodeOper->GetStrName();
	string Type = NodeOper->GetStrType();

	if (Type == "string")
		return;

	//cout << "OperationName: " << OperationName << endl;
	if (NodeOper->GetExpressionL() == nullptr)
	{
		return;
	}
	//cout << "Left is good!" << endl;

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
	//cout << "UnaryOperations" << endl;

	if (NodeNameOne == "ASTIdentifier")
	{
		if (OperationName == "++")
		{
			Fileoutput << "\tincl " << OperandIdL << endl;
		}
		else if (OperationName == "--")
		{
			Fileoutput << "\tdecl " << OperandIdL << endl;
		}
	}
	else 
	{
		string str = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(str);
		//cout << "CalculationsTraversal1: str = " << str << " " << OperandIdL << endl;
		if (str == "ASTDigit")
		{
			int ArrayIndexNumber = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl " << OperandIdL << "+" << ArrayIndexNumber * 4 << ", %eax" << endl;

			if (OperationName == "++")
			{
				Fileoutput << "\tdecl %eax" << endl;
			}
			else if (OperationName == "--")
			{
				Fileoutput << "\tincl %eax" << endl;
			}

			Fileoutput << "\tmovl %eax, " << OperandIdL << "+" << ArrayIndexNumber * 4 << endl;
		}
		else
		{
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "\tmovl " << OperandIdL << "(, %ecx, 4), %eax" << endl;

			if (OperationName == "++")
			{
				Fileoutput << "\tdecl %eax" << endl;
			}
			else if (OperationName == "--")
			{
				Fileoutput << "\tincl %eax" << endl;
			}

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
		Fileoutput << "\tpushl " << "$format_l" << endl;
		Fileoutput << "\tcall scanf" << endl << endl;
	}
	else
	{
		string NodeArrayIndex = typeid(*NodeOper->GetExpressionL()->GetIndex()[0]).name();
		RedactionString(NodeArrayIndex);
		//cout << "CalculationsTraversal1: str = " << str << " " << OperandIdL << endl;
		if (NodeArrayIndex == "ASTDigit")
		{
			int ArrayIndexNumber = atoi(NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName().c_str());
			Fileoutput << "\tmovl $" << OperandIdL << "+" << ArrayIndexNumber * 4 << ", %eax" << endl;
			Fileoutput << "\tpushl %eax" << endl;
			Fileoutput << "\tpushl " << "$format_l" << endl;
			Fileoutput << "\tcall scanf" << endl << endl;
		} 
		else
		{
			Fileoutput << "\tmovl $Temp, %eax" << endl;
			Fileoutput << "\tpushl %eax" << endl;
			Fileoutput << "\tpushl " << "$format_l" << endl;
			Fileoutput << "\tcall scanf" << endl;
			Fileoutput << "\tmovl Temp, %eax" << endl;
			Fileoutput << "\tmovl " << NodeOper->GetExpressionL()->GetIndex()[0]->GetStrName() << ", %ecx" << endl;
			Fileoutput << "\tmovl %eax, " << OperandIdL << "(, %ecx, 4)" << endl << endl;	
		}
	}

	
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
	Fileoutput << "\txorl %ebx, %ebx" << endl << endl;

	// Если достигли узла, где слева и справа узлы не являются узлами операций, генерируем ассемблерный код
	if (NodeNameTwo != "ASTOperationBin" && NodeNameOne != "ASTOperationBin")
	{

		//cout << "OperationIdToId: " << OperandIdL << OperationName << OperandIdR << endl;

		OperationIdToId(NodeOper, Fileoutput);
		//OperationGeneration(NodeNameOne, NodeNameTwo, Operator);
		Fileoutput << "\tpushl %eax" << endl; // Результат сохраняется в стек

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
		Operation(Fileoutput, OperandIdL, OperationName);
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
		Operation(Fileoutput, OperandIdL,  OperationName);
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
	else
	{
		string NodeArrayIndex = typeid(*NodeOperand->GetIndex()[0]).name();
		RedactionString(NodeArrayIndex);

		string Type = NodeOperand->GetStrType();
		if(Type != "string")
		{
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
		else
		{
			if (NodeArrayIndex == "ASTDigit")
			{
				int ArrayIndexNumber = atoi(NodeOperand->GetIndex()[0]->GetStrName().c_str());
				Fileoutput << "\tmovl $" << ArrayIndexNumber << ", %ecx" << endl;
				Fileoutput << "\tmovb " << OperandId << "(, %ecx, 1), " << Register << endl;
			}
			else 
			{
				string ArrayIndexId = NodeOperand->GetIndex()[0]->GetStrName();
				Fileoutput << "\tmovl " << ArrayIndexId << ", %ecx" << endl;
				Fileoutput << "\tmovb " << OperandId << "(, %ecx, 1), " << Register << endl;
			}
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
		Operation(Fileoutput, OperandIdL, OperationName);
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
		Operation(Fileoutput, OperandIdL, OperationName);
	}
}