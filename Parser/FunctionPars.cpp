#include "Parser.h"

using namespace std;

bool GetPredToken(int &index, int size, vector<unique_ptr<Token>> const &VectorToken)
{
	if (index > 0)
	{
		index--;
		//cout << VectorToken[index]->GetString() << " " << index << "/" << size << endl;
		return true;
	}
	return false;
}

bool GetNextToken(int &index, int size, vector<unique_ptr<Token>> const &VectorToken)
{
	if (index < size - 1)
	{
		index++;
		//cout << VectorToken[index]->GetString() << " " << index << "/" << size << endl;
		return true;
	}
	return false;
}

int Parser::GetTokPrecedence(vector<unique_ptr<Token>> const &VectorToken, int &index, int size)
{
	if (VectorToken[index]->GetTypeToken() == Token::Words::Unexpected)
	{
		cerr << " enexpected token in gettokprecedence" << endl;
		return 0;
	}	

	Token temp;
	int TokPrec = temp.GetBinopPrecedence(VectorToken[index]->GetString());
	if (TokPrec <= 0) return -1;
	if (VectorToken[index]->GetTypeToken() != Token::Words::Assignment) FlagEqual = 1;
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

bool Parser::CheckThisIsBlock(string str)
{
	//cout << "CheckThisIsBlock = " << str << endl;
 	if (str == "class ASTIf" || str == "class ASTWhile:" || str == "class ASTSubBlock" || str == "class ASTElse")
		return true;
	return false;
}

bool Parser::CheckDigit(vector<unique_ptr<Token>> const &VectorToken, int index)
{
	switch (VectorToken[index]->GetTypeToken())
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

bool Parser::CheckType(vector<unique_ptr<Token>> const &VectorToken, int index)
{
	switch (VectorToken[index]->GetTypeToken())
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

unique_ptr<AST> Parser::ParseParenAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable)
{
	GetNextToken(index, size, VectorToken);

	auto V = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable);
	if (!V) return nullptr;
		
	if (VectorToken[index]->GetTypeToken() != Token::Words::RightParen) return Correctness(VectorToken, index, size, " expected ')'");
		
	GetNextToken(index, size, VectorToken);
	return move(V);
}

unique_ptr<AST> Parser::ArrayDefenition(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, vector<ScopeVar> &SubTable, string scope, int indexFun, string idType)
{
	GetNextToken(index, size, VectorToken);//Пропуск [
	if (VectorToken[index]->GetTypeToken() != Token::Words::RightSquare) return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "Expected ] after type[");

	GetNextToken(index, size, VectorToken);//Пропуск ]
	if (VectorToken[index]->GetTypeToken() != Token::Words::Identifier) return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "Expected id after type[]");

	string name = VectorToken[index]->GetString();//Запись id

	GetNextToken(index, size, VectorToken);//Пропуск id
	if (VectorToken[index]->GetTypeToken() != Token::Words::Assignment) 
		return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " expected '=' after 'type[] id'");

	GetNextToken(index, size, VectorToken);//Пропуск =
	if (VectorToken[index]->GetTypeToken() != Token::Words::KeyWordNew) return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "Expected new after type[] id = ");
	
	auto LHS = make_unique<ASTIdentifier>(name, idType, scope);

	return KeyWordNew(0, move(LHS), VectorToken, index, size, scope, indexFun, SubTable);
}

unique_ptr<AST> Parser::ParseTypeIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, vector<ScopeVar> &SubTable, string scope, int indexFun)
{
	string Idtype = VectorToken[index]->GetString();//Запись типа
	if (FlagDefenition != "") 
	{
		Idtype =  FlagDefenition; 
		--index;
	}
	GetNextToken(index, size, VectorToken);//Пропуск типа

	if (VectorToken[index]->GetTypeToken() == Token::Words::LeftSquare && VectorToken[index - 1]->GetTypeToken() != Token::Words::Var)
	{
		//Запуск чтения/обработки определения массива
		return CorrectnessDefenition(VectorToken, index, size, ArrayDefenition(VectorToken, index, size, SubTable, scope, indexFun, Idtype));
	}
	else if (VectorToken[index]->GetTypeToken() == Token::Words::LeftSquare && VectorToken[index - 1]->GetTypeToken() == Token::Words::Var)
		return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " error: var[]");

	if (VectorToken[index]->GetTypeToken() != Token::Words::Identifier) return Correctness(VectorToken, index, size, " expected 'id' after 'type...'");

	string Idname = VectorToken[index]->GetString(); //Запись id
	GetNextToken(index, size, VectorToken);//Пропуск id

	if (VectorToken[index]->GetTypeToken() == Token::Words::LeftParen)
	{
		//Запуск чтения/обработки определения локальной функции
		return CorrectnessDefenition(VectorToken, index/*(index -= 3)*/, size, ReadFunLocal(VectorToken, index -= 3, size, scope, indexFun, SubTable));
	}

	return make_unique<ASTIdentifier>(Idname, Idtype, scope, true);
}

