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

#include "evalstck.h"

void EvaluationStack::Init()
{
  Top        = -1;
  ErrorFlag  = StackOK;
  StackArray = new Complex[MaxStack];
  assert(StackArray != 0);
}

StackBase::stackerror EvaluationStack::Error()
{
  stackerror t = ErrorFlag;
  ErrorFlag = StackOK;
  return t;
}

bool EvaluationStack::Push(const Complex &element)
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

Complex EvaluationStack::Pop()
{
  if (Empty()) {
    ErrorFlag = StackEmpty;
    return StackArray[0];
  }
  else return StackArray[Top--];
}

Complex EvaluationStack::TOS()
{
  if (Empty()) {
    ErrorFlag = StackEmpty;
    return StackArray[0];
  }
  else return StackArray[Top];
}

// EOF

