#include "Parser.h"

using namespace std;

bool GetPredToken(int &Index, int size, vector<unique_ptr<Token>> const &VectorToken)
{
	if (Index > 0)
	{
		Index--;
		//cout << VectorToken[Index]->GetString() << " " << Index << "/" << size << endl;
		return true;
	}
	return false;
}

bool GetNextToken(int &Index, int size, vector<unique_ptr<Token>> const &VectorToken)
{
	if (Index < size - 1)
	{
		Index++;
		//cout << VectorToken[Index]->GetString() << " " << Index << "/" << size << endl;
		return true;
	}
	return false;
}

int Parser::GetTokPrecedence(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size)
{
	if (VectorToken[Index]->GetTypeToken() == Token::Words::Unexpected)
	{
		cerr << " Unexpected token in gettokprecedence" << endl;
		return 0;
	}	

	Token Operator;
	int TokPrec = Operator.GetBinopPrecedence(VectorToken[Index]->GetString());

	if (TokPrec <= 0) return -1;

	if (VectorToken[Index]->GetTypeToken() != Token::Words::Assignment) FlagEqual = 1;

	return TokPrec;
}

string CheckSystemDigit(Token::Words t)
{
	switch (t)
	{
	case Token::Words::Binary:
		return "Binary";
	case Token::Words::Octal:
		return "Octal";
	case Token::Words::DecimalInteger:
		return "DecimalInteger";
	case Token::Words::Hexadecimal:
		return "Hexadecimal";
	case Token::Words::NotInteger:
		return "NotInteger";
	case Token::Words::Symbol:
		return "char";
	default:
		return "";
	}
}

bool Parser::CheckThisIsBlock(string NodeName)
{
	//cout << "CheckThisIsBlock = " << NodeName << endl;
 	if (NodeName == "class ASTIf" || NodeName == "class ASTWhile:" || NodeName == "class ASTSubBlock" || NodeName == "class ASTElse")
 	{
		return true;
 	}

	return false;
}

bool Parser::CheckDigit(vector<unique_ptr<Token>> const &VectorToken, int Index)
{
	switch (VectorToken[Index]->GetTypeToken())
	{
	case Token::Words::Binary:
	case Token::Words::Octal:
	case Token::Words::DecimalInteger:
	case Token::Words::Hexadecimal:
	case Token::Words::NotInteger:
	case Token::Words::Symbol:
		return true;
	default:
		return false;
	}
}

bool Parser::CheckType(vector<unique_ptr<Token>> const &VectorToken, int Index)
{
	switch (VectorToken[Index]->GetTypeToken())
	{
	case Token::Words::Void:
	case Token::Words::Var:
	case Token::Words::Char:
	case Token::Words::Float:
	case Token::Words::Int:
	case Token::Words::Double:
	case Token::Words::Bool:
	case Token::Words::string:
		return true;
	default:
		return false;
	}
}

unique_ptr<AST> Parser::ParseParenAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable)
{
	GetNextToken(Index, size, VectorToken);

	auto Expression = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable);
	if (!Expression)
	{
		return nullptr;
	}
		
	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightParen) 
	{
		return Correctness(VectorToken, Index, size, " expected ')'");
	}
		
	GetNextToken(Index, size, VectorToken);
	return move(Expression);
}

unique_ptr<AST> Parser::ArrayDefenition(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, vector<ScopeVar> &SubTable, string Scope, int IndexFun, string IdType)
{
	GetNextToken(Index, size, VectorToken);//Пропуск [
	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightSquare) 
	{
		return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "Expected ] after type[");
	}

	GetNextToken(Index, size, VectorToken);//Пропуск ]
	if (VectorToken[Index]->GetTypeToken() != Token::Words::Identifier) 
	{
		return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "Expected id after type[]");
	}

	string name = VectorToken[Index]->GetString();//Запись id

	GetNextToken(Index, size, VectorToken);//Пропуск id
	if (VectorToken[Index]->GetTypeToken() != Token::Words::Assignment) 
	{
		return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " expected '=' after 'type[] id'");
	}

	GetNextToken(Index, size, VectorToken);//Пропуск =
	if (VectorToken[Index]->GetTypeToken() != Token::Words::KeyWordNew)
	{
	 	return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "Expected new after type[] id = ");
	}
	
	auto LHS = make_unique<ASTIdentifier>(name, IdType, Scope);

	return KeyWordNew(0, move(LHS), VectorToken, Index, size, Scope, IndexFun, SubTable);
}

unique_ptr<AST> Parser::ParseTypeIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, vector<ScopeVar> &SubTable, string Scope, int IndexFun)
{
	string IdType = VectorToken[Index]->GetString();//Запись типа
	if (FlagDefenition != "") 
	{
		IdType = FlagDefenition; 
		--Index;
	}

	GetNextToken(Index, size, VectorToken);//Пропуск типа
	if (VectorToken[Index]->GetTypeToken() == Token::Words::LeftSquare && VectorToken[Index - 1]->GetTypeToken() != Token::Words::Var)
	{
		//Запуск чтения/обработки определения массива
		return CorrectnessDefenition(VectorToken, Index, size, ArrayDefenition(VectorToken, Index, size, SubTable, Scope, IndexFun, IdType));
	}
	else if (VectorToken[Index]->GetTypeToken() == Token::Words::LeftSquare && VectorToken[Index - 1]->GetTypeToken() == Token::Words::Var)
	{
		return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " error: var[]");
	}

	if (VectorToken[Index]->GetTypeToken() != Token::Words::Identifier)
	{
		return Correctness(VectorToken, Index, size, " expected 'id' after 'type...'");
	}

	string Idname = VectorToken[Index]->GetString(); //Запись id
	GetNextToken(Index, size, VectorToken);//Пропуск id

	if (VectorToken[Index]->GetTypeToken() == Token::Words::LeftParen)
	{
		//Запуск чтения/обработки определения локальной функции
		return CorrectnessDefenition(VectorToken, Index/*(Index -= 3)*/, size, ReadFunLocal(VectorToken, Index -= 3, size, Scope, IndexFun, SubTable));
	}

	return make_unique<ASTIdentifier>(Idname, IdType, Scope, true);
}

