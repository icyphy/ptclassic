static const char file_id[] = "Star.cc";
/******************************************************************
Version identification:
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

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

 SDF-specific stuff moved to SDFStar.cc, 5/29/90.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Star.h"
#include "StringList.h"
#include "GalIter.h"
#include "SimControl.h"

/*******************************************************************

	class Star methods

********************************************************************/

StringList
Star :: print(int verbose) const {
	StringList out;
	return out << "Star: " << fullName() << "\n"
		   << "Descriptor: " << descriptor() << "\n"
		   << printPorts ("star",verbose)
		   << printStates("star",verbose);
}

// small virtual functions

// default go does nothing
void Star :: go () {}

// default run() function.  Returns TRUE if everything OK, false if
// a halt condition arises.
int Star :: run() {
	if (!SimControl::doPreActions(this)) return FALSE;
	go();
	return SimControl::doPostActions(this);
}

// return myself as a Star.
Star& Star :: asStar () { return *this;}
const Star& Star :: asStar () const { return *this;}

// set the target for the star.
void Star :: setTarget(Target* t) { 
	targetPtr = t;
}

// sets the index values of each star in the galaxy.  Returns the
// total number of stars.
int setStarIndices(Galaxy& g) {
	GalStarIter nextStar(g);
	Star* s;
	int cnt = 0;
	while ((s = nextStar++) != 0)
		s->indexValue = cnt++;
	return cnt;
}

const char* Star::className() const {return "Star";}

ISA_FUNC(Star,Block);
