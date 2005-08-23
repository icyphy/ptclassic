/******************************************************************
Version identification:
@(#)FSMStateStar.h	1.6 10/08/98

Copyright (c) 1990-1999 The Regents of the University of California.
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

 Programmer:       Bilung Lee
 Date of creation: 3/4/96

 Definition of FSMStateStar which is put into the state transition diagram
 to represent a state.

*******************************************************************/
#ifndef _FSMStateStar_h
#define _FSMStateStar_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "FSMStar.h"
#include "Geodesic.h"
#include "InfString.h"
#include "IntArrayState.h"
#include "IntState.h"
#include "StringState.h"
#include "tcl.h"

class FSMStateStar : public FSMStar {
public:
    // constructor & destructor
    FSMStateStar();
    ~FSMStateStar();

    // Class identification.
    /* virtual*/ int isA(const char*) const;

    MultiInFSMPort stateIn;
    MultiOutFSMPort stateOut;

    // Return if this state is initial state.
    int isInit() { return int(isInitState); }

    // Return the next state according the conditions.
    virtual FSMStateStar *nextState(int& condNum, int preemption);

    // Execute the action.
    virtual int execAction(int actNum);

    // Execute the internal machine.
    virtual int execSlave(int curEntryType);

    // Get the entry type of a possible transition out of this state.
    int getEntryType(int transNum);

protected:
    IntState isInitState;
    StringState events;
    StringState conditions;
    StringState actEvents;
    StringState actExprs;
    IntArrayState entryType;
    IntArrayState preemptive;
    StringState slaveNm;
    StringState where_is_defined;

    int numTrans;
    char** parsedEvents;
    char** parsedConditions;
    InfString* parsedActEvents;
    InfString* parsedActExprs;

    Star* slave;
    const char* slaveInnerDomain;

    static int count;

    // Following just point to the copies of its master FSM.
    Tcl_Interp *myInterp;

    /* virtual */ void setup();  // Redefine.
    /* virtual */ void begin();  // Redefine.
    /* virtual */ void wrapup(); // Redefine.

    Star * createWormhole (const char *galname, const char* where_defined);
    const char* ptkCompile(const char *galname, const char* where_defined);
    int setupGeodesic (Star* worm, Block* parent);

 private:
    // Used for debugging.
    void printInfo();
};
#endif
