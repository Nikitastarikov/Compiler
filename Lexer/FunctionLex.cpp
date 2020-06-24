//#pragma once
#include "Lexer.h"

using namespace std;

std::ostream& operator << (std::ostream& os, const Token::Words& kind) 
{
	static const char* const names[]
	{
	    "Identifier",    "LeftParen",     "RightParen",  "LeftSquare",   "RightSquare",    "LeftCurly",     
		"RightCurly",    "LessThan",      "GreaterThan", "Equal",        "Plus",           "Minus",         
		"Asterisk",      "Slash",         "Percent",     "Semicolon",    "DoubleQuote",    "Comma",         
		"String",        "Double",        "Int",         "Float",         "Char",          "Bool",         
		"Var",           "Void",          "string",      "Library",        "End",        "Unexpected",
		"Assignment",   "KeyWordIf",   "KeyWordElse",  "KeyWordWhile",  "KeyWordNew",    "Negation",
		"Conjunction",  "Disjunction", "False",        "True",          "NotEqual",      "Increment",  
		"Decrement",    "LessOrEqual", "MoreOrEqual",  "AddAssign",     "SubAssign",     "MultAssign",
		"DivAssign",     "Null",        "KeyWordUsing", "KeyWordReturn", "KeyWordStatic", "Class",      
		"CallMethodId",	"Binary",   	"Octal", 		"Hexadecimal", 	"DecimalInteger", "Point",
		"NotInteger", 	"Public",       "Symbol"
 	};
	return os << names[static_cast<int>(kind)];
}


void Lexer::CharacterReadModeTwo(string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size)
{
	unique_ptr<Token> TokenObject(new Token());
	unique_ptr<Token> doubleduote(new Token());
	int SizeSubLine = 1;
	int TemporaryIdentifierString = IdentifierString + 1;

	while (Line[TemporaryIdentifierString] != '"' && TemporaryIdentifierString < Size - 1)
	{
		TemporaryIdentifierString++;
		SizeSubLine++;
	}

	if (Line[TemporaryIdentifierString] == '"')
	{
		TokenObject->SetTypeToken(Token::Words::String);
		TokenObject->SetString(Line.substr(IdentifierString, SizeSubLine + 1));
		//cout << "Loc<" << LineNumber << ":" << (TemporaryIdentifierString + 1) << "> " << doubleduote->GetTypeToken() << " '" << doubleduote->GetString() << "'" << endl;
	}
	else
	{
		TokenObject->SetTypeToken(Token::Words::Unexpected);
		TokenObject->SetString(Line.substr(IdentifierString, SizeSubLine));
	}
	
	TokenObject->SetStrNumber(LineNumber);
	TokenObject->SetColNumber(IdentifierString + 1);
	//cout << "Loc<" << LineNumber << ":" << (IdentifierString + 1) << "> " << TokenObject->GetTypeToken() << " '" << TokenObject->GetString() << "'" << endl;
	IdentifierString = TemporaryIdentifierString;
	CharacterReadMode = 0;

	if (TokenObject != nullptr && TokenObject->GetString() != "")
		VectorToken.push_back(move(TokenObject));
}

void Lexer::UnexpectedToken(string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size)
{
	unique_ptr<Token> TokenObject(new Token());
	int SizeSubLine = 1;
	int TemporaryIdentifierString = IdentifierString;
	while (TemporaryIdentifierString < Size - 1 && !IsSpace(Line[TemporaryIdentifierString]) && Line[TemporaryIdentifierString] != ';')
	{
		TemporaryIdentifierString++;
		SizeSubLine++;
	}

	TokenObject->SetTypeToken(Token::Words::Unexpected);
	TokenObject->SetStrNumber(LineNumber);
	TokenObject->SetColNumber(IdentifierString + 1);
	TokenObject->SetString(Line.substr(IdentifierString, SizeSubLine - 1));

	if (TokenObject != nullptr && TokenObject->GetString() != "")
	{
		//cout << "Loc<" << LineNumber << ":" << TokenObject->GetColNum() << "> " << TokenObject->GetTypeToken() << " '" << TokenObject->GetString() << "'" << endl;
		VectorToken.push_back(move(TokenObject));
	}
	IdentifierString = TemporaryIdentifierString - 1;// отступает на индекс назад, чтобы корректно прочитать следующий токен
}

void Lexer::CheckOb(unique_ptr<Token> TokenObject, string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size)
{
	if (TokenObject != nullptr && TokenObject->GetString() != "")
	{
		VectorToken.push_back(move(TokenObject));
	}
	else
	{ 
		UnexpectedToken(Line, LineNumber, VectorToken, IdentifierString, Size);
	}
}

