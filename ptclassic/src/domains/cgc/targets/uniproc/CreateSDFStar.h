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


// This target allows for the creation of target specific code generation
// wormholes to simulation.  It can be used stand-alone, for CGC inside of
// simulation wormholes, or as a helper target such as 
// $PTOLEMY/src/domains/cg56/targets/S56XTargetWH.{h,cc} for target specific
// simulation wormholes.

/* The ideas behind the implementation of CG wormholes is document in:
      J.L. Pino, T.M. Parks and E.A. Lee, "Automatic Code Generation for 
      Heterogeneous Multiprocessors," Proceedings of the IEEE International
      Conference on Acoustics, Speech, and Signal Processing, vol. 2, 
      pp. 445-448, Adelaide, Australia, April 1994.

   available on-line: 
http://ptolemy.eecs.berkeley.edu/papers/autoMultiCodeGen/www/proceedings_1.html
*/

// This structure represents a communication star pair.  This target
// can function as a helper target to allow for simulation wormholes.  The
// outer star is optional --  if specified, it is target specific CGC 
// communication star which receives the communication from an inner CGStar.
// This outer star is NULL iff the inner star is a CGC star.  
// In other words, the outer star is optional when this target is used 
// standalone, not as a helper target.
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

// A simple method to fire a target-generated linear SDF schedule. 
void fireSDFSchedule(SDFSchedule&,CGTarget&);

// Each target that uses this target as a helper target must define
// two functions (NOT methods) of the type CommPairF.  These functions
// must return a send/receive communication pair.
typedef CommPair (*CommPairF) (PortHole&,int,CGTarget&);

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
    // another wormhole target.  It contains all of the code that splices
    // in the new communication stars and creates the wormhole communication
    // schedules.  The 'inputs' and 'outputs' schedules are for the 
    // wormhole input ports and output ports respectively.  The function
    // pointers "incoming" and "outgoing" return the target specific
    // communcation star pairs.
    int prepareCGCWorm(SDFSchedule& inputs, SDFSchedule& outputs,
		       CommPairF incoming, CommPairF outgoing, CGTarget&);

    // This string list collects the include directory locations (in
    // the form of "-I<directory name >"
    StringList starIncludeDirs;

    // This string list collects the libraries needed at link time to link
    // in the replacement SDF worm star
    StringList starLinkOptions;

protected:
    // method to connect the new SDF star
    int connectStar();

    /*virtual*/ void mainLoopCode();
private:

    // Schedule containing the CGC stars that receive input from SDF
    SDFSchedule wormInputStars;

    // Schedule containing the CGC stars that send output to SDF
    SDFSchedule wormOutputStars;

    // We may leave graph in broken state if an Error::abortRun is
    // called.  When we disconnect the SDF-CGC wormhole, we mark the
    // graph as dirty.  When we successfully link in the SDFCGC worm
    // star, the graph is marked as ok.
    int dirty;

    // Codestream that collects all of the SDF porthole definitions
    CodeStream starPorts;

    // Codestream that collects the setSDFParams instructions
    CodeStream starSetup;

    // Pointer to wormhole that I am replacing
    Block* wormhole;
};

#endif
