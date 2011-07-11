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

#ifndef __LIST_H__
#define __LIST_H__

//****************************************************************************
// LinkedList (template)
//
// Features:
//	Supports a linked-list structure with insertions at the head of the
//	list. Items can be removed from the list if their value is given.
//	In case of duplicated items, the LAST ENTERED item will be removed.
//
//
//
//****************************************************************************

template <class type> class LinkedList {
public:

			LinkedList()	{ head = 0; }
	virtual		~LinkedList();

	bool		Add(const type&);
	type		Delete(const type&);	// The return value is useful
						// if type is a pointer to
						// something. Eg. you can call
						// like this:
						//     delete Delete(myptr);
						// instead of:
						//     Delete(myptr);
						//     delete myptr;
private:
	class Item {
	public:
	  type	value;
	  Item  *next;
	};

	Item	*head;

	friend class LinkedListIterator<type>;	// accesses head and class Item
};

//****************************************************************************
// LinkedLinkedListIterator (template)
//
// Features:
//	Implements an iterator for a LinkedList. When elements have been
//	added to the linked list, Init() should be called to initiate private
//	pointers. The iterator constructor takes a linked linked as parameter.
//
//
//
//****************************************************************************

template <class type> class LinkedListIterator {
public:

			LinkedListIterator(LinkedList<type> &lst)
			: list(&lst), head(lst.head), ptr(head)
			{ }

	LinkedListIterator	&Init()	{ ptr = head = list->head; return *this; }

	type*		operator () ();

private:
	LinkedList<type>::Item	*ptr, *head;
	LinkedList<type>	*list;
};

//****************************************************************************
// Implementations:
//****************************************************************************

//****************************************************************************
// LinkedList
//****************************************************************************

template <class type> LinkedList<type>::~LinkedList()
{
  while (head) Delete(head->value);
}

template <class type> bool LinkedList<type>::Add(const type &item)
{
  Item *i;

  if ( (i = new Item) == 0 ) return cFALSE;

  i->next  = head;
  i->value = item;
  head = i;

  return cTRUE;
}

template <class type> type LinkedList<type>::Delete(const type &item)
{
  Item *h = head;
  if (h == 0) return 0;

  type t;
  if (h->value == item) {	// deleting head
    t = head->value;
    head = head->next;
    delete h;
  }
  else {			// deleting item down-list
    while (h->next && (h->next->value != item)) h = h->next;
    if (h->next) {
      Item *temp = h->next;
      h->next = h->next->next;
      t = temp->value;
      delete temp;
    }
  }
  return t;
}

//****************************************************************************
// LinkedListIterator
//****************************************************************************

template <class type> type *LinkedListIterator<type>::operator () ()
{
  type *result;
  if (ptr == 0) {
    result = 0;
    ptr = head;
  }
  else {
    result = &ptr->value;
    ptr = ptr->next;
  }
  return result;
}

#endif

// EOF
