#ifndef _CGRecurScheduler_h
#define _CGRecurScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGConScheduler.h"
#include "CGMacroCluster.h"
#include "Recur.h"
#include "Profile.h"

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

 Scheduler for "Recur" construct.

**************************************************************************/

class CGRecurScheduler : public CGConScheduler, public Recur {

public:
	// read the statistics on the number of iteration steps.
	int getStatistics();

	// download the code
	int downLoadCode(CGStar*, int, int, int);

	// constructor
	CGRecurScheduler(MultiTarget* t, ostream* l);
	~CGRecurScheduler();

	// do nothing in control synchronization.
	/* virtual */ void buildSyncLinks(Profile*);

	// build data synchronization
	/* virtual */ void buildDataSyncs(Profile*);

protected:
	// redefine the virtual methods.
	int examine(CGMacroClusterGal* galaxy);
	int closerExamine();
	// redefine
	int setProfile(int, int, Profile*);

	// return the assumed execution time
	int assumeExecTime();

	// save optimal information
	/* virtual */ void saveOptInfo() { saveK = bestK; }

private:
	// add control and synchronization codes to the profile.
	// returns the cost of these codes.
	int addControlCodes(int, Profile*);

	// synchronization time
	int myControlTime();
	int mySyncTime();
	
	// number of "Self" stars -> width of recursion.
	int numSelf;

	double calcCost(int, int);	  // compute the optimal total cost.
	double costInGeometric(int, int); // in Geometric dist.
	double costInUniform(int, int);   // in Uniform dist.
	double costInGeneral(int, int);   // in General dist.
	int adjustX(int,int,int,int);	  // adjust "x" value.

	// macro clusters and profiles
	Profile** body;		 // profile of the recursion body.
	CGMacroClusterBag* cbagA;
	CGMacroClusterBag* cbagB;
	CGMacroClusterBag* cbagC;
	CGMacroClusterBag* cbagD;
	
	// length of one path
	int pathLeng;

	// temporary variable for the optimal profile
	int bestX;		// recursion depth
	int bestK; 		// level of parallelism
	int tau;		// a + c + d
	int tauO;		// a + b;

	// variable for the final profile
	int saveK;

	// comm. stars for data synchronization.
	BlockList dataSendStars;
	BlockList dataReceiveStars;

	// local variable for naming a "Const" star
	int localId;

// methods for downLoadCode

	// In case saveK >= 1, numSelf > 1, and depth < saveK, call this
	// method recursively.
	int recursiveDownLoad(int, int, int, Geodesic*, Geodesic*, int);

	// saveK = 0, numSelf = 1, or depth = saveK.
	int recursionProcedure(int, int, int, Geodesic*, Geodesic*, int);

	// common code for the above two routines
	int initRecurCode(int, int, int, Geodesic*, Geodesic*, int);

	// Send codes for data synchronization.
	int addDataSend(int, int, int, CGTarget*);

	// Receive codes at sync proc. for data synchronization
	int addDataReceive(Geodesic*, int, int, int, CGTarget*, CGStar*);

	// Send codes for control synchronization
	int addControlSend(Geodesic*, int, int, int, CGTarget*, int);

	// connect a star to a blackhole (direction = 0) or Const, 
	// (direction = 1) return the created blackhole (or Const).
	// The caller has to delete the blackhole (or Const) later.
	CGStar* dummyConnect(CGStar*, int direction);

	// dummy star list
	BlockList dummyStars;

	// Make communication links
	void makeLink(int, int, Target*, Target*, int, int, int);

	// incrementalAdd
	int incrementalAdd(CGTarget*, CGStar*, int flag = 1);
};

#endif
