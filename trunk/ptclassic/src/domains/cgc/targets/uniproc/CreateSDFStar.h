#ifndef _CGCTargetWH_h
#define _CGCTargetWH_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1995  The Regents of the University of California.
All Rights Reserved.

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

 Programmer: Jose Luis Pino


*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGCTarget.h"
#include "SDFScheduler.h"

// This structure represents a communication star pair.  This target
// can function as a helper target to allow for simulation wormholes.  The
// outer star is optional -- it is the CGCStar which will receives
// the communication from the inner CGStar.  This outer star is NULL iff
// the inner star is a CGC star.  In other words, the outer star is
// optional when this target is used standalone, not as a helper
// target.
//
//             Wormhole
//          |-------------|
//          |             |
//        X-|-O I-...-I O-|-X 
//          |             |
//          |     ...-I O-|-X     KEY: X stars external to the wormhole
//          |             |            O 'outer' stars, optional, must be CGC
//          |-------------|            I 'inner' stars, may be any CG
//

class CommPair {
public:
    CGCStar* outer;
    CGStar* inner;
};

typedef CommPair (*CommPairF) (PortHole&,int);

class CGCTargetWH : public CGCTarget {
public:
    CGCTargetWH(const char* name, const char* starclass, const char* desc);

    int isA(const char*) const;
    /*virtual*/ void setup();
    /*virtual*/ int run();
    /*virtual*/ Block* makeNew() const;
    /*virtual*/ void wrapup();
	
    /*virtual*/ int compileCode();
    /*virtual*/ int loadCode();
    /*virtual*/ int runCode() {return TRUE;}
    /*virtual*/ void frameCode();
    /*virtual*/ void writeCode();
    /*virtual*/ void initCodeStrings();

    /*virtual*/ void wormPrepare();
    /*virtual*/ void allWormInputCode();
    /*virtual*/ void allWormOutputCode();

    int linkFiles();
    
    // This method allows CGCTargetWH to be used as a helper target to
    // another wormhole target.
    int prepareCGCWorm(SDFSchedule& inputs, SDFSchedule& outputs,
		       CommPairF incoming, CommPairF outgoing, Galaxy&);
protected:
    int connectStar(Galaxy& galaxy);

    /*virtual*/ void mainLoopCode();
private:
    SDFSchedule wormInputStars;
    SDFSchedule wormOutputStars;

    // We may leave graph in broken state if an Error::abortRun is
    // called.  When we disconnect the SDF-CGC wormhole, we mark the
    // graph as dirty.  When we successfully link in the SDFCGC worm
    // star, the graph is marked as ok.
    int dirty;

    CodeStream starPorts;
    CodeStream starSetup;
};

#endif
