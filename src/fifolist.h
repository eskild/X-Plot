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

#ifndef __FIFOLIST_H__
#define __FIFOLIST_H__

class FifoList;
class DestructiveFifoList;
class FifoListElement;

typedef FifoList*		pFifoList;
typedef	DestructiveFifoList*	pDestructiveFifoList;
typedef FifoListElement*	pFifoListElement;



//****************************************************************************
// FifoListElement
//
// Features:
//	Base class for elements to be inserted in FifoLists.
//
//
//
//
//
//****************************************************************************

class FifoListElement {
public:
	virtual		~FifoListElement()	{ }
private:
	pFifoListElement	next;

	friend class 		FifoList;
	friend class 		DestructiveFifoList;
};

//****************************************************************************
// FifoList
//
// Features:
//	FIFO list queue. Is a list of pointers to FifoListElements. The list
//	can be iterated by InitNext() and Next() member functions.
//	When the object is destroyed, the elements pointed to by the list
//	are NOT destroyed. DestructiveFifoList, below, does this.
//
//
//****************************************************************************

class FifoList {
public:
				FifoList();
				FifoList(const FifoList&);

	virtual			~FifoList()	{ }

	void			operator += (const pFifoListElement);

	void			InitNext();
	pFifoListElement	Next();

	int			Size()	const	{ return sz; }

protected:
	pFifoListElement	head, tail, next;
	int			sz;
};

//****************************************************************************
// DestructiveFifoList
//
// Features:
//	Same operations as FifoList, only differences are: The elements in the
//      list are destroyed when the object is destroyed; the copy constructor
//	is implemented as an assert() to ensure that these objects aren't
//	copied. This should prevent accidental destruction of the list con-
//	tents.
//
//****************************************************************************

class DestructiveFifoList : public FifoList {
public:
			DestructiveFifoList() : FifoList() { }
	virtual		~DestructiveFifoList();
private:
			DestructiveFifoList(const DestructiveFifoList&);
};

#endif

// EOF
