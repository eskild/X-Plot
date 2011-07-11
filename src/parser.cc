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

#include "parser.h"
#include "stdconst.h"
#include "stdfuncs.h"
#include "userdefs.h"
#include <assert.h>

//****************************************************************************
// ParserBase
//
// Features:
//	Base class for parsers.
//
//
//
//
//
//****************************************************************************

ParserBase::ParserBase(pScanner s, bool DeleteScanner) : scanner(s), delete_scanner(DeleteScanner)
{
  assert(scanner != 0);
}

ParserBase::~ParserBase()
{
  if (delete_scanner) delete scanner;
}

pParseTree ParserBase::Parse(ErrorObj &err, const CppStr &param_id)
{
  error = &err;
  parameter_id = param_id;

  GetCurrentToken();
  if (tok == Scanner::tok_nothing) GetToken();   // Get token if scanner has just been initialized

  pParseTree tree = ParserCore();

  if (tok == Scanner::tok_semicolon)		// skip semicolons at eoln;
    GetToken();
  else						// everything else is bad
    ThisWasntExpected("This doesn't belong here:");

  return tree;
}

void ParserBase::GetToken()
{
  tok = scanner->GetNextToken();
  if (scanner->ScanError()) {
    ThisWasntExpected("Unknown symbol encountered/Unexpected end-of-input.");
    scanner->Init();
  }
}

void ParserBase::ThisWasntExpected(const CppStr &msg)
{
  *error += msg;
  *error += scanner->GetCopyOfScanLine();
  *error += CppStr('-', scanner->GetErrorPosition()-1) + "^";
}

//****************************************************************************
// ExpressionParser
//
// Features:
//	Parses arithmetic expression.
//
//
//
//
//
//****************************************************************************

pParseTree ExpressionParser::expr()
{
  pParseTree tree = signedterm();
  while (tok == Scanner::tok_plus  ||  tok == Scanner::tok_minus) {
    pParseTree left = tree;
    Scanner::Token prev_tok = tok; GetToken();
    tree = new ParseTree(prev_tok, left, term() );
    assert(tree != 0);
  }
  return tree;
}

pParseTree ExpressionParser::signedterm()
{
  Scanner::Token prev_tok;
  switch (tok) {
    case Scanner::tok_minus: // Negation uses right branch:
			     prev_tok = tok; GetToken();
			     return new ParseTree(prev_tok, 0, term() );

    case Scanner::tok_plus : GetToken();
			     return term();

    default:		     return term();
  }
}

pParseTree ExpressionParser::term()
{
  pParseTree tree = strongterm();
  while (tok == Scanner::tok_star  ||  tok == Scanner::tok_slash) {
    pParseTree left = tree;
    Scanner::Token prev_tok = tok; GetToken();
    tree = new ParseTree(prev_tok, left, strongterm() );
  }
  return tree;
}

pParseTree ExpressionParser::strongterm()
{
  pParseTree tree = factor();
  while (tok == Scanner::tok_caret) {
    pParseTree left = tree;
    Scanner::Token prev_tok = tok; GetToken();
    tree = new ParseTree(prev_tok, left, factor() );
  }
  return tree;
}

pParseTree ExpressionParser::factor()
{
  pParseTree tree;
  switch(tok) {
  case Scanner::tok_number:
    tree = new ParseTree(Complex(scanner->GetNumber(),0.0));
    assert(tree != 0);
    GetToken();
    break;
  case Scanner::tok_lpar:
    GetToken();
    tree = expr();
    if (tok != Scanner::tok_rpar) *error += "')' expected in (...) expression.";
    GetToken();
    break;
  case Scanner::tok_id:
    tree = factor_id();
    break;

  default: ThisWasntExpected("Error in factor.");  tree = 0;
  }
  return tree;
}

pParseTree ExpressionParser::factor_id()
{
  CppStr     id = scanner->GetId();
  pParseTree tree;
  DynaIdx 	     i;

  if (id == parameter_id) {
    tree = new ParseTree(id_is_param, -1);
    assert(tree != 0);
  }

  else
    if ((i=stdfuncs->BinaryFind(id)) != -1) {		// sin(), cos() etc. ?
      GetToken();
      if (tok != Scanner::tok_lpar) *error += "Standard Function '" + id + "' needs '(' to start parameter expression.";
      GetToken();
      tree = new ParseTree(id_is_stdfunc, i, expr() ); // parameter in right branch
      assert(tree != 0);
      if (tok != Scanner::tok_rpar) *error += "Standard Function '" + id + "' needs ')' to end parameter expression.";
    }

    else
      if ((i=stdconsts->BinaryFind(id)) != -1) {		// i, pi etc. ?
	tree = new ParseTree((*stdconsts)[i].value);
	assert(tree != 0);
      }

      else
	if ((i=userdefs->LinearFind(id)) != -1) {	// user-defined ?
	  if ( (*userdefs)[i].value.IsFunc() ) {
	    GetToken();
	    if (tok != Scanner::tok_lpar) *error += "User-defined Function '" + id + "' needs '(' to start parameter expression.";
	    GetToken();
	    tree = new ParseTree(id_is_func, i, expr() ); // parameter in right branch
	    assert(tree != 0);
	    if (tok != Scanner::tok_rpar) *error += "User-defined Function '" + id + "' needs ')' to end parameter expression.";
	  }
	  else {
	    tree = new ParseTree(id_is_const, i);
	    assert(tree != 0);
	  }
	}

	else {
	  *error += "Unknown identifier: '" + id + "'.";
	  tree = 0;
	}

  GetToken();
  return tree;
}

