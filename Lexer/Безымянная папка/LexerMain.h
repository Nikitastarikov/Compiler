#pragma once
#include "Token.h"
#include "Lexer.h"
#include "FunctionLex.h"

using namespace std;

//CharacterReadMode = 0; Ôëàã, Cèãíàëèçèðóþùèé î òîì, ÷òî èäåò îáû÷íîå C÷èòûâàíèå
//CharacterReadMode = 2; Ôëàã, Cèãíàëèçèðóþùèé î òîì, ÷òî èäåò C÷èòûâàíèå çíà÷åíèå èç äâîéíûõ êîâû÷åê

void Lexer::CharacterReadModeTwo(string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size)
{
	unique_ptr<Token> ob(new Token());
	unique_ptr<Token> doubleduote(new Token());
	int SizeSubLine = 0;
	int TemporaryIdentifierString = IdentifierString;
	while (Line.at(TemporaryIdentifierString) != '"' && TemporaryIdentifierString < Size)
	{
		TemporaryIdentifierString++;
		SizeSubLine++;
	}

	if (Line.at(TemporaryIdentifierString) == '"')
	{
		doubleduote->SetTypeToken(Token::Words::DoubleQuote);
		ob->SetTypeToken(Token::Words::String);
		doubleduote->SetStrNumber(LineNumber);
		doubleduote->SetColNumber(IdentifierString + 2);
		doubleduote->SetString(Line.substr(TemporaryIdentifierString, 1));
		//cout << "Loc<" << LineNumber << ":" << (TemporaryIdentifierString + 1) << "> " << doubleduote->GetTypeToken() << " '" << doubleduote->GetString() << "'" << endl;
	}
	else
	{
		ob->SetTypeToken(Token::Words::Unexpected);
	}

	ob->SetString(Line.substr(IdentifierString, SizeSubLine));
	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(IdentifierString + 1);
	//cout << "Loc<" << LineNumber << ":" << (IdentifierString + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
	IdentifierString = TemporaryIdentifierString;
	CharacterReadMode = 0;
	if (ob != nullptr)
		VectorToken.push_back(move(ob));
}
void Lexer::UnexpectedToken(string const &Line, int LineNumber, vector<unique_ptr<Token>> &VectorToken, int &IdentifierString, int Size)
{
	unique_ptr<Token> ob(new Token());
	int SizeSubLine = 1;
	int TemporaryIdentifierString = IdentifierString;
	while (TemporaryIdentifierString < Size - 1 && !isspace(Line.at(IdentifierString)))
	{
		TemporaryIdentifierString++;
		SizeSubLine++;
	}

	ob->SetTypeToken(Token::Words::Unexpected);
	ob->SetStrNumber(LineNumber);
	ob->SetColNumber(IdentifierString + 1);
	ob->SetString(Line.substr(IdentifierString, SizeSubLine));
	if (ob != nullptr)
	{
		VectorToken.push_back(move(ob));
		//cout << TemporaryIdentifierString << "/" << SizeSubLine << endl;
		//cout << "Loc<" << LineNumber << ":" << (IdentifierString + 1) << "> " << ob->GetTypeToken() << " '" << ob->GetString() << "'" << endl;
	}
	IdentifierString = TemporaryIdentifierString;
}


void Lexer::GetNextToken(string Line, int LineNumber, int EndFileId, vector<unique_ptr<Token>> &VectorToken) 
{
	
	int IdentifierString = 0;
	int Size = (int)Line.size();
	CharacterReadMode = 0; // Ôëàã, Cèãíàëèçèðóþùèé î òîì, ÷òî èäåò C÷èòûâàíèå èç êîâû÷åê

	if (EndFileId) 
	{ // Ïðîâåðêà íà êîíåö ôàéëà
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
		//cout << "line size = " << Line.size() << endl;
		while (IdentifierString < Size) 
		{
			while (CharacterReadMode != 2 && isspace(Line.at(IdentifierString))) 
			{ // ÏðîïóCêàåì âCå ïðîáåëû
				IdentifierString++;
			}

			if (CharacterReadMode == 2)
			{ // C÷èòûâàåì Còðîêó ïîêà íå âCòðåòèì çíàê " èëè Còðîêà íå çàêîí÷èòCÿ
				CharacterReadModeTwo(Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			else if (strchr("+-/*%&|=()[]{}<>;,.", Line.at(IdentifierString))) { // C÷èòûâàíèå îïåðàòîðîâ
				//cout << "i = " << IdentifierString << endl;
				auto ob = WordZnak(Line, LineNumber, Size, IdentifierString);
				if (ob != nullptr)
					VectorToken.push_back(move(ob));
			}
			else if (Size - 1 == IdentifierString);
			else if (isletter(Line.at(IdentifierString))) { // C÷èòûâàíèå êëþ÷åâûõ Cëîâ
				auto ob = WordString(Line, LineNumber, Size, IdentifierString);
				if (ob != nullptr)
					VectorToken.push_back(move(ob));
			}
			else if (isdigit(Line.at(IdentifierString))) { // C÷èòûâàíèå ÷èCåë
				auto ob = WordDigit(Line, LineNumber, IdentifierString, Size);
				if (ob != nullptr)
					VectorToken.push_back(move(ob));
			}
			else if (Line.at(IdentifierString) == '\"') { // Íà÷èíàåì C÷èòûâàòü çíà÷åíèå èç äâîéíûõ êîâû÷åê 
				unique_ptr<Token> ob(new Token());
				ob->SetTypeToken(Token::Words::DoubleQuote);
				ob->SetStrNumber(LineNumber);
				ob->SetColNumber(IdentifierString + 1);
				//cout << "Loc<" << LineNumber << ":" << (IdentifierString + 1) << "> " << ob->GetTypeToken() << " '" << '\"' << "'" << endl;
				CharacterReadMode = 2; 
			}
			else
			{
				UnexpectedToken(Line, LineNumber, VectorToken, IdentifierString, Size);
			}
			IdentifierString++;
		}
	}
}