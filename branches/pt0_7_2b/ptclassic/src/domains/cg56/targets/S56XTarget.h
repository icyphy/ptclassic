/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino
	Modified code from S56XTarget.h

 Target for Ariel S-56X DSP board.

*******************************************************************/
#ifndef _S56XTarget_h
#define _S56XTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Sim56Target.h"
#include "StringState.h"
#include "IntState.h"

class S56XTarget : public Sim56Target {
protected:
	void initStates();
public:
	S56XTarget(const char*,const char*);
	S56XTarget(const S56XTarget&);
	void headerCode();
	int setup(Galaxy&);
	void wrapup();
	Block* clone() const;
	void addCode(const char*);
	int isA(const char*) const;
};

#endif
