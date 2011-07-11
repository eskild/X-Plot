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

//****************************************************************************
// C++ CppStr Class
//
// Features:
//
//
//
//
// Martin Eskildsen, 07.11.93
//
// Revised 14.11.93: Ptr() member function added.
// Revised 16.11.93: CharPos() member function added.
// Revised 26.02.94: Added CppStr(ch, size) constructor.
// Revised 20.09.94: Added SubstituteChar() function.
//
//****************************************************************************

#ifndef __CPPSTR_H__
#define __CPPSTR_H__

#include <string.h>
#include <iostream>
#include <ctype.h>

using namespace std;

class CppStr {
public:
// Constructors:
		CppStr()			: s(0) { Alloc(""); }
		CppStr(int size)		       { s = new char [size];  s[0]='\0'; }
		CppStr(const char *str)		: s(0) { Alloc(str);   }
		CppStr(const CppStr &str)	: s(0) { Alloc(str.s); }
		CppStr(char ch, int size);	// CppStr of 'size' chars 'ch'
// Destructor:
	virtual	~CppStr()			{ delete [] s; }
// Case conversion:
	CppStr&	UpCaseFirst();
	CppStr&	UpCase();
	CppStr&	LowCase();
// Manipulation: Removing a specific char from tail of string:
	CppStr&	KillTerminatingChars(const char ch_to_kill);
	CppStr&	KillTerminatingSpaces()		{ return KillTerminatingChars(' '); }
// Manipulation: Removing a specific char from head of string:
	CppStr&	KillInitiatingChars(const char ch_to_kill);
	CppStr&	KillInitiatingSpaces()		{ return KillInitiatingChars(' '); }
// Manipulation: Substrings:
	CppStr&	Insert(const CppStr&, int at_index);
	CppStr&	Delete(int from_index, int len);
	CppStr	SubString(int from_index, int to_index) const;
// Manipulation: Substitute one char with another:
        CppStr& SubstituteChar(const char from, const char to);
// Predicates:
	int	CharOccurs(const char ch) const;
	int	CharPos(const char ch) const;
	int	exists() const			{ return (*s != '\0'); }
// Length:
	int	len() const			{ return strlen(s); }
// Assignment:
	CppStr&	operator = (const char *str)	{ Alloc(str);   return *this; }
	CppStr& operator = (const CppStr &str);
	CppStr&	operator = (const char ch);
	CppStr& operator +=(const CppStr&);
	CppStr&	operator +=(const char);
friend	CppStr	operator + (const CppStr&, const CppStr&);
friend	CppStr	operator + (const CppStr&, const char);
// Indexing:
	char&	operator [](const int n)	{ return s[n]; }
// Pointer:
	const	char*	 Ptr() const		{ return s; }
// Comparison:
	int	operator < (const CppStr &str) const { return strcmp(s, str.s) < 0; }
	int	operator > (const CppStr &str) const { return strcmp(s, str.s) > 0; }
	int	operator ==(const CppStr &str) const { return strcmp(s, str.s)== 0; }
	int	operator !=(const CppStr &str) const { return strcmp(s, str.s)!= 0; }
	int	operator <=(const CppStr &str) const { return strcmp(s, str.s)<= 0; }
	int	operator >=(const CppStr &str) const { return strcmp(s, str.s)>= 0; }
// Stream I/O:
friend	ostream& operator << (ostream&, const CppStr&);
friend	istream& operator >> (istream&, CppStr&);

private:
	void	Alloc(const char *str);
	char	*s;
};

#endif
