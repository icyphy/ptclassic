/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino

 Target for Motorola 56000 assembly code generation that generates
 two subroutines, ptolemyInit, ptolemyMain.
 
*******************************************************************/
#ifndef _Sub56Target_h
#define _Sub56Target_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "StringState.h"
#include "IntState.h"

class Sub56Target : public CG56Target {
private:
	void initStates();
protected:
        /*virtual*/ void headerCode();
public:
	Sub56Target(const char*,const char*);
	Sub56Target(const Sub56Target&);
	/*virtual*/ void mainLoopCode();
	/*virtual*/ Block* makeNew() const;
	/*virtual*/ int isA(const char*) const;
};

#endif
