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

#ifndef __STDBASE_H__
#define __STDBASE_H__

#include "nameclas.h"
#include "dynaarra.h"
#include <Complex.h>

template <class ValueClass>
class StdBase : public NameClass {
public:
		StdBase()	{ }
		StdBase(const CppStr &n, const ValueClass &val)
		: NameClass(n), value(val)
		{
		}


	bool	operator == (const StdBase &n) const	{ return bool(name == n.name); }
	bool	operator <  (const StdBase &n) const	{ return bool(name <  n.name); }
	bool	operator >  (const StdBase &n) const	{ return bool(name >  n.name); }
	bool	operator <= (const StdBase &n) const	{ return bool(name <= n.name); }
	bool	operator >= (const StdBase &n) const	{ return bool(name >= n.name); }

	ValueClass	value;
};

template <class RecClass>	// RecClass is a StdBase derivate.
class StdTableBase {
public:
                        StdTableBase()   { }
        virtual        ~StdTableBase()   { }
	virtual DynaIdx	BinaryFind(const CppStr &name);
	virtual DynaIdx	LinearFind(const CppStr &name);

	// Read/Write access to table
	virtual RecClass& operator [] (DynaIdx index) { return table[index]; }

protected:
	DynaArrayWithOrdering<RecClass>	table;
};

template <class RecClass>	// RecClass is a StdBase derivate.
class StdTableWithInsert : public StdTableBase<RecClass> {
public:
 StdTableWithInsert() : StdTableBase<RecClass>(), size(0) { }

	// Insertion operator
	virtual DynaIdx Insert(const RecClass &);
	// Don't do binary search, as insertions are unordered!

protected:
	DynaIdx		size;		// no. of elements inserted

};

template <class RecClass>
DynaIdx StdTableBase<RecClass>::BinaryFind(const CppStr &name)
{
  RecClass r;  r.name = name;
  return table.BinFind(r);
}

template <class RecClass>
DynaIdx StdTableBase<RecClass>::LinearFind(const CppStr &name)
{
  RecClass r;  r.name = name;
  return table.LinFind(r);
}

template <class RecClass>
DynaIdx StdTableWithInsert<RecClass>::Insert(const RecClass &r)
{
  if (size == this->table.NoOfElements()) this->table.Grow(10);
  this->table[size] = r;
  return size++;
}

#endif
