#ifndef _CompileTarget_h
#define _CompileTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and J. Buck
 Date of creation: 7/14/92 (formerly there was only a .cc file)

This Target produces and compiles a standalone C++ program for
a universe.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "BaseCTarget.h"

class CompileTarget : public BaseCTarget {
public:
	int setup(Galaxy& g);
	int run();
	void wrapup ();
	CompileTarget(const char* nam,const char* stype,const char* desc) :
		BaseCTarget(nam,stype,desc) {}
	Block* clone() const
		{ LOG_NEW; CompileTarget* t = new CompileTarget(readName(),
						starType(), readDescriptor());
		  return &t->copyStates(*this); }

	// Routines for writing code: schedulers may call these
	void writeFiring(Star& s, int depth);

private:
	// Method to return a pointer to the MultiPortHole that spawned a
	// given PortHole, if there is such a thing.  If not, the pointer
	// to the PortHole is returned as pointer to a GenericPort.
	GenericPort* findMasterPort(const PortHole* p) const;

	// Returns the name of an ordinary porthole, or
	// "name.newPort()" for a MultiPortHole.
	StringList expandedName(const GenericPort* p) const;

	StringList galDef(Galaxy* galaxy, StringList className, int level);

	int writeGalDef(Galaxy& galaxy, StringList className);

};

#endif
