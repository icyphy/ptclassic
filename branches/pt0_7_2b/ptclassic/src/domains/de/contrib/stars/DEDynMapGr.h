#ifndef _DEDynMapGr_h
#define _DEDynMapGr_h 1
// header file generated from DEDynMapGr.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology 
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "DEDynMap.h"

class DEDynMapGr : public DEDynMap
{
public:
	DEDynMapGr();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void wrapup();
	/* virtual */ ~DEDynMapGr();
	MultiInDEPort exin;
	MultiOutDEPort exout;

protected:
	/* virtual */ void setup();
private:
# line 41 "DEDynMapGr.pl"
Block *masterBlock_p;
        InfString outputMap, inputMap;
	Block * createBlock ();

};
#endif
