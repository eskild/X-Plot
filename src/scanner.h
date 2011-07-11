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

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "defs.h"
#include "cppstr.h"

/* The Scanner Class.
 *
 * The scanner views its input as a line-based character stream, ie. it reads
 * lines, but these lines are then considered as character-streams.
 * Whenever an error occurs, ScanError returns cTRUE and ScanErrorPosition will
 * return the last successfully scanned position in the line, ie. the error
 * must have occured somewhere after that position. Note that ScanError only
 * returns cTRUE once per error, ie. the error flag is cleared after each call.
 */

CLASSDEF(Scanner);
class Scanner {
public:

  enum Token {
	       tok_id, tok_number, tok_string,
	       tok_plus, tok_minus, tok_star, tok_slash, tok_caret,
	       tok_equal, tok_comma, tok_semicolon,
	       tok_lpar, tok_rpar,	// ( )
	       tok_lbrk, tok_rbrk,	// [ ]
	       tok_lbrc, tok_rbrc,	// { }
	       tok_eoln, tok_eof,
	       tok_other, tok_nothing };

			Scanner();
  virtual              ~Scanner();

		void	Init();

		// Token generation:
		Token   GetNextToken();
		Token	GetCurrentToken()	{ return tok; }

		// Token attributes:
		double  GetNumber()             { return number; }
		CppStr  GetId()                 { return id; }
		CppStr  GetCppStr()             { return str; }

		// These are for error handling:
		CppStr  GetCopyOfScanLine()     { return line; }
		int     GetErrorPosition()	{ return prev_linepos; }
		bool    ScanError();

protected:
  virtual       CppStr  GetScanLine() = 0;	// touches 'eof'

  bool          errorstate;
  bool		eof;

private:
  void          GetNextCh();
  const char    eoln_ch;

  void          ScanSpaces();
  void          ScanNumber();
  void          ScanId();
  void          ScanCppStr();
  int           ScanSign();

  void          ScanReserved();         // tok_XXX where XXX is in CAPITALS

  CppStr        line, id, str;
  int           linepos, prev_linepos, line_length;
  char          ch;
  double        number;

  Token         tok;
};
/* line:        the line just read by GetScanLine
 * line_length: the length of line
 * linepos:     current position in line (modified by GetNextToken)
 * prev_linepos:position in line immediately before call of GetNextToken
 * ch:          char being processed
 * number:      number just read; returned by GetNumber
 * id:          identifier just read; returned by GetId
 * str:         string just read; returned by GetCppStr
 * errorstate:  cTRUE when a scanner error has occured; cFALSE otherwise
 * tok:         Token being generated
 *
 * GetScanLine: Return next line of input.
 * Init:	Should be called before next parsing if error occured
 */

#endif
