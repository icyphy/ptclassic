static const char file_id[] = "DFPortHole.cc";
#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1993 The Regents of the University of California.
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

 Programmer:  E. A. Lee, J. Buck, D. G. Messerschmitt
 Date of creation: 5/29/90

Code for SDF-type portholes and multiportholes.

**************************************************************************/

#include "DFPortHole.h"
#include "DataFlowStar.h"
#include "CircularBuffer.h"
#include "Geodesic.h"

// small virtual function implementations

PortHole* DFPortHole :: assocPort() { return 0;}

int DFPortHole :: assocRelation() const { return -1;}

int DFPortHole :: isDynamic() const { return FALSE;}

// constructor for DFPortHole ... default maxBackValue is 0
DFPortHole :: DFPortHole() : maxBackValue(0) {}

// number of repetitions of parent
int DFPortHole :: parentReps() const {
	DataFlowStar * parStar = (DataFlowStar*)parent();
	return parStar->reps();
}

PortHole& DFPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             DataType t,
                             unsigned numTokens,
			     unsigned delay)
{
	// Initialize PortHole
        PortHole::setPort(s,parent,t);

	// Based on the delay, we allocate a Buffer assuming no
	// past Particles are allocated
        numberTokens = numTokens;
	maxBackValue = delay;
	// The number of Particles the buffer has to hold is:
	//		numberTokens current and future Particles
	//		delay past Particles
	if ( numberTokens > int(delay)) bufferSize = numberTokens;
	else bufferSize = delay + 1;

        return *this;
}

// Function to alter only numTokens and delay.
// We re-do porthole initialization if bufferSize changes
PortHole& DFPortHole :: setSDFParams(unsigned numTokens, unsigned delay) {
	numberTokens = numTokens;
	maxBackValue = delay;
	if ( numberTokens > int(delay)) bufferSize = numberTokens;
	else bufferSize = delay + 1;
	if (myBuffer && myBuffer->size() != bufferSize)
		initialize();
	return *this;
}

// functions to manipulate the count of maximum buffer size.
void DFPortHole :: incCount(int n) { myGeodesic->incCount(n);}
void DFPortHole :: decCount(int n) { myGeodesic->decCount(n);}
void DFPortHole :: setMaxArcCount(int n) { myGeodesic->setMaxArcCount(n);}

ISA_FUNC(DFPortHole,PortHole);

// constructor
MultiDFPort :: MultiDFPort() : numberTokens(0) {}

// destructor -- nothing extra, but avoids cfront generating many
// out-of-line copies
MultiDFPort :: ~MultiDFPort() {}

MultiPortHole& MultiDFPort :: setPort (const char* s,
                             Block* parent,
                             DataType t,
                             unsigned numTokens) {
        MultiPortHole::setPort(s,parent,t);
        numberTokens = numTokens;
        return *this;
}

// Function to alter only numTokens and delay.
// We re-do porthole initialization if bufferSize changes
MultiPortHole& MultiDFPort :: setSDFParams(unsigned numTokens,
					    unsigned maxPstValue)
{
	MPHIter next(*this);
	PortHole* p;
	while((p = next++) != 0) {
	    ((DFPortHole*)p)->setSDFParams(numTokens,maxPstValue);
	}
	return *this;
}
