#ifndef _DEParallel2Serial_h
#define _DEParallel2Serial_h 1
// header file generated from DEParallel2Serial.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology,
                    Mobile Communications Systems
 */
#include "DEStar.h"

class DEParallel2Serial : public DEStar
{
public:
	DEParallel2Serial();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	InDEPort x_in;
	InDEPort y_in;
	InDEPort demand;
	OutDEPort x_out;
	OutDEPort y_out;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
private:
# line 41 "DEParallel2Serial.pl"
float listex[21], listey[21];
	    int i, j;

};
#endif