unique_ptr<AST> Parser::ParseIdentifierASTArray(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string const &IdName, unique_ptr<AST> id, string Scope, int IndexFun, vector<ScopeVar> &SubTable)
{
	vector<unique_ptr<AST>> Args;//Вектор аргументов
	GetNextToken(Index, size, VectorToken);//Пропуск [

	string Type = CheckDeclaretion(Scope + "_" + to_string(IndexFun), id->GetStrName(), id->GetStrType(), id, SubTable);//Проверка наличия в тиблицы символов массива с данным id

	if (Type == id->GetStrName() + " without definition" || Type == id->GetStrName() + " has definition at this lvl")
	{
		//cout << "ParseIdentifierASTArray" << " " << Type << endl;
		return Correctness(VectorToken, Index, size, " " + Type);	
	}
	id->SetStrType(Type);// Запись типа в узел

	if (VectorToken[Index]->GetTypeToken() != Token::Words::Identifier && !CheckDigit(VectorToken, Index)) 
	{
		return Correctness(VectorToken, Index, size, " error Index: expected 'id' after 'id['");
	}

	while (VectorToken[Index]->GetTypeToken() == Token::Words::Identifier || CheckDigit(VectorToken, Index))
	{
		auto Arg = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable);
		if (Arg == nullptr)	
		{
			return Correctness(VectorToken, Index, size, " error arguments array: " +  IdName);
		}
			
		Args.push_back(move(Arg));

		if (VectorToken[Index]->GetTypeToken() == Token::Words::RightSquare) break;

		if (VectorToken[Index]->GetTypeToken() != Token::Words::Comma) 
		{
			return Correctness(VectorToken, Index, size, " expected ']' or ',' in argument list");
		}
		
		GetNextToken(Index, size, VectorToken);

		if (VectorToken[Index]->GetTypeToken() != Token::Words::Identifier && !CheckDigit(VectorToken, Index)) 
		{
			return Correctness(VectorToken, Index, size, "Error arguments");
		}
	}

	//if (VectorToken[Index]->GetTypeToken() != Token::Words::RightSquare) return Correctness(VectorToken, Index, size, "Expected ] after 'id = new type[...'");
	GetNextToken(Index, size, VectorToken);// Пропуск ] 

	return make_unique<ASTArray>(move(id), move(Args), Scope);
}

unique_ptr<AST> Parser::ParseIdentifierASTFun(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string const &IdName, string Scope, int IndexFun, vector<ScopeVar> &SubTable, int FlagMethod)
{
	vector<unique_ptr<AST>> Args;//Вектор аргументов вызываемой функции
	GetNextToken(Index, size, VectorToken);//Пропуск (
	
	while (VectorToken[Index]->GetTypeToken() == Token::Words::Identifier || CheckDigit(VectorToken, Index) || VectorToken[Index]->GetTypeToken() == Token::Words::String)
	{
		if (FlagMethod == 1)
		{
			return Correctness(VectorToken, Index, size, IdName + " error arguments fun: " +  IdName);
		}

		auto Arg = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable);// Чтение выражения
		if (Arg == nullptr) 
		{
			return Correctness(VectorToken, Index, size, " error arguments fun: " +  IdName);
		}

		Args.push_back(move(Arg));// Считанный аргумент закидывается в вектор аргументов

		if (VectorToken[Index]->GetTypeToken() == Token::Words::RightParen) break;

		if (VectorToken[Index]->GetTypeToken() != Token::Words::Comma) 
		{
			return Correctness(VectorToken, Index, size, "Expected ')' or ',' in argument list");
		}
		
		GetNextToken(Index, size, VectorToken);//Пропуск ,

		if (!(VectorToken[Index]->GetTypeToken() == Token::Words::Identifier || CheckDigit(VectorToken, Index) || VectorToken[Index]->GetTypeToken() == Token::Words::String))
		{
			return Correctness(VectorToken, Index, size, " error arguments fun: " +  IdName);
		}
	}
	//if (VectorToken[Index]->GetTypeToken() != Token::Words::RightParen) return Correctness(VectorToken, Index, size, "Expected ) after 'id(...'");
	
	GetNextToken(Index, size, VectorToken);//Пропуск )

	return make_unique<ASTCallFun>(IdName, move(Args), Scope);//Возвращается узел вызываемой функции
}

unique_ptr<AST> Parser::ParseIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable) {

	string IdName = VectorToken[Index]->GetString();//Запись id
	auto Identifier = make_unique<ASTIdentifier>(IdName, Scope);//Создание узла переменной

	GetNextToken(Index, size, VectorToken);//Пропуск id

	//Если это обычная переменная, то возвращается созданный узел
	if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftParen && VectorToken[Index]->GetTypeToken() != Token::Words::LeftSquare && VectorToken[Index]->GetTypeToken() != Token::Words::Point)
	{
		return move(Identifier);
	}

	if (VectorToken[Index]->GetTypeToken() == Token::Words::LeftParen)
	{ 
		//Запуск чтения/обработки вызова функции
		return ParseIdentifierASTFun(VectorToken, Index, size, IdName, Scope, IndexFun, SubTable, 0);
	}
	else if (VectorToken[Index]->GetTypeToken() == Token::Words::LeftSquare)
	{
		//Запуск чтения/обработки массива
		return ParseIdentifierASTArray(VectorToken, Index, size, IdName, move(Identifier), Scope, IndexFun, SubTable);
	}
	else if (VectorToken[Index]->GetTypeToken() == Token::Words::Point && IdName == "Console")/*(IdName == "Console" || CheckDeclaretion(Scope + to_string(IndexFun), Identifier->GetStrName(), Identifier->GetStrType(), Identifier, SubTable) == "string"))*/
	{
		//cout << "Identifier =========" << CheckDeclaretion(Scope + to_string(IndexFun), Identifier->GetStrName(), Identifier->GetStrType(), Identifier, SubTable) << endl;
		//Запуск чтения/обработки вызова метода
		return CallMethod(VectorToken, Index, size, Scope, IndexFun, SubTable, IdName);
	}
	
	return Correctness(VectorToken, Index, size, IdName + " has not the method");
}

