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

char* makeLower(const char*);

class Sim56Target : public CG56Target {
private:
	virtual void initStates();
protected:
	char* uname;
	StringList cmds;
	StringState dirName;
	StringState plotFile;
	StringState plotTitle;
	StringState plotOptions;
	IntState disCode;
	IntState simCode;
public:
	Sim56Target(const char*,const char*);
	Sim56Target(const Sim56Target&);
	~Sim56Target();
	void headerCode();
	int setup(Galaxy&);
	void wrapup();
	Block* clone() const;
	void addCode(const char*);
	int isA(const char*) const;
};

#endif
