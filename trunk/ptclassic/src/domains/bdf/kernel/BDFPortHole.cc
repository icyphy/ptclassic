/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  J. T. Buck

Code for BDF-type portholes and multiportholes.

**************************************************************************/

static const char file_id[] = "BDFPortHole.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Block.h"
#include "BDFPortHole.h"
#include "CircularBuffer.h"


// table used by reverse(BDFRelation) function.
BDFRelation
BDFPortHole::reversals[4] = { BDF_TRUE,BDF_FALSE,BDF_COMPLEMENT,BDF_SAME};

int InBDFPort :: isItInput () const { return TRUE;}

// note: grabData and sendData shouldn't be called if, based on
// associated booleans, an arc is not to consume or produce tokens.
void InBDFPort :: receiveData () {
	getParticle();
	alreadyReadFlag = TRUE;
}

int OutBDFPort :: isItOutput () const { return TRUE;}

void OutBDFPort :: receiveData () {
	clearParticle();
}

void OutBDFPort :: sendData () {
	putParticle();
}

int MultiInBDFPort :: isItInput () const { return TRUE;}
int MultiOutBDFPort :: isItOutput () const { return TRUE;}

// Function to alter only BDF parameters
// We re-do porthole initialization if bufferSize changes
PortHole& BDFPortHole :: setBDFParams(unsigned num,BDFPortHole* assoc,
				      BDFRelation sense, int delay) {
	numberTokens = num;
	bufferSize = numberTokens + delay;
	alreadyReadFlag = FALSE;
	setRelation(sense,assoc);
	if (myBuffer && myBuffer->size() != bufferSize)
		initialize();
	return *this;
}

// return true if I am dynamic
int BDFPortHole :: isDynamic() const {
	return TorF(relType());
}

// mph functions.
PortHole& MultiInBDFPort :: newPort () {
	BDFPortHole& p = *new InBDFPort;
	p.setBDFParams(numberTokens,assocBoolean,relation,delay);
	return installPort(p);
}

PortHole& MultiOutBDFPort :: newPort () {
	BDFPortHole& p = *new OutBDFPort;
	p.setBDFParams(numberTokens,assocBoolean,relation,delay);
	return installPort(p);
}

