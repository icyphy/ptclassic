/**********************************************************************
Copyright (c) 1996-%Q% The Regents of the University of California.
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

 Programmer:  Raza Ahmed and Brian L. Evans
 Created:     07/09/96
 Version:     $Id$

***********************************************************************/
#ifndef _CGCostTarget_h
#define _CGCostTarget_h 1

#ifdef __GNUG__
#pragma implementation
#endif

#include "StringList.h"
#include "StringArrayState.h"

#include "Block.h"
#include "Galaxy.h"

#include "CGTarget.h"

class SDFPtclTarget;

class CGCostTarget : public CGTarget {
public:
	// constructor
	CGCostTarget(const char* nam, const char* startype, const char* desc,
		     const char* assocDomain = CGdomainName);

	// destructor
	~CGCostTarget();

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const;

	/*virtual*/ int run();
	/*virtual*/ void wrapup();

	/*virtual*/ int isA(const char* cname) const;

	// List of targets to use to generate implementation cost information
	StringArrayState userTargetList;

protected:
	// Initialize the input/output portholes of the stars in the galaxy
	void initializeStarPorts(Galaxy& parent);

	// Disconnect all stars
	void disconnectAllStars(Galaxy& parent);

	// Generate implementation costs for all stars for one target
	int costInfoForOneTarget(CGTarget* userTarget, const char* domain);

	// Find the right source or sink star to match the block port
	int selectConnectStarBlock(Galaxy* localGalaxy, PortHole* localHole);

	// Find the code generation target corresponding to a name
	CGTarget* findCodeGenTarget(const char* userTargetName);

	// Generate the Tcl code
	int convertGalaxyToPtcl(Galaxy* localGalaxy);

	// Report the cost estimates for the stars found in the galaxy
	void reportCosts(CGTarget* userTarget);

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