//Функция обработки числа и строки, если задается flag = 0, то обрабатывается число, если flag = 1, то обрабатывается строка 
unique_ptr<AST> Parser::ParseNumberStringAST(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, vector<ScopeVar> &SubTable, string Scope, int Flag)
{
	unique_ptr<AST> Result;

	if (Flag == 0)
	{
		string SystemNumber = CheckSystemDigit(VectorToken[Index]->GetTypeToken());//Опеределяется система счисления
		//Если система счисления не определена вернет ошибку
		if (SystemNumber == "") 
		{
			return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetStrNum(), "");
		}

		Result = make_unique<ASTDigit>(SystemNumber, VectorToken[Index]->GetString(), Scope);// Создание узла числа
	}
	else
	{	
		Result = make_unique<ASTString>(VectorToken[Index]->GetString(), Scope);//Создание узла строки
	}
	
	GetNextToken(Index, size, VectorToken);//Пропуск токена числа или строки

	return move(Result);
}

unique_ptr<AST> Parser::ParsePrimary(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable)
{
	unique_ptr<AST> NodeObject(new AST());//Вспомогательный узел AST
	string NodeName = "";
	if (FlagDefenition != "")
	{
		
		NodeObject = ParseTypeIdentifierAST(VectorToken, Index, size, SubTable, Scope, IndexFun);// Определение выражения
		FlagDefenition = "";
		//Возвращаем функцию проверки узла
		return CheckOb(VectorToken, Index, Scope, IndexFun, SubTable, " parseTypeIdentifierAST return nullptr", move(NodeObject));
	}

	switch (VectorToken[Index]->GetTypeToken()) 
	{
	case Token::Words::Minus://В случае если встретится унарный минус
		return ParseBinOpRHS(0, make_unique<AST>(), VectorToken, Index, size, Scope, IndexFun, SubTable);
	case Token::Words::Void:
	case Token::Words::Char:
	case Token::Words::Int:
	case Token::Words::Float:
	case Token::Words::Double:
	case Token::Words::string:
	case Token::Words::Var:
	case Token::Words::Bool:
		
		NodeObject = ParseTypeIdentifierAST(VectorToken, Index, size, SubTable, Scope, IndexFun);// Определение выражения
		//cout << "ParseTypeIdentifierAST->" << endl;

		//Возвращаем функцию проверки узла
		if (NodeObject != nullptr)
		{
			NodeName = typeid(*NodeObject).name();
			RedactionString(NodeName);//Строка приводится в годное состояние
			
			if (NodeName == "ASTLocal")
			{
				return move(NodeObject);
			}
		}

		return CheckOb(VectorToken, Index, Scope, IndexFun, SubTable, " parseTypeIdentifierAST return nullptr", move(NodeObject));
	case Token::Words::Identifier:
		
		NodeObject = ParseIdentifierAST(VectorToken, Index, size, Scope, IndexFun, SubTable);// выражение
		
		//Возвращаем функцию проверки узла
		if (NodeObject != nullptr && NodeObject->GetStrType() == "int" && NodeObject->GetMethodLength()) 
		{
			return move(NodeObject);
		}

		return CheckOb(VectorToken, Index, Scope, IndexFun, SubTable, " parseIdentifierAST return nullptr", move(NodeObject));
	case Token::Words::CallMethodId:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		NodeObject = ParseIdentifierAST(VectorToken, Index, size, Scope, IndexFun, SubTable);// выражение
		//if (NodeObject != nullptr && NodeObject->GetStrType() == "int" && NodeObject->GetMethodLength()) return move(NodeObject);
		//Возвращаем функцию проверки узла
		return CheckOb(VectorToken, Index, Scope, IndexFun, SubTable, " parseIdentifierAST return nullptr", move(NodeObject));
	case Token::Words::Binary:
	case Token::Words::Octal:
	case Token::Words::DecimalInteger:
	case Token::Words::Hexadecimal:
	case Token::Words::NotInteger:
	case Token::Words::Symbol:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		return ParseNumberStringAST(VectorToken, Index, size, SubTable, Scope, 0);// Обработка числа
	case Token::Words::String:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		return ParseNumberStringAST(VectorToken, Index, size, SubTable, Scope, 1);//Обработка строки
	case Token::Words::LeftParen:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		return ParseParenAST(VectorToken, Index, size, Scope, IndexFun, SubTable);// Обработка скобок
	default:
		//Любой другой токен, который встретиться в этом блоке вызовет ошибку
		return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " unknown token when expecting an expression");
	}
}

//Функция чтения/обработки блока new
unique_ptr<AST> Parser::KeyWordNew(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable)
{
	GetNextToken(Index, size, VectorToken);//Пропуск new

	if (CheckType(VectorToken, Index))//Провера наличия типа
	{
		string Type = VectorToken[Index]->GetString();//Сохраняется тип

		GetNextToken(Index, size, VectorToken);//Пропуск типа

		if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftSquare) 
		{
			return Correctness(VectorToken, Index, size, " exprected '['' after  'id = new type'");
		}

		GetNextToken(Index, size, VectorToken);//Пропуск [
		vector<unique_ptr<AST>> Size;//Вектор размеров массива

		while (1) {//Цикл считывания размеров
			/*if (CheckType(VectorToken, Index))//Проверка наличия типа
			{ 
				return Correctness(VectorToken, Index, size, "Error callfun arguments");
			}
			else */if (auto Arg = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable))
			{
					Size.push_back(move(Arg));
			}

			if (VectorToken[Index]->GetTypeToken() == Token::Words::RightSquare) break;//Если встртился ], то конец цикла

			//Если запятой нет, вернется ошибка
			if (VectorToken[Index]->GetTypeToken() != Token::Words::Comma) 
			{
				return Correctness(VectorToken, Index, size, " expected ')' or ',' in argument list");
			}

			GetNextToken(Index, size, VectorToken);//Пропуск ,

		}
		//if (VectorToken[Index]->GetTypeToken() != Token::Words::RightSquare) return Correctness(VectorToken, Index, size, "Expected ] after 'id = new type[...'");
		
		GetNextToken(Index, size, VectorToken); //Пропуск ]

		//Проверка наличия ;
		if (VectorToken[Index]->GetTypeToken() != Token::Words::Semicolon) 
		{
			return Correctness(VectorToken, Index, size, " expected ';' after 'id = new type[...]'");
		}
		
		return make_unique<ASTArray>(move(LHS), Type, move(Size), Scope);//Возвращение узла new
	}
	else//В случае отсутствия типа вернется ошибка
	{
		return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " expected 'type' after 'new'");
	}
}

