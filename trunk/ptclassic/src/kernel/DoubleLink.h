/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
	DoubleLinkList() : head(0), tail(0), size(0) {}
	DoubleLinkList(Pointer* e) { firstNode(createLink(e)); }

        // Insert at the head of the list
        void insertLink(DoubleLink *x);  
	void insert(Pointer e) { insertLink(createLink(e)); }

        // Insert at the end of the list
        void appendLink(DoubleLink *x);
	void append(Pointer e) { appendLink(createLink(e)); }

        // insert y immediately ahead of x
	void insertAhead(DoubleLink *y, DoubleLink *x);
        	
        // insert y immediately behind of x
	void insertBehind(DoubleLink *y, DoubleLink *x);	

        // Unlink a link from the list & return ptr to it.
	// Alert! make sure that x is in the list before call this method!
	DoubleLink* unlink(DoubleLink *x);	

        // Remove a link from the list.
	// Alert! make sure that x is in the list before call this method!
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
        int mySize() { return size;}

        // get a pointer to the head of the list
        DoubleLink *myHead() {return head;}
        
        // get a pointer to the tail of the list
        DoubleLink *myTail() {return tail;}

        // pop the head node
        DoubleLink *getHeadLink() 
		{ if (size==0) return 0;
           	  else return unlink(head); }
	Pointer takeFromFront();
        
        // get the tail node & unlink it from the list
        DoubleLink *getTailLink() 
		{ if (size==0) return 0;
		  else return unlink(tail); }
	Pointer takeFromBack();

protected: 
        DoubleLink *head;
        DoubleLink *tail;
       
private:
        int size;

	// add the first node into the list
	void firstNode(DoubleLink*);
};



	///////////////////////////////////
	// class DoubleLinkIter
	///////////////////////////////////

class DoubleLinkIter 
{
public:
	DoubleLinkIter(const DoubleLinkList& l) : list(&l), ref(l.head) {}
	void reset() { ref = list->head;}
        void reset(DoubleLink *start) {ref=start;}
	DoubleLink* nextLink();
	Pointer	next(); 
	Pointer operator++ () { return next();}

	// attach the DoubleLinkIter to a different object
	void reconnect(const DoubleLinkList& l) {
		list = &l; ref = l.head;
	}
private:
	const DoubleLinkList* list;
	DoubleLink *ref;
};

#endif
