/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
#include "IntState.h"
#include "StringState.h"
#include "IntArrayState.h"
#include "tcl.h"

class SlaveProcess {
public:
  Star* worm;
  StringList inNmList;
  StringList outNmList;
  Geodesic **inGeo;
  Geodesic **outGeo;
  int* inGeoIndx;
  int* outGeoIndx;
  SlaveProcess() { 
    worm = NULL;
    inGeo = NULL; outGeo = NULL;
    inGeoIndx = NULL; outGeoIndx = NULL;
  }
  ~SlaveProcess() { 
    delete [] inGeo; delete [] outGeo;
    delete [] inGeoIndx; delete [] outGeoIndx;
  }
};

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
    virtual int execAction(int);

    // Execute the internal machine.
    virtual int execSlave(int);

    // Compile time one-writer rule checking.
    virtual int compOneWriterCk();

    // Get the entry type of a possible transition out of this state.
    int getEntryType(int transNum);
    
protected:
    IntState isInitState;
    StringState conditions;
    StringState actions;
    IntArrayState entryType;
    IntArrayState preemptive;
    StringState slaveNm;
    StringState where_is_defined;

    char** parsedCond;
    int numConds;
    IntArrayState* parsedAct;

    SlaveProcess* slave;

    static int count;

    // Following just point to the copies of its master FSM.
    char* oneWriterType;
    const StringList* internalEventNm;
    Tcl_Interp *myInterp;

    /* virtual */ void setup();  // Redefine.
    /* virtual */ void begin();  // Redefine.
    /* virtual */ void wrapup(); // Redefine.

    Star * createWormhole (const char *galname, const char* where_defined);
    const char* ptkCompile(const char *galname, const char* where_defined);
    int ioNmLists(StringList& inNmList, StringList& outNmList, Star* worm);
    virtual Geodesic** setupGeodesic (int* indxArray, MultiPortHole* mph,
				      Star* worm, StringList& pNmList);
};
#endif
