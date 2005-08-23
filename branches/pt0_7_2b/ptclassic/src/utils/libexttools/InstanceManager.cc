/**************************************************************************
Version identification:
@(#)InstanceManager.cc	1.2	2/4/96

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

This file implements a class to manage instances of objects.  It is
primarily syntactic sugar to the SequentialList class.

**************************************************************************/
static const char file_id[] = "InstanceManager.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include "miscFuncs.h"
#include "StringList.h"
#include "InstanceManager.h"

// make a unique instance name
Pointer InstanceManager::makeInstanceName(Pointer ptr, char* name) {
    StringList uniqueName;
    if (name) uniqueName = name;
    char handle[32];
    sprintf(handle, "%-.8lx", long(ptr));
    uniqueName << handle;
    return (Pointer *) hashstring(uniqueName);
}

// add a record of the instance
int InstanceManager::addInstance(Pointer key) {
    int exists = instanceList.member(key);
    if (! exists) instanceList.prepend(key);
    return exists;
}