//Функция обработки операций
unique_ptr<AST> Parser::ParseBinOpRHS(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable)
{
	while (true)
	{
		//Проверка корректности оперций
		if (VectorToken[Index]->GetTypeToken() == Token::Words::Assignment && FlagEqual == 1) Correctness(VectorToken, Index, size, " incorrect operation");
		int TokPrec = GetTokPrecedence(VectorToken, Index, size);//Получение индекса приоритета операции 

		if (TokPrec < ExprPrec)//Проверка наличия операции, если вернулось значение меньше 0, то такой операции нет в таблицы операций
		{
			FlagEqual = 0;//Вспомогательный флаг для проверки корректности операций
			return move(LHS);
		}

		string BinOp = VectorToken[Index]->GetString();// Запись операции
		GetNextToken(Index, size, VectorToken); // Пропуск операции
		if (TokPrec == 5)//Операция инк/декрементации
		{
			FlagEqual = 0;//Вспомогательный флаг для проверки корректности операций
			LHS = make_unique<ASTOperationBin>(move(LHS), nullptr, BinOp, Scope);

			return move(LHS);
		}

		// Проверка на наличие оператора выделения динамической памяти
		if (VectorToken[Index]->GetTypeToken() == Token::Words::KeyWordNew && LHS->GetStrType() == "var") return KeyWordNew(0, move(LHS), VectorToken, Index, size, Scope, IndexFun, SubTable);
		
		//Считывание следующего операнда
		auto RHS = ParsePrimary(VectorToken, Index, size, Scope, IndexFun, SubTable);
		if (!RHS)
		{
			return nullptr;
		}

		if (RHS->GetExpressionL() != nullptr)//Проверка инициализации переменной с помощью ввода
		{
			string NodeName = typeid(*RHS->GetExpressionL()).name();
			//cout << NodeName << " " << RHS->GetExpressionL()->GetStrName() << endl;
			if (NodeName == "class ASTCallFun" && RHS->GetExpressionL()->GetStrName() != "ReadLine")
			{
				Correctness(VectorToken, Index, size, " use output method in operations");
			}
		}
		
		//Получает следующюю операцию
		int NextPrec = GetTokPrecedence(VectorToken, Index, size);
		if (TokPrec < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec + 1, move(RHS), VectorToken, Index, size, Scope, IndexFun, SubTable);//Обработка дальнейших оперций
			if (!RHS)
			{
				return nullptr;
			}
		}

		LHS = make_unique<ASTOperationBin>(move(LHS), move(RHS), BinOp, Scope);//Возвращение узла операций
	}
	FlagEqual = 0;
}

////////////////////////////////////HARD Threre BLOCKS

unique_ptr<AST> Parser::ReadWhile(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов
	GetNextToken(Index, size, VectorToken);//Пропуск while

	if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftParen) 
	{
		return Correctness(VectorToken, Index, size, " expected '(' after 'while'");	
	}
	GetNextToken(Index, size, VectorToken);//Пропуск (

	//Чтение/обработка выражения в скобках
	auto Compare = ReadExpression(VectorToken, Index, size, Scope + "_" + to_string(IndexFun), IndexFun, SubTable);
	if (Compare == nullptr) 
	{
		return nullptr;// Проверка корректности завершения обработки
	}

	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightParen) 
	{
		return Correctness(VectorToken, Index, size, " expected ')' after 'while(...'");
	}
	GetNextToken(Index, size, VectorToken);//Пропуск )

	if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftCurly) 
	{
		return Correctness(VectorToken, Index, size, " expected '{' after 'while(expr)'");
	}
	GetNextToken(Index, size, VectorToken);//Пропуск {
	
	//Чтение/обработка тела while
	auto whilebody = ReadFunBody(VectorToken, Index, size, Scope + "_" + to_string(IndexFun), SubTable);

	//Проверка корректности завершения обработки тела while
	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightCurly) 
	{
		return Correctness(VectorToken, Index, size, " expected '}' after 'while(expr){...'");
	}
	GetNextToken(Index, size, VectorToken);//Пропуск }

	return make_unique<ASTWhile>(move(Compare), move(whilebody), Scope);
}

unique_ptr<AST> Parser::ReadElse(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов
	GetNextToken(Index, size, VectorToken);//Пропуск }

	//Если встретился else, обрабатывается и привязывается к узлу if
	if (VectorToken[Index]->GetTypeToken() != Token::Words::KeyWordElse) 
	{
		return nullptr;
	}

	GetNextToken(Index, size, VectorToken);//Пропуск else

	if (VectorToken[Index]->GetTypeToken() == Token::Words::LeftCurly)
	{
		GetNextToken(Index, size, VectorToken);//Пропуск {

		//Чтение/обработка тела else
		auto elsefunbody = ReadFunBody(VectorToken, Index, size, Scope + "_" + to_string(IndexFun), SubTable);

		//Проверка корректности завершения тела else
		if (VectorToken[Index]->GetTypeToken() != Token::Words::RightCurly)  
		{
			return Correctness(VectorToken, Index, size, " expected '}' after 'else{...'");
		}

		GetNextToken(Index, size, VectorToken);//Пропуск }

		return make_unique<ASTElse>(move(elsefunbody), Scope + "_" + to_string(IndexFun));
	}
	return Correctness(VectorToken, Index, size, " expected '{' after 'else'");
}

unique_ptr<AST> Parser::ReadIf(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int &IndexFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов
	GetNextToken(Index, size, VectorToken);//Пропуск if

	//Проверка наличия (
	if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftParen) 
	{
		return Correctness(VectorToken, Index, size, " expected '(' after 'if'");
	}
	GetNextToken(Index, size, VectorToken);//Пропуск (

	auto Compare = ReadExpression(VectorToken, Index, size, Scope + "_" + to_string(IndexFun), IndexFun, SubTable);//Чтение/обработка выражения в скобках
	if (Compare == nullptr) 
	{
		return nullptr;//Проверка корректности обработки
	}

	//Проверка наличия )
	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightParen) 
	{
		return Correctness(VectorToken, Index, size, " expected ')' after 'if(...'");
	}
	GetNextToken(Index, size, VectorToken);//Пропуск )

	//Проверка наличия {
	if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftCurly) 
	{
		return Correctness(VectorToken, Index, size, " expected '{' after 'if(expr)'");
	}
	GetNextToken(Index, size, VectorToken);//Пропуск {

	//Чтение и обработка тела if
	auto IfBody = ReadFunBody(VectorToken, Index, size, Scope + "_" + to_string(IndexFun), SubTable);

	//Проверка корректности завершения блока if
	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightCurly) 
	{
		return Correctness(VectorToken, Index, size, " expected '}' after 'if(expr){...'");
	}

	auto ElseBody = ReadElse(VectorToken, Index, size, Scope, 1 + IndexFun);//Проверка наличия else(обработка)
	if (ElseBody != nullptr)
	{
		IndexFun++;
		return make_unique<ASTIf>(move(Compare), move(IfBody), move(ElseBody), Scope);
	}
	return make_unique<ASTIf>(move(Compare), move(IfBody), Scope);
}

