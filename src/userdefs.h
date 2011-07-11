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

#ifndef __USERDEFS_H__
#define __USERDEFS_H__

#include "dynaarra.h"
#include "stdbase.h"
#include "code.h"
#include <Complex.h>

class UserDef {
public:
		UserDef() : text(""), c_value(0.0,0.0), pCode(0)
		{ }

		UserDef(const CppStr &txt, const pCodeSegment p_code)
		: text(txt), c_value(0.0,0.0), pCode(p_code)
		{ }

		UserDef(const CppStr &txt, const Complex &cmplxval)
		: text(txt), c_value(cmplxval), pCode(0)
		{ }

	UserDef& operator = (const UserDef &r)
	{
	  text    = r.text;
	  c_value = r.c_value;
	  pCode   = r.pCode;
	  return *this;
	}

	// Predicate: Does this record contain code, ie. is it a function?
	bool		IsFunc()	{ return bool(pCode != 0); }

	CppStr		text;		// text line defining entry
	Complex		c_value;
	pCodeSegment	pCode;
};

class UserRec : public StdBase<UserDef> {
public:
		UserRec() : StdBase<UserDef>()	{ }

		UserRec(const CppStr &n, const CppStr &txt, const pCodeSegment p_code)
		: StdBase<UserDef>(n, UserDef(txt, p_code))
		{ }

		UserRec(const CppStr &n, const CppStr &txt, const Complex &cmplxval)
		: StdBase<UserDef>(n, UserDef(txt, cmplxval))
		{ }

	UserRec& operator = (const UserRec &r)
	{
	  name  = r.name;
	  value = r.value;
	  return *this;
	}
};

class UserDefs : public StdTableWithInsert<UserRec> {
public:
	virtual	~UserDefs();
};

typedef UserDefs* pUserDefs;

extern pUserDefs userdefs;

#endif
