#ifndef _CGWormStar_h
#define _CGWormStar_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGWormBase.h"
#include "CGStar.h"

/*******************************************************************
 SCCS Version identification :
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
 Date of Creation : 2/21/93
	
 This is the dummy star class to be used in sub-galaxy. It has a
pointer to the original wormholes. Before code generation, this star
is replace with the original wormhole.

********************************************************************/

	//////////////////////////////
	// CGWormStar
	//////////////////////////////
// instead of cloning a wormhole, it points to the original CGWormBase class
// and clone the portholes and states only.

class CGWormStar: public CGStar {
public:
	CGWormStar(CGStar* s, int pix, int invoc, int flag);
	~CGWormStar();

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE;}

	// do nothing in the initialize method
	void initialize();

	// redefine
	int run();

protected:
	// redefine go() method to generate codes for wormholes.
	void go();

private:
	CGWormBase* myWorm;
	CGStar* myStar;

	// this star corresponds to this invocation of the wormhole
	int invocNum;

	// indicate the index of the profile this star represents.
	int profileIndex;
};

#endif