unique_ptr<AST> Parser::ReadFunLocal(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTableFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов

	auto Fun = ReadFun(VectorToken, Index, size, Scope + "_" + to_string(0));//Чтение/обработка типа, id и аргументов локальной функции

	if (Fun == nullptr) 
	{
		return Correctness(VectorToken, Index, size, "");//Проверка корректности завершения обработки блока
	}

	//Проверка наличия id фун-ии в таблице символов на соответствующем уровне
	string str = CheckDeclaretion(Scope, Fun->GetStrName(), Fun->GetStrType(), Fun, SubTableFun);
	//cout << "ReadFunLocal" << " " << Fun->GetStrName() << " " << Fun->GetStrType() << endl;

	//Возвращение ошибки в случаи, если имеются id на данном уровне с таким же именем
	if (str == Fun->GetStrName() + " has definition at this lvl") 
	{
		return Correctness(VectorToken, Index, size, str);
	}
	Fun->SetStrType(str);// Занесение типа в узел

	//Проверка наличия {
	if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftCurly)  
	{
		return Correctness(VectorToken, Index, size, " expected '{' after 'static type id (...)'");
	}

	//Аргументы функции заносятся в таблицу символов
	for (int i = 0; i < Fun->GetArgs().size(); i++)
		if (Fun->GetArgs()[i] != nullptr)
		{
			SubTable.push_back(ScopeVar(Fun->GetArgs()[i].get(), Scope + "_" + to_string(IndexFun)));
			Table[Scope + "_" + to_string(IndexFun)] = SubTable;
		}

	GetNextToken(Index, size, VectorToken);// Пропуск {

	auto FunBody = ReadFunBody(VectorToken, Index, size, Scope + "_" + to_string(IndexFun), SubTable);// Чтение/обработка тела локальной функции
	if (FlagError == 1) 
	{
		return Correctness(VectorToken, Index, size, "");//Проверка корректности завершения обработки блока
	}

	//Проверка корректности завершения локальной фун-ии
	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightCurly) 
	{
		return Correctness(VectorToken, Index, size, " expected '}' in end fun");
	}

	GetNextToken(Index, size, VectorToken);//Пропуск }

	//cout << "end read fun local" << endl;
	return make_unique<ASTLocal>(move(Fun), move(FunBody), Scope);//Возвращение узла локальной функции
}

unique_ptr<AST> Parser::ReadSubBlock(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun)
{
	vector<ScopeVar> SubTable;// lvl таблицы символов
	GetNextToken(Index, size, VectorToken);// Пропуск {

	auto SubBlockBody = ReadFunBody(VectorToken, Index, size, Scope + "_" + to_string(IndexFun), SubTable);//Чтение/обработка тела подблока
	if (FlagError == 1) 
	{
		return Correctness(VectorToken, Index, size, "");//Проперка успешности чтения/обработки тела подблока
	}

	//Проверка корректности завершения подблока
	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightCurly) return Correctness(VectorToken, Index, size, " expected '}' after 'SubBlock{...'");

	GetNextToken(Index, size, VectorToken);//Пропуск }

	return make_unique<ASTSubBlock>(move(SubBlockBody), Scope);//Возвращение узла подблока
}

unique_ptr<AST> Parser::ReadReturn(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexFun, vector<ScopeVar> &SubTable)
{
	GetNextToken(Index, size, VectorToken);//Пропуск return

	auto Expression = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable);//Обработка возвращаемого выражения
	if (Expression == nullptr) 
	{
		return LogError(0, 0, "");//Если return;, возвращается соответствующая версия узла return
	}

	//Отсутствие ;, возвращеется ошибка
	if (VectorToken[Index]->GetTypeToken() != Token::Words::Semicolon) 
	{
		return Correctness(VectorToken, Index, size, " expected ';' after 'return...'");
	}
	
	GetNextToken(Index, size, VectorToken);//Пропуск ;

	return make_unique<ASTReturn>(move(Expression), Scope);
}

