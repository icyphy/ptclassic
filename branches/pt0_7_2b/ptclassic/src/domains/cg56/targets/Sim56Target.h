/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

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

class Sim56Target : public CG56Target {
private:
	int genFile(StringList&,const char*);
	void initStates();
	StringList cmds;
	char* uname;
protected:
	StringState dirName;
	StringState plotFile;
	StringState plotTitle;
	StringState plotOptions;
public:
	Sim56Target(const char*,const char*,
		    unsigned,unsigned,unsigned,unsigned);
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