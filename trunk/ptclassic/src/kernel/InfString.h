#ifndef _InfString_h
#define _InfString_h 1

/****************************************************************
SCCS version identification
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

This implementation replaces an older one by E. Lee.

InfString is just like StringList with a cast to char*.  Even though the
resulting type is char*, the string that is returned should not be
modified, except for code like that in Tcl that undoes its modification
before returning.

All the functionality is actually in StringList, and the StringList.chars()
method works just fine.

See the warnings in StringList.h about possible problems with early
destruction of temporaries when using the (char*) cast.

Since InfString is publicly inherited from StringList, any routine that
wants a StringList can be given an InfString instead, but *not* vice
versa, so it is not a good idea to require InfString arguments (less
flexibility)

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"

class InfString : public StringList {
public:
	operator char*() { return consolidate();}

	// constructors do not inherit, so we need to redo them.
	InfString() {}
	InfString(const StringList& arg) : StringList(arg) {}
	InfString(const char* arg) : StringList(arg) {}
	InfString(char arg) : StringList(arg) {}
	InfString(int arg) : StringList(arg) {}
	InfString(double arg) : StringList(arg) {}
	InfString(unsigned arg) : StringList(arg) {}

	// Assignment operators do not inherit, so we need to redo them.

	inline InfString& operator = (const StringList& arg) {
		StringList::operator=(arg);
		return *this;
	}
	inline InfString& operator = (const char* arg) {
		StringList::operator=(arg);
		return *this;
	}
	inline InfString& operator = (char arg) {
		StringList::operator=(arg);
		return *this;
	}
	inline InfString& operator = (int arg) {
		StringList::operator=(arg);
		return *this;
	}
	inline InfString& operator = (double arg) {
		StringList::operator=(arg);
		return *this;
	}
	inline InfString& operator = (unsigned arg) {
		StringList::operator=(arg);
		return *this;
	}
};

#endif