//Чтение и обработка тела функции
vector<unique_ptr<AST>> Parser::ReadFunBody(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, vector<ScopeVar> &SubTable)
{
	vector<unique_ptr<AST>> FunBody;// Вектор узлов тела функции
	unique_ptr<AST> NodeObject(new AST());//Вспомогательный узел AST
	int IndexFun = 1;//Нужен для Scope(lvl)
	string type;

	while (Index < size - 1)//Пока не кончатся токены
	{
		switch (VectorToken[Index]->GetTypeToken())
		{
		case Token::Words::KeyWordIf://Начала обработки блока if
			NodeObject = ReadIf(VectorToken, Index, size, Scope, IndexFun);
			IndexFun++;// lvl++

			if (NodeObject != nullptr)// Проверка успешности чтения обработки блока if
			{
				FunBody.push_back(move(NodeObject));// Запись в вектор узлов функции
			}
			else
			{
				return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");//Возвращение ошибки в противном случае
			}

			break;
		case Token::Words::KeyWordWhile://Начала обработки блока while
			NodeObject = ReadWhile(VectorToken, Index, size, Scope, IndexFun);
			IndexFun++;//lvl++

			if (NodeObject != nullptr)// Проверка успешности чтения обработки блока while
			{
				FunBody.push_back(move(NodeObject));// Запись в вектор узлов функции
			}
			else
			{
				return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");//Возвращение ошибки в противном случае
			}

			break;
		case Token::Words::Void:
		case Token::Words::Char:
		case Token::Words::Int:
		case Token::Words::Float:
		case Token::Words::Double:
		case Token::Words::Var:
		case Token::Words::string:
		case Token::Words::Bool:
			
			if (CheckType(VectorToken, Index + 1))// Проверка на зацикливание
			{
				return CorrectnessVect(VectorToken, Index, size, " error 'Type' after 'Type'");
			}

			type = VectorToken[Index]->GetString();
			do
			{
				NodeObject = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable); // Запуск чтения и обработки выражения
				IndexFun++;//lvl++
				
				if (NodeObject != nullptr)// Проверка успешности чтения обработки блока
				{
					FunBody.push_back(move(NodeObject));// Запись в вектор узлов функции
				}
				else//Возвращение ошибки в противном случае
				{
					return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");
				}

				//Проверка на корректное завершение чтения выражения
				if (VectorToken[Index]->GetTypeToken() != Token::Words::Semicolon && 
					VectorToken[Index - 1]->GetTypeToken() != Token::Words::RightCurly &&  
					!(VectorToken[Index]->GetTypeToken() == Token::Words::Comma && VectorToken[Index + 1]->GetTypeToken() == Token::Words::Identifier))
				{
					//cout << "BINGO!!!" << endl;
					return CorrectnessVect(VectorToken, Index, size, " expected ';' after 'expression'");//Возвращение ошибки в противном случае
				}

				if (VectorToken[Index]->GetTypeToken() == Token::Words::Comma) 
				{
					GetNextToken(Index, size, VectorToken);
					FlagDefenition = type;
				}
			} while (VectorToken[Index-1]->GetTypeToken() == Token::Words::Comma);

			//if (VectorToken[Index]->GetTypeToken() != Token::Words::Semicolon && VectorToken[Index - 1]->GetTypeToken() != Token::Words::RightCurly)
			//	return CorrectnessVect(VectorToken, Index, size, "Expected ; after expression");//Возвращение ошибки в противном случае

			break;
		case Token::Words::Identifier:
			NodeObject = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable);// Запуск чтения и обработки выражения
			IndexFun++;//lvl++

			if (NodeObject != nullptr)// Проверка успешности чтения обработки блока
			{
				FunBody.push_back(move(NodeObject));// Запись в вектор узлов функции
			}
			else//Возвращение ошибки в противном случае
			{
				return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");
			}

			//Проверка на корректное завершение чтения выражения
			if (VectorToken[Index]->GetTypeToken() != Token::Words::Semicolon)
			{
				return CorrectnessVect(VectorToken, Index, size, " expected ';' after 'expression'");
			}

			break;
		case Token::Words::CallMethodId://Чтение и обработка вызова функции
			NodeObject = ReadExpression(VectorToken, Index, size, Scope, IndexFun, SubTable);// Запуск чтения и обработки выражения
			IndexFun++;//lvl++

			if (NodeObject != nullptr)// Проверка успешности чтения обработки блока
			{
				FunBody.push_back(move(NodeObject));// Запись в вектор узлов функции
			}
			else//Возвращение ошибки в противном случае
			{
				return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");
			}

			//Проверка на корректное завершение чтения выражения
			if (VectorToken[Index]->GetTypeToken() != Token::Words::Semicolon)
			{
				return CorrectnessVect(VectorToken, Index, size, " expected ';' after 'expression'");
			}

			break;
		case Token::Words::KeyWordReturn://Чтение и обработка оператора return
			NodeObject = ReadReturn(VectorToken, Index, size, Scope, IndexFun, SubTable);
			IndexFun++;//lvl++

			if (NodeObject != nullptr)// Проверка успешности чтения обработки блока
			{
				FunBody.push_back(move(NodeObject));// Запись в вектор узлов функции
			}
			else//Возвращение ошибки в противном случае
			{
				return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");
			}

			break;
		case Token::Words::RightCurly://Если встретили }, завершение чтения тела функции, вектор узлов вернется
			//cout << "end fun" << endl;
			return move(FunBody);
		case Token::Words::Semicolon:

			GetNextToken(Index, size, VectorToken);
			break;
		case Token::Words::LeftCurly:// Начала чтения и обработки подблока
			NodeObject = ReadSubBlock(VectorToken, Index, size, Scope, IndexFun);
			IndexFun++;//lvl++

			if (NodeObject != nullptr)// Проверка успешности чтения обработки блока
			{
				FunBody.push_back(move(NodeObject));// Запись в вектор узлов функции
			}
			else//Возвращение ошибки в противном случае
			{
				return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");
			}

			break;
		case Token::Words::Unexpected://Возвращение ошибки, если встретился undexpected

			return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " unexpected token");
		default:// остальные выражения не должны появляться в данном блоке, в случаи их появления, вернется ошибка

			return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " unexpected token");
		}
		//GetNextToken(Index, size, VectorToken);
	}
	return move(FunBody);
}


unique_ptr<AST> Parser::ReadFun(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope)
{
	GetNextToken(Index, size, VectorToken);//Пропуск Static

	if (CheckType(VectorToken, Index))// Проверка наличия типа функции
	{
		string FunType = VectorToken[Index]->GetString();// Запись типа функции
		GetNextToken(Index, size, VectorToken);//Пропуск типа

		//Проверка наличия id функции
		if (VectorToken[Index]->GetTypeToken() != Token::Words::Identifier) 
		{
			return CorrectnessOpr(VectorToken, Index, size, " expected 'id' after 'static type'");
		}

		string FunName = VectorToken[Index]->GetString();// Запись id фун-ии
		GetNextToken(Index, size, VectorToken);//Пропуск id

		//Проверка наличия (
		if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftParen) 
		{
			return CorrectnessOpr(VectorToken, Index, size, " expected '(' after 'static type id'");
		}

		GetNextToken(Index, size, VectorToken);//Пропуск (
		vector<unique_ptr<AST>> Args;// Вектор для хранения аргументов фунции

		while (CheckType(VectorToken, Index))//Проверка наличия типа аргумента
		{
			string type = VectorToken[Index]->GetString();//Запись типа аргумента
			GetNextToken(Index, size, VectorToken);//Пропуск типа

			//Проверка наличия id
			if (VectorToken[Index]->GetTypeToken() != Token::Words::Identifier) 
			{
				return CorrectnessOpr(VectorToken, Index, size, "fun arguments: expected 'id' after 'type'");
			}
			//Запись считанного аргумента в вектор аргуметнов
			Args.push_back(make_unique<ASTIdentifier>(VectorToken[Index]->GetString(), type, Scope));
			GetNextToken(Index, size, VectorToken);// Пропуск id

			//Проверка корректности
			if (VectorToken[Index]->GetTypeToken() == Token::Words::Comma)
			{
				GetNextToken(Index, size, VectorToken);// Пропуск ,

				//Проверка наличия типа после ,
				if (!CheckType(VectorToken, Index)) 
				{
					return CorrectnessOpr(VectorToken, Index, size, " error fun arguments");
				}
			}//Если встречается ), чтение аргументов завершается
			else if (VectorToken[Index]->GetTypeToken() == Token::Words::RightParen) continue;
			else
			{
				return CorrectnessOpr(VectorToken, Index, size, " error fun arguments");// Если ни запятой, ни ) не встретилось вернет ошибку
			}
		}

		//if (VectorToken[Index]->GetTypeToken() != Token::Words::RightParen)  return CorrectnessOpr(VectorToken, Index, size, "Expected ) after 'static type id(...'");
		//Создание узла AST
		auto Fun = make_unique<ASTFun>(FunName, FunType, move(Args), Scope.substr(0, Scope.size() - 2));
		GetNextToken(Index, size, VectorToken);// Пропуск )
		return move(Fun);//Возвращение обработанного узла
	}
	else //Вернет ошибку в случае отсутствия типа функции
	{
		return CorrectnessOpr(VectorToken, Index, size, " expected 'type' after 'static'");
	}
}

