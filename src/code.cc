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

#include "code.h"
#include "userdefs.h"
#include "stdfuncs.h"

void CodeSegment::Emit(const CodeElement &c)
{
  if (size == code.NoOfElements()) {
    bool b = code.Grow(10);
    assert(b);
  }
  code[size++] = c;
}

Complex CodeSegment::Run(const Complex &param, ErrorObj &error)
{
  CalcEnv env;
  env.eval_stack.Push(param);
  Run(env);
  error = env.run_error;
  if (env.eval_stack.Empty()) error += "Stack Underflow.";
  return env.eval_stack.Pop();
}

void CodeSegment::Run(CalcEnv &env) 
{
  int  		PC    = 0;			// Program counter
  Complex	param = env.eval_stack.Pop();	// Parameter
  pCodeSegment  f;                              // a function to evaluate

  if (setjmp(MathErrorJumpBuf) == -1) {
    env.run_error = cTRUE;
  }

  while ( ! env.run_error ) {
    switch(code[PC].opcode) {

      case cod_loadnumconst : env.eval_stack.Push(code[PC].constant);
			      break;

      case cod_loadparameter: env.eval_stack.Push(param);
			      break;

      case cod_loadsymconst : assert( ! (*userdefs)[code[PC].index].value.IsFunc() );
			      env.eval_stack.Push(
				(*userdefs)[ code[PC].index ].value.c_value
			      );
			      break;

      case cod_add          : env.eval_stack.Push( env.eval_stack.Pop() +
						   env.eval_stack.Pop() );
			      break;

      case cod_sub          : {
				Complex c1 = env.eval_stack.Pop();
				Complex c2 = env.eval_stack.Pop();
				env.eval_stack.Push(c2 - c1);
			      }
			      break;


      case cod_mult         : env.eval_stack.Push( env.eval_stack.Pop() *
						   env.eval_stack.Pop() );
			      break;


      case cod_divide       : {
				Complex c1 = env.eval_stack.Pop();
				Complex c2 = env.eval_stack.Pop();
				if (c1 == Complex(0.0,0.0))
				  env.run_error += "Division by zero";
				else
				  env.eval_stack.Push(c2/c1);
			      }
			      break;

      case cod_raise        : {
				Complex c1 = env.eval_stack.Pop();
				Complex c2 = env.eval_stack.Pop();
				if (c2 != Complex(0.0, 0.0))
				  env.eval_stack.Push( pow(c2, c1) );
				else
				  env.eval_stack.Push( Complex(0.0, 0.0) );
			      }
			      break;


      case cod_negate       : env.eval_stack.Push(-env.eval_stack.Pop());
			      break;

      case cod_return       : goto done;

      case cod_callstd      : env.eval_stack.Push(
				(*stdfuncs)[ code[PC].index ].value(env.eval_stack.Pop())
			      );
			      break;

      case cod_callusr      : assert( (*userdefs)[code[PC].index].value.IsFunc() );
			      f = (*userdefs)[ code[PC].index ].value.pCode;
			      f->Run(env);
			      break;

      default: env.run_error += "Unknown op-code";
    }
    PC++;
    if (MathRuntimeError())     env.run_error += "Math Exception.";
    if (env.eval_stack.Error()) env.run_error += "Evaluation Error.";
  }
done: ;
}

// EOF
