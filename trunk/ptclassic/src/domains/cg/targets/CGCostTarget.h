/**********************************************************************
Version identification:
 $Id$

Copyright (c) %Q% The Regents of the University of California.
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

 Programmer:  Raza Ahmed
 Date of creation: 07/09/96

 Target for the generation of cost of individual stars in a galaxy.

***********************************************************************/
#ifndef _CGCostTarget_h
#define _CGCostTarget_h 1

#ifdef __GNUG__
#pragma implementation
#endif

#include "StringList.h"
#include "StringState.h"
#include "IntState.h"

#include "Block.h"
#include "Galaxy.h"

#include "CGTarget.h"

class CGCostTarget : public CGTarget {
public:
	// constructor
	CGCostTarget(const char* nam, const char* startype, const char* desc,
		     const char* assocDomain = CGdomainName);

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const;

	/*virtual*/ int run();
	/*virtual*/ void wrapup();

	/*virtual*/ int isA(const char* cname) const;

protected:
	// Find the right source or sink star to match the block port
	int selectConnectStarBlock(Galaxy* localGalaxy,
		MultiPortHole* localHole);

	// Print the information about the stars found in the galaxy
	void printGalaxy(Galaxy* localGalaxy);

	// Add a newly created block
	inline void addTempBlock(Block* block) { tempBlockList.put(block); };

	// Delete newly allocated blocks
	void deleteTempBlocks();

private:
	// Map a data type into a standard abbreviation used in star names
	const char* dataTypeAbbreviation(const char* datatype);

	// List of dynamically allocated blocks
	SequentialList tempBlockList;
};

#endif
