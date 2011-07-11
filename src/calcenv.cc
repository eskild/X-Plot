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

#include "calcenv.h"
#include <signal.h>
#include <setjmp.h>

static bool runerror = cFALSE;

bool MathRuntimeError()
{
  bool b = runerror;
  runerror = cFALSE;
  return b;
}

jmp_buf MathErrorJumpBuf;

#ifdef __BORLANDC__
int matherr(struct exception *a)
{
  runerror = cTRUE;
  a->retval = HUGE_VAL;
  return 1;
} 

#endif

static void FPE_catch(int)
{
/*  debug << "Into FPE_catch." << endl;  */
  runerror = cTRUE;
  signal(SIGFPE, FPE_catch);
  longjmp(MathErrorJumpBuf, -1);
  return;
}

void InstallCalcErrorHandlers()
{
  signal(SIGFPE, FPE_catch);
}

// EOF
