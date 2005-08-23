static const char file_id[] = "Star.cc";
/******************************************************************
Version identification:
@(#)Star.cc	2.27	02/04/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

 SDF-specific stuff moved to SDFStar.cc, 5/29/90.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Target.h"
#include "Star.h"
#include "StringList.h"
#include "GalIter.h"
#include "SimControl.h"

#ifdef PT_PTCL_WITH_TK

#include <tk.h>
int & tkUpdate() { static int doUpdate = 0; return doUpdate; }

#endif // PT_PTCL_WITH_TK

/*******************************************************************

	class Star methods

********************************************************************/

StringList Star::print(int verbose) const {
	StringList out;
	return out << "Star: " << fullName() << "\n"
		   << "Descriptor: " << descriptor() << "\n"
		   << printPorts ("Star",verbose)
		   << printStates("Star",verbose)
		   << "_______________________________________"
		   << "_______________________________________\n\n";
	
}

// small virtual functions

// default go does nothing
void Star :: go () {}

// default run() function.  Returns TRUE if everything OK, false if
// a halt condition arises.
int Star :: run() {
	if (!SimControl::doPreActions(this)) return FALSE;
	go();

#ifdef PT_PTCL_WITH_TK
	// Perform update of Tk if desired
	if( tkUpdate() )
		while (Tcl_DoOneEvent(TK_DONT_WAIT|TK_ALL_EVENTS));
#endif // PT_PTCL_WITH_TK

	return SimControl::doPostActions(this);
}

// return myself as a Star.
Star& Star :: asStar () { return *this;}
const Star& Star :: asStar () const { return *this;}

// Destructor
Star::~Star() {
}

// make a duplicate Star.  This will call Block::clone 
// and then set Star specific data members
/* virtual */ Block* Star::clone () const {
	Star* star = (Star*)Block::clone();
	return star;
}

// set the target for the star.
int Star :: setTarget(Target* t) { 
    if (!Block::setTarget(t)) return FALSE;
    if(target()->support(this)) {
	return TRUE;
    }
    else {
	Error::abortRun (*this, "wrong star type for target ",t->name());
	return FALSE;
    }
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

// return NULL
Wormhole* Star::asWormhole() { return NULL; }

const char* Star::className() const {return "Star";}

ISA_FUNC(Star,Block);
