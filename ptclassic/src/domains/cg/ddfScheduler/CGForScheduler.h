#ifndef _CGForScheduler_h
#define _CGForScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGConScheduler.h"
#include "For.h"

/**************************************************************************
Version identification:
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

 Programmer:  Soonhoi Ha

 Scheduler for "For" construct.

**************************************************************************/

class CGMacroClusterBag;

class CGForScheduler : public CGConScheduler, public For {

public:
	// read the statistics on the number of iteration steps.
	int getStatistics();

	// download the code
	int downLoadCode(CGStar*, int, int, int);

	// constructor
	CGForScheduler(MultiTarget* t, ostream* l); 
	~CGForScheduler();

	// build data synchronization links between processors in case
	// saveK > 1.
	/* virtual */ void buildDataSyncs(Profile* pf);

	// Before creating control syncs, do some preparation 
	/* virtual */ void buildSyncLinks(Profile* pf);

protected:
	// redefine
	int examine(CGMacroClusterGal* galaxy);
	int closerExamine();
	// redefine
	void initMembers();
	int setProfile(int, int, Profile*);

	// assume execution time
	int assumeExecTime();

	// save optimal information
	/* virtual */ void saveOptInfo() { saveK = bestK; }

private:
	// add control codes to the profile, and return the cost of the codes.
	int addControlCodes(int,Profile*);

	double calcCost(int, int);	  // compute the optimal total cost.
	double costInGeometric(int, int); // in Geometric dist.
	double costInUniform(int, int);   // in Uniform dist.
	double costInGeneral(int, int);   // in General dist.
	int adjustX(int,int,int,int);	  // adjust "x" value.

	Profile* body;		 // profile of the iteration body.
	CGMacroClusterBag* cbag;
	
	// temporary variables for the optimal profile
	int bestK;
	int bestX;
	int tau;

	// variables for the final profile
	int saveK;

	// comm. stars for data synchronization
	BlockList dataSendStars;
	BlockList dataReceiveStars;

	// add send codes for data synchronization
	int dataSendCodes(Geodesic*, int, CGTarget*);

	// incrementalAdd
	int incrementalAdd(CGTarget*, CGStar*, int flag = 1);

	// local id
	int localId;
};

#endif
