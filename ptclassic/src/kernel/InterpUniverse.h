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
#pragma once
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
		Runnable(0, KnownBlock::domain(),this), targName(0)
        { setBlock(name,NULL);}
        int newTarget(const char* newTargName = 0);
	const char* targetName() const;
	Scheduler* mySched() const;
	Target* myTarget() const { return target;}

	// class identification
	int isA(const char*) const;
};

#endif


