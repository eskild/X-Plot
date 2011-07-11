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

#include "fifolist.h"
#include <assert.h>

FifoList::FifoList() : head(0), tail(0), sz(0)
{
}

FifoList::FifoList(const FifoList& lst)
{
  head = lst.head;
  tail = lst.tail;
  sz   = lst.sz;
}

void FifoList::operator += (const pFifoListElement p)
{
  assert(p != 0);
  if (head == 0)	// empty list:
    head = p;
  else			// non-empty list
    tail->next = p;
  tail = p;		// move tail
  p->next = 0;		// mark end-of-list
  sz++;
}

void FifoList::InitNext()
{
  next = head;
}

pFifoListElement FifoList::Next()
{
  pFifoListElement p = next;
  if (next != 0) next = next->next;
  return p;
}


DestructiveFifoList::DestructiveFifoList(const DestructiveFifoList&)
{
  assert(0);
}


DestructiveFifoList::~DestructiveFifoList()
{
  pFifoListElement p;
  while (head != 0) {
    p    = head;
    head = head->next;
    delete p;
  }
}

// EOF
