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

#ifndef __STDCONST_H__
#define __STDCONST_H__

#include "dynaarra.h"
#include "stdbase.h"
#include <Complex.h>




class ComplexConstRec : public StdBase<Complex> {
public:
		ComplexConstRec()	{ }
		ComplexConstRec(const CppStr &n, const Complex &val)
		: StdBase<Complex>(n, val)
		{
		}
};

CLASSDEF(StdConsts);
class StdConsts : public StdTableBase<ComplexConstRec> {
public:
	StdConsts();
};

extern pStdConsts stdconsts;	// new'ed in main module

#endif