ExpressionParser::ExpressionParser(pScanner s, bool DeleteScanner) : ParserBase(s, DeleteScanner)
{
}

pParseTree ExpressionParser::ParserCore()
{
  return expr();
}

//****************************************************************************
// ConstExprParser
//
// Features:
//	Handles constant-expressions, ie. expressions involving numbers and
//	standard constants (no user-defined constants). No functions of any
//	kind are allowed.
//
//
//
//****************************************************************************

pParseTree ConstExprParser::factor_id()
{
  CppStr id = scanner->GetId();  GetToken();
  DynaIdx   i;
  if ( (i=stdconsts->BinaryFind(id)) == -1) {		// is user-defined or doesn't exist:
    *error += "Unknown identifier/isn't a standard constant: '" + id + "'.";
    return 0;
  }
  else return new ParseTree((*stdconsts)[i].value);	// std. const.
}

ConstExprParser::ConstExprParser(pScanner s, bool DeleteScanner) : ExpressionParser(s, DeleteScanner)
{
}


//****************************************************************************
// AssignmentParser
//
// Features:
//	Handles input of the following form:
//
//		constant = constant_expression
//		function ( variable ) = expression
//
//
//****************************************************************************

AssignmentParser::AssignmentParser(pScanner s, bool DeleteScanner) : ParserBase(s, DeleteScanner)
{
}

pParseTree AssignmentParser::ParserCore()
{
  if (tok != Scanner::tok_id) *error += "Declaration expected.";
  else {
    CppStr id   = scanner->GetId();
    CppStr line = scanner->GetCopyOfScanLine();
    if (  stdconsts->BinaryFind(id) != -1   ||
	  stdfuncs ->BinaryFind(id) != -1)
    {
      *error += "'" + id + "' is pre-defined.";
      return 0;
    }
    GetToken();
    if (tok == Scanner::tok_lpar) {	// function decl.
      GetToken();  if (tok != Scanner::tok_id)    *error += "Parameter expected.";
      CppStr param = scanner->GetId();
      GetToken();  if (tok != Scanner::tok_rpar)  *error += "')' expected.";
      GetToken();  if (tok != Scanner::tok_equal) *error += "'=' expected.";
      GetToken();

      pParser pp = new ExpressionParser(scanner, cFALSE);  assert(pp != 0);
      pParseTree pt = pp->Parse(*error, param);

      if (! (*error) ) {
	pCodeSegment cs = pt->GenerateCode(*error);
	if ( (*error) == cTRUE) {
	  delete cs;
	  return 0;
	}

	DynaIdx i = userdefs->LinearFind(id);

	if (i == -1) { // new:
	  i = userdefs->Insert( UserRec(id, line, cs) );
	}
	else {        // existing:
	  if ( ! (*userdefs)[i].value.IsFunc() ) {
	    *error += "Type Error: '" + id + "' used to be a constant.";
	    delete cs;
	  }
	  else {
	    delete (*userdefs)[i].value.pCode;
	    (*userdefs)[i].value = UserDef( line, cs );
	  }
	}
      }
      delete pt;
      delete pp;
      GetCurrentToken();
    }
    else {
      if (tok != Scanner::tok_equal) *error += "'=' expected.";
      GetToken();

      pParser pp = new ExpressionParser(scanner, cFALSE);  assert(pp != 0);
      pParseTree pt = pp->Parse(*error);
      if (! (*error) ) {
	DynaIdx i = userdefs->LinearFind(id);

	if (i == -1) // new:
	  i = userdefs->Insert( UserRec( id, line, pt->Evaluate(*error) ) );
	else {       // existing:
	  if ( (*userdefs)[i].value.IsFunc() ) *error += "Type Error: '" + id + "' used to be a function.";
	  else (*userdefs)[i].value = UserDef( line, pt->Evaluate(*error) );
	}
      }
      delete pt;
      delete pp;
      GetCurrentToken();
    }
  }
  return 0;
}

// EOF