unique_ptr<AST> Parser::ParseIdentifierASTArray(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string const &IdName, unique_ptr<AST> id, string scope, int indexFun, vector<ScopeVar> &SubTable)
{
	vector<unique_ptr<AST>> Args;//Вектор аргументов
	GetNextToken(index, size, VectorToken);//Пропуск [

	string str = CheckDeclaretion(scope + "_" + to_string(indexFun), id->GetStrName(), id->GetStrType(), id, SubTable);//Проверка наличия в тиблицы символов массива с данным id

	if (str == id->GetStrName() + " without definition" || str == id->GetStrName() + " has definition at this lvl")
	{
		//cout << "ParseIdentifierASTArray" << " " << str << endl;
		return Correctness(VectorToken, index, size, " " + str);	
	}
	id->SetStrType(str);// Запись типа в узел

	if (VectorToken[index]->GetTypeToken() != Token::Words::Identifier && !CheckDigit(VectorToken, index)) return Correctness(VectorToken, index, size, " error index: expected 'id' after 'id['");

	while (VectorToken[index]->GetTypeToken() == Token::Words::Identifier || CheckDigit(VectorToken, index))
	{
		auto Arg = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable);
		if (Arg == nullptr)	return Correctness(VectorToken, index, size, " error arguments array: " +  IdName);
			
		Args.push_back(move(Arg));

		if (VectorToken[index]->GetTypeToken() == Token::Words::RightSquare) break;

		if (VectorToken[index]->GetTypeToken() != Token::Words::Comma) return Correctness(VectorToken, index, size, " expected ']' or ',' in argument list");
		
		GetNextToken(index, size, VectorToken);

		if (VectorToken[index]->GetTypeToken() != Token::Words::Identifier && !CheckDigit(VectorToken, index)) return Correctness(VectorToken, index, size, "Error arguments");
	}

	//if (VectorToken[index]->GetTypeToken() != Token::Words::RightSquare) return Correctness(VectorToken, index, size, "Expected ] after 'id = new type[...'");
	GetNextToken(index, size, VectorToken);// Пропуск ] 

	return make_unique<ASTArray>(move(id), move(Args), scope);
}

unique_ptr<AST> Parser::ParseIdentifierASTFun(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string const &IdName, string scope, int indexFun, vector<ScopeVar> &SubTable, int flagMethod)
{
	vector<unique_ptr<AST>> Args;//Вектор аргументов вызываемой функции
	GetNextToken(index, size, VectorToken);//Пропуск (
	//cout << VectorToken[index]->GetString() << endl << endl;
	
	while (VectorToken[index]->GetTypeToken() == Token::Words::Identifier || CheckDigit(VectorToken, index) || VectorToken[index]->GetTypeToken() == Token::Words::String)
	{
		if (flagMethod == 1)
			return Correctness(VectorToken, index, size, IdName + " error arguments fun: " +  IdName);

		auto Arg = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable);// Чтение выражения
		if (Arg == nullptr) return Correctness(VectorToken, index, size, " error arguments fun: " +  IdName);

		Args.push_back(move(Arg));// Считанный аргумент закидывается в вектор аргументов

		if (VectorToken[index]->GetTypeToken() == Token::Words::RightParen) break;

		if (VectorToken[index]->GetTypeToken() != Token::Words::Comma) return Correctness(VectorToken, index, size, "Expected ')' or ',' in argument list");
		
		GetNextToken(index, size, VectorToken);//Пропуск ,

		if (!(VectorToken[index]->GetTypeToken() == Token::Words::Identifier || CheckDigit(VectorToken, index) || VectorToken[index]->GetTypeToken() == Token::Words::String))
			return Correctness(VectorToken, index, size, " error arguments fun: " +  IdName);
	}
	//if (VectorToken[index]->GetTypeToken() != Token::Words::RightParen) return Correctness(VectorToken, index, size, "Expected ) after 'id(...'");
	
	GetNextToken(index, size, VectorToken);//Пропуск )

	return make_unique<ASTCallFun>(IdName, move(Args), scope);//Возвращается узел вызываемой функции
}

unique_ptr<AST> Parser::ParseIdentifierAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable) {

	string IdName = VectorToken[index]->GetString();//Запись id
	auto id = make_unique<ASTIdentifier>(IdName, scope);//Создание узла переменной

	GetNextToken(index, size, VectorToken);//Пропуск id

	//Если это обычная переменная, то возвращается созданный узел
	if (VectorToken[index]->GetTypeToken() != Token::Words::LeftParen && VectorToken[index]->GetTypeToken() != Token::Words::LeftSquare && VectorToken[index]->GetTypeToken() != Token::Words::Point)
		return move(id);

	if (VectorToken[index]->GetTypeToken() == Token::Words::LeftParen)
	{ 
		//Запуск чтения/обработки вызова функции
		return ParseIdentifierASTFun(VectorToken, index, size, IdName, scope, indexFun, SubTable, 0);
	}
	else if (VectorToken[index]->GetTypeToken() == Token::Words::LeftSquare)
	{
		//Запуск чтения/обработки массива
		return ParseIdentifierASTArray(VectorToken, index, size, IdName, move(id), scope, indexFun, SubTable);
	}
	else if (VectorToken[index]->GetTypeToken() == Token::Words::Point && IdName == "Console")/*(IdName == "Console" || CheckDeclaretion(scope + to_string(indexFun), id->GetStrName(), id->GetStrType(), id, SubTable) == "string"))*/
	{
		//cout << "id =========" << CheckDeclaretion(scope + to_string(indexFun), id->GetStrName(), id->GetStrType(), id, SubTable) << endl;
		//Запуск чтения/обработки вызова метода
		return CallMethod(VectorToken, index, size, scope, indexFun, SubTable, IdName);
	}
	
	return Correctness(VectorToken, index, size, IdName + " has not the method");
}

//Функция обработки числа и строки, если задается flag = 0, то обрабатывается число, если flag = 1, то обрабатывается строка 
unique_ptr<AST> Parser::ParseNumberStringAST(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, vector<ScopeVar> &SubTable, string scope, int flag)
{
	//cout << "ParseNumberStringAST" << endl;
	//cout << VectorToken[index]->GetString() << endl;
	unique_ptr<AST> Result;
	if (flag == 0)
	{
		string SystemNumber = CheckSystemDigit(VectorToken[index]->GetTypeToken());//Опеределяется система счисления
		//Если система счисления не определена вернет ошибку
		if (SystemNumber == "") return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetStrNum(), "");
		Result = make_unique<ASTDigit>(SystemNumber, VectorToken[index]->GetString(), scope);// Создание узла числа
	}
	else
		Result = make_unique<ASTString>(VectorToken[index]->GetString(), scope);//Создание узла строки
	
	GetNextToken(index, size, VectorToken);//Пропуск токена числа или строки

	return move(Result);
}

