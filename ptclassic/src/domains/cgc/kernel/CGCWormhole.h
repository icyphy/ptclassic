#ifndef _CGCWormhole_h
#define _CGCWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif


#include "StringList.h"
#include "Wormhole.h"
#include "EventHorizon.h"
#include "CGCStar.h"

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
 Date of Creation : 12/1/92
	
********************************************************************/

        //////////////////////////////////////////
        // class CGCtoUniversal
        //////////////////////////////////////////

// Input Boundary of ??inCGC_Wormhole
class CGCtoUniversal : public ToEventHorizon, public InCGCPort
{
public:
	// constructor
	CGCtoUniversal() : ToEventHorizon(this) {}

	// redefine
	void receiveData() ;

	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};

        //////////////////////////////////////////
        // class CGCfromUniversal
        //////////////////////////////////////////

// Output Boundary of ??inCGC_Wormhole
class CGCfromUniversal : public FromEventHorizon, public OutCGCPort
{
protected:
	// redefine the "ready" method for implicit synchronization.
	int ready() ;

public:
	// constructor
	CGCfromUniversal() : FromEventHorizon(this) {}

	// redefine
	void sendData() ;	

	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as EventHorizon
	EventHorizon* asEH();
};
	
#endif
