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

#include "parsetre.h"

ParseTree::~ParseTree()
{
  if (l) delete l;
  if (r) delete r;
}

Complex ParseTree::Evaluate(ErrorObj &e)
{
  switch(nodetype) {
    case Scanner::tok_plus  : if (l && r) return l->Evaluate(e) + r->Evaluate(e);
			      else break;
    case Scanner::tok_minus : if (r) 
    				if (l == 0) 
    				  return - r->Evaluate(e);
    				else 
    				  return l->Evaluate(e) - r->Evaluate(e);
    			      else break;
    case Scanner::tok_star  : if (l && r) return l->Evaluate(e) * r->Evaluate(e);
			      else break;
    case Scanner::tok_caret : if (l && r) return pow(l->Evaluate(e), r->Evaluate(e));
			      else break;
    case Scanner::tok_slash : if (l && r) {
				Complex c1 = l->Evaluate(e);
				Complex c2 = r->Evaluate(e);
				if (c2 == Complex(0.0, 0.0)) {
				  e += "Division by zero";
				  return Complex(1.0,0.0);
				}
				else return c1/c2;
			      }
			      else break;
    case Scanner::tok_number: return number;

    default: assert(0);
  }

  // If we ever get here, it's because the tree is corrupt!
  if (!l || !r)
    e += "System Error: Constant-Tree branch == 0";
  else
    e += "System Error: Unknown Constant-Tree node type";
  return Complex(1.0, 1.0);
}


pCodeSegment ParseTree::GenerateCode(ErrorObj &e)
{
  pCodeSegment cs = new CodeSegment;  assert(cs != 0);

  doGenerateCode(cs, e);
  cs->Emit( CodeElement(cod_return) );

  return cs;
}

// In the below, all "break;"'s are used if an error occured
void ParseTree::doGenerateCode(pCodeSegment cs, ErrorObj &e)
{
  switch(nodetype) {
    case Scanner::tok_plus  : if (l && r) {
				l->doGenerateCode(cs, e);
				r->doGenerateCode(cs, e);
				cs->Emit( CodeElement(cod_add) );
				return;
			      }
			      break;
    case Scanner::tok_minus : if (r) {
				if (l == 0) {
				  r->doGenerateCode(cs, e);
				  cs->Emit( CodeElement(cod_negate) );
				}
				else {
				  l->doGenerateCode(cs, e);
				  r->doGenerateCode(cs, e);
				  cs->Emit( CodeElement(cod_sub) );
				}
				return;
			      }
			      break;
    case Scanner::tok_star  : if (l && r) {
				l->doGenerateCode(cs, e);
				r->doGenerateCode(cs, e);
				cs->Emit( CodeElement(cod_mult) );
				return;
			      }
			      break;
    case Scanner::tok_caret : if (l && r) {
				l->doGenerateCode(cs, e);
				r->doGenerateCode(cs, e);
				cs->Emit( CodeElement(cod_raise) );
				return;
			      }
			      break;
    case Scanner::tok_slash : if (l && r) {
				l->doGenerateCode(cs, e);
				r->doGenerateCode(cs, e);
				cs->Emit( CodeElement(cod_divide) );
				return;
			      }
			      break;
    case Scanner::tok_number: cs->Emit( CodeElement(number) );
			      return;

    case Scanner::tok_id    : if (! l) {	// (l != 0)  =>  error
				switch(id_attr) {
				  case id_is_stdfunc:
					if (r) r->doGenerateCode(cs, e); else break;
					cs->Emit( CodeElement(cod_callstd, index) );
					return;
				  case id_is_func:
					if (r) r->doGenerateCode(cs, e); else break;
					cs->Emit( CodeElement(cod_callusr, index) );
					return;
				  case id_is_const:
					cs->Emit( CodeElement(cod_loadsymconst, index) );
					return;
				  case id_is_param:
					cs->Emit( CodeElement(cod_loadparameter) );
					return;
				  default: e += "System Error: Unknown Code-Tree id type";
				}
			      }
			      break;
    default: assert(0);
  }

  // If we ever get here, it's because the tree is corrupt!
  if (!l || !r)
    e += "System Error: Code-Tree branch == 0";
  else
    e += "System Error: Unknown Code-Tree node type";
}


// EOF
