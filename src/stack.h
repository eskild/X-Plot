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

#ifndef __STACK_H__
#define __STACK_H__

#include <assert.h>
#include "defs.h"

/* Class StackBase is required in order to make 'stackerror' a general type
 * for all Stack template instantiations, regardless of their types.
 * This could be accomplished by introducing 'stackerror' as a global type,
 * but this is not desirable, as it `clutters up the global name space'.
 * (Yes, it's more religion than reason . . .)
 */
class StackBase {
public:
	enum    stackerror      { StackOK, StackEmpty, StackFull };
};

template < class T >
class Stack : public StackBase {
public:
			Stack() : MaxStack(20)
					{ Init(); }
			Stack(int stacksize) : MaxStack(stacksize)
					{ Init(); }

	virtual        ~Stack()         { delete [] StackArray; }

	bool            Push(const T &element);
	T               Pop();
	T		TOS();		// Top-Of-Stack
	bool            Empty()         { return bool(Top == -1); }
	int             StackSize()     { return Top+1; }
	int             MaxStackSize()  { return MaxStack; }
	stackerror      Error();

protected:
	T               *StackArray;
	int             Top;
	stackerror      ErrorFlag;
	int             MaxStack;
private:
	void            Init();
};

template < class T >
void Stack<T>::Init()
{
  Top        = -1;
  ErrorFlag  = StackOK;
  StackArray = new T[MaxStack];
  assert(StackArray != 0);
}

template < class T >
StackBase::stackerror Stack<T>::Error()
{
  stackerror t = ErrorFlag;
  ErrorFlag = StackOK;
  return t;
}

template < class T >
bool Stack<T>::Push(const T &element)
{
  if (Top == (MaxStack - 1)) {
    ErrorFlag = StackFull;
    return cFALSE;
  }
  else {
    Top++;
    StackArray[Top] = element;
    return cTRUE;
  }
}

template < class T >
T Stack<T>::Pop()
{
  if (Empty()) {
    ErrorFlag = StackEmpty;
    return StackArray[0];
  }
  else return StackArray[Top--];
}

template < class T >
T Stack<T>::TOS()
{
  if (Empty()) {
    ErrorFlag = StackEmpty;
    return StackArray[0];
  }
  else return StackArray[Top];
}

#endif