unique_ptr<AST> Parser::ParsePrimary(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable)
{
	unique_ptr<AST> ob(new AST());//Вспомогательный узел AST
	string NodeName = "";
	if (FlagDefenition != "")
	{
		
		ob = ParseTypeIdentifierAST(VectorToken, index, size, SubTable, scope, indexFun);// Определение выражения
		FlagDefenition = "";
		//Возвращаем функцию проверки узла
		return CheckOb(VectorToken, index, scope, indexFun, SubTable, " parseTypeIdentifierAST return nullptr", move(ob));
	}

	switch (VectorToken[index]->GetTypeToken()) 
	{
	case Token::Words::Minus://В случае если встретится унарный минус
		return ParseBinOpRHS(0, make_unique<AST>(), VectorToken, index, size, scope, indexFun, SubTable);
	case Token::Words::Void:
	case Token::Words::Char:
	case Token::Words::Int:
	case Token::Words::Float:
	case Token::Words::Double:
	case Token::Words::string:
	case Token::Words::Var:
	case Token::Words::Bool:
		
		ob = ParseTypeIdentifierAST(VectorToken, index, size, SubTable, scope, indexFun);// Определение выражения
		//cout << "ParseTypeIdentifierAST->" << endl;

		//Возвращаем функцию проверки узла
		if (ob != nullptr)
		{
			NodeName = typeid(*ob).name();
			RedactionString(NodeName);//Строка приводится в годное состояние
			//cout << NodeName << endl;
			if (NodeName == "ASTLocal")
				return move(ob);
		}
		return CheckOb(VectorToken, index, scope, indexFun, SubTable, " parseTypeIdentifierAST return nullptr", move(ob));
	case Token::Words::Identifier:
		//cout << "Identifier" << endl;
		ob = ParseIdentifierAST(VectorToken, index, size, scope, indexFun, SubTable);// выражение
		
		//Возвращаем функцию проверки узла
		if (ob != nullptr && ob->GetStrType() == "int" && ob->GetMethodLength()) return move(ob);
		return CheckOb(VectorToken, index, scope, indexFun, SubTable, " parseIdentifierAST return nullptr", move(ob));
	case Token::Words::CallMethodId:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		ob = ParseIdentifierAST(VectorToken, index, size, scope, indexFun, SubTable);// выражение
		//if (ob != nullptr && ob->GetStrType() == "int" && ob->GetMethodLength()) return move(ob);
		//Возвращаем функцию проверки узла
		return CheckOb(VectorToken, index, scope, indexFun, SubTable, " parseIdentifierAST return nullptr", move(ob));
	case Token::Words::Binary:
	case Token::Words::Octal:
	case Token::Words::DecimalInteger:
	case Token::Words::Hexadecimal:
	case Token::Words::NotInteger:
	case Token::Words::Symbol:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		return ParseNumberStringAST(VectorToken, index, size, SubTable, scope, 0);// Обработка числа
	case Token::Words::String:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		return ParseNumberStringAST(VectorToken, index, size, SubTable, scope, 1);//Обработка строки
	case Token::Words::LeftParen:
		FlagEqual = 1;//Флаг нужный для проверки корректности операций
		
		return ParseParenAST(VectorToken, index, size, scope, indexFun, SubTable);// Обработка скобок
	default:
		//Любой другой токен, который встретиться в этом блоке вызовет ошибку
		return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " unknown token when expecting an expression");
	}
}

//Функция чтения/обработки блока new
unique_ptr<AST> Parser::KeyWordNew(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable)
{
	GetNextToken(index, size, VectorToken);//Пропуск new

	if (CheckType(VectorToken, index))//Провера наличия типа
	{
		string type = VectorToken[index]->GetString();//Сохраняется тип

		GetNextToken(index, size, VectorToken);//Пропуск типа

		if (VectorToken[index]->GetTypeToken() != Token::Words::LeftSquare) return Correctness(VectorToken, index, size, " exprected '['' after  'id = new type'");

		GetNextToken(index, size, VectorToken);//Пропуск [
		vector<unique_ptr<AST>> Size;//Вектор размеров массива

		while (1) {//Цикл считывания размеров
			/*if (CheckType(VectorToken, index))//Проверка наличия типа
			{ 
				return Correctness(VectorToken, index, size, "Error callfun arguments");
			}
			else */if (auto Arg = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable))
			{
					Size.push_back(move(Arg));
			}

			if (VectorToken[index]->GetTypeToken() == Token::Words::RightSquare) break;//Если встртился ], то конец цикла

			//Если запятой нет, вернется ошибка
			if (VectorToken[index]->GetTypeToken() != Token::Words::Comma) return Correctness(VectorToken, index, size, " expected ')' or ',' in argument list");
			GetNextToken(index, size, VectorToken);//Пропуск ,

		}
		//if (VectorToken[index]->GetTypeToken() != Token::Words::RightSquare) return Correctness(VectorToken, index, size, "Expected ] after 'id = new type[...'");
		
		GetNextToken(index, size, VectorToken); //Пропуск ]

		//Проверка наличия ;
		if (VectorToken[index]->GetTypeToken() != Token::Words::Semicolon) return Correctness(VectorToken, index, size, " expected ';' after 'id = new type[...]'");
		
		return make_unique<ASTArray>(move(LHS), type, move(Size), scope);//Возвращение узла new
	}
	else//В случае отсутствия типа вернется ошибка
		return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " expected 'type' after 'new'");
}