//Функция чтения и обработки тела класса 
vector<unique_ptr<AST>> Parser::ReadBodyClass(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexClass)
{
	vector<unique_ptr<AST>> Vector;
	int IndexFun = 1;// Нужен для Scope

	vector<ScopeVar> SubTableFun;

	GetNextToken(Index, size, VectorToken);// Пропуск id class

	if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftCurly) 
	{
		return CorrectnessVect(VectorToken, Index, size, " expected '{' after 'class id'");// Проверка {
	}

	GetNextToken(Index, size, VectorToken);// Пропуск {


	while (VectorToken[Index]->GetTypeToken() == Token::Words::KeyWordStatic || VectorToken[Index]->GetTypeToken() == Token::Words::Public)// Условие начала чтения и обработки статической функции
	{
		if (VectorToken[Index]->GetTypeToken() == Token::Words::Public)
		{
			GetNextToken(Index, size, VectorToken);// Пропуск public
		}

		vector<ScopeVar> SubTable;// Таблица символов

		auto Fun = ReadFun(VectorToken, Index, size, Scope + to_string(IndexClass) + "_" + to_string(IndexFun));// Запуск чтения/обработки имени, типа и аргументов функции
		if (Fun == nullptr) 
		{
			return CorrectnessVect(VectorToken, Index, size, "");// Проверка успешности чтения функции
		}

		string Type = CheckDeclaretion(Scope + to_string(IndexClass), Fun->GetStrName(), Fun->GetStrType(), Fun, SubTableFun);// Проверка наличия такого же id(имя функции) в таблице символов

		//Если уже имеется статическая функция с таким же именем, то вернет ошибку
		if (/*Type == Fun->GetStrName() + " without definition" || */Type == Fun->GetStrName() + " has definition at this lvl") 
		{
			return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), Type); 
		}
		Fun->SetStrType(Type);// Запись типа в узел

		//Начала обработки тела функции
		if (VectorToken[Index]->GetTypeToken() != Token::Words::LeftCurly) 
		{
			return CorrectnessVect(VectorToken, Index, size, " expected '{' after static type id (...)");
		}

		//Аргументы функции заносятся на соответствующий lvl таблицы символов
		for (int i = 0; i < Fun->GetArgs().size(); i++)
			if (Fun->GetArgs()[i] != nullptr)
			{
				SubTable.push_back(ScopeVar(Fun->GetArgs()[i].get(), to_string(IndexClass) + "_" + to_string(IndexFun)));
				Table[to_string(IndexClass) + "_" + to_string(IndexFun)] = SubTable;
			}
		
		GetNextToken(Index, size, VectorToken);// Пропуск {

		//Чтение и обработка тела функции
		auto FunBody = ReadFunBody(VectorToken, Index, size, to_string(IndexClass) + "_" + to_string(IndexFun), SubTable);

		//Проверка успешного завершения чтения тела функции
		if (FlagError == 1) 
		{
			return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), "");
		}

		// Завершение тела функции }
		if (VectorToken[Index]->GetTypeToken() != Token::Words::RightCurly) 
		{
			return CorrectnessVect(VectorToken, Index, size, " expected '}' in end fun");
		}
			
		GetNextToken(Index, size, VectorToken);//Пропуск }

		//Определяется узел статической функции и помещается в вектор тела класса
		unique_ptr<ASTStatic> StaticFun(new ASTStatic(move(Fun), move(FunBody)));
		Vector.push_back(move(StaticFun));
		IndexFun++;//Вычисление индекса следующей функции
	} 

	if (VectorToken[Index]->GetTypeToken() != Token::Words::RightCurly) 
	{
		return CorrectnessVect(VectorToken, Index, size, " expected '}' in end class");
	}

	//GetNextToken(Index, size, VectorToken);//пропуск }
	return move(Vector);
}

//Функция чтения класса
unique_ptr<AST> Parser::ReadClass(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size, string Scope, int IndexClass)
{
	GetNextToken(Index, size, VectorToken);// Пропуск class
	if (VectorToken[Index]->GetTypeToken() == Token::Words::Identifier)
	{
		string ClassName = VectorToken[Index]->GetString();//Имя класса
		auto Class = make_unique<ASTClass>(ClassName);// Узел класса

		auto ClassNodesVector = ReadBodyClass(VectorToken, Index, size, Scope, IndexClass);// Вектор узлов тела класса

		if (Class == nullptr || FlagError == 1) 
		{
			return Correctness(VectorToken, Index, size, "");// Если произошла ошибка вернется Correctness 
		}

		Class->AddVectorChildren(move(ClassNodesVector));// Привязывается вектор узлов тела класса к узлу соответствующего класса
		//cout << "Index = " << Index << endl;
		return move(Class);
	}
	
	return LogError(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " expected 'id' after class");
}

