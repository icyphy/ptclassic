#ifndef _CG56MultiSimTarget_h
#define  _CG56MultiSimTarget_h 1

/******************************************************************
Version identification:
@(#)CG56MultiSimTarget.h	1.18	11/22/95

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

 Programmer: S. Ha, J. Pino

 This is a test multitarget class for CG56 domain.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGMultiTarget.h"
#include "IntState.h"
#include "StringState.h"
#include "ProcMemory.h"

class CG56MultiSimTarget : public CGMultiTarget {
public:
	CG56MultiSimTarget(const char* name, const char* starclass, const char* desc);

	/* return the sum of execution times of all stars in my galaxy */
	int totalExecTime();

	/*virtual*/ Block* makeNew() const;
	/*virtual*/ int isA(const char*) const;

	// the code is not run in this target
	/*virtual*/ int runCode() {return TRUE;}

	// redefine IPC funcs
	DataFlowStar* createSend(int from, int to, int num);
	DataFlowStar* createReceive(int from, int to, int num);

	/*virtual*/ void pairSendReceive(DataFlowStar* s, DataFlowStar* r);

	// allocate the shared memory
	/*virtual*/ void prepareCodeGen();

	// write the .asm and .cmd files
	/*virtual*/ void writeCode();

protected:
	/*virtual*/ void setup();

	// redefine 
	Target* createChild(int);

	// redefine
	int sendWormData(PortHole&);
	int receiveWormData(PortHole&);

	// shared memory
	ProcMemory* sharedMem;

private:
	StringState sMemMap;
};

#endif









