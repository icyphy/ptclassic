#ifndef _CGStar_h
#define _CGStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

*******************************************************************/

#include "SDFStar.h"
#include "Fraction.h"
#include "CGConnect.h"
#include "Particle.h"
#include "DataStruct.h"
#include "Code.h"
#include "Architecture.h"


	////////////////////////////////////
	// class CGCodeBlock
	////////////////////////////////////

// In this generic architecture, a code block is identical to the base class
#define CGCodeBlock CodeBlock

	////////////////////////////////////
	// class CGStar
	////////////////////////////////////

class CGStar : public SDFStar  {

public:
	// my domain
	const char* domain() const;

	// define the firing 
	void fire();

	// class identification
	int isA(const char*) const;

protected:

	// Output lines of code
	void gencode(CGCodeBlock& block);
};

/*
class CGStarPortIter : public BlockPortIter {
public:
	CGStarPortIter(CGStar& s) : BlockPortIter(s) {}
	CGPortHole* next() { return (CGPortHole*)BlockPortIter::next();}
	CGPortHole* operator++() { return next();}
};
*/

#endif
