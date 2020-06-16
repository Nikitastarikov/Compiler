#pragma once
#include "AST.h"

class ScopeVar
{
	AST* Node;
	string Scope;
	//string Type;

public:
	ScopeVar(AST* const node, string const &scope) : Node(node), Scope(scope) {}

	void setNode(AST* const node)
	{
		Node = node;
	}

	void setScope(string const &scope)
	{
		Scope = scope;
	}

	AST* const getNode()
	{
		return Node;
	}

	string getScope()
	{
		return Scope;
	}

	/*
	void setType(string type)
	{
		Type = type;
	}

	string getType()
	{
		return Type;
	}
	*/
};