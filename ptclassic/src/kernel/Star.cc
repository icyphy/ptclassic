/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

 SDF-specific stuff moved to SDFStar.cc, 5/29/90.

*******************************************************************/
#include "Star.h"
#include "StringList.h"
#include <ACG.h>

// common random number generator for all Star classes.
ACG gen(10,20);

/*******************************************************************

	class Star methods

********************************************************************/

StringList
Star :: printVerbose () const {
	StringList out;
	out = "Star: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += printPorts ("star");
        out += printStates("star");
	return out;
}

// small virtual functions

// default go does nothing
void Star :: go () {}

// default fire() function
void Star :: fire() { go() ;}

// return myself as a Star.
Star& Star :: asStar () const { return *this;}

