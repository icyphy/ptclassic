static const char file_id[] = "CGStar.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGStar.h"
#include "CGWormhole.h"
#include "Target.h"

/*******************************************************************

	class CGStar methods

********************************************************************/



// firing CG star
void CGStar :: fire() {
	BlockPortIter next(*this);
	for(int i = numberPorts(); i > 0; i--)
		(next++)->grabData();
	go();
	next.reset();
	for(i = numberPorts(); i > 0; i--)
		(next++)->sendData();
}

// Output a code block to the target architecture
void CGStar :: gencode (CGCodeBlock& block) {
	// In this generic code generator, the block of code
	// is simply written verbatim to the Architecture
	// A more specific code generator would probably do more.
	targetPtr->addCode(block.getText());
}

// Output a text line of code, raw, to the target architecture
void CGStar :: addCode (const char* code) {
	targetPtr->addCode(code);
}

// max Comm. time.
int CGStar :: maxComm() {

	int max = 0;	// maximum traffic with anscestors.

	BlockPortIter next(*this);
	for(int i = numberPorts(); i > 0; i--) {
		PortHole* p = next++;
		if (p->isItInput() && max < p->numberTokens)
				max = p->numberTokens;
	}

	// processor 1 to 2, "max" sample, both sending plus receiving.
	return targetPtr->commTime(1,2,max,2);
}

// return NULL
CGWormhole* CGStar :: myWormhole() { return NULL; }

// The following is defined in CGDomain.cc -- this forces that module
// to be included if any CG stars are linked in.
extern const char CGdomainName[];

const char* CGStar :: domain () const { return CGdomainName;}

// isa

ISA_FUNC(CGStar, SDFStar);
