#ifndef _CGDDFCluster_h
#define _CGDDFCluster_h 1

/**************************************************************************
Version identification:
@(#)CGDDFCluster.h	1.2	1/1/96

Copyright (c) 1995 Seoul National University
Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  S. Ha

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGMacroCluster.h"
#include "CGConScheduler.h"

class ostream;

class CGDDFClusterGal : public CGMacroClusterGal {
public:
	// contructor
	CGDDFClusterGal(Galaxy* g, ostream* l = 0) : CGMacroClusterGal(g,l) {}
	CGDDFClusterGal(CGMacroClusterBag* c, ostream* l = 0) :
		CGMacroClusterGal(c,l) {}

protected:
	CGMacroClusterBag* createGalBag(Galaxy* g);
};


class CGDDFClusterBag : public CGMacroClusterBag {
public:
	// constructor: makes an empty bag
	CGDDFClusterBag(Galaxy* g = 0): CGMacroClusterBag(g), conSched(0) {}
	~CGDDFClusterBag();

	// cluster inside galaxy
	/* virtual */ int clusterMyGal();

	// prepare myself
	/* virtual */ int prepareBag(MultiTarget* t, ostream* l);

	// parallel schedule
	/* virtual */ int parallelSchedule(MultiTarget* t);

	// compute the optimal number of the assigned processors
	/* virtual */ int computeProfile(int nP, int rW, IntArray* avail);

	// manual scheduling
	/* virtual */ Profile* manualSchedule(int);

	// inside scheduling
	/* virtual */ int finalSchedule(int numProcs);

	// average execution time
	/* virtual */ int myExecTime();

	// print my schedule
	StringList displaySchedule();

	// download code
	/* virtual */ void downLoadCode(CGStar*, int , int);

	// return fixNum
	int numFix();

private:
	int fixNum;	// set if the number of assigned processors 
			// are fixed. reset to 0 otherwise

        CGConScheduler* conSched;        // parallel scheduler

};

#endif