//Функция обработки операций
unique_ptr<AST> Parser::ParseBinOpRHS(int ExprPrec, unique_ptr<AST> LHS, vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable)
{
	while (true)
	{
		//Проверка корректности оперций
		if (VectorToken[index]->GetTypeToken() == Token::Words::Assignment && FlagEqual == 1) Correctness(VectorToken, index, size, " incorrect operation");
		int TokPrec = GetTokPrecedence(VectorToken, index, size);//Получение индекса приоритета операции 

		if (TokPrec < ExprPrec)//Проверка наличия операции, если вернулось значение меньше 0, то такой операции нет в таблицы операций
		{
			FlagEqual = 0;//Вспомогательный флаг для проверки корректности операций
			return move(LHS);
		}

		string BinOp = VectorToken[index]->GetString();// Запись операции
		GetNextToken(index, size, VectorToken); // Пропуск операции
		if (TokPrec == 5)//Операция инк/декрементации
		{
			FlagEqual = 0;//Вспомогательный флаг для проверки корректности операций
			LHS = make_unique<ASTOperationBin>(move(LHS), nullptr, BinOp, scope);
			return move(LHS);
		}

		// Проверка на наличие оператора выделения динамической памяти
		if (VectorToken[index]->GetTypeToken() == Token::Words::KeyWordNew && LHS->GetStrType() == "var") return KeyWordNew(0, move(LHS), VectorToken, index, size, scope, indexFun, SubTable);
		
		//Считывание следующего операнда
		auto RHS = ParsePrimary(VectorToken, index, size, scope, indexFun, SubTable);
		if (!RHS)
			return nullptr;

		if (RHS->GetExpressionL() != nullptr)//Проверка инициализации переменной с помощью ввода
		{
			string s = typeid(*RHS->GetExpressionL()).name();
			//cout << s << " " << RHS->GetExpressionL()->GetStrName() << endl;
			if (s == "class ASTCallFun" && RHS->GetExpressionL()->GetStrName() != "ReadLine")
			{
				Correctness(VectorToken, index, size, " use output method in operations");
			}
		}
		
		//Получает следующюю операцию
		int NextPrec = GetTokPrecedence(VectorToken, index, size);
		if (TokPrec < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec + 1, move(RHS), VectorToken, index, size, scope, indexFun, SubTable);//Обработка дальнейших оперций
			if (!RHS) return nullptr;
		}

		LHS = make_unique<ASTOperationBin>(move(LHS), move(RHS), BinOp, scope);//Возвращение узла операций
	}
	FlagEqual = 0;
}

////////////////////////////////////HARD Threre BLOCKS

unique_ptr<AST> Parser::ReadWhile(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов
	GetNextToken(index, size, VectorToken);//Пропуск while

	if (VectorToken[index]->GetTypeToken() != Token::Words::LeftParen) return Correctness(VectorToken, index, size, " expected '(' after 'while'");	
	GetNextToken(index, size, VectorToken);//Пропуск (

	//Чтение/обработка выражения в скобках
	auto Compare = ReadExpression(VectorToken, index, size, scope + "_" + to_string(indexFun), indexFun, SubTable);
	if (Compare == nullptr) return nullptr;// Проверка корректности завершения обработки

	if (VectorToken[index]->GetTypeToken() != Token::Words::RightParen) return Correctness(VectorToken, index, size, " expected ')' after 'while(...'");
	GetNextToken(index, size, VectorToken);//Пропуск )

	if (VectorToken[index]->GetTypeToken() != Token::Words::LeftCurly) return Correctness(VectorToken, index, size, " expected '{' after 'while(expr)'");
	GetNextToken(index, size, VectorToken);//Пропуск {
	
	//Чтение/обработка тела while
	auto whilebody = ReadFunBody(VectorToken, index, size, scope + "_" + to_string(indexFun), SubTable);

	//Проверка корректности завершения обработки тела while
	if (VectorToken[index]->GetTypeToken() != Token::Words::RightCurly) return Correctness(VectorToken, index, size, " expected '}' after 'while(expr){...'");
	GetNextToken(index, size, VectorToken);//Пропуск }

	return make_unique<ASTWhile>(move(Compare), move(whilebody), scope);
}

unique_ptr<AST> Parser::ReadElse(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов
	GetNextToken(index, size, VectorToken);//Пропуск }

	//Если встретился else, обрабатывается и привязывается к узлу if
	if (VectorToken[index]->GetTypeToken() != Token::Words::KeyWordElse) return nullptr;

	GetNextToken(index, size, VectorToken);//Пропуск else

	if (VectorToken[index]->GetTypeToken() == Token::Words::LeftCurly)
	{
		GetNextToken(index, size, VectorToken);//Пропуск {

		//Чтение/обработка тела else
		auto elsefunbody = ReadFunBody(VectorToken, index, size, scope + "_" + to_string(indexFun), SubTable);

		//Проверка корректности завершения тела else
		if (VectorToken[index]->GetTypeToken() != Token::Words::RightCurly)  return Correctness(VectorToken, index, size, " expected '}' after 'else{...'");

		GetNextToken(index, size, VectorToken);//Пропуск }

		return make_unique<ASTElse>(move(elsefunbody), scope + "_" + to_string(indexFun));
	}
	return Correctness(VectorToken, index, size, " expected '{' after 'else'");
}

unique_ptr<AST> Parser::ReadIf(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int &indexFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов
	GetNextToken(index, size, VectorToken);//Пропуск if

	//Проверка наличия (
	if (VectorToken[index]->GetTypeToken() != Token::Words::LeftParen) return Correctness(VectorToken, index, size, " expected '(' after 'if'");
	GetNextToken(index, size, VectorToken);//Пропуск (

	auto Compare = ReadExpression(VectorToken, index, size, scope + "_" + to_string(indexFun), indexFun, SubTable);//Чтение/обработка выражения в скобках
	if (Compare == nullptr) return nullptr;//Проверка корректности обработки

	//Проверка наличия )
	if (VectorToken[index]->GetTypeToken() != Token::Words::RightParen) return Correctness(VectorToken, index, size, " expected ')' after 'if(...'");
	GetNextToken(index, size, VectorToken);//Пропуск )

	//Проверка наличия {
	if (VectorToken[index]->GetTypeToken() != Token::Words::LeftCurly) return Correctness(VectorToken, index, size, " expected '{' after 'if(expr)'");
	GetNextToken(index, size, VectorToken);//Пропуск {

	//Чтение и обработка тела if
	auto ifbody = ReadFunBody(VectorToken, index, size, scope + "_" + to_string(indexFun), SubTable);

	//Проверка корректности завершения блока if
	if (VectorToken[index]->GetTypeToken() != Token::Words::RightCurly) return Correctness(VectorToken, index, size, " expected '}' after 'if(expr){...'");

	auto elsebody = ReadElse(VectorToken, index, size, scope, 1 + indexFun);//Проверка наличия else(обработка)
	if (elsebody != nullptr)
	{
		indexFun++;
		return make_unique<ASTIf>(move(Compare), move(ifbody), move(elsebody), scope);
	}
	return make_unique<ASTIf>(move(Compare), move(ifbody), scope);
}

