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

#ifndef __DEFS_H_XPLOT_
#define __DEFS_H_XPLOT_

#ifndef __DBGFILE_H__
#include "dbgfile.h"
#endif

//enum bool { cFALSE, cTRUE };
#define cTRUE true
#define cFALSE false

#define CLASSDEF(name)	class name;	\
			typedef name * p##name;	\
			typedef	name & r##name;

extern const char *VERSION;   // The version string. To be found in x.cc.

extern const char *UserDir;   // Default path for user's files. Also located in x.cc.

template <class T>
inline const T Abs(T x)
{
  return (x>0) ? x : -x;
}

template <class T>
inline const T Min(T a, T b)
{
  return (a<b) ? a : b;
}

template <class T>
inline const T Max(T a, T b)
{
  return (a>b) ? a : b;
}

#endif

// EOF
