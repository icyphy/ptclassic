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
#include "SDFStar.h"
class LSGraph;
class ClusterNodeList;

// simple schedule class
class SDFFiring {
friend class LSCluster;

	SDFStar* s;
	int count;

	SDFFiring* next;

public:
	SDFFiring(SDFStar* p, int i) : s(p), count(i), next(0) {}
	~SDFFiring() { INC_LOG_DEL; delete next; }

	int getExecTime() { return s->myExecTime() * count; }
};

//////////////////////
// class LSCluster  //
//////////////////////

// Each cluster contains two components associated with the repetition
// counters.

class LSCluster : public SDFStar {
private:
	SDFFiring* firing;	// local schedule
	SDFFiring* prev;	// previous firing.

	void determineOrder(LSGraph&, ClusterNodeList*);
	void addFiring(SDFStar*, int);

public:
	// constructor.
	LSCluster(LSGraph&, ClusterNodeList*);
	~LSCluster() { INC_LOG_DEL; delete firing; }

	// display schedule
	StringList displaySchedule(int depth);

	// fire the cluster
	void fire() { go(); }
	void go();

	// class identification
	int isA(const char*) const;

	// generate code
	StringList genCode(Target&, int);
};

#endif
