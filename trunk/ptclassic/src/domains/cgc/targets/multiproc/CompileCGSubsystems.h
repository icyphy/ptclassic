/******************************************************************
Version identification:
$Id$

@Copyright (c) 1995-%Q% The Regents of the University of California.
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

 Programmer: Jose Luis Pino

*******************************************************************/

#ifndef _CompileCGSubsystems_h
#define _CompileCGSubsystems_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGSharedBus.h"
#include "StringState.h"

class CreateSDFStar;

class CompileCGSubsystems: public CGSharedBus {
public:	
    CompileCGSubsystems(const char* name,const char* starType,const char* desc);
    
    // Add dummy CGC stars - set StringArrayState childType based on
    // wormholes.
    /*virtual*/ int modifyGalaxy();
    /*virtual*/ int isA(const char*) const;
    /*virtual*/ const char* className() const;
    /*virtual*/ int childIsA(const char*) const;
    /*virtual*/ Block* makeNew() const {
	 return new CompileCGSubsystems(name(),starType(),descriptor());
     }
    /*virtual*/ DataFlowStar* createSend(int from, int to, int num);
    /*virtual*/ DataFlowStar* createReceive(int from, int to, int num);
    /*virtual*/ void pairSendReceive(DataFlowStar* s, DataFlowStar* r);
    /*virtual*/ int runCode();
    /*virtual*/ void wormPrepare();
    /*virtual*/ void generateCode();
protected:
    /*virtual*/ void prepareChildren();
    /*virtual*/ void flattenWorm();
private:
    int replaceCommBlock(DataFlowStar& /*newStar*/,
			 DataFlowStar& /*oldStar*/);
    CreateSDFStar *cgcWorm;
};

#endif
    
