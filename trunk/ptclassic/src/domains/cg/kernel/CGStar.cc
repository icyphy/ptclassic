/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

*******************************************************************/

#include "CGStar.h"

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
	target->addCode(block.getText());
}

// The following is defined in CGDomain.cc -- this forces that module
// to be included if any CG stars are linked in.
extern const char CGdomainName[];

const char* CGStar :: domain () const { return CGdomainName;}

// isa

ISA_FUNC(CGStar, SDFStar);
