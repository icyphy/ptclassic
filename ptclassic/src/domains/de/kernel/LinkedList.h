#ifndef _LinkedList_h
#define _LinkedList_h 1
/**************************************************************************
Version identification:
@(#)LinkedList.h	2.17	02/12/97

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  D. G. Messerschmitt, J. Buck and Jose Luis Pino
 Date of creation: 1/17/89

This header file contains basic container class data structures
used widely. Each container class stores a set of void*'s,
with different ways of accessing them. Normally, a data structure
is derived from one of these classes by adding conversion from
the type of interest to and from the void*, allocating and
deallocating memory for the objects, etc.

 Modified: John Davis, 5/18/97
        LinkedList class has been rewrittent to be more
        general. In particular, an arbitrary member can be
        removed (regardless of the position in the list) and
        the Links can be directly accessed in an efficient
        manner. 

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"

	//////////////////////////////////////
	// Class Link
	//////////////////////////////////////

class Link {
	friend class LinkedList;
	friend class LinkedListIter;
protected:
	Link *next,*previous; 
	Pointer e; 
	Link(Pointer,Link*); 
	void remove();
};

	//////////////////////////////////////
	// Class Linked List
	//////////////////////////////////////

class LinkedList
{
	friend class LinkedListIter;
public:
	// Destructor
	~LinkedList()  { initialize(); }

	// Constructor
	LinkedList() : lastNode(0), dimen(0) {}

	// Constructor, with argument
	LinkedList(Pointer a);

	// Return the size of the list.
	inline int size() const { return dimen;}
	
	// Add at head of list
	void prepend(Pointer a);	

	// Add at tail of list
	void append(Pointer a);	        

	// Synonym for append()
	void put(Pointer a) {append(a);} 

	// Add at tail of list and get pointer.
	Link * appendGet(Pointer a);    

	// Remove pointer to link directly without searching;
	// Return TRUE if link is found; return false otherwise
	void directRemove(Link * a);	

	// Remove pointer by searching; return TRUE if removed
	int searchAndRemove(Pointer a);  

	// Return and remove head of list
	Pointer getAndRemove();	

	// Return and remove tail of list
        Pointer getTailAndRemove();     

	// Return head, do not remove
	inline Pointer head() const  
	{
		return lastNode ? lastNode->next->e : 0;
	}
	
	// Return tail, do not remove
	inline Pointer tail() const {	
		return lastNode ? lastNode->e : 0;
	}
	
	// Return arbitary node of list
	Pointer elem(int) const;

	// Remove all links
	void initialize();	

	// Return TRUE if the list empty; return FALSE otherwise.
	inline int empty() const { return (lastNode == 0);}

	// Return TRUE if arg is a member of the list; return FALSE otherwise. 
	int member(Pointer arg) const;

private:
	// Remove a link from the list
        Link* removeLink(Link&);

        // Store head of list, so that there is a notion of 
        // first node on the list, lastNode->next is head of list 
        Link *lastNode;
	int dimen;
};


        ///////////////////////////////////
        // class LinkedListIter
        ///////////////////////////////////

// LinkedListIter steps sequentially through a LinkedList.  Warning: if
// the list is modified "out from under" the LinkedListIter, bad things may
// happen if next() is called, though reset() will be safe.

class LinkedListIter {
public:
        // constructor: attach to a LinkedList
        inline LinkedListIter(const LinkedList& l) : list(&l) { reset(); }

        // reset to the beginning of a list
        void reset();

        // next and operator++ are synonyms.  Return the next element,
        // return 0 if there are no more.
        // This routine has been re-implemented and optimized for speed
        // because of its heavy usage.  The if-structure is organized
        // so that necessary ifs are executed, but for cases where only
        // a few ifs are needed, the minimum number of ifs is done for
        // the most common cases, with rarer cases taking decreasing
        // priority in the if-structure.
        inline Pointer next() {
          if (startAtHead) {
            // Starting at the head of the list.  Use list->lastNode to 
	    // find the head of the list.  If list->lastNode is NULL, 
	    // the list is empty.
            startAtHead = FALSE;
            if (list->lastNode) {
              ref = list->lastNode->next;
              return ref->e;
            }
            // The list is empty, no entries:  passing the end of the list.
            else {
              ref = 0;
              return ref;
            }
          }
          else {
            // Check to see if we're not at the end of the list.
            if (ref != list->lastNode) {
              // Check to see if ref is not NULL:  end of list not yet passed.
              if (ref) {
                ref = ref->next;
                return ref->e;
              }
              // ref is NULL:  end of list was already passed.
              else {
                return ref;
              }
            }
            // Passing the end of the list, no more entries.
            else {
              ref = 0;
              return ref;
            }
          }
        }

        inline Pointer operator++ (POSTFIX_OP) { return next();}

        // attach the LinkedListIter to a different object
        void reconnect(const LinkedList&);

        // remove the currently pointed to ref and update the
        // the ref pointer - if next hasn't been called, the lastNode
        // will be removed
        void remove();
private:
        const LinkedList* list;
        Link *ref;
        int startAtHead;
};



#endif
