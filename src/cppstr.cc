///    X-Plot  --  a program for visualizing complex functions
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

//****************************************************************************
// C++ CppStr Class
//
// Features:
//	Higher-level handling of strings in C++.
//
//
//
// Martin Eskildsen, 07.11.93
//
// Revised 14.11.93: Operator >> is commented out; doesn't work with Signed
//		     or unsigned characters (the good, old problem...)
// Revised 16.11.93: CharPos() added.
// Revised 02.12.93: Bug in CharPos removed.
// Revised 08.12.93: operator + and += added for chars. SubCppStr modified.
//		     _CPPSTR_UNSIGNED should be defined for operator >>
//		     to be included.
// Revised 05.02.94: _CPPSTR_UNSIGNED removed! I'll have to live with it...
// Revised 26.02.94: CppStr(ch, size) constructor added.
// Revised 23.04.94: Bug in KillTerminatingChars() removed; didn't delete the
//		     first character in string when it should be deleted.
// Revised 08.09.94: Operator >> patched for bug in GNU C++ iostream lib.
//
//****************************************************************************

#include "cppstr.h"
#include <ctype.h>
#include <assert.h>

//****************************************************************************
// Constructors
//
// Features:
//
//	CppStr(ch, size) will generate a string containing 'size' similar
//	characters 'ch'.
//
//****************************************************************************

CppStr::CppStr(char ch, int size)
{
  s = new char[size+1];
  s[size] = '\0';
  memset(s, ch, size);
}

//****************************************************************************
// Operators << and >>
//
// Features:
//	Stream I/O. As Input is considered all chars (max 256) upto '\n'.
//	Output is *not* ended with '\n'.
//
//****************************************************************************

ostream& operator << (ostream& os, const CppStr &str)
{
  return os << str.s;
}

istream& operator >> (istream& is, CppStr &str)
{
  char temp[256];
#ifdef __GNUC__
  int ch; unsigned int i=0;
  while ( is.good()   &&   (ch = is.get()) != 0  &&  is.good()   &&   ch != '\n'   &&   i<sizeof(temp)-1 )
    temp[i++] = (char)ch;
  temp[i] = '\0';
#else
  is.getline(temp, 256, '\n');
#endif
  str.Alloc(temp);
  return is;
}


//****************************************************************************
// Insert, Delete, SubCppStr
//
// Features:
//	Inserts a string; deletes a substring; extracts a substring as new
//	object.
//
//****************************************************************************


CppStr& CppStr::Insert(const CppStr &str, int at_index)
{
  int l  = len();
  int sl = str.len();

  if (at_index > l) at_index = l; else		// append to end if at_index too large
  if (at_index < 0) at_index = 0;		// insert at head if at_index too small

  char *temp = new char [l + str.len() + 1];

  strcpy(temp, s);
  strcpy(&temp[at_index], str.s);
  strcpy(&temp[at_index+sl], &s[at_index]);

  delete [] s;
  s = temp;

  return *this;
}

CppStr& CppStr::Delete(int from_index, int length)
{
  if (length > 0) {
    int l = len();

    if (from_index < 0) from_index = 0;		// if negative start, remove from head

    if (from_index < l) {
      char *temp = new char[l];
      s[from_index] = '\0';
      strcpy(temp, s);
      if (from_index+length < l) strcat(temp, &s[from_index+length]);
      delete [] s;
      s = new char[strlen(temp)+1];
      strcpy(s, temp);
      delete [] temp;
    }
  }

  return *this;
}

CppStr CppStr::SubString(int from_index, int to_index) const
{
  if (from_index>len() || to_index<from_index || to_index<0 || from_index<0) return CppStr("");

  CppStr result(&s[from_index]);
  result.Delete(to_index-from_index+1, result.len());

  return result;
}

//****************************************************************************
// Operators += and +
//
// Features:
//	Concatenates strings.
//
//****************************************************************************

CppStr& CppStr::operator += (const CppStr &str)
{
  char *temp = new char [strlen(s) + strlen(str.s) + 1];
  strcpy(temp, s);
  strcat(temp, str.s);
  delete [] s;
  s = temp;
  return *this;
}

