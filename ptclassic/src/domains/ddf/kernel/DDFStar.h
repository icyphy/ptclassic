#ifndef _DDFStar_h
#define _DDFStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

 DDFStar is a Star that runs under the DDF Scheduler.

*******************************************************************/

#include "Star.h"
#include "Particle.h"
#include "Fraction.h"
// next is included so SDFStar defns don't have to.
#include "SDFConnect.h"
#include "DDFConnect.h"

	////////////////////////////////////
	// class DDFStar
	////////////////////////////////////

class DDFStar : public Star  {

 	friend class DDFScheduler;
	PortHole* waitPort;
	int waitNum;

public:
	// my domain
	const char* domain() const;

	// Redefine method setting internal data in the Block
	// so that various DDF-specific initilizations can be performed.
	// If the parent pointer is not provied, it defaults to NULL
	Block& setBlock(char* starName, Block* parent = NULL);

	// wait until this porthole has as many tokens as "waitNum"
	void wait(PortHole& p, int num = 1) ;
	
	// is it recursion star?
	virtual int isItSelf();
};

#endif
