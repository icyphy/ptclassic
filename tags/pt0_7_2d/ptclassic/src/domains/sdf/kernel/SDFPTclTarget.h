#ifndef _SDFPTclTarget_h
#define _SDFPTclTarget_h 1
/******************************************************************
Version identification:
@(#)SDFPTclTarget.h	1.9 9/2/96

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

 Programmer:  Jose Luis Pino
 Date of creation: 10/25/95

 This Target produces a PTcl graph corresponding to the SDF graph.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

// Ptolemy kernel includes
#include "Galaxy.h"
#include "GalIter.h"
#include "StringList.h"
#include "StringState.h"

// Ptolemy domain includes
#include "SDFTarget.h"

class StarProfile {
public:
    StarProfile() : star(0), time(0), iterations(0) {};

    inline void addTime(double iterationTime) {
	iterations++;
	time += iterationTime;
    }
    
    inline double avgTime() { return time*1000.0/double(iterations); }
    
    Star* star;
private:
    double time;
    int iterations;
};

class StarProfiles {
public:
    StarProfiles() : starProfiles(0) {};

    void set (Galaxy& gal){
	delete [] starProfiles;
	numProfiles = 0;
	GalStarIter nextStar(gal);
	Star* star;
	while ((star = nextStar++) != NULL) numProfiles++;
	starProfiles = new StarProfile[numProfiles];
	nextStar.reset();
	int i = 0;
	while ((star = nextStar++) != NULL) starProfiles[i++].star = star;
    }
    
    StarProfile* lookup(Star& star) {
	int i = 0;
	while (i < numProfiles && &star != starProfiles[i++].star);
	i--;
	if (&star == starProfiles[i].star) return &starProfiles[i];
	return NULL;
    }

    ~StarProfiles() { delete [] starProfiles; }
    
private:
    StarProfile* starProfiles;
    int numProfiles;
};

class SDFPTclTarget : public SDFTarget {
public:
    // Constructor
    /*virtual*/ SDFPTclTarget(const char*, const char*);

    /*virtual*/ int run();

    /*virtual*/ void wrapup();

    // Return a new copy of the target
    /*virtual*/ Block* makeNew() const;

    /*virtual*/ void writeFiring(Star&,int);

    /*virtual*/ void setStopTime(double);

    // Directory 
    StringState destDirectory;

protected:
    // Generate Ptcl code specifying the execution time estimates
    StringList ptclExecTimeCode();

private:
    int numIters;
    StarProfiles starProfiles;
};
#endif
