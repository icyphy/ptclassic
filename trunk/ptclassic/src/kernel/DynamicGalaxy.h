/*******************************************************************
SCCS version identification
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
 Programmer : Soonhoi Ha
 Date : Jan. 14, 1992

********************************************************************/
#ifndef _DynamicGalaxy_h
#define _DynamicGalaxy_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Galaxy.h"

// This galaxy is the baseclass for a family of dynamically created
// galaxies.  Blocks, portholes, and other objects in the galaxy are
// assumed to be on the heap and are removed by the destructor.

class DynamicGalaxy : public Galaxy {
protected:
	void zero();		// remove contained objects
public:
	~DynamicGalaxy() { zero();}

	// the makeNew function returns an error and a null pointer.
	Block* makeNew() const;

	// class identification
        int isA(const char*) const;

};

#endif
