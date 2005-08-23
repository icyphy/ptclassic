#ifndef _DEDynMap_h
#define _DEDynMap_h 1
// header file generated from DEDynMap.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "DEDynForkBase.h"
#include "DEDynMerge.h"
#include "DEDynMapBase.h"

class DEDynMap : public DEDynMapBase
{
public:
	DEDynMap();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void wrapup();
	/* virtual */ ~DEDynMap();
	InDEPort newBlock;
	InDEPort delBlock;
	MultiInDEPort in;
	MultiOutDEPort out;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
# line 103 "DEDynMap.pl"
int numberOfOutputs, numberOfInputs;
        DEDynMerge *myDynMerge_p;
        DEDynForkBase *myDynFork_p;
        PortHole *Dest;
	void disconnectOutputs (Block *delBlock_p);
	void disconnectInputs (Block *delBlock_p);

private:
	void connectOutputs (Block *block_p);
	void connectInputs (Block *block_p, int number);

};
#endif