unique_ptr<AST> Parser::ReadFunLocal(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTableFun)
{
	vector<ScopeVar> SubTable;//lvl таблицы символов

	auto fun = ReadFun(VectorToken, index, size, scope + "_" + to_string(0));//Чтение/обработка типа, id и аргументов локальной функции

	if (fun == nullptr) return Correctness(VectorToken, index, size, "");//Проверка корректности завершения обработки блока

	//Проверка наличия id фун-ии в таблице символов на соответствующем уровне
	string str = CheckDeclaretion(scope, fun->GetStrName(), fun->GetStrType(), fun, SubTableFun);
	//cout << "ReadFunLocal" << " " << fun->GetStrName() << " " << fun->GetStrType() << endl;

	//Возвращение ошибки в случаи, если имеются id на данном уровне с таким же именем
	if (str == fun->GetStrName() + " has definition at this lvl") return Correctness(VectorToken, index, size, str);
	fun->SetStrType(str);// Занесение типа в узел

	//Проверка наличия {
	if (VectorToken[index]->GetTypeToken() != Token::Words::LeftCurly)  return Correctness(VectorToken, index, size, " expected '{' after 'static type id (...)'");

	//Аргументы функции заносятся в таблицу символов
	for (int i = 0; i < fun->GetArgs().size(); i++)
		if (fun->GetArgs()[i] != nullptr)
		{
			SubTable.push_back(ScopeVar(fun->GetArgs()[i].get(), scope + "_" + to_string(indexFun)));
			Table[scope + "_" + to_string(indexFun)] = SubTable;
		}

	GetNextToken(index, size, VectorToken);// Пропуск {

	auto funbody = ReadFunBody(VectorToken, index, size, scope + "_" + to_string(indexFun), SubTable);// Чтение/обработка тела локальной функции
	if (FlagError == 1) return Correctness(VectorToken, index, size, "");//Проверка корректности завершения обработки блока

	//Проверка корректности завершения локальной фун-ии
	if (VectorToken[index]->GetTypeToken() != Token::Words::RightCurly) return Correctness(VectorToken, index, size, " expected '}' in end fun");

	GetNextToken(index, size, VectorToken);//Пропуск }

	//cout << "end read fun local" << endl;
	return make_unique<ASTLocal>(move(fun), move(funbody), scope);//Возвращение узла локальной функции
}

unique_ptr<AST> Parser::ReadSubBlock(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun)
{
	vector<ScopeVar> SubTable;// lvl таблицы символов
	GetNextToken(index, size, VectorToken);// Пропуск {

	auto SubBlockBody = ReadFunBody(VectorToken, index, size, scope + "_" + to_string(indexFun), SubTable);//Чтение/обработка тела подблока
	if (FlagError == 1) return Correctness(VectorToken, index, size, "");//Проперка успешности чтения/обработки тела подблока

	//Проверка корректности завершения подблока
	if (VectorToken[index]->GetTypeToken() != Token::Words::RightCurly) return Correctness(VectorToken, index, size, " expected '}' after 'SubBlock{...'");

	GetNextToken(index, size, VectorToken);//Пропуск }

	return make_unique<ASTSubBlock>(move(SubBlockBody), scope);//Возвращение узла подблока
}

unique_ptr<AST> Parser::ReadReturn(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexFun, vector<ScopeVar> &SubTable)
{
	GetNextToken(index, size, VectorToken);//Пропуск return

	auto ob = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable);//Обработка возвращаемого выражения
	if (ob == nullptr) return LogError(0, 0, "");//Если return;, возвращается соответствующая версия узла return

	//Отсутствие ;, возвращеется ошибка
	if (VectorToken[index]->GetTypeToken() != Token::Words::Semicolon) return Correctness(VectorToken, index, size, " expected ';' after 'return...'");
	
	GetNextToken(index, size, VectorToken);//Пропуск ;

	return make_unique<ASTReturn>(move(ob), scope);
}

