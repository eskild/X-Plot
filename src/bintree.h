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

#ifndef __BINTREE_H__
#define __BINTREE_H__

#include <iostream>

const int OK 	     = 1;
const int KEY_EXISTS = 0;

template <class keytype, class datatype> class Node {
public:

			Node() { }

			Node(keytype &k, datatype &d) : data(d)
			{
			  key = k;
			}

			Node(const Node<keytype,datatype>&n) : data(n.data)
			{
			  key = n.key;
			}

	keytype		key;
	datatype	data;

private:
	Node		*l, *r;
	friend	class	Tree<keytype, datatype>;
};


template <class keytype, class datatype> class Tree {
public:
				Tree(const char separator='\0',
				     const int  printkey=0) : root(0), os_sepa(separator), print_key(printkey)
				{ }

				Tree(const Tree<keytype,datatype>&);

		Tree &		operator = (const Tree<keytype,datatype>&);
		Tree &		operator + (const Tree<keytype,datatype>&);

		Node<keytype,datatype> * Find(const keytype &);

	virtual			~Tree()		{ Wipe(root); }

	void			DeleteTree()	{ Wipe(root); }

	friend	ostream&	operator << (ostream&, Tree<keytype,datatype>&);

	int			Insert(Node<keytype,datatype> *);

private:
	Node<keytype,datatype>	*root;

	char		os_sepa;
	int		print_key;	// 1 == print key too

	void 		print(ostream &, Node<keytype,datatype>*);

	int		Ins(Node<keytype,datatype> **tree,
			    Node<keytype,datatype> *element);

	void		Wipe(Node<keytype,datatype>*);

	void		CopyTree(const Tree<keytype,datatype>&);
	void		cpy(Node<keytype,datatype>*n);

	Node<keytype,datatype>* find(Node<keytype,datatype>*, const keytype&);

	void		add(Node<keytype,datatype>*);
};

template <class keytype, class datatype>
void Tree<keytype,datatype>::add(Node<keytype,datatype> *n)
{
  if (n) {
    Ins(&root, n);
    add(n->l);
    add(n->r);
  }
}

template <class keytype, class datatype>
Tree<keytype,datatype> & Tree<keytype,datatype>::operator + (const Tree<keytype,datatype>&t)
{
  add(t.root);
  return *this;
}

template <class keytype, class datatype>
Node<keytype,datatype> * Tree<keytype,datatype>::find(Node<keytype,datatype>*n, const keytype &k)
{
  if (n && n->key != k) {
    Node<keytype,datatype> *p;
    p = find(n->l, k);
    if (!p) p = find(n->r, k);
    return p;
  }
  else return n;
}

template <class keytype, class datatype>
Node<keytype,datatype> *Tree<keytype,datatype>::Find(const keytype &k)
{
  return find(root, k);
}

template <class keytype, class datatype>
void Tree<keytype,datatype>::cpy(Node<keytype,datatype>*n)
{
  if (n) {
    Ins(&root, n);
    cpy(n->l);
    cpy(n->r);
  }
}

template <class keytype, class datatype>
Tree<keytype,datatype>::Tree(const Tree<keytype,datatype>&t)
{
  CopyTree(t);
}

template <class keytype, class datatype>
Tree<keytype,datatype>& Tree<keytype,datatype>::operator = (const Tree<keytype,datatype> &t)
{
  if (t.root != root) CopyTree(t);
  return *this;
}

template <class keytype, class datatype>
void Tree<keytype,datatype>::CopyTree(const Tree<keytype,datatype>&t)
{
  root      = 0;
  os_sepa   = t.os_sepa;
  print_key = t.print_key;
  DeleteTree();			// Remove the existing tree
  cpy(t.root);			// before copying
}

template <class keytype, class datatype>
void Tree<keytype,datatype>::Wipe(Node<keytype,datatype>*n)
{
  if (n) {
    Wipe(n->l);
    Wipe(n->r);
    delete n;
  }
}

template <class keytype, class datatype>
int Tree<keytype,datatype>::Insert(Node<keytype,datatype> *n)
{
  return Ins(&root, n);
}

template <class keytype, class datatype>
int Tree<keytype,datatype>::Ins(Node<keytype,datatype> **t, Node<keytype,datatype> *n)
{
  if (*t) {
    if ((*t)->key < n->key) return Ins(&(*t)->r, n); else
    if ((*t)->key > n->key) return Ins(&(*t)->l, n); else return KEY_EXISTS;
  }
  else {
    *t = new Node<keytype,datatype>(n->key, n->data);
    (*t)->l = 0;
    (*t)->r = 0;
    return OK;
  }
}

template <class keytype, class datatype>
void Tree<keytype,datatype>::print(ostream &os, Node<keytype,datatype>*n)
{
  if (n) {
    print(os, n->l);
    if (print_key) os << n->key << " ";
    os << n->data << os_sepa;
    print(os, n->r);
  }
}

template <class keytype, class datatype>
ostream& operator << (ostream& os, Tree<keytype,datatype>&t)
{
  t.print(os, t.root);
  return os;
}

#endif
