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

#include "scanner.h"
#include "stdbase.h"
#include <math.h>
#include <ctype.h>

class _ReservedRec : public StdBase<Scanner::Token> {
public:
		_ReservedRec()	{ }
		_ReservedRec(const CppStr &n, Scanner::Token val)
		: StdBase<Scanner::Token>(n, val)
		{
		}
};

class _Reserved : public StdTableBase<_ReservedRec> {
public:
	_Reserved()
	{
	  table.Grow(0);
	//  table[ ] = ReservedRec("", Scanner::tok_);
	// NOTE: The table must be alphabetically ordered!
	}
};

static _Reserved* _reserved = 0;

template class StdTableBase<_ReservedRec>;
template class DynaArray<_ReservedRec>;
template class DynaArrayWithOrdering<_ReservedRec>;

Scanner::Scanner() : eoln_ch('\n')
{
  Init();
  _reserved = new _Reserved;
  assert(_reserved != 0);
}

Scanner::~Scanner()
{
  delete _reserved;
}

void Scanner::Init()
{
  line		= "";
  line_length	= 0;
  linepos	= 0;
  id		= "";
  str		= "";
  prev_linepos	= 0;
  ch		= ' ';
  errorstate	= cFALSE;
  eof		= cFALSE;
  number	= 0.0;
  tok		= tok_nothing;
}

bool Scanner::ScanError()
{
  bool b = errorstate;
  errorstate = cFALSE;
  return b;
}

void Scanner::GetNextCh()
{
  if (eof) {
    errorstate = cTRUE;  ch = '\0';
    return;
  }

  if (linepos == line_length) {			// at eoln
    linepos++;
    ch = ' ';
  }
  else {
    if (linepos > line_length) {		// read new line
      do {
	line = GetScanLine();
	line_length = line.len();                 // using line_length instead of line.len() is faster
      } while (!eof && !errorstate && line_length == 0);
      linepos = prev_linepos = 0;
    }

    ch = eof ? eoln_ch : line[linepos++];
  }
/*
  if (eof) {
    errorstate = cTRUE;  ch = '\0';
    return;
  }

  if (linepos == line_length) {			// at eoln
    linepos++;
    ch = eoln_ch;
  }
  else {
    if (linepos > line_length) {		// read new line
      do {
	line = GetScanLine();
	line_length = line.len();                 // using line_length instead of line.len() is faster
      } while (!eof && !errorstate && line_length == 0);
      linepos = prev_linepos = 0;
    }

    ch = eof ? eoln_ch : line[linepos++];
  }
*/
}

void Scanner::ScanSpaces()
{
  while (ch == ' ' && !errorstate && !eof) GetNextCh();
}

/* ScanId: Scan identifier according to the syntax:
 *
 *      id = letter { letter | digit | '_' } .
 *
 * Entry: ch  = first letter
 * Exit : ch  = char following identifier
 *        id  = identifier
 *        tok = tok_id
 */
void Scanner::ScanId()
{
  id = "";
  while (isalpha(ch) || isdigit(ch) || (ch == '_')) {
    id += ch;
    GetNextCh();
  }
  tok = tok_id;
}

/* ScanReserved: Scan reserved word:
 *
 * Entry: ch  = first letter == '_'
 * Exit : ch  = char following identifier
 *        id  = identifier
 *        tok = tok_xxx
 */

void Scanner::ScanReserved()
{
  ScanId();
  tok = tok_other;      // default value if no reserved word found
  if (!errorstate) {
    DynaIdx i = _reserved->BinaryFind(id); 
    if (i == -1)		// not a reserved word
      errorstate = cTRUE;
    else
      tok = (*_reserved)[i].value;
  }
  // As we only got here because of an underscore ('_') and only reserved
  // words are allowed to start with an underscore, there must be an error
  // if the word couldn't be found in the table.
}

