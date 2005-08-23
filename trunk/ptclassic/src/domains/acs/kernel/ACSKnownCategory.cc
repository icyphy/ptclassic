static const char file_id[] = "ACSKnownCategory.cc";
/******************************************************************
Version identification:
@(#)ACSKnownCategory.cc	1.8 09/08/99

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer:  J. A. Lundblad
 Date of creation: 2/25/98

The ACS Known Category class.

This class provides a list of all known core categories.  There is a single
list, so all ACS Category names must be unique.

The idea is that each Core Category that is "known to the system"
should add an instance of itself to the known list by code something
like

static char* category = "FPSim";
static ACSKnownCategory entry(category);

The corona class can then use the KnownCategory list to find any Cores
that might be available by concatenation of it's star name with one of
the Core categories from the list.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DataStruct.h"
#include "ACSKnownCategory.h"
#include "ACSCore.h"

// The StringList of categorys
SequentialList* ACSKnownCategory::allCategories = NULL;

// Add a category to the list.
void ACSKnownCategory::addEntry( ACSCore& core ) {
	if ( allCategories == NULL ) {
		allCategories = new SequentialList;
		allCategories->put(&core);		
	} else if ( find(core.getCategory()) == NULL ) 
		allCategories->put(&core);
}

// Get the whole category list.
StringList ACSKnownCategory::getCategories() {
	static StringList categories;
	ListIter next(*allCategories);
	ACSCore* ptr;
	categories.initialize();
	while((ptr=(ACSCore*)(next++)) != 0) {
		categories << ptr->getCategory();
	}
	return categories;
}

// Find a category, return NULL otherwise.
const ACSCore* ACSKnownCategory::find( const char* category ) {
	ListIter next(*allCategories);
	ACSCore* ptr;
	while((ptr=(ACSCore*)(next++)) != 0) {
		if ( strcmp(ptr->getCategory(),category) == 0 )
			return ptr;
	}
	return NULL;
}


	
	
	
