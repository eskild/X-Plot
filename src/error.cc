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

#include <assert.h>
#include "error.h"

ErrorObj::ErrorObj() : isError(cFALSE), restart(cTRUE)
{
  list = new DestructiveFifoList;
  assert(list != 0);
}

ErrorObj::~ErrorObj()
{
  delete list;
}

ErrorObj& ErrorObj::operator = (ErrorObj &e)
{
  isError = e.isError;
  restart = cTRUE;

  delete list;
  list = new DestructiveFifoList;
  assert(list != 0);

  e.restart = cTRUE;

  CppStr str;
  while ( (str=e()) != "@" ) *list += new _ErrorItem(str);

  e.restart = cTRUE;

  return *this;
}

ErrorObj& ErrorObj::operator = (bool flag)
{
  isError = flag;
  if ( !flag  &&  list->Size() > 0 ) {	// Clean list if flag == cFALSE
    delete list;
    list = new DestructiveFifoList;
    assert(list != 0);
  }
  return *this;
}

ErrorObj& ErrorObj::operator += (const CppStr &str)
{
  if (!isError) *this = cTRUE;
  *list += new _ErrorItem(str);
  restart = cTRUE;
  return *this;
}

ErrorObj& ErrorObj::operator &= (bool flag)
{
  return ( *this  =  bool(isError && flag) );
}

ErrorObj& ErrorObj::operator |= (bool flag)
{
  return ( *this  =  bool(isError || flag) );
}

CppStr ErrorObj::operator() ()
{
  if (restart) {
    list->InitNext();
    restart = cFALSE;
  }
  _pErrorItem p = (_pErrorItem) list->Next();
  if (p == 0) {
    restart = cTRUE;
    return CppStr("@");
  }
  else return p->_s;
}

// EOF
