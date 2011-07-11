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

#include "stdfuncs.h"
#include <stdlib.h>

pStdFuncs stdfuncs;

// return pseudo-random number in the range [0..1]
static inline double _random()
{
  return double(rand()) / RAND_MAX;
}

static Complex std_real(const Complex &c)	{ return Complex(::real(c), 0.0); }
static Complex std_imag(const Complex &c)	{ return Complex(::imag(c), 0.0); }
static Complex std_arg(const Complex &c)	{ return Complex(arg(c),  0.0); }
static Complex std_abs(const Complex &c)	{ return Complex(sqrt(norm(c)), 0.0); }
static Complex std_rand(const Complex &c)	{ return Complex(_random(), _random()) * c; }

StdFuncs::StdFuncs()
{
#ifdef __BORLANDC__
  table.Grow(19);
//  table[ ] = ComplexFuncRec("", );
  table[ 0] = ComplexFuncRec("Arg",	::std_arg);
  table[ 1] = ComplexFuncRec("abs",	::std_abs);
  table[ 2] = ComplexFuncRec("acos",	::acos);
  table[ 3] = ComplexFuncRec("asin",	::asin);
  table[ 4] = ComplexFuncRec("atan",	::atan);
  table[ 5] = ComplexFuncRec("conj",	::conj);
  table[ 6] = ComplexFuncRec("cos",	::cos);
  table[ 7] = ComplexFuncRec("cosh",	::cosh);
  table[ 8] = ComplexFuncRec("exp",	::exp);
  table[ 9] = ComplexFuncRec("im", 	::std_imag);
  table[10] = ComplexFuncRec("log",	::log);
  table[11] = ComplexFuncRec("log10",	::log10);
  table[12] = ComplexFuncRec("random",  ::std_rand);
  table[13] = ComplexFuncRec("re", 	::std_real);
  table[14] = ComplexFuncRec("sin",	::sin);
  table[15] = ComplexFuncRec("sinh",	::sinh);
  table[16] = ComplexFuncRec("sqrt",	::sqrt);
  table[17] = ComplexFuncRec("tan",	::tan);
  table[18] = ComplexFuncRec("tanh",	::tanh);
#else
  // GNU C++
  table.Grow(13);
  table[ 0] = ComplexFuncRec("Arg",	std_arg);
  table[ 1] = ComplexFuncRec("abs",	std_abs);
  table[ 2] = ComplexFuncRec("conj",	&conj);
  table[ 3] = ComplexFuncRec("cos",	&cos);
  table[ 4] = ComplexFuncRec("cosh",	cosh);
  table[ 5] = ComplexFuncRec("exp",	exp);
  table[ 6] = ComplexFuncRec("im", 	std_imag);
  table[ 7] = ComplexFuncRec("log",	log);
  table[ 8] = ComplexFuncRec("random",  std_rand);
  table[ 9] = ComplexFuncRec("re", 	std_real);
  table[10] = ComplexFuncRec("sin",	sin);
  table[11] = ComplexFuncRec("sinh",	sinh);
  table[12] = ComplexFuncRec("sqrt",	sqrt);
#endif
}

// EOF
