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
#include "MotorolaSimTarget.h"

class Sim56Target :public CG56Target, public MotorolaSimTarget {
private:
	void initStates();
public:
	Sim56Target(const char*,const char*);
	Sim56Target(const Sim56Target&);
	void headerCode();
	void wrapup();
	int compileCode();
	int loadCode();
	int runCode();
	Block* makeNew() const;
	int isA(const char*) const;
	const char* className() const {return "Sim56Target";}
};

#endif
