#ifndef _CGConScheduler_h
#define _CGConScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "Galaxy.h"
#include "Profile.h"
#include "Distribution.h"
#include "CGPortHole.h"
#include "MultiTarget.h"
#include "CGMacroCluster.h"

class CGDDFClusterBag;

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

 Base class of the schedulers for dynamic constructs.
 The virtual methods in this class should be redefined in the derived
 classes.
 A topology of each construct is enforced.

**************************************************************************/

class CGConScheduler : public Scheduler {

public:
	CGConScheduler(MultiTarget* t, ostream* l);
	~CGConScheduler();

	// set-up
	void setup(); 

	// virtual function to download the code
	virtual int downLoadCode(CGStar*, int, int, int);

	// compute the profile with the given number of processors
	int calcProfile(int, IntArray*);
	// fix the number of processors assigned.
	void fixProfile(int, int, IntArray*);

	// copy the profile.
	void copyProfile(Profile* obj) { profile = obj; } 

	// set the number of processors
	void setProcs(int n);
	void setOptNum(int n) { optNum = n; }

	// its owner cluster
	void setOwner(CGDDFClusterBag* b) { owner = b; }
	void setRefGal(Galaxy* g) { refGal = g; }

	// read the statistical information on some parameters.
	virtual int getStatistics();

	// calculate the assumed execution time.
	int avgExecTime() 
		{ if (dist.myParam()) return assumeExecTime(); 
		  else		return 0; }

        // the following are do-nothing functions -- they may be
        // overridden by dynamic construct schedulers if needed
	int run();
	void setStopTime(double);
	double getStopTime() { return 0.0;}

	// build the control synchronization links between processors
	virtual void buildSyncLinks(Profile*);

	// build the data synchronization links between processors if
	// necessary (For, Recur construct)
	virtual void buildDataSyncs(Profile*);

	// find geodesic of the outside macro-actor connection.
	Geodesic* findGeo(CGStar*, const char*);

protected:
	MultiTarget* mtarget;
	ostream* logstrm;
	CGDDFClusterBag* owner;
	Galaxy* refGal;

	// calculate the assumed execution time.
	virtual int assumeExecTime();

	// execution profile of the dynamic construct
	Profile* profile;

	// number of the currently assigned processors.
	int numProcs;

	// number of processors to be assigned to the inside 
	// for the optimal profile
	int optNum;

	// Topology checking.
	virtual int examine(CGMacroClusterGal* galaxy);

        DistBase dist;      // description of the parameter distribution.

	// Examine the topology more closely to check the more restricted
	// syntax.
	virtual int closerExamine();

	// Member initialization after topology checking if necessary.
	virtual void initMembers();

	// compute the quasi-static profile of the construct with a given
	// number of processors.
	virtual int setProfile(int, int, Profile*);

	// After optimal profile is determined, save necessary information
	// if necessary
	virtual void saveOptInfo();

	// give a unique name for communication stars for a given profile.
	// The first argument is the invocation id, the second argument is
	// is the profile index. 
	// The third argument differentiate the name of the stars:
	//   "snd", "rcv" for control synchronization pairs. (flag = 0, 1)
	//   "snd1", "rcv1" for forward data synch. pairs. (flag = 2, 3)
	//   "snd2", "rcv2" for backward data synch. pairs. (flag = 4, 5)
	const char* nameComm(int invoc, int ix, int flag);

	// Get the communication star based on the name information
	CGStar* syncCommStar(BlockList&, int invoc, int ix, int flag);

	// add codes of Send stars for control synchronization.
	int addSendCodes(Geodesic*, int, CGTarget*);

	// find the boundary porthole of the clusterBag (first argument)
	// depending on the second argument, return the input or the output.
	// Assume that the macro has at most two portholes, one input and
	// one output.
	CGPortHole* boundaryPort(CGMacroCluster*, int);

	// return the porthole of the copy star associated with the
	// argument clust PortHole
	CGPortHole* copyPortHole(CGClustPort*);

	// down-load the code of the body of the argument macro actor
	int downLoadBodyCode(CGMacroClusterBag* cbag, int pix, CGTarget* t);

	// control communication stars
	BlockList syncSendStars;
	BlockList syncReceiveStars;

private:
	int syncSz;	// number of control sync. pairs.

	// temporal profiles
	Profile* temp1;
	Profile* temp2;
};

#endif
