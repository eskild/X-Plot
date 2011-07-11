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

#ifndef __ERROROBJ_H__
#define __ERROROBJ_H__

#ifndef __DEFS_H__
#include "defs.h"
#endif

#ifndef __CPPSTR_H__
#include "cppstr.h"
#endif

#ifndef __FIFOLIST_H__
#include "fifolist.h"
#endif

class _ErrorItem : public FifoListElement {
public:
	_ErrorItem(const CppStr& str) : _s(str) 	{ }

	CppStr _s;
};

typedef _ErrorItem* _pErrorItem;

CLASSDEF(ErrorObj);

class ErrorObj {
public:
		ErrorObj();
  virtual      ~ErrorObj();

  ErrorObj&	operator = (ErrorObj &e);	// Sets 'e.restart' = cTRUE
  ErrorObj& 	operator = (bool);
  ErrorObj& 	operator += (const CppStr &);

  ErrorObj&	operator &= (bool);
  ErrorObj&	operator |= (bool);

  bool          operator == (bool b)            { return bool(isError == b); }
  bool          operator != (bool b)            { return bool(isError != b); }

  bool 	    	operator ! ()     		{ return bool(!isError); }

		operator bool()			{ return isError; }

  CppStr	operator () ();

private:
  bool			isError;
  bool			restart;	// cTRUE: restart operator ()
  pDestructiveFifoList	list;		// destroys elements when destroyed
};

#endif
