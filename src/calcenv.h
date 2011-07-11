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

#ifndef __CALCENV_H__
#define __CALCENV_H__

#ifndef __EVALSTCK_H__
#include "evalstck.h"
#endif

#ifndef __ERROROBJ_H__
#include "error.h"
#endif

#include <setjmp.h>

class CalcEnv {
public:
	EvaluationStack	eval_stack;	// numeric evaluation stack

	ErrorObj		run_error;	// run-time error
};

extern jmp_buf MathErrorJumpBuf;

bool MathRuntimeError();	// true if error; false otherwise. Clears flag.

void InstallCalcErrorHandlers();

#endif
