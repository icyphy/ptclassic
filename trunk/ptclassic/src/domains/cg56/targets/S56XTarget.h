/******************************************************************
Version identification:
$Id$

Copyright (c) 1992 The Regents of the University of
California.
All Rights Reserved.

Programmer: J. Pino

Target for Ariel S-56X DSP board.

*******************************************************************/
#ifndef _S56XTarget_h
#define _S56XTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "StringState.h"
#include "IntState.h"

class S56XTarget : public CG56Target {
private:
    void initStates();
    CodeStream aioCmds;
    CodeStream shellCmds;
    StringState monitorProg;
protected:
        void headerCode();
        void trailerCode();
public:
	S56XTarget(const char*,const char*);
	S56XTarget(const S56XTarget&);
	int compileCode();
	void writeCode();
	int runCode();
	Block* makeNew() const;
	int isA(const char*) const;
};

#endif
