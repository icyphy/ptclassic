/**************************************************************************
Version identification:
@(#)InstanceManager.h	1.1	2/2/96

Copyright (c) 1990-1996 The Regents of the University of California.
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
		       
 Programmer:  B. L. Evans
 Date of creation: 02/02/96

This file defines a class to manage instances of objects.  It is
primarily syntactic sugar to the SequentialList class.

**************************************************************************/

#ifndef _InstanceManager_h
#define _InstanceManager_h 1

#ifdef __GNUG__
#pragma interface
#endif


#include "DataStruct.h"
#include "StringList.h"


class InstanceManager {

public:
	// make a unique name that can also be used as a pointer
	Pointer makeInstanceName(Pointer ptr, char* name);

	// add an instance
	inline int add(Pointer ptr) { return addInstance(ptr); }
	inline int add(Pointer ptr, char* name) {
		return addInstance(makeInstanceName(ptr, name));
	}

	// delete an instance
	inline int remove(Pointer ptr) { return instanceList.remove(ptr); }
	inline int remove(Pointer ptr, char* name) {
		return instanceList.remove(makeInstanceName(ptr, name));
	}

	// does a particular instance exist
	inline int exists(Pointer ptr) { return instanceList.member(ptr); }
	inline int exists(Pointer ptr, char* name) {
		return instanceList.member(makeInstanceName(ptr, name));
	}

	// returns TRUE if there are no more instances
	inline int noMoreInstances() { return instanceList.empty(); }

protected:
	// list of handle names
	SequentialList instanceList;

	// add an instance
	int addInstance(Pointer ptr);
};

#endif