//Чтение и обработка тела функции
vector<unique_ptr<AST>> Parser::ReadFunBody(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, vector<ScopeVar> &SubTable)
{
	vector<unique_ptr<AST>> bodyfun;// Вектор узлов тела функции
	unique_ptr<AST> ob(new AST());//Вспомогательный узел AST
	int indexFun = 1;//Нужен для scope(lvl)
	string type;

	while (index < size - 1)//Пока не кончатся токены
	{
		switch (VectorToken[index]->GetTypeToken())
		{
		case Token::Words::KeyWordIf://Начала обработки блока if
			ob = ReadIf(VectorToken, index, size, scope, indexFun);
			indexFun++;// lvl++
			if (ob != nullptr)// Проверка успешности чтения обработки блока if
				bodyfun.push_back(move(ob));// Запись в вектор узлов функции
			else
				return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");//Возвращение ошибки в противном случае
			break;
		case Token::Words::KeyWordWhile://Начала обработки блока while
			ob = ReadWhile(VectorToken, index, size, scope, indexFun);
			indexFun++;//lvl++
			if (ob != nullptr)// Проверка успешности чтения обработки блока while
				bodyfun.push_back(move(ob));// Запись в вектор узлов функции
			else
				return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");//Возвращение ошибки в противном случае
			break;
		case Token::Words::Void:
		case Token::Words::Char:
		case Token::Words::Int:
		case Token::Words::Float:
		case Token::Words::Double:
		case Token::Words::Var:
		case Token::Words::string:
		case Token::Words::Bool:
			
			if (CheckType(VectorToken, index + 1))// Проверка на зацикливание
				return CorrectnessVect(VectorToken, index, size, " error 'Type' after 'Type'");
			type = VectorToken[index]->GetString();
			do
			{
				ob = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable); // Запуск чтения и обработки выражения
				indexFun++;//lvl++
				
				if (ob != nullptr)// Проверка успешности чтения обработки блока
					bodyfun.push_back(move(ob));// Запись в вектор узлов функции
				else//Возвращение ошибки в противном случае
					return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");

				//Проверка на корректное завершение чтения выражения
				if (VectorToken[index]->GetTypeToken() != Token::Words::Semicolon && 
					VectorToken[index - 1]->GetTypeToken() != Token::Words::RightCurly &&  
					!(VectorToken[index]->GetTypeToken() == Token::Words::Comma && VectorToken[index + 1]->GetTypeToken() == Token::Words::Identifier))
				{
					//cout << "BINGO!!!" << endl;
					return CorrectnessVect(VectorToken, index, size, " expected ';' after 'expression'");//Возвращение ошибки в противном случае
				}

				if (VectorToken[index]->GetTypeToken() == Token::Words::Comma) 
				{
					GetNextToken(index, size, VectorToken);
					FlagDefenition = type;
				}
			} while (VectorToken[index-1]->GetTypeToken() == Token::Words::Comma);

			//if (VectorToken[index]->GetTypeToken() != Token::Words::Semicolon && VectorToken[index - 1]->GetTypeToken() != Token::Words::RightCurly)
			//	return CorrectnessVect(VectorToken, index, size, "Expected ; after expression");//Возвращение ошибки в противном случае

			break;
		case Token::Words::Identifier:
			ob = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable);// Запуск чтения и обработки выражения
			indexFun++;//lvl++

			if (ob != nullptr)// Проверка успешности чтения обработки блока
				bodyfun.push_back(move(ob));// Запись в вектор узлов функции
			else//Возвращение ошибки в противном случае
				return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");

			//Проверка на корректное завершение чтения выражения
			if (VectorToken[index]->GetTypeToken() != Token::Words::Semicolon)
				return CorrectnessVect(VectorToken, index, size, " expected ';' after 'expression'");

			break;
		case Token::Words::CallMethodId://Чтение и обработка вызова функции
			ob = ReadExpression(VectorToken, index, size, scope, indexFun, SubTable);// Запуск чтения и обработки выражения
			indexFun++;//lvl++

			if (ob != nullptr)// Проверка успешности чтения обработки блока
				bodyfun.push_back(move(ob));// Запись в вектор узлов функции
			else//Возвращение ошибки в противном случае
				return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");

			//Проверка на корректное завершение чтения выражения
			if (VectorToken[index]->GetTypeToken() != Token::Words::Semicolon)
				return CorrectnessVect(VectorToken, index, size, " expected ';' after 'expression'");

			break;
		case Token::Words::KeyWordReturn://Чтение и обработка оператора return
			ob = ReadReturn(VectorToken, index, size, scope, indexFun, SubTable);
			indexFun++;//lvl++

			if (ob != nullptr)// Проверка успешности чтения обработки блока
				bodyfun.push_back(move(ob));// Запись в вектор узлов функции
			else//Возвращение ошибки в противном случае
				return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");

			break;
		case Token::Words::RightCurly://Если встретили }, завершение чтения тела функции, вектор узлов вернется
			//cout << "end fun" << endl;
			return move(bodyfun);
		case Token::Words::Semicolon:
			GetNextToken(index, size, VectorToken);
			break;
		case Token::Words::LeftCurly:// Начала чтения и обработки подблока
			ob = ReadSubBlock(VectorToken, index, size, scope, indexFun);
			indexFun++;//lvl++

			if (ob != nullptr)// Проверка успешности чтения обработки блока
				bodyfun.push_back(move(ob));// Запись в вектор узлов функции
			else//Возвращение ошибки в противном случае
				return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");

			break;
		case Token::Words::Unexpected://Возвращение ошибки, если встретился undexpected
			return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " unexpected token");
		default:// остальные выражения не должны появляться в данном блоке, в случаи их появления, вернется ошибка
			return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " unexpected token");
		}
		//GetNextToken(index, size, VectorToken);
	}
	return move(bodyfun);
}


