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

// Dynamically expandable array.

#ifndef __DYNAARRAY_H__
#define __DYNAARRAY_H__

#include "defs.h"
#include <string.h>
#include <assert.h>

typedef int DynaIdx;

template <class T>
class DynaArray {
public:
			DynaArray();
			DynaArray(DynaIdx initial_size, bool delete_when_destructed);
	virtual        ~DynaArray();

	bool		Grow(DynaIdx delta);
	void		Empty();

	DynaIdx		NoOfElements()			{ return size; }

	T&		operator [] (DynaIdx);

protected:
	T		*a;
	DynaIdx		size;
        bool		del_when_destructed;
};

template <class T>     	// T must have '<=', '>=' etc.
class DynaArrayWithOrdering : public DynaArray<T> {
public:
			DynaArrayWithOrdering() : DynaArray<T>()
			{ }

			DynaArrayWithOrdering(DynaIdx initial_size, bool delete_when_destructed)
			: DynaArray<T>(initial_size, delete_when_destructed)
			{ }

	DynaIdx		BinFind(const T&);	// Binary search
	DynaIdx		LinFind(const T&);	// Linear search
};

//-------------------- DynaArray --------------------------------------------

template <class T>
DynaArray<T>::DynaArray(DynaIdx s, bool d_w_d) : a(0), size(s), del_when_destructed(d_w_d)
{
  if (s > 0) {
    a = new T [size];
    assert(a != 0);
  }
}

template <class T>
DynaArray<T>::DynaArray() : a(0), size(0), del_when_destructed(cTRUE)
{
}

template <class T>
DynaArray<T>::~DynaArray()
{
  if (del_when_destructed) delete [] a;
}

template <class T>
bool DynaArray<T>::Grow(DynaIdx delta)
{
  T *b = new T [size + delta];
  if (b == 0) return cFALSE;
  memcpy(b, a, size*sizeof(T));
  delete [] a;
  a = b;
  size += delta;
  return cTRUE;
}

template <class T>
void DynaArray<T>::Empty()
{
  delete [] a;
  a = 0;
  size = 0;
}

template <class T>
T& DynaArray<T>::operator [] (DynaIdx index)
{
  assert(index < size);
  return a[index];
}

//-------------------- DynaArrayWithOrdering --------------------------------

template <class T>
DynaIdx DynaArrayWithOrdering<T>::LinFind(const T &x)
{
  DynaIdx i = 0;
  while (i<this->size  &&  !(x == this->a[i])) i++;
  return (i == this->size) ? -1 : i;
}

template <class T>
DynaIdx DynaArrayWithOrdering<T>::BinFind(const T &x)
{
  if (this->size == 0) return -1;

  DynaIdx i=0, j=this->size-1, mid;

  do {
    mid = (i+j) / 2L;
    if (x <= this->a[mid]) j = mid-1;
    if (x >= this->a[mid]) i = mid+1;
  } while (i <= j);

  return (i > j+1) ? mid : -1;
}

#endif

// EOF
