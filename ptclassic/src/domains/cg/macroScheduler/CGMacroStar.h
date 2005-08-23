#ifndef _CGMacroStar_h
#define _CGMacroStar_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGMacroCluster.h"

/*******************************************************************
 SCCS Version identification :
@(#)CGMacroStar.h	1.3	1/24/96

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

 Programmer : Soonhoi Ha
 Date of Creation : 2/21/93
	
 This is the dummy star class to be used in sub-galaxy. It has a
pointer to the original macro task. 

********************************************************************/

	//////////////////////////////
	// CGMacroStar
	//////////////////////////////
// instead of cloning a clusterGal, it points to the original CGMacroClusterGal 
// and clone the portholes and states only.

class CGMacroStar: public CGStar {
public:
	CGMacroStar();
	void setProp(CGStar* s, int pix, int invoc, int flag);

	// do nothing in the initialize method
	// void initialize();

	// redefine
	int run();

	MultiInCGPort input;
	MultiOutCGPort output;

protected:
	// redefine go() method to generate codes for macro actor
	void go();

private:
	CGMacroClusterBag* myCluster;

	// this star corresponds to this invocation of the macro actor
	int invocNum;

	// indicate the index of the profile this star represents.
	int profileIndex;
};

#endif