unique_ptr<AST> Parser::ReadFun(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope)
{
	GetNextToken(index, size, VectorToken);//Пропуск Static

	if (CheckType(VectorToken, index))// Проверка наличия типа функции
	{
		string funtype = VectorToken[index]->GetString();// Запись типа функции
		GetNextToken(index, size, VectorToken);//Пропуск типа

		//Проверка наличия id функции
		if (VectorToken[index]->GetTypeToken() != Token::Words::Identifier) return CorrectnessOpr(VectorToken, index, size, " expected 'id' after 'static type'");

		string funname = VectorToken[index]->GetString();// Запись id фун-ии
		GetNextToken(index, size, VectorToken);//Пропуск id

		//Проверка наличия (
		if (VectorToken[index]->GetTypeToken() != Token::Words::LeftParen) return CorrectnessOpr(VectorToken, index, size, " expected '(' after 'static type id'");

		GetNextToken(index, size, VectorToken);//Пропуск (
		vector<unique_ptr<AST>> args;// Вектор для хранения аргументов фунции

		while (CheckType(VectorToken, index))//Проверка наличия типа аргумента
		{
			string type = VectorToken[index]->GetString();//Запись типа аргумента
			GetNextToken(index, size, VectorToken);//Пропуск типа

			//Проверка наличия id
			if (VectorToken[index]->GetTypeToken() != Token::Words::Identifier)  return CorrectnessOpr(VectorToken, index, size, "fun arguments: expected 'id' after 'type'");
			//Запись считанного аргумента в вектор аргуметнов
			args.push_back(make_unique<ASTIdentifier>(VectorToken[index]->GetString(), type, scope));
			GetNextToken(index, size, VectorToken);// Пропуск id

			//Проверка корректности
			if (VectorToken[index]->GetTypeToken() == Token::Words::Comma)
			{
				GetNextToken(index, size, VectorToken);// Пропуск ,

				//Проверка наличия типа после ,
				if (!CheckType(VectorToken, index)) return CorrectnessOpr(VectorToken, index, size, " error fun arguments");
			}//Если встречается ), чтение аргументов завершается
			else if (VectorToken[index]->GetTypeToken() == Token::Words::RightParen) continue;
			else return CorrectnessOpr(VectorToken, index, size, " error fun arguments");// Если ни запятой, ни ) не встретилось вернет ошибку
		}

		//if (VectorToken[index]->GetTypeToken() != Token::Words::RightParen)  return CorrectnessOpr(VectorToken, index, size, "Expected ) after 'static type id(...'");
		//Создание узла AST
		auto fun = make_unique<ASTFun>(funname, funtype, move(args), scope.substr(0, scope.size() - 2));
		GetNextToken(index, size, VectorToken);// Пропуск )
		return move(fun);//Возвращение обработанного узла
	}
	else //Вернет ошибку в случае отсутствия типа функции
		return CorrectnessOpr(VectorToken, index, size, " expected 'type' after 'static'");
}

//Функция чтения и обработки тела класса 
vector<unique_ptr<AST>> Parser::ReadBodyClass(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexClass)
{
	vector<unique_ptr<AST>> Vector;
	int indexFun = 1;// Нужен для scope

	vector<ScopeVar> SubTableFun;

	GetNextToken(index, size, VectorToken);// Пропуск id class

	if (VectorToken[index]->GetTypeToken() != Token::Words::LeftCurly) return CorrectnessVect(VectorToken, index, size, " expected '{' after 'class id'");// Проверка {

	GetNextToken(index, size, VectorToken);// Пропуск {


	while (VectorToken[index]->GetTypeToken() == Token::Words::KeyWordStatic || VectorToken[index]->GetTypeToken() == Token::Words::Public)// Условие начала чтения и обработки статической функции
	{
		if (VectorToken[index]->GetTypeToken() == Token::Words::Public)GetNextToken(index, size, VectorToken);// Пропуск public

		vector<ScopeVar> SubTable;// Таблица символов

		auto fun = ReadFun(VectorToken, index, size, scope + to_string(indexClass) + "_" + to_string(indexFun));// Запуск чтения/обработки имени, типа и аргументов функции
		if (fun == nullptr)  return CorrectnessVect(VectorToken, index, size, "");// Проверка успешности чтения функции

		string str = CheckDeclaretion(scope + to_string(indexClass), fun->GetStrName(), fun->GetStrType(), fun, SubTableFun);// Проверка наличия такого же id(имя функции) в таблице символов

		//Если уже имеется статическая функция с таким же именем, то вернет ошибку
		if (/*str == fun->GetStrName() + " without definition" || */str == fun->GetStrName() + " has definition at this lvl") return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), str); 
		fun->SetStrType(str);// Запись типа в узел

		//Начала обработки тела функции
		if (VectorToken[index]->GetTypeToken() != Token::Words::LeftCurly) return CorrectnessVect(VectorToken, index, size, " expected '{' after static type id (...)");

		//Аргументы функции заносятся на соответствующий lvl таблицы символов
		for (int i = 0; i < fun->GetArgs().size(); i++)
			if (fun->GetArgs()[i] != nullptr)
			{
				SubTable.push_back(ScopeVar(fun->GetArgs()[i].get(), to_string(indexClass) + "_" + to_string(indexFun)));
				Table[to_string(indexClass) + "_" + to_string(indexFun)] = SubTable;
			}
		
		GetNextToken(index, size, VectorToken);// Пропуск {

		//Чтение и обработка тела функции
		auto funbody = ReadFunBody(VectorToken, index, size, to_string(indexClass) + "_" + to_string(indexFun), SubTable);

		//Проверка успешного завершения чтения тела функции
		if (FlagError == 1) return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), "");

		// Завершение тела функции }
		if (VectorToken[index]->GetTypeToken() != Token::Words::RightCurly) return CorrectnessVect(VectorToken, index, size, " expected '}' in end fun");
			
		GetNextToken(index, size, VectorToken);//Пропуск }

		//Определяется узел статической функции и помещается в вектор тела класса
		unique_ptr<ASTStatic> StaticFun(new ASTStatic(move(fun), move(funbody)));
		Vector.push_back(move(StaticFun));
		indexFun++;//Вычисление индекса следующей функции
	} 

	if (VectorToken[index]->GetTypeToken() != Token::Words::RightCurly)  return CorrectnessVect(VectorToken, index, size, " expected '}' in end class");

	//GetNextToken(index, size, VectorToken);//пропуск }
	return move(Vector);
}

//Функция чтения класса
unique_ptr<AST> Parser::ReadClass(vector<unique_ptr<Token>> const &VectorToken, int &index, int size, string scope, int indexClass)
{
	GetNextToken(index, size, VectorToken);// Пропуск class
	if (VectorToken[index]->GetTypeToken() == Token::Words::Identifier)
	{
		string ClassName = VectorToken[index]->GetString();//Имя класса
		auto Class = make_unique<ASTClass>(ClassName);// Узел класса

		auto ClassNodesVector = ReadBodyClass(VectorToken, index, size, scope, indexClass);// Вектор узлов тела класса

		if (Class == nullptr || FlagError == 1) return Correctness(VectorToken, index, size, "");// Если произошла ошибка вернется Correctness 

		Class->AddVectorChildren(move(ClassNodesVector));// Привязывается вектор узлов тела класса к узлу соответствующего класса
		//cout << "index = " << index << endl;
		return move(Class);
	}
	
	return LogError(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " expected 'id' after class");
}

