#ifndef _DEpos_generator_h
#define _DEpos_generator_h 1
// header file generated from DEpos_generator.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology,
                Mobile Communications Systems
 */
#include "DEStar.h"
#include <Uniform.h>
#include <complex.h>
#include "IntState.h"

class DEpos_generator : public DEStar
{
public:
	DEpos_generator();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ ~DEpos_generator();
	InDEPort restart;
	OutDEPort R_Pos;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
	IntState x_min;
	IntState x_max;
	IntState y_min;
	IntState y_max;
# line 46 "DEpos_generator.pl"
Uniform *random_x;
        Uniform *random_y;

};
#endif
