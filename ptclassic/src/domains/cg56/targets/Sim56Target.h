/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Target for Motorola 56000 assembly code generation that runs its
 output on the simulator.

*******************************************************************/
#ifndef _Sim56Target_h
#define _Sim56Target_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "StringState.h"
#include "IntState.h"

class Sim56Target : public CG56Target {
private:
	void initStates();
protected:
	StringState plotFile;
	StringState plotTitle;
	StringState plotOptions;
	void initializeCmds();
public:
	Sim56Target(const char*,const char*);
	Sim56Target(const Sim56Target&);
	void headerCode();
	void wrapup();
	Block* clone() const;
	int isA(const char*) const;
	const char* readClassName() const {return "Sim56Target";}
};

#endif
