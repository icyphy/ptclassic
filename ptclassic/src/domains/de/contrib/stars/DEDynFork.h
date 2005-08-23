#ifndef _DEDynFork_h
#define _DEDynFork_h 1
// header file generated from DEDynFork.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1998 Dresden University of Technology, WiNeS-Project.
All rights reserved.
 */
#include "DEDynForkBase.h"

class DEDynFork : public DEDynForkBase
{
public:
	DEDynFork();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	InDEPort in;
	MultiOutDEPort out;
	PortHole* createPortHole (int);
	void removePortHole (PortHole *delPort_p);

protected:
	/* virtual */ void go();
};
#endif