void Lexer::GetNextToken(string Line, int LineNumber, int EndFileId, vector<unique_ptr<Token>> &VectorToken) 
{
	
	int IdentifierString = 0;//Идентификатор строки
	int Size = (int)Line.size();//Получаем размер строки, которую собираемся обрабатывать
	CharacterReadMode = 0;// Флаг для проверки наличия ковычек

	if (Line == "")
	{
		unique_ptr<Token> TokenObject(new Token());
		TokenObject->SetTypeToken(Token::Words::Unexpected);
		TokenObject->SetStrNumber(LineNumber);
		TokenObject->SetColNumber(1);
		TokenObject->SetString("");
		VectorToken.push_back(std::move(TokenObject));
	}

	if (EndFileId)// Проверка конца файла
	{
		unique_ptr<Token> TokenObject(new Token());
		TokenObject->SetTypeToken(Token::Words::End);
		TokenObject->SetString("\0");
		TokenObject->SetStrNumber(LineNumber);
		TokenObject->SetColNumber(1);
		//cout << "Loc<" << LineNumber << ":" << 1 << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
		VectorToken.push_back(std::move(TokenObject));
	}
	else 
	{
		//cout << LineNumber << " line size = " << Line.size() << endl;
		while (IdentifierString < Size) 
		{
			while (CharacterReadMode != 2 && IsSpace(Line[IdentifierString])) IdentifierString++;

			if (CharacterReadMode == 2)
			{
				IdentifierString--;
				CharacterReadModeTwo(Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else if (strchr("+-/*%&|=()[]{}<>;,.", Line[IdentifierString])) 
			{
				auto TokenObject = ReadOperator(Line, LineNumber, Size, IdentifierString);
				if (TokenObject != nullptr && TokenObject->GetString() != "")
					VectorToken.push_back(move(TokenObject));
			}
			else if (Size - 1 == IdentifierString);
			else if (IsLetter(Line[IdentifierString])) 
			{
				auto TokenObject = ReadString(Line, LineNumber, Size, IdentifierString);
				CheckOb(move(TokenObject), Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else if (IsDigit(Line[IdentifierString])) 
			{
				auto TokenObject = ReadNumber(Line, LineNumber, IdentifierString, Size);
				CheckOb(move(TokenObject), Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else if (Line[IdentifierString] == '\"') 
			{ 
				CharacterReadMode = 2; 
			}
			else if (Line[IdentifierString] == '\'')
			{
				auto TokenObject = IsSymbol(Line, LineNumber, IdentifierString, Size);
				CheckOb(move(TokenObject), Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else
			{
				UnexpectedToken(Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			IdentifierString++;
		}
	}
	//cout << "size = " << VectorToken.size() << endl;
}

unique_ptr<Token> IsSymbol(string &Line, int &LineNumber, int &Identifier, int LineSize)
{
	unique_ptr<Token> TokenObject(new Token());
	int SizeSubLine = 3, IndexTemp;

	if (Identifier + 2 > LineSize - 1)
	{
		return nullptr;
	}
	else if (Line[Identifier + 2] != '\'')
	{
		return nullptr;
	}
	else
	{
		IndexTemp = Identifier + 2; 	
		TokenObject->SetTypeToken(Token::Words::Symbol);
		TokenObject->SetString(Line.substr(Identifier, SizeSubLine));
	}

	TokenObject->SetStrNumber(LineNumber);
	TokenObject->SetColNumber(Identifier + 1);
	//cout << "Loc<" << LineNumber << ":" << (Identifier + 1) << "> " << TokenObject->GetTypeToken() << " '" << TokenObject->GetString() << "'" << endl;

	Identifier = IndexTemp;
	return move(TokenObject);
}

int IsSpace(char Symbol)
{
	return isspace(Symbol);
}

int IsLetter(char Symbol) 
{
	if (('A' <= Symbol && Symbol <= 'Z') || ('a' <= Symbol && Symbol <= 'z') || Symbol == '_') {
		return 1;
	}
	return 0;
}

int IsDigit(char Symbol)
{
	return isdigit(Symbol);
}

bool IsBinary(string const &Line, int &LineIndex, int &SizeSubLine, int LineSize)
{
	int TemporaryIdentifier = LineIndex;
	int TempSizeSubLine = SizeSubLine;
	int Correctness = 1;

	if (LineSize - LineIndex < 2)
		return false;

	if (Line[TemporaryIdentifier - 1] != '0')
		Correctness = 0;

	if (Line[TemporaryIdentifier] != 'b' && Line[TemporaryIdentifier] != 'B')
		Correctness = 0;

	if (IsSpace(Line[TemporaryIdentifier]) || !Correctness)
		return false;

	TemporaryIdentifier++;
	TempSizeSubLine++;

	while (TemporaryIdentifier < LineSize - 1 && strchr("_01", Line[TemporaryIdentifier])) {
		TemporaryIdentifier++;
		TempSizeSubLine++;
	}

	if (TemporaryIdentifier < LineSize - 1)
	{

		if (!IsLetter(Line[TemporaryIdentifier]))
		{
			LineIndex = TemporaryIdentifier;
			SizeSubLine = TempSizeSubLine;
			return true;
		}
		else
			return false;
	}
	else if (TemporaryIdentifier == LineSize - 1 && IsDigit(Line[TemporaryIdentifier]))
	{
		LineIndex = TemporaryIdentifier;
		SizeSubLine = TempSizeSubLine + 1;
		return true;
	}

	LineIndex = TemporaryIdentifier;
	SizeSubLine = TempSizeSubLine;
	return true;
}

bool IsOctal(string &Line, int &LineIndex, int &SizeSubLine, int LineSize)
{
	
	int TemporaryIdentifier = LineIndex;
	int TempSizeSubLine = SizeSubLine;
	int Correctness = 1;

	if (LineSize - LineIndex < 1)
		return false;

	if (Line[TemporaryIdentifier - 1] != '0' || !strchr("1234567", Line[TemporaryIdentifier]))
		Correctness = 0;

	if (IsSpace(Line[TemporaryIdentifier]) || !Correctness)
		return false;

	while (strchr("01234567", Line[TemporaryIdentifier]) && TemporaryIdentifier < LineSize - 1) {
		TemporaryIdentifier++;
		TempSizeSubLine++;
	}

	if (TemporaryIdentifier < LineSize - 1)
	{

		if (!IsLetter(Line[TemporaryIdentifier]))
		{
			LineIndex = TemporaryIdentifier;
			SizeSubLine = TempSizeSubLine;
			return true;
		}
		else
			return false;
	}
	else if (TemporaryIdentifier == LineSize - 1 && IsDigit(Line[TemporaryIdentifier]))
	{
		LineIndex = TemporaryIdentifier;
		SizeSubLine = TempSizeSubLine + 1;
		return true;
	}

	LineIndex = TemporaryIdentifier;
	SizeSubLine = TempSizeSubLine;
	return true;
}

bool IsHexadecimal(string &Line, int &LineIndex, int &SizeSubLine, int LineSize)
{
	
	int TemporaryIdentifier = LineIndex;
	int TempSizeSubLine = SizeSubLine;
	int Correctness = 1;

	if (LineSize - LineIndex < 2)
		return false;

	if (Line[TemporaryIdentifier - 1] != '0')
		Correctness = 0;
	
	if (Line[TemporaryIdentifier] != 'x' && Line[TemporaryIdentifier] != 'X')
		Correctness = 0;
	
	if (IsSpace(Line[TemporaryIdentifier]) || !Correctness)
		return false;

	TemporaryIdentifier++;
	TempSizeSubLine++;

	while (strchr("0123456789abcdefABCDEF", Line[TemporaryIdentifier]) && TemporaryIdentifier < LineSize - 1) {
		TemporaryIdentifier++;
		TempSizeSubLine++;
	}

	if (TemporaryIdentifier < LineSize - 1)
	{
		if (!IsLetter(Line[TemporaryIdentifier]))
		{
			LineIndex = TemporaryIdentifier;
			SizeSubLine = TempSizeSubLine;
			return true;
		}
		else
			return false;
	}

	LineIndex = TemporaryIdentifier;
	SizeSubLine = TempSizeSubLine + 1;
	return true;
}

bool IsDecimalInteger(string &Line, int &LineIndex, int &SizeSubLine, int LineSize, bool &NotInteger)
{
	int TemporaryIdentifier = LineIndex;
	int TempSizeSubLine = SizeSubLine;

	if (Line[TemporaryIdentifier] == '.')
	{
		TemporaryIdentifier++;
		TempSizeSubLine++;
		NotInteger = true;

		if (TemporaryIdentifier == LineSize - 1 || !IsDigit(Line[TemporaryIdentifier]))
		{
			return false;
		}
	}

	while (TemporaryIdentifier < LineSize - 1 && IsDigit(Line[TemporaryIdentifier]))
	{
		TemporaryIdentifier++;
		TempSizeSubLine++;

		if (TemporaryIdentifier < LineSize - 1 && Line[TemporaryIdentifier] == '.' && NotInteger == false)
		{
			TemporaryIdentifier++;
			TempSizeSubLine++;
			NotInteger = true;

			if (TemporaryIdentifier == LineSize - 1 || !IsDigit(Line[TemporaryIdentifier]))
			{
				return false;
			}
		}
		else if (TemporaryIdentifier < LineSize - 1 && Line[TemporaryIdentifier] == '.' && NotInteger == true)
		{
			return false;
		}

	}
	
	if (TemporaryIdentifier < LineSize - 1)
	{
		
		if (!IsLetter(Line[TemporaryIdentifier]))
		{
			LineIndex = TemporaryIdentifier;
			SizeSubLine = TempSizeSubLine;
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (TemporaryIdentifier == LineSize - 1 && IsDigit(Line[TemporaryIdentifier]))
	{
		LineIndex = TemporaryIdentifier;
		SizeSubLine = TempSizeSubLine + 1;
		return true;
	}
		
	LineIndex = TemporaryIdentifier;
	SizeSubLine = TempSizeSubLine;
	return true;
}

unique_ptr<Token>  ReadNumber(string &Line, int &LineNumber, int &Identifier, int LineSize) 
{

	int SizeSubLine = 1, TemporaryIdentifier = Identifier + 1;
	unique_ptr<Token> TokenObject(new Token());
	bool NotInteger = false;

	if (IsBinary(Line, TemporaryIdentifier, SizeSubLine, LineSize))
	{ 
		TokenObject->SetTypeToken(Token::Words::Binary);
	}
	else if (IsOctal(Line, TemporaryIdentifier, SizeSubLine, LineSize))
	{ 
		TokenObject->SetTypeToken(Token::Words::Octal);
	}
	else if (IsHexadecimal(Line, TemporaryIdentifier, SizeSubLine, LineSize))
	{
		TokenObject->SetTypeToken(Token::Words::Hexadecimal);
	}
	else if (IsDecimalInteger(Line, TemporaryIdentifier, SizeSubLine, LineSize, NotInteger))
	{
		if (NotInteger)
		{
			TokenObject->SetTypeToken(Token::Words::NotInteger);
		}
		else
		{
			TokenObject->SetTypeToken(Token::Words::DecimalInteger);
		}
	}
	else
	{
		TokenObject->SetTypeToken(Token::Words::Unexpected);
		while (TemporaryIdentifier < LineSize - 1 && !IsSpace(Line[TemporaryIdentifier]))
		{
			TemporaryIdentifier++;
			SizeSubLine++;
		}
		if (!IsSpace(Line[TemporaryIdentifier]))
		{
			SizeSubLine++;
		}
	}

	TokenObject->SetStrNumber(LineNumber);
	TokenObject->SetColNumber(Identifier + 1);
	TokenObject->SetString(Line.substr(Identifier, SizeSubLine));
	//cout << "Loc<" << LineNumber << ":" << (Identifier + 1) << "> " << TokenObject->GetTypeToken() << " '" << TokenObject->GetString() << "'" << endl;
	Identifier = TemporaryIdentifier - 1;

	return move(TokenObject);
}

void ReadWord(int TemporaryIdentifier, unique_ptr<Token> const &TokenObject, string &Line, int SizeSubLine, int &LineSize, int &LineIndex)
{
	if (TokenObject->GetString() == "using") 
	{
		TokenObject->SetTypeToken(Token::Words::KeyWordUsing);
	}
	else if (TokenObject->GetString() == "public") 
	{
		TokenObject->SetTypeToken(Token::Words::Public);
	}
	else if (TokenObject->GetString() == "class") 
	{
		TokenObject->SetTypeToken(Token::Words::Class);
	}
	else if (TokenObject->GetString() == "static") 
	{
		TokenObject->SetTypeToken(Token::Words::KeyWordStatic);
	}
	else if (TokenObject->GetString() == "while") 
	{
		TokenObject->SetTypeToken(Token::Words::KeyWordWhile);
	}
	else if (TokenObject->GetString() == "if") 
	{
		TokenObject->SetTypeToken(Token::Words::KeyWordIf);
	}
	else if (TokenObject->GetString() == "new") 
	{
		TokenObject->SetTypeToken(Token::Words::KeyWordNew);
	}
	else if (TokenObject->GetString() == "return") 
	{
		TokenObject->SetTypeToken(Token::Words::KeyWordReturn);
	}
	else if (TokenObject->GetString() == "else") 
	{
		TokenObject->SetTypeToken(Token::Words::KeyWordElse);
	}
	else if (TokenObject->GetString() == "System") 
	{
		TokenObject->SetTypeToken(Token::Words::Library);
	}
	else if (TokenObject->GetString() == "void") 
	{
		TokenObject->SetTypeToken(Token::Words::Void);
	}
	else if (TokenObject->GetString() == "var") 
	{
		TokenObject->SetTypeToken(Token::Words::Var);
	}
	else if (TokenObject->GetString() == "int") 
	{
		TokenObject->SetTypeToken(Token::Words::Int);
	}
	else if (TokenObject->GetString() == "char") 
	{
		TokenObject->SetTypeToken(Token::Words::Char);
	}
	else if (TokenObject->GetString() == "float") 
	{
		TokenObject->SetTypeToken(Token::Words::Float);
	}
	else if (TokenObject->GetString() == "double") 
	{
		TokenObject->SetTypeToken(Token::Words::Double);
	}
	else if (TokenObject->GetString() == "string") 
	{
		TokenObject->SetTypeToken(Token::Words::string);
	}
	else if (TokenObject->GetString() == "bool") 
	{
		TokenObject->SetTypeToken(Token::Words::Bool);
	}
	else 
	{
		TokenObject->SetTypeToken(Token::Words::Identifier);
		TokenObject->SetString(Line.substr(LineIndex, SizeSubLine));
	}
}

unique_ptr<Token> ReadString(string &Line, int &LineNumber, int &LineSize, int &LineIndex) 
{
	unique_ptr<Token> TokenObject(new Token());
	int SizeSubLine = 1;
	int TemporaryIdentifier = LineIndex + 1;

	if (LineSize == 1)
	{
		TokenObject->SetStrNumber(LineNumber);
		TokenObject->SetColNumber(LineIndex + 1);
		TokenObject->SetString(Line.substr(0, 1));
		TokenObject->SetTypeToken(Token::Words::Identifier);
		return move(TokenObject);
	}

	if (LineSize > TemporaryIdentifier)
	{
		while (TemporaryIdentifier < LineSize - 1 && (IsLetter(Line[TemporaryIdentifier]) || IsDigit(Line[TemporaryIdentifier]))) {
			SizeSubLine++;
			TemporaryIdentifier++;
		}

		if (TemporaryIdentifier == LineSize - 1 && (IsLetter(Line[TemporaryIdentifier]) || IsDigit(Line[TemporaryIdentifier])))
		{
			SizeSubLine++;
		}
		else if (!(strchr("+-/*%&|=()[]{}<>;,.", Line[TemporaryIdentifier]) || Line[TemporaryIdentifier] == ' ' || LineSize - 1 == TemporaryIdentifier))
		{ 
			return nullptr;
		}
	}

	TokenObject->SetString(Line.substr(LineIndex, SizeSubLine));
	ReadWord(TemporaryIdentifier, TokenObject, Line, SizeSubLine, LineSize, LineIndex);

	//cout << "1Loc<" << LineNumber << ":" << (LineIndex + 1) << "> " << TokenObject->GetTypeToken() << " '" << TokenObject->GetString() << "'" << endl;
	TokenObject->SetStrNumber(LineNumber);
	TokenObject->SetColNumber(LineIndex + 1);

	//cout << "2Loc<" << LineNumber << ":" << (LineIndex + 1) << "> " << TokenObject->GetTypeToken() << " '" << TokenObject->GetString() << "'" << endl;

	LineIndex = TemporaryIdentifier - 1;

	if (TemporaryIdentifier == LineSize - 1 && IsLetter(Line[TemporaryIdentifier]))
	{
		LineIndex = TemporaryIdentifier;
	}
	return move(TokenObject);
}

void CheckOperEqual(unique_ptr<Token> const &TokenObject, string &Line, int &LineSize, int &LineIndex, Token::Words TokenAssign, Token::Words TokenOperand)
{
	string Operator;
	if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '=')
	{
		TokenObject->SetTypeToken(TokenAssign);
		Operator = Line[LineIndex];
		Operator += "=";
		
		TokenObject->SetString(Operator);
		LineIndex++;
	}
	else
	{
		TokenObject->SetTypeToken(TokenOperand);
		Operator = Line[LineIndex];
		TokenObject->SetString(Operator);
	}
}

unique_ptr<Token> ReadOperator(string &Line, int &LineNumber, int &LineSize, int &LineIndex) 
{
	unique_ptr<Token> TokenObject(new Token());

	switch (Line[LineIndex]) {
	case '+':
		if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '+')
		{
			TokenObject->SetTypeToken(Token::Words::Increment);
			TokenObject->SetString("++");
			LineIndex++;
		}
		else
		{
			CheckOperEqual(TokenObject, Line, LineSize, LineIndex, Token::Words::AddAssign, Token::Words::Plus);
		}
		break;
	case '-':
		if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '-')
		{
			TokenObject->SetTypeToken(Token::Words::Decrement);
			TokenObject->SetString("--");
			LineIndex++;
		}
		else
		{
			CheckOperEqual(TokenObject, Line, LineSize, LineIndex, Token::Words::SubAssign, Token::Words::Minus);
		}
		break;
	case '/':
		if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '/')
		{
			LineIndex = LineSize;
			return nullptr;
		}
		else if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '=')
		{
			TokenObject->SetTypeToken(Token::Words::DivAssign);
			TokenObject->SetString("/=");
			LineIndex++;
		}
		else
		{
			TokenObject->SetTypeToken(Token::Words::Slash);
			TokenObject->SetString("/");
		}
		break;
	case '*':
		CheckOperEqual(TokenObject, Line, LineSize, LineIndex, Token::Words::MultAssign, Token::Words::Asterisk);
		break;
	case '%':
		TokenObject->SetTypeToken(Token::Words::Percent);
		TokenObject->SetString("%");
		break;
	case '=':
		CheckOperEqual(TokenObject, Line, LineSize, LineIndex, Token::Words::Equal, Token::Words::Assignment);
		break;
	case '(':
		TokenObject->SetTypeToken(Token::Words::LeftParen);
		TokenObject->SetString("(");
		break;
	case ')':
		TokenObject->SetTypeToken(Token::Words::RightParen);
		TokenObject->SetString(")");
		break;
	case '[':
		TokenObject->SetTypeToken(Token::Words::LeftSquare);
		TokenObject->SetString("[");
		break;
	case ']':
		TokenObject->SetTypeToken(Token::Words::RightSquare);
		TokenObject->SetString("]");
		break;
	case '{':
		TokenObject->SetTypeToken(Token::Words::LeftCurly);
		TokenObject->SetString("{");
		break;
	case '}':
		TokenObject->SetTypeToken(Token::Words::RightCurly);
		TokenObject->SetString("}");
		break;
	case '.':
		TokenObject->SetTypeToken(Token::Words::Point);
		TokenObject->SetString(".");
		break;
	case ';':
		TokenObject->SetTypeToken(Token::Words::Semicolon);
		TokenObject->SetString(";");
		break;
	case '<':
		CheckOperEqual(TokenObject, Line, LineSize, LineIndex, Token::Words::LessOrEqual, Token::Words::LessThan);
		break;
	case '>':
		CheckOperEqual(TokenObject, Line, LineSize, LineIndex, Token::Words::MoreOrEqual, Token::Words::GreaterThan);
		break;
	case ',':
		TokenObject->SetTypeToken(Token::Words::Comma);
		TokenObject->SetString(",");
		break;
	case '&':
		if (LineIndex + 1 < LineSize &&Line[LineIndex + 1] == '&')
		{
			TokenObject->SetTypeToken(Token::Words::Conjunction);
			TokenObject->SetString("&&");
			LineIndex++;
		}
		else
		{
			TokenObject->SetTypeToken(Token::Words::Unexpected);
			TokenObject->SetString("&");
		}
		break;
	case '|':
		if (LineIndex + 1 < LineSize &&Line[LineIndex + 1] == '|')
		{
			TokenObject->SetTypeToken(Token::Words::Disjunction);
			TokenObject->SetString("||");
			LineIndex++;
		}
		else
		{
			TokenObject->SetTypeToken(Token::Words::Unexpected);
				TokenObject->SetString("|");
		}
		break;
	}
	TokenObject->SetStrNumber(LineNumber);
	TokenObject->SetColNumber(LineIndex + 1);	
	//cout << "Loc<" << LineNumber << ":" << (LineIndex + 1) << "> " << TokenObject->GetTypeToken() << " '" << TokenObject->GetString() << "'" << endl;
	return move(TokenObject);
}