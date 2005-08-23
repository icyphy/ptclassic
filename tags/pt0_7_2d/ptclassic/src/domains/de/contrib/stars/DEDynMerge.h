#ifndef _DEDynMerge_h
#define _DEDynMerge_h 1
// header file generated from DEDynMerge.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "DEStar.h"

class DEDynMerge : public DEStar
{
public:
	DEDynMerge();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	MultiInDEPort in;
	OutDEPort out;
	PortHole* createPortHole ();
	void removePortHole (PortHole *delPort_p);

protected:
	/* virtual */ void go();
};
#endif