CppStr operator + (const CppStr& s1, const CppStr& s2)
{
  CppStr result = s1;
  result += s2;
  return result;
}

CppStr& CppStr::operator += (const char ch)
{
  int l = len();
  char *temp = new char [l + 1 + 1];
  strcpy(temp, s);
  temp[l]   = ch;
  temp[l+1] = '\0';
  delete [] s;
  s = temp;
  return *this;
}

CppStr operator + (const CppStr& s, const char ch)
{
  CppStr result = s;
  result += ch;
  return result;
}

//****************************************************************************
// Operator =
//
// Features:
//	Assigns strings.
//
//****************************************************************************

CppStr& CppStr::operator = (const CppStr& str)
{
  if (&str != this) Alloc(str.s);
  return *this;
}

CppStr& CppStr::operator = (const char ch)
{
  char str[2]=" "; str[0]=ch;
  *this += str;
  return *this;
}

//****************************************************************************
// UpCaseFirst, UpCase, LowCase
//
// Features:
//	Convers first char in string to upper case; convert all of string to
//	upper case; convert all of string to lower case.
//
//****************************************************************************


CppStr& CppStr::UpCaseFirst()
{
  if (exists()) s[0] = toupper(s[0]);
  return *this;
}

CppStr& CppStr::UpCase()
{
  for (int i=0; s[i]; i++) s[i] = toupper(s[i]);
  return *this;
}

CppStr& CppStr::LowCase()
{
  for (int i=0; s[i]; i++) s[i] = tolower(s[i]);
  return *this;
}

//****************************************************************************
// KillTerminatingChars, KillTerminatingSpaces,
// KillInitiatingChars,  KillInitiatingSpaces
//
// Features:
//	Kills chars at head or tail of string.
//
//****************************************************************************

CppStr&	CppStr::KillTerminatingChars(const char ch_to_kill)
{
  if (exists()) {
    int i;
    for (i=0; s[i]; i++) ;			// go to end
    i--;					// skip back over \0
    for (; i>=0 && s[i]==ch_to_kill; i--) ;	// skip back over ch_to_kill
    i++;					// point at first ch_to_kill
    s[i] = '\0';				// terminate string here
    char *temp = new char[i+1];			// create new string
    strcpy(temp, s);				// and copy old into it
    delete [] s;				// delete old
    s = temp;					// let old point to new.
  }
  return *this;
}

CppStr& CppStr::KillInitiatingChars(const char ch_to_kill)
{
  if (exists()) {
    char *p;
    for (p=s; *p == ch_to_kill; p++) ;		// now, *p != ch_to_kill
    char *temp = new char [strlen(p)+1];	// create new string
    strcpy(temp, p);				// copy p into new str.
    delete [] s;				// delete old
    s = temp;					// let old point to new.
  }
  return *this;
}

//****************************************************************************
// CharOccurs, CharPos, SubstituteChar
//
// Features:
//	CharOccurs: Returns the number of times, a specific char occurs in
//		    the string.
//	CharPos:    Returns the index of the first occurence of a specific
//		    char in the string or -1 if no match.
//      SubstituteChar: Change all occurances of one char to another char
//
//****************************************************************************

int CppStr::CharOccurs(const char ch) const
{
  int n=0;

  for (int i=0; s[i]; i++) if (s[i]==ch) n++;

  return n;
}

int CppStr::CharPos(const char ch) const
{
  int i;
  for (i=0; s[i] && (s[i] != ch); i++) ;
  return s[i] ? i : -1;
}

CppStr& CppStr::SubstituteChar(const char from, const char to)
{
  char *p = s;
  while (*p) {
    if (*p == from) *p = to;
    p++;
  }
  return *this;
}

//****************************************************************************
// Alloc
//
// Features:
//	Private; allocation of memory for (char*) s.
//
//****************************************************************************

void CppStr::Alloc(const char *str)
{
  delete [] s;
  s = new char[strlen(str)+1];
  strcpy(s, str);
}

// EOF