/* ScanNumber: Scan number according to the syntax:
 *
 *      number          = mantissa exponent .
 *      sign            = '+' | '-' | empty .
 *      mantissa        = digitsequence [ '.' digitsequence ] .
 *      exponent        = ('e' | 'E') sign digitsequence | empty .
 *
 * Entry: ch                = first character: digit.
 * Exit : ch                = char following number
 *        number            = the number
 *        tok               = tok_number
 */
void Scanner::ScanNumber()
{
  number = 0.0;

  // mantissa: first digitsequence:
  while (isdigit(ch)) {
    number = number*10.0 + (ch - '0');
    GetNextCh();
  }

  if (ch == '.') {      // second digitsequence: fractional part
    GetNextCh();
    if (!isdigit(ch)) { errorstate = cTRUE; return; }
    double factor = 1.0;
    while (isdigit(ch)) {
      factor *= 10.0;
      number += (ch - '0') / factor;
      GetNextCh();
    }
  }

  if (ch == 'e'  ||  ch == 'E') {  // exponent:
    GetNextCh();
    int sign = ScanSign();

    double exponent = 0.0;
    if (!isdigit(ch)) { errorstate = cTRUE; return; }
    while (isdigit(ch)) {
      exponent = exponent*10.0 + (ch - '0');
      GetNextCh();
    }

    number *= pow(10.0, sign*exponent);
  }
  tok = tok_number;
}

int Scanner::ScanSign()
{
  if (ch == '-') {
    GetNextCh();
    return -1;
  }
  else {
    if (ch == '+') GetNextCh();
    return 1;
  }
}

/* ScanCppStr: Scan string according to the syntax:
 *
 *      string = '"' { chars } '"' .
 *
 * Entry: ch  = first '"'
 * Exit : ch  = char following last '"'
 *        str = the string, without '"'
 *        tok = tok_string
 */
void Scanner::ScanCppStr()
{
  str = "";
  GetNextCh();          // skip first '"'
  while (ch != '"'  &&  ch != eoln_ch  &&  !errorstate  &&  !eof) {
    str += ch;
    GetNextCh();
  }
  if (ch != '"') { errorstate = cTRUE; return; }
  GetNextCh();          // skip last '"'
}


Scanner::Token Scanner::GetNextToken()
{
  ScanSpaces();
  prev_linepos = linepos;
  switch(ch) {
    case '_'    : ScanReserved();                                       break;
    case '"'    : ScanCppStr();                                         break;
    case '+'    : tok = tok_plus;               GetNextCh();            break;
    case '-'    : tok = tok_minus;              GetNextCh();            break;
    case '*'    : tok = tok_star;               GetNextCh();            break;
    case '/'    : tok = tok_slash;              GetNextCh();            break;
    case '^'    : tok = tok_caret;              GetNextCh();            break;
    case '('    : tok = tok_lpar;               GetNextCh();            break;
    case ')'    : tok = tok_rpar;               GetNextCh();            break;
    case '['    : tok = tok_lbrk;               GetNextCh();            break;
    case ']'    : tok = tok_rbrk;               GetNextCh();            break;
    case '{'    : tok = tok_lbrc;               GetNextCh();            break;
    case '}'    : tok = tok_rbrc;               GetNextCh();            break;
    case ';'    : tok = tok_semicolon;		GetNextCh();            break;
    case ','    : tok = tok_comma;              GetNextCh();            break;
    case '='    : tok = tok_equal;              GetNextCh();            break;
    case '\0'   : tok = tok_eof;					break;
    default     : if (ch == eoln_ch) {
		    if (tok == tok_eoln) {
		      GetNextCh();
		      return GetNextToken();
		    }
		    else tok = tok_eoln;
		  }
		  else
		    if (isalpha(ch)) ScanId();
		    else
		      if (isdigit(ch)) ScanNumber();
		      else {
			errorstate = cTRUE;
			tok = tok_other;
			GetNextCh();
		      }
  }
  return tok;
}

// EOF
