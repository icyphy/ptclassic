static const char file_id[] = "NamedObj.cc";
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "NamedObj.h"
#include "StringList.h"
#include "Block.h"
#include "SimControl.h"

/***********************************************************************

  Return the full name of the object

  @Description This has no relation to the class name; it specifies
  the specific instance's place in the universe-galaxy-star
  hierarchy. The default implementation returns names that might look
  like

<PRE>
      universe.galaxy.star.port
</PRE>

  for a porthole; the output is the fullName of the parent, plus a
  period, plus the name of the NamedObj it is called on.

***********************************************************************/
StringList NamedObj :: fullName () const
{
	StringList out;
	if(prnt != NULL)
		out << prnt->fullName() << ".";
	out << nm;
	return out;
}

/***********************************************************************

 Return TRUE if the argument is the name either of the class or of one
   of the baseclasses.

 @Description The isA method should be redefined for all classed
 derived from NamedObj. To make this easy to implement, a macro
 ISA_FUNC is provided; for example, in the file Block.cc we see the
 line

<PRE>
      ISA_FUNC(Block,NamedObj);
</PRE>

  since NamedObj is the base class from which Block is derived. This
  macro creates the function definition

<PRE>
      int Block::isA(const char* cname) const {
              if (strcmp(cname,"Block") == 0) return TRUE;
              else return NamedObj::isA(cname);
      }
</PRE>

  Methods isA and className are overriden in derived classes; the
  re-definitions will not be described for each individual class.

***********************************************************************/
int NamedObj :: isA(const char* cname) const { return
(strcmp(cname,"NamedObj") == 0); }

// Return the class name of this object
const char* NamedObj :: className() const
{
	return "NamedObj";
}

/***********************************************************************

   Return a verbose description of the object.

   @Description If <B>verbose</B> is 0, a somewhat more compact form
   is printed than if verbose is 1.

***********************************************************************/
StringList NamedObj :: print(int) const
{
	StringList out;
	out << fullName() << ": " << className() << "\n";
	return out;
}

// Do nothing
NamedObj::~NamedObj() {}

// NamedObjList methods

/***********************************************************************

  Find a NamedObj with the given name and return pointer

  @Description This is the guts for both forms of objWithName.
  ListIter is used to bypass the const restrictions we impose. <P>

  Publicly, we enforce the rule that you can only get const pointers
  from a const list.  However, findObj can get non const pointers from
  a const list; it implements the guts of both objWithName methods.

***********************************************************************/
NamedObj* NamedObjList::findObj(const char* name) const {
	NamedObj *obj;
	ListIter next(*this);
	while ((obj = (NamedObj*)next++) != 0) {
		if (strcmp(name,obj->name()) == 0)
			return obj;
	}
	return 0;
}

/**********************************************************************

  Apply <CODE>initialize()</CODE> to all object on the list

  @Description Stop if an error occurs.

***********************************************************************/
void NamedObjList::initElements() {
	NamedObj *p;
	NamedObjListIter next(*this);
	while ((p = next++) != 0 && !SimControl::haltRequested()) {
		p->initialize();
	};
};

/***********************************************************************

   Delete all elements

   @Description <B>WARNING:</B> elements assumed to be on the heap
   (i.e., were created with <CODE>new</CODE> and deleted with
   <CODE>delete</CODE>.

***********************************************************************/
void NamedObjList::deleteAll() {
	NamedObj *p;
	// get and remove list nodes, while any remain.
	// delete the pointed-to objects.
	while ((p = (NamedObj*)getAndRemove()) != 0) {
		LOG_DEL; delete p;
	}
}

NamedObjList::NamedObjList() {}

NamedObjListIter :: NamedObjListIter(NamedObjList& sl)
  : ListIter (sl) {}
CNamedObjListIter :: CNamedObjListIter (const NamedObjList& sl)
  : ListIter (sl) {}