unique_ptr<AST> Parser::ReadUsing(vector<unique_ptr<Token>> const &VectorToken, int &Index, int size)
{
	GetNextToken(Index, size, VectorToken);//Пропуск using

	if (VectorToken[Index]->GetTypeToken() != Token::Words::Library) 
	{
		return Correctness(VectorToken, Index, size, " expected 'library' after 'using'");
	}

	string NameLibrary = VectorToken[Index]->GetString();// Запись id библиотеки
	GetNextToken(Index, size, VectorToken);//Пропуск id

	if (VectorToken[Index]->GetTypeToken() != Token::Words::Semicolon) 
	{
		return Correctness(VectorToken, Index, size, " expected ';' after 'using library'");
	}

	return make_unique<ASTUsing>(NameLibrary);
}

vector<unique_ptr<AST>> Parser::BuildAST(vector<unique_ptr<Token>> const &VectorToken, int size) {
	int Index = 0;
	int IndexClass = 1;
	bool Move = true;
	vector<unique_ptr<AST>> RootVector;
	unique_ptr<AST> NodeName(new AST());

	while (Move) 
	{
		switch (VectorToken[Index]->GetTypeToken()) 
		{
		case Token::Words::End:

			return move(RootVector);
		case Token::Words::Class:
			NodeName = ReadClass(VectorToken, Index, size, "", IndexClass);
			IndexClass++;

			if (NodeName != nullptr)
			{
				RootVector.push_back(move(NodeName));
			}
			else
			{
				return move(RootVector);
			}

			break;
		case Token::Words::KeyWordUsing:

			NodeName = ReadUsing(VectorToken, Index, size);

			if (NodeName != nullptr)
			{
				RootVector.push_back(move(NodeName));
			}
			else
			{
				return move(RootVector);
			}

			break;
		case Token::Words::Public:

			NodeName = ReadClass(VectorToken,	++Index, size, "", IndexClass);
			IndexClass++;

			if (NodeName != nullptr)
			{
				RootVector.push_back(move(NodeName));
			}
			else
			{
				return move(RootVector);
			}

			break;
		default:

			return LogErrorVector(VectorToken[Index]->GetStrNum(), VectorToken[Index]->GetColNum(), " unexpected Token for mainloop: " + VectorToken[Index]->GetString());
			//cerr << "Unexpected Token for mainloop" << endl;
			//break;
		}
		Move = GetNextToken(Index, size, VectorToken);
	}
	return move(RootVector);
}

void Parser::Parsing(vector<unique_ptr<Token>> const &VectorToken, int size, int Option)
{
	auto Vector = BuildAST(VectorToken, size);
	unique_ptr<AST> R(new ASTRoot(move(Vector)));
	Root = move(R);

	if (FlagError == 0 && Option)
	{
		Root->print(2);
		//PrintTable();
	}
}

string Parser::CheckIdName(vector<ScopeVar> &Iterator, string Name)
{
	if (!Iterator.empty())
	{
		for (int i = 0; i < Iterator.size(); i++)
			if (Iterator[i].getNode() != nullptr && Iterator[i].getNode()->GetStrName() == Name) 
				return Iterator[i].getNode()->GetStrType();
	}
	return "";
}

string Parser::CheckIdTableSymbol(string Name, string Scope, string Type)
{
	map<string, vector<ScopeVar>>::iterator Iterator = Table.begin();
	for (; Iterator != Table.end(); Iterator++)
	{
		if (Type != "unssigned")
		{
			if (Scope == Iterator->first) 
			{
				return CheckIdName(Iterator->second, Name);	
			}
		}
		else
		{
			int i = Iterator->first.size();

			if (Scope.substr(0, i) == Iterator->first.substr(0, Iterator->first.size()))
			{
				string temp;
				temp = CheckIdName(Iterator->second, Name);
				
				if (temp != "") 
				{
					return temp;
				}
			}
		}
	}

	return "";
}

string Parser::CheckDeclaretion(string Scope, string Name, string Type, unique_ptr<ASTIdentifier> const &Tree, vector<ScopeVar> &SubTable)
{
	string TypeChange = "";
	
	if (Type == "unssigned")
	{
		if ((Type = CheckIdTableSymbol(Name, Scope, Type)) == "") 
		{
			return (Name + " without definition");
		}
	}
	else
	{
		if ((TypeChange = CheckIdTableSymbol(Name, Scope, Type)) != "") 
		{
			return (Name + " has definition at this lvl");
		}

		SubTable.push_back(ScopeVar(Tree.get(), Scope));
		Table[Scope] = SubTable;
	}
	return Type;
}

string Parser::CheckDeclaretion(string Scope, string Name, string Type, unique_ptr<AST> const &Tree, vector<ScopeVar> &SubTable)
{
	string TypeChange = "";
	
	if (Type == "unssigned")
	{
		if ((Type = CheckIdTableSymbol(Name, Scope, Type)) == "") 
		{
			return (Name + " without definition");
		}
	}
	else
	{
		if ((TypeChange = CheckIdTableSymbol(Name, Scope, Type)) != "") 
		{
			return (Name + " has definition at this lvl");
		}

		SubTable.push_back(ScopeVar(Tree.get(), Scope));
		Table[Scope] = SubTable;
	}
	return Type;
}

void RedactionString(string &String)
{
	for (int IndexI = 0; IndexI < String.size(); IndexI++)
	{
		if ('0' <= String[IndexI] && String[IndexI] <= '9')
		{
			for (int IndexJ = IndexI; IndexJ < String.size() - 1; IndexJ++)
			{
				String[IndexJ] = String[IndexJ + 1];
			}

			String.pop_back();
			IndexI--;
		}
	}
}

void Parser::PrintTable()
{
	cout << endl << "SymbolTable:" << endl;
	map<string, vector<ScopeVar>>::iterator Iterator = Table.begin();

	for (; Iterator != Table.end(); Iterator++)
	{
		cout << Iterator->first << ": ";
		if (!Iterator->second.empty())
			for (int i = 0; i < Iterator->second.size(); i++)
			{
				if (Iterator->second[i].getNode() != nullptr)
				{
					string NodeName = typeid(*(Iterator->second[i]).getNode()).name();
					RedactionString(NodeName);
					
					cout << Iterator->second[i].getNode()->GetStrName();
					cout << "(" << NodeName << ")";
					cout << " ";
				}
			}
		cout << endl;
	}
	cout << endl;
}