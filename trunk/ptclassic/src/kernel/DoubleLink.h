/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
*******************************************************************/

#ifndef _DoubleLink_h
#define _DoubleLink_h
#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"

	//////////////////////////////////////
	// class DoubleLinkList
	//////////////////////////////////////

class DoubleLink
{ 
	friend class DoubleLinkList;
	friend class DoubleLinkIter;

public:
	DoubleLink(Pointer a, DoubleLink* n, DoubleLink* p): 
		e(a), next(n), prev(p) {}
	DoubleLink(Pointer a): e(a), next(0), prev(0) {}

	Pointer content() { return e; }

	// virtual destructor
	virtual ~DoubleLink();

	void unlinkMe();

protected:
	DoubleLink *next;    // next node in the DL list
	DoubleLink *prev;    // previous node in the list
	Pointer e;
};

class DoubleLinkList
{
        friend class DoubleLinkIter;
public:
	// virtual destructor
	virtual ~DoubleLinkList();

	// create link
	DoubleLink* createLink(Pointer e) {
		INC_LOG_NEW; return new DoubleLink(e, 0, 0);
	}

	// constructor
	DoubleLinkList() : myHead(0), myTail(0), mySize(0) {}
	DoubleLinkList(Pointer* e) { firstNode(createLink(e)); }

        // Insert at the head of the list
        void insertLink(DoubleLink *x);  
	void insert(Pointer e) { insertLink(createLink(e)); }

        // Insert at the end of the list
        void appendLink(DoubleLink *x);
	void append(Pointer e) { appendLink(createLink(e)); }

        // insert y immediately ahead of x
	void insertAhead(DoubleLink *y, DoubleLink *x);
        	
        // insert y immediately behind x
	void insertBehind(DoubleLink *y, DoubleLink *x);	

        // Unlink a link from the list & return ptr to it.
	// Alert! make sure that x is in the list before calling this method!
	DoubleLink* unlink(DoubleLink *x);	

        // Remove a link from the list.
	// Alert! make sure that x is in the list before calling this method!
	void removeLink(DoubleLink *x) { unlink(x);	
					 INC_LOG_DEL; delete x; }
	void remove(Pointer e);
        
	// return TRUE if the argument is in the list
	int find(Pointer e);

        // Remove all links.
	virtual void initialize();	

        // Reset the list to be an empty list, without
        // deleting the previous contents.
        void reset(); 

        // get the number of elements in the list
        int size() { return mySize;}

        // get a pointer to the head of the list
        DoubleLink *head() {return myHead;}
        
        // get a pointer to the tail of the list
        DoubleLink *tail() {return myTail;}

        // pop the head node
        DoubleLink *getHeadLink() 
		{ if (mySize==0) return 0;
           	  else return unlink(myHead); }
	Pointer takeFromFront();
        
        // get the tail node & unlink it from the list
        DoubleLink *getTailLink() 
		{ if (mySize==0) return 0;
		  else return unlink(myTail); }
	Pointer takeFromBack();

protected: 
        DoubleLink *myHead;
        DoubleLink *myTail;
       
private:
        int mySize;

	// add the first node into the list
	void firstNode(DoubleLink*);
};



	///////////////////////////////////
	// class DoubleLinkIter
	///////////////////////////////////

class DoubleLinkIter 
{
public:
	DoubleLinkIter(const DoubleLinkList& l) : list(&l), ref(l.myHead) {}
	void reset() { ref = list->myHead;}
        void reset(DoubleLink *start) {ref=start;}
	DoubleLink* nextLink();
	Pointer	next(); 
	Pointer operator++ (POSTFIX_OP) { return next();}

	// attach the DoubleLinkIter to a different object
	void reconnect(const DoubleLinkList& l) {
		list = &l; ref = l.myHead;
	}
private:
	const DoubleLinkList* list;
	DoubleLink *ref;
};

#endif
