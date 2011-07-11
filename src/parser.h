//    X-Plot  --  a program for visualizing complex functions
//
//    Copyright (C) 1996-2011 Martin Eskildsen.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Comments, suggestions, questions to:
//    	     Martin Eskildsen
//    	     eskild.opensource@gmail.com
//
//
//    Programming by Martin Eskildsen, (then) student at the
//    Technical University of Denmark.

#ifndef __PARSER_H__
#define __PARSER_H__

//****************************************************************************
// ParserBase, ExpressionParser, AssignmentParser
//
// Features:
//
//
//
//
//
//
//****************************************************************************

#include "parsetre.h"

class ParserBase {
public:
			ParserBase(pScanner s, bool DeleteScanner = cTRUE);
	virtual	       ~ParserBase();

	pParseTree	Parse(ErrorObj &, const CppStr &param_id = "");

	// ParserCore() should always return with the next token ready.
virtual	pParseTree	ParserCore() = 0;

protected:
		void	ThisWasntExpected(const CppStr &msg = "This wasn't expected:");
		void	GetToken();
		void	GetCurrentToken()	{ tok = scanner->GetCurrentToken(); }

	Scanner::Token 	tok;
	pScanner 	scanner;
	ErrorObj 	*error;

	CppStr		parameter_id;
private:
	bool		delete_scanner;
};

typedef ParserBase* pParser;

class ExpressionParser : public ParserBase {
public:
			ExpressionParser(pScanner s, bool DeleteScanner = cTRUE);

virtual	pParseTree	ParserCore();
protected:

	pParseTree	expr();		// handles +, -
	pParseTree	term();		// handles *, /
	pParseTree	signedterm();	// handles sign on term
	pParseTree	strongterm();	// handles ^
	pParseTree	factor();	// handles numbers, (), functions etc.
virtual	pParseTree	factor_id();	// handles id's in factor()
};

class ConstExprParser : public ExpressionParser {
public:
			ConstExprParser(pScanner s, bool DeleteScanner = cTRUE);
protected:
virtual	pParseTree	factor_id();	// handles id's in factor()
};

class AssignmentParser : public ParserBase {
public:
			AssignmentParser(pScanner s, bool DeleteScanner = cTRUE);

virtual	pParseTree	ParserCore();
};

#endif

// EOF
