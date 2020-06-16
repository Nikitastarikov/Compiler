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
	unique_ptr<Token> ob(new Token());
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
		ob->SetTypeToken(Token::Words::String);
		ob->SetString(Line.substr(IdentifierString, SizeSubLine + 1));
		//cout << "Loc<" << LineNumber << ":" << (TemporaryIdentifierString + 1) << "> " << doubleduote->GetTypeToken() << " '" << doubleduote->GetString() << "'" << endl;
	}
	else
	{
		ob->SetTypeToken(Token::Words::Unexpected);
		ob->SetString(Line.substr(IdentifierString, SizeSubLine));
	}
	
	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(IdentifierString + 1);
	//cout << "Loc<" << LineNumber << ":" << (IdentifierString + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
	IdentifierString = TemporaryIdentifierString;
	CharacterReadMode = 0;
	if (ob != nullptr && ob->GetString() != "")
		VectorToken.push_back(move(ob));
}

void Lexer::UnexpectedToken(string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size)
{
	unique_ptr<Token> ob(new Token());
	int SizeSubLine = 1;
	int TemporaryIdentifierString = IdentifierString;
	while (TemporaryIdentifierString < Size - 1 && !isspace(Line[TemporaryIdentifierString]) && Line[TemporaryIdentifierString] != ';')
	{
		TemporaryIdentifierString++;
		SizeSubLine++;
	}

	ob->SetTypeToken(Token::Words::Unexpected);
	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(IdentifierString + 1);
	ob->SetString(Line.substr(IdentifierString, SizeSubLine - 1));
	if (ob != nullptr && ob->GetString() != "")
	{
		//cout << TemporaryIdentifierString << "/" << SizeSubLine << endl;
		//cout << "Loc<" << LineNumber << ":" << ob->GetColNum() << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
		VectorToken.push_back(move(ob));
	}
	IdentifierString = TemporaryIdentifierString - 1;// отступает на индекс назад, чтобы корректно прочитать следующий токен
}

void Lexer::CheckOb(unique_ptr<Token> ob, string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size)
{
	if (ob != nullptr && ob->GetString() != "")
		VectorToken.push_back(move(ob));
	else 
		UnexpectedToken(Line, LineNumber, VectorToken, IdentifierString, Size);
}