unique_ptr<AST> Parser::ReadUsing(vector<unique_ptr<Token>> const &VectorToken, int &index, int size)
{
	GetNextToken(index, size, VectorToken);//Пропуск using

	if (VectorToken[index]->GetTypeToken() != Token::Words::Library) return Correctness(VectorToken, index, size, " expected 'library' after 'using'");

	string nameusing = VectorToken[index]->GetString();// Запись id библиотеки
	GetNextToken(index, size, VectorToken);//Пропуск id

	if (VectorToken[index]->GetTypeToken() != Token::Words::Semicolon) return Correctness(VectorToken, index, size, " expected ';' after 'using library'");

	return make_unique<ASTUsing>(nameusing);
}

vector<unique_ptr<AST>> Parser::BuildAST(vector<unique_ptr<Token>> const &VectorToken, int size) {
	int index = 0;
	int indexClass = 1;
	bool Move = true;
	vector<unique_ptr<AST>> RootVector;
	unique_ptr<AST> ob(new AST());

	while (Move) 
	{
		switch (VectorToken[index]->GetTypeToken()) 
		{
		case Token::Words::End:
			return move(RootVector);
		case Token::Words::Class:
			ob = ReadClass(VectorToken, index, size, "", indexClass);
			indexClass++;
			if (ob != nullptr)
				RootVector.push_back(move(ob));
			else
				return move(RootVector);

			break;
		case Token::Words::KeyWordUsing:
			ob = ReadUsing(VectorToken, index, size);
			if (ob != nullptr)
				RootVector.push_back(move(ob));
			else
				return move(RootVector);

			break;
		case Token::Words::Public:
			ob = ReadClass(VectorToken,	++index, size, "", indexClass);
			indexClass++;
			if (ob != nullptr)
				RootVector.push_back(move(ob));
			else
				return move(RootVector);

			break;
		default:
			return LogErrorVector(VectorToken[index]->GetStrNum(), VectorToken[index]->GetColNum(), " unexpected Token for mainloop: " + VectorToken[index]->GetString());
			//cerr << "Unexpected Token for mainloop" << endl;
			//break;
		}
		Move = GetNextToken(index, size, VectorToken);
	}
	return move(RootVector);
}

void Parser::Parsing(vector<unique_ptr<Token>> const &VectorToken, int size, int option)
{
	auto Vect = BuildAST(VectorToken, size);
	unique_ptr<AST> R(new ASTRoot(move(Vect)));
	Root = move(R);

	if (FlagError == 0 && option)
	{
		Root->print(2);
		//PrintTable();
	}
}

string Parser::CheckIdName(vector<ScopeVar> &it, string name)
{
	if (!it.empty())
	{
		for (int i = 0; i < it.size(); i++)
			if (it[i].getNode() != nullptr && it[i].getNode()->GetStrName() == name) 
				return it[i].getNode()->GetStrType();
	}
	return "";
}

string Parser::CheckIdTableSymbol(string name, string scope, string type)
{
	map<string, vector<ScopeVar>>::iterator it = Table.begin();
	for (; it != Table.end(); it++)
	{
		if (type != "unssigned")
		{
			if (scope == it->first) return CheckIdName(it->second, name);	
		}
		else
		{
			int i = it->first.size();
			if (scope.substr(0, i) == it->first.substr(0, it->first.size()))
			{
				string temp;
				temp = CheckIdName(it->second, name);
				if (temp != "") return temp;
			}
		}
	}

	return "";
}

string Parser::CheckDeclaretion(string scope, string name, string type, unique_ptr<ASTIdentifier> const &tree, vector<ScopeVar> &SubTable)
{
	string temp = "";
	
	if (type == "unssigned")
	{
		if ((type = CheckIdTableSymbol(name, scope, type)) == "") return (name + " without definition");
	}
	else
	{
		if ((temp = CheckIdTableSymbol(name, scope, type)) != "") return (name + " has definition at this lvl");

		SubTable.push_back(ScopeVar(tree.get(), scope));
		Table[scope] = SubTable;
	}
	return type;
}

string Parser::CheckDeclaretion(string scope, string name, string type, unique_ptr<AST> const &tree, vector<ScopeVar> &SubTable)
{
	string temp = "";
	
	if (type == "unssigned")
	{
		if ((type = CheckIdTableSymbol(name, scope, type)) == "") return (name + " without definition");
	}
	else
	{
		if ((temp = CheckIdTableSymbol(name, scope, type)) != "") return (name + " has definition at this lvl");

		SubTable.push_back(ScopeVar(tree.get(), scope));
		Table[scope] = SubTable;
	}
	return type;
}

void RedactionString(string &str)
{
	for (int i = 0; i < str.size(); i++)
	{
		if ('0' <= str[i] && str[i] <= '9')
		{
			for (int j = i; j < str.size() - 1; j++)
				str[j] = str[j + 1];
			str.pop_back();
			i--;
		}
	}
}

void Parser::PrintTable()
{
	cout << endl << "SymbolTable:" << endl;
	map<string, vector<ScopeVar>>::iterator it = Table.begin();
	for (; it != Table.end(); it++)
	{
		cout << it->first << ": ";
		if (!it->second.empty())
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i].getNode() != nullptr)
				{
					string str = typeid(*(it->second[i]).getNode()).name();
					RedactionString(str);
					cout << it->second[i].getNode()->GetStrName();
					cout << "(" << str << ")";
					cout << " ";
				}
			}
		cout << endl;
	}
	cout << endl;
}