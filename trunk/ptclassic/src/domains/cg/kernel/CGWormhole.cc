static const char file_id[] = "CGWormhole.cc";

/******************************************************************
Version identification:
 $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Date of revision: 6/20/90

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGWormhole.h"
#include "CGPortHole.h"

/**************************************************************************

	methods for CGtoUniversal

**************************************************************************/

void CGtoUniversal :: initialize() {
	PortHole :: initialize();
	ToEventHorizon :: initialize();
}

int CGtoUniversal :: isItInput() const 
	{ return EventHorizon :: isItInput(); }
int CGtoUniversal :: isItOutput() const 
	{ return EventHorizon :: isItOutput(); }
	
EventHorizon* CGtoUniversal :: asEH() { return this; }

/**************************************************************************

	methods for CGfromUniversal

**************************************************************************/

void CGfromUniversal :: initialize() {
	PortHole :: initialize();
	FromEventHorizon :: initialize();
}

int CGfromUniversal :: isItInput() const 
	{ return EventHorizon :: isItInput(); }
int CGfromUniversal :: isItOutput() const 
	{ return EventHorizon :: isItOutput(); }
	
EventHorizon* CGfromUniversal :: asEH() { return this; }

