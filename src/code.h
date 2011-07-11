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

#ifndef __CODE_H__
#define __CODE_H__

#include "dynaarra.h"
#include "calcenv.h"
#include <Complex.h>

enum Code { cod_loadnumconst, cod_loadsymconst, cod_loadparameter,
	    cod_add, cod_sub, cod_mult, cod_divide, cod_raise, cod_negate,
	    cod_return, cod_callstd, cod_callusr };

class CodeElement {
public:
	CodeElement()							{ }
	CodeElement(Code c) 		 : opcode(c)			{ }
	CodeElement(Code c, DynaIdx i) 	 : opcode(c), index(i)		{ }
	CodeElement(const Complex &cnst) : opcode(cod_loadnumconst),
					   constant(cnst)		{ }

	Code	opcode;
	DynaIdx	index;		// index in various tables
	Complex	constant;	// numeric constant for cod_loadnumconst
};

class CodeSegment {
public:
		CodeSegment() : size(0) { }

	void	Emit(const CodeElement &);
	Complex	Run(const Complex &parameter, ErrorObj&);     
	void	Run(CalcEnv&);
private:
	DynaArray<CodeElement>	code;
	DynaIdx			size;
};

typedef CodeSegment* pCodeSegment;

#endif