void Lexer::GetNextToken(string Line, int LineNumber, int EndFileId, vector<unique_ptr<Token>> &VectorToken) 
{
	
	int IdentifierString = 0;
	int Size = (int)Line.size();
	CharacterReadMode = 0;

	if (EndFileId) 
	{
		unique_ptr<Token> ob(new Token());
		ob->SetTypeToken(Token::Words::End);
		ob->SetString("\0");
		ob->SetStrNumber(LineNumber);
		ob->SetColNumber(1);
		//cout << "Loc<" << LineNumber << ":" << 1 << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
		VectorToken.push_back(std::move(ob));
	}
	else 
	{
		//cout << LineNumber << " line size = " << Line.size() << endl;
		while (IdentifierString < Size) 
		{
			while (CharacterReadMode != 2 && isspace(Line[IdentifierString])) IdentifierString++;

			if (CharacterReadMode == 2)
			{
				IdentifierString--;
				CharacterReadModeTwo(Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else if (strchr("+-/*%&|=()[]{}<>;,.", Line[IdentifierString])) 
			{
				auto ob = WordZnak(Line, LineNumber, Size, IdentifierString);
				if (ob != nullptr && ob->GetString() != "")
					VectorToken.push_back(move(ob));
			}
			else if (Size - 1 == IdentifierString);
			else if (isletter(Line[IdentifierString])) 
			{
				CheckOb(WordString(Line, LineNumber, Size, IdentifierString), Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else if (isdigit(Line[IdentifierString])) 
			{
				CheckOb(WordDigit(Line, LineNumber, IdentifierString, Size), Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else if (Line[IdentifierString] == '\"') 
			{ 
				//cout << "Loc<" << LineNumber << ":" << (IdentifierString + 1) << "> " << ob->GetTypeToken() << " '" << '\"' << "'" << endl;
				CharacterReadMode = 2; 
			}
			else if (Line[IdentifierString] == '\'')
			{
				CheckOb(isSymbol(Line, LineNumber, IdentifierString, Size), Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else
			{
				UnexpectedToken(Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			IdentifierString++;
		}
	}
}

unique_ptr<Token> isSymbol(string &Line, int &LineNumber, int &Identifier, int LineSize)
{
	unique_ptr<Token> ob(new Token());
	int SizeSubLine = 3, IndexTemp;

	if (Identifier + 2 > LineSize - 1)
	{
		return nullptr;
	}
	else if (Line[Identifier + 2] != '\'')
	{
		/*IndexTemp = Identifier + 2; 	
		ob->SetTypeToken(Token::Words::Unexpected);
		ob->SetString(Line.substr(Identifier, SizeSubLine));*/
		return nullptr;
	}
	else
	{
		IndexTemp = Identifier + 2; 	
		ob->SetTypeToken(Token::Words::Symbol);
		ob->SetString(Line.substr(Identifier, SizeSubLine));
	}

	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(Identifier + 1);
	//cout << "Loc<" << LineNumber << ":" << (Identifier + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;

	Identifier = IndexTemp;
	return move(ob);
}

int isletter(char a) 
{
	if (('A' <= a && a <= 'Z') || ('a' <= a && a <= 'z') || a == '_') {
		return 1;
	}
	return 0;
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

	if (isspace(Line[TemporaryIdentifier]) || !Correctness)
		return false;

	TemporaryIdentifier++;
	TempSizeSubLine++;
	while (TemporaryIdentifier < LineSize - 1 && strchr("_01", Line[TemporaryIdentifier])) {
		TemporaryIdentifier++;
		TempSizeSubLine++;
	}

	if (TemporaryIdentifier < LineSize - 1)
	{

		if (!isletter(Line[TemporaryIdentifier]))
		{
			LineIndex = TemporaryIdentifier;
			SizeSubLine = TempSizeSubLine;
			return true;
		}
		else
			return false;
	}
	else if (TemporaryIdentifier == LineSize - 1 && isdigit(Line[TemporaryIdentifier]))
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

	if (Line[TemporaryIdentifier - 1] != '0')
		Correctness = 0;

	if (!strchr("1234567", Line[TemporaryIdentifier]))
		Correctness = 0;

	if (isspace(Line[TemporaryIdentifier]) || !Correctness)
		return false;

	while (strchr("01234567", Line[TemporaryIdentifier]) && TemporaryIdentifier < LineSize - 1) {
		TemporaryIdentifier++;
		TempSizeSubLine++;
	}

	if (TemporaryIdentifier < LineSize - 1)
	{

		if (!isletter(Line[TemporaryIdentifier]))
		{
			LineIndex = TemporaryIdentifier;
			SizeSubLine = TempSizeSubLine;
			return true;
		}
		else
			return false;
	}
	else if (TemporaryIdentifier == LineSize - 1 && isdigit(Line[TemporaryIdentifier]))
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
	
	if (isspace(Line[TemporaryIdentifier]) || !Correctness)
		return false;

	TemporaryIdentifier++;
	TempSizeSubLine++;
	while (strchr("0123456789abcdefABCDEF", Line[TemporaryIdentifier]) && TemporaryIdentifier < LineSize - 1) {
		TemporaryIdentifier++;
		TempSizeSubLine++;
	}

	if (TemporaryIdentifier < LineSize - 1)
	{
		if (!isletter(Line[TemporaryIdentifier]))
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

bool IsDecimalInteger(string &Line, int &LineIndex, int &SizeSubLine, int LineSize, bool &real)
{
	//cout << LineIndex << endl;
	int TemporaryIdentifier = LineIndex;
	int TempSizeSubLine = SizeSubLine;

	if (Line[TemporaryIdentifier] == '.')
	{
		TemporaryIdentifier++;
		TempSizeSubLine++;
		real = true;
		if (TemporaryIdentifier == LineSize - 1 || !isdigit(Line[TemporaryIdentifier]))
		{
			return false;
		}
	}

	while (TemporaryIdentifier < LineSize - 1 && isdigit(Line[TemporaryIdentifier]))
	{
		//cout << Line[TemporaryIdentifier] << endl;
		TemporaryIdentifier++;
		TempSizeSubLine++;
		if (TemporaryIdentifier < LineSize - 1 && Line[TemporaryIdentifier] == '.' && real == false)
		{
			TemporaryIdentifier++;
			TempSizeSubLine++;
			real = true;
			if (TemporaryIdentifier == LineSize - 1 || !isdigit(Line[TemporaryIdentifier]))
			{
				return false;
			}
		}
		else if (TemporaryIdentifier < LineSize - 1 && Line[TemporaryIdentifier] == '.' && real == true)
		{
			return false;
		}

	}
	//cout << TemporaryIdentifier << endl;
	
	if (TemporaryIdentifier < LineSize - 1)
	{
		
		if (!isletter(Line[TemporaryIdentifier]))
		{
			LineIndex = TemporaryIdentifier;
			SizeSubLine = TempSizeSubLine;
			return true;
		}
		else
			return false;
	}
	else if (TemporaryIdentifier == LineSize - 1 && isdigit(Line[TemporaryIdentifier]))
	{
		LineIndex = TemporaryIdentifier;
		SizeSubLine = TempSizeSubLine + 1;
		return true;
	}
		
	LineIndex = TemporaryIdentifier;
	SizeSubLine = TempSizeSubLine;
	return true;
}

unique_ptr<Token>  WordDigit(string &Line, int &LineNumber, int &Identifier, int LineSize) 
{

	int SizeSubLine = 1, TemporaryIdentifier = Identifier + 1;
	unique_ptr<Token> ob(new Token());
	bool real = false;

	if (IsBinary(Line, TemporaryIdentifier, SizeSubLine, LineSize))
	{ 
		ob->SetTypeToken(Token::Words::Binary);
	}
	else if (IsOctal(Line, TemporaryIdentifier, SizeSubLine, LineSize))
	{ 
		ob->SetTypeToken(Token::Words::Octal);
	}
	else if (IsHexadecimal(Line, TemporaryIdentifier, SizeSubLine, LineSize))
	{
		ob->SetTypeToken(Token::Words::Hexadecimal);
	}
	else if (IsDecimalInteger(Line, TemporaryIdentifier, SizeSubLine, LineSize, real))
	{
		if (real)
			ob->SetTypeToken(Token::Words::NotInteger);
		else
			ob->SetTypeToken(Token::Words::DecimalInteger);
	}
	else
	{
		ob->SetTypeToken(Token::Words::Unexpected);
		while (TemporaryIdentifier < LineSize - 1 && !isspace(Line[TemporaryIdentifier]))
		{
			TemporaryIdentifier++;
			SizeSubLine++;
		}
		if (!isspace(Line[TemporaryIdentifier]))
			SizeSubLine++;
	}

	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(Identifier + 1);
	ob->SetString(Line.substr(Identifier, SizeSubLine));
	//cout << "Loc<" << LineNumber << ":" << (Identifier + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
	Identifier = TemporaryIdentifier - 1;

	return move(ob);
}

void ReadWord(int TemporaryIdentifier, unique_ptr<Token> const &ob, string &Line, int SizeSubLine, int &LineSize, int &LineIndex)
{
	if (ob->GetString() == "using") {
		ob->SetTypeToken(Token::Words::KeyWordUsing);
	}
	else if (ob->GetString() == "public") {
		ob->SetTypeToken(Token::Words::Public);
	}
	else if (ob->GetString() == "class") {
		ob->SetTypeToken(Token::Words::Class);
	}
	else if (ob->GetString() == "static") {
		ob->SetTypeToken(Token::Words::KeyWordStatic);
	}
	else if (ob->GetString() == "while") {
		ob->SetTypeToken(Token::Words::KeyWordWhile);
	}
	else if (ob->GetString() == "if") {
		ob->SetTypeToken(Token::Words::KeyWordIf);
	}
	else if (ob->GetString() == "new") {
		ob->SetTypeToken(Token::Words::KeyWordNew);
	}
	else if (ob->GetString() == "return") {
		ob->SetTypeToken(Token::Words::KeyWordReturn);
	}
	else if (ob->GetString() == "else") {
		ob->SetTypeToken(Token::Words::KeyWordElse);
	}
	else if (ob->GetString() == "System") {
		ob->SetTypeToken(Token::Words::Library);
	}
	else if (ob->GetString() == "void") {
		ob->SetTypeToken(Token::Words::Void);
	}
	else if (ob->GetString() == "var") {
		ob->SetTypeToken(Token::Words::Var);
	}
	else if (ob->GetString() == "int") {
		ob->SetTypeToken(Token::Words::Int);
	}
	else if (ob->GetString() == "char") {
		ob->SetTypeToken(Token::Words::Char);
	}
	else if (ob->GetString() == "float") {
		ob->SetTypeToken(Token::Words::Float);
	}
	else if (ob->GetString() == "double") {
		ob->SetTypeToken(Token::Words::Double);
	}
	else if (ob->GetString() == "string") {
		ob->SetTypeToken(Token::Words::string);
	}
	else if (ob->GetString() == "bool") {
		ob->SetTypeToken(Token::Words::Bool);
	}
	else {
		ob->SetTypeToken(Token::Words::Identifier);
		ob->SetString(Line.substr(LineIndex, SizeSubLine));
		//cout << "!!!! = " << ob->GetString() << endl; 
	}
}

unique_ptr<Token> WordString(string &Line, int &LineNumber, int &LineSize, int &LineIndex) 
{
	unique_ptr<Token> ob(new Token());
	int SizeSubLine, TemporaryIdentifier;
	//cout << "word" << endl;
	SizeSubLine = 1;
	TemporaryIdentifier = LineIndex + 1;

	if (LineSize > TemporaryIdentifier)
	{
		while (TemporaryIdentifier < LineSize - 1 && (isletter(Line[TemporaryIdentifier]) || isdigit(Line[TemporaryIdentifier]))) {
			SizeSubLine++;
			TemporaryIdentifier++;
		}

		if (TemporaryIdentifier == LineSize - 1 && (isletter(Line[TemporaryIdentifier]) || isdigit(Line[TemporaryIdentifier])))
		{
			SizeSubLine++;
		}
		else if (!(strchr("+-/*%&|=()[]{}<>;,.", Line[TemporaryIdentifier]) || Line[TemporaryIdentifier] == ' ' || LineSize - 1 == TemporaryIdentifier)) 
			return nullptr;

	}

	ob->SetString(Line.substr(LineIndex, SizeSubLine));
	ReadWord(TemporaryIdentifier, ob, Line, SizeSubLine, LineSize, LineIndex);
	//cout << "1Loc<" << LineNumber << ":" << (LineIndex + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(LineIndex + 1);
	//cout << "2Loc<" << LineNumber << ":" << (LineIndex + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;

	LineIndex = TemporaryIdentifier - 1;
	if (TemporaryIdentifier == LineSize - 1 && isletter(Line[TemporaryIdentifier]))
	{
		LineIndex = TemporaryIdentifier;
	}
	return move(ob);
}

void checkOperEqual(unique_ptr<Token> const &ob, string &Line, int &LineSize, int &LineIndex, Token::Words tAs, Token::Words t)
{
	string s;
	if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '=')
	{
		ob->SetTypeToken(tAs);
		s = Line[LineIndex];
		s += "=";
		
		ob->SetString(s);
		LineIndex++;
	}
	else
	{
		ob->SetTypeToken(t);
		s = Line[LineIndex];
		ob->SetString(s);
	}
}
//+-/*%&|=()[]{}<>;,.
unique_ptr<Token> WordZnak(string &Line, int &LineNumber, int &LineSize, int &LineIndex) 
{
	unique_ptr<Token> ob(new Token());
	switch (Line[LineIndex]) {
	case '+':
		if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '+')
		{
			ob->SetTypeToken(Token::Words::Increment);
			ob->SetString("++");
			LineIndex++;
		}
		else
		{
			checkOperEqual(ob, Line, LineSize, LineIndex, Token::Words::AddAssign, Token::Words::Plus);
		}
		break;
	case '-':
		if (LineIndex + 1 < LineSize && Line[LineIndex + 1] == '-')
		{
			ob->SetTypeToken(Token::Words::Decrement);
			ob->SetString("--");
			LineIndex++;
		}
		else
		{
			checkOperEqual(ob, Line, LineSize, LineIndex, Token::Words::SubAssign, Token::Words::Minus);
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
			ob->SetTypeToken(Token::Words::DivAssign);
			ob->SetString("/=");
			LineIndex++;
		}
		else
		{
			ob->SetTypeToken(Token::Words::Slash);
			ob->SetString("/");
		}
		break;
	case '*':
		checkOperEqual(ob, Line, LineSize, LineIndex, Token::Words::MultAssign, Token::Words::Asterisk);
		break;
	case '%':
		ob->SetTypeToken(Token::Words::Percent);
		ob->SetString("%");
		break;
	case '=':
		checkOperEqual(ob, Line, LineSize, LineIndex, Token::Words::Equal, Token::Words::Assignment);
		break;
	case '(':
		ob->SetTypeToken(Token::Words::LeftParen);
		ob->SetString("(");
		break;
	case ')':
		ob->SetTypeToken(Token::Words::RightParen);
		ob->SetString(")");
		break;
	case '[':
		ob->SetTypeToken(Token::Words::LeftSquare);
		ob->SetString("[");
		break;
	case ']':
		ob->SetTypeToken(Token::Words::RightSquare);
		ob->SetString("]");
		break;
	case '{':
		ob->SetTypeToken(Token::Words::LeftCurly);
		ob->SetString("{");
		break;
	case '}':
		ob->SetTypeToken(Token::Words::RightCurly);
		ob->SetString("}");
		break;
	case '.':
		ob->SetTypeToken(Token::Words::Point);
		ob->SetString(".");
		break;
	case ';':
		ob->SetTypeToken(Token::Words::Semicolon);
		ob->SetString(";");
		break;
	case '<':
		checkOperEqual(ob, Line, LineSize, LineIndex, Token::Words::LessOrEqual, Token::Words::LessThan);
		break;
	case '>':
		checkOperEqual(ob, Line, LineSize, LineIndex, Token::Words::MoreOrEqual, Token::Words::GreaterThan);
		break;
	case ',':
		ob->SetTypeToken(Token::Words::Comma);
		ob->SetString(",");
		break;
	case '&':
		if (LineIndex + 1 < LineSize &&Line[LineIndex + 1] == '&')
		{
			ob->SetTypeToken(Token::Words::Conjunction);
			ob->SetString("&&");
			LineIndex++;
		}
		else
		{
			ob->SetTypeToken(Token::Words::Unexpected);
				ob->SetString("&");
		}
		break;
	case '|':
		if (LineIndex + 1 < LineSize &&Line[LineIndex + 1] == '|')
		{
			ob->SetTypeToken(Token::Words::Disjunction);
			ob->SetString("||");
			LineIndex++;
		}
		else
		{
			ob->SetTypeToken(Token::Words::Unexpected);
				ob->SetString("|");
		}
		break;
	}
	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(LineIndex + 1);	
	//cout << "Loc<" << LineNumber << ":" << (LineIndex + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
	return move(ob);
}