/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 11/29/90

 An InterpUniverse is an Universe with an interpreted galaxy
 in it.  This is accomplished by making it Runnable and an InterpGalaxy.
 This is the main object that interfaces talk to that makes Ptolemy
 work.

 There used to be two slightly different versions of this, one
 in pigiRpc/kernelCalls.h and one in Interpreter.h.  Now there is
 one file.

*******************************************************************/
#ifndef _InterpUniverse_h
#define _InterpUniverse_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Universe.h"
#include "InterpGalaxy.h"
#include "KnownBlock.h"

class InterpUniverse : public InterpGalaxy, public Runnable {
private:
	const char* targName;
public:
        InterpUniverse (const char* name = "mainGalaxy") :
		Runnable((Target*)0, KnownBlock::domain(),this), targName(0)
        { setBlock(name,NULL);}
	~InterpUniverse();
        int newTarget(const char* newTargName = 0);
	const char* targetName() const;
	Scheduler* scheduler() const;
	Target* myTarget() const { return target;}
	int run() { return Runnable::run();}
	void wrapup() { target->wrapup();}
	// class identification
	int isA(const char*) const;
};

#endif


