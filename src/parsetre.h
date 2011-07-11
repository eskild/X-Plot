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

#ifndef __PARSETRE_H__
#define __PARSETRE_H__

#include "scanner.h"
#include "error.h"
#include "code.h"
#include "Complex.h"

//****************************************************************************
// ParseTree
//
// Features:
//	Datastructure holding some of the contents of a parsing.
//
//
//
//
//
//****************************************************************************

class ParseTree;
typedef ParseTree* pParseTree;

enum IdAttr { id_is_stdfunc, id_is_func, id_is_const, id_is_param };

class ParseTree {
public:
	// Default constructor:
	ParseTree() : l(0), r(0), nodetype(Scanner::tok_other), number(0.0,0.0)
	{ }

	// Constructor for tok_plus, tok_minus etc.
	ParseTree(Scanner::Token t, pParseTree left, pParseTree right) :
	  l(left), r(right), nodetype(t), number(0.0,0.0)
	{ }

	// Constructor for Complex constants:
	ParseTree(const Complex &n) :
	  l(0), r(0), nodetype(Scanner::tok_number), number(n)
	{ }

	// Constructor for functions and user constants
	ParseTree(IdAttr _type, DynaIdx _idx, pParseTree parameter_tree = 0) :
	  l(0), r(parameter_tree), nodetype(Scanner::tok_id), number(0.0,0.0),
	  index(_idx), id_attr(_type)
	{ }

  virtual ~ParseTree();

  Complex Evaluate(ErrorObj&);	// returns value; sets ErrorObj if something goes wrong

  pCodeSegment GenerateCode(ErrorObj&);

private:

  void doGenerateCode(pCodeSegment, ErrorObj&);

  pParseTree		l, r;		// left, right nodes

  // Attributes
  Scanner::Token	nodetype;
  Complex		number;		// nodetype == tok_number
  DynaIdx		index;		// nodetype == tok_id
  IdAttr		id_attr;	// nodetype == tok_id
};

#endif

// EOF
