#ifndef _NamedObj_h
#define _NamedObj_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Date of creation: 1/28/90
 Revisions:

A NamedObj is an object that has a name, a descriptor, and a parent,
where the parent is a Block (a specific type of NamedObj).

**************************************************************************/
#include "StringList.h"
#include "isa.h"
#include "FlagArray.h"

class Block;

/***********************************************************************

  An object with a name, a descriptor, and a parent, which is a Block

  @Description NamedObj is the baseclass for most of the common
               Ptolemy objects.  A NamedObj is, simply put, a named
               object; in addition to a name, a NamedObj has a pointer
               to a parent object, which is always a Block (a type of
               NamedObj). This pointer can be null. A NamedObj also
               has a descriptor. <P>

               <B>Warning!</B> NamedObj assumes that the name and
	       descriptor "live" as long as the NamedObj does.  They
	       are not deleted by the destructor, so that they can be
	       compile-time strings. <P>

  @Author J. T. Buck

***********************************************************************/
class NamedObj {
public:

  /*****
    Construct a blank NamedObj
    @Description Set the name and descriptor to empty strings and the
    	         parent pointer to null.
   *****/
  NamedObj () : nm(""), prnt(0), myDescriptor("") {}

  NamedObj (const char* n,Block* p,const char* d)
    : nm(n), prnt(p), myDescriptor(d) {}

  /*****
    Return the name of the class.

     @Description Every derived class should supply a new
     impementation (except for abstract classes, where this is not
     necessary)
   *****/
  virtual const char* className() const;

  // Return just the the local portion of the name of the class 
  inline const char* name() const { return nm; }

  // Return the descriptor
  inline const char* descriptor() const { return myDescriptor; }

  // Return a pointer to the parent block
  inline Block* parent() const { return prnt;}
 
  virtual StringList fullName() const;

  // Set the name of this object
  inline void setName (const char* my_name) {
    nm = my_name;
  }

  // Set the parent of this object
  inline void setParent (Block* my_parent) {
    prnt = my_parent;
  }

  // Set the name and parent of this object
  void setNameParent (const char* my_name,Block* my_parent) {
    setName(my_name);
    setParent(my_parent);
  }

  // Prepare the object for system execution.
  virtual void initialize() = 0;
  
  virtual StringList print (int verbose) const;

  virtual int isA(const char* cname) const;

  // make destructors for all NamedObjs virtual
  virtual ~NamedObj();

  /*****
    An array of flags for use by targets and schedulers

    @Description Many schedulers and targets need to be able to mark
                 blocks in various ways, to count invocations, or flag
                 that the block has been visited, or to classify it as
                 a particular type of block.  To support this, we
                 provide an array of flags that are not used by class
                 Block, and may be used in any way by a Target or
                 scheduler.  The array can be of any size, and the
                 size will be increased automatically as elements are
                 referenced.  For readability and consistency, the
                 user should define an enum in the Target class to
                 give the indices, so that mnemonic names can be
                 associated with flags, and so that multiple
                 schedulers for the same target are consistent. <P>

                 For efficiency, there is no checking to prevent two
		 different pieces of code (say a target and scheduler)
		 from using the same flags (which are indexed only by
		 non-negative integers) for different purposes.  The
		 policy, therefore, is that the target is in charge.
		 It is incumbent upon the writer of the target to know
		 what flags are used by schedulers invoked by that
		 target, and to avoid corrupting those flags if the
		 scheduler needs them preserved.
   *****/
  FlagArray flags;

protected:

  /*****
    Set the descriptor of this object

    @Description The string pointed to by desc must live as long as the
    NamedObj does.
   *****/
  void setDescriptor(const char* d) { myDescriptor = d;}
private:
  // name of the object
  const char* nm;

  // pointer to this object's parent Block
  Block* prnt;

  // descriptor of the object
  const char* myDescriptor;
};

// This structure is a list of NamedObj objects.  It should be privately
// inherited from when creating, say, a list of Block objects.

/***********************************************************************

   A sequential list of NamedObjs.

   @Description Class NamedObjList is simply a list of objects of
                class NamedObj. It is privately inherited from class
                SequentialList, and, as a rule, other classes
                privately inherit from it. It supports only a subset
                of the operations provided by SequentialList; in
                particular, objects are added only to the end of the
                list. It provides extra operations, like searching for
                an object by name and deleting objects. <P>

                This object enforces the rule that only const pointers
                to members can be obtained if the list is itself
                const; hence, two versions of some functions are
                provided.

   @Author J. T. Buck

***********************************************************************/
class NamedObjList : private SequentialList
{
  friend class NamedObjListIter;
  friend class CNamedObjListIter;
public:
  NamedObjList();

  // Add an object to the end of the list
  void put(NamedObj& s) {SequentialList::put(&s);}

  SequentialList::size;
  SequentialList::initialize;

  /*****
    Return the first object with the given name

    @Description Return 0 if no object with the name is found
   *****/
  NamedObj* objWithName(const char* name) { return findObj(name); }

  const NamedObj* objWithName(const char* name) const {
    return findObj(name);
  }

  void initElements();

  // Return the first object on the list
  inline NamedObj* head() {return (NamedObj*)SequentialList::head();}

  inline const NamedObj* head() const {
    return (const NamedObj*)SequentialList::head();
  }

  /*****
     Remove the object from the list
     @Description Note: The object is not deleted
   *****/
  int remove(NamedObj* o) { return SequentialList::remove(o);}

  void deleteAll();
private:
  NamedObj* findObj(const char* name) const;
};

// An iterator for NamedObjList
class NamedObjListIter : public ListIter {
public:
	NamedObjListIter(NamedObjList& sl);
	inline NamedObj* next() { return (NamedObj*)ListIter::next();}
	inline NamedObj* operator++(POSTFIX_OP) { return (NamedObj*)ListIter::next();}
	ListIter::reset;
	ListIter::remove;
};

// An iterator for NamedObjList, const form
class CNamedObjListIter : public ListIter {
public:
	CNamedObjListIter(const NamedObjList& sl);
	inline const NamedObj* next() { return (const NamedObj*)ListIter::next();}
	inline const NamedObj* operator++(POSTFIX_OP) { return (const NamedObj*)ListIter::next();}
	ListIter::reset;
};

#endif
