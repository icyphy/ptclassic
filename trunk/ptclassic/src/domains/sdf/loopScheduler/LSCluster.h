/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on Shuvra Bhattacharyya's work
 Date of creation: 4/92

 LS Cluster is a master of cluster invocations. It is derived from
 SDF star

*******************************************************************/


#ifndef _LSCluster_h
#define _LSCluster_h
#ifdef __GNUG__
#pragma interface
#endif

#include "LSNode.h"
#include "SDFCluster.h"
class LSGraph;
class ClusterNodeList;

// simple schedule class
class SDFFiring {
friend class LSCluster;

public:
	SDFFiring(DataFlowStar* p, int i) : s(p), count(i), next(0) {}
	~SDFFiring();

	int getExecTime() { return s->myExecTime() * count; }

private:
	DataFlowStar* s;
	int count;

	SDFFiring* next;
};

//////////////////////
// class LSCluster  //
//////////////////////

// Each cluster contains two components associated with the repetition
// counters.

class LSCluster : public SDFBaseCluster {

public:
	LSCluster(LSGraph&, ClusterNodeList*);
	~LSCluster() { INC_LOG_DEL; delete firing; }

	// display schedule
	StringList displaySchedule(int depth);

	// run the cluster
	int run();
	void go() { run();}

	// generate code
	void genCode(Target&, int);

	// simulate execution for schedule generation.  We also
	// call simRunStar on the "real" star, so info like the
	// maximum # of tokens on each arc will get set right.
	int simRunStar(int deferFiring);

private:
	SDFFiring* firing;	// local schedule
	SDFFiring* prev;	// previous firing.

	void determineOrder(LSGraph&, ClusterNodeList*);
	void addFiring(DataFlowStar*, int);
};

#endif
