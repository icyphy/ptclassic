/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Target for Motorola assembly code generation that runs its
 output on the simulator.

*******************************************************************/
#ifndef _MotorolaSimTarget_h
#define _MotorolaSimTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "MotorolaTarget.h"
#include "StringState.h"
#include "IntState.h"

class MotorolaSimTarget : public virtual MotorolaTarget {
protected:
	StringState plotFile;
	StringState plotTitle;
	StringState plotOptions;
	IntState interactiveFlag;
	StringList dspType;	//either 56000 or 96000
	StringList startAddress;
	StringList endAddress;
public:
	MotorolaSimTarget(const char* nam,const char* desc,
		  const char* sclass) : MotorolaTarget(nam,desc,sclass) {}
	MotorolaSimTarget(const MotorolaSimTarget& arg) 
	: MotorolaTarget(arg) {}
	void initStates(const char* dsp,const char* start, const char* end);
	void headerCode();
	void wrapup();
	int compileCode();
	int loadCode();
	int runCode();
};

#endif
