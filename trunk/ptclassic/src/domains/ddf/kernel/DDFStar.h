#ifndef _DDFStar_h
#define _DDFStar_h 1
#ifdef __GNUG__
#pragma once
#pragma interface
#endif
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

 DDFStar is a Star that runs under the DDF Scheduler.

*******************************************************************/

#include "SDFStar.h"
#include "Particle.h"
#include "Fraction.h"
#include "DDFConnect.h"

	////////////////////////////////////
	// class DDFStar
	////////////////////////////////////

class DDFStar : public DataFlowStar  {

 	friend class DDFScheduler;
	PortHole* waitPort;
	int waitNum;

public:
	// constructor
	DDFStar() { waitNum = 0; waitPort = NULL; }

	// initialize DDF-specific members
	void prepareForScheduling();

	// class identification
	int isA(const char*) const;

	// my domain
	const char* domain() const;

	// wait until this porthole has as many tokens as "waitNum"
	void waitFor(PortHole& p, int num = 1) ;
	
	// return the number of tokens waiting for waitPort
	int reqTokens() { return (waitNum - waitPort->numTokens()) ; }
	int hasWaitPort() { return ( waitPort ? TRUE : FALSE ) ;}

	// is it recursion star?
	virtual int isItSelf();

	virtual const char* readClassName();
};

#endif
