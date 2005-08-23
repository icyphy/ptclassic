#ifndef _ACSKnownCategory_h
#define _ACSKnownCategory_h 1
/******************************************************************
Version identification:
@(#)ACSKnownCategory.h	1.8 09/08/99

Copyright (c) 1997-1998 The Regents of the University of California.
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
#pragma interface
#endif

class StringList;
class SequentialList;
class ACSCore;

class ACSKnownCategory {
public:

	// Constructor used by core base classes to add category to list.
	ACSKnownCategory( ACSCore& category ) {
		addEntry(category);
	}

	// Add and entry to the list
	static void addEntry( ACSCore& );

	// return the entire list
	static StringList getCategories();

	// find an entry in the list
	static const ACSCore* find( const char* category );

private:

	// pointer to the list of categories
	static SequentialList* allCategories;

};


#endif // _ACSKnownCategory_h
