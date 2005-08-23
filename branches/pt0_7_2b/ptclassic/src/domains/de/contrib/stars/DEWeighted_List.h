#ifndef _DEWeighted_List_h
#define _DEWeighted_List_h 1
// header file generated from DEWeighted_List.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology,
                    Mobile Communications Systems
 */
#include "DEStar.h"

class DEWeighted_List : public DEStar
{
public:
	DEWeighted_List();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	InDEPort CIR;
	OutDEPort testx;
	OutDEPort testy;

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
private:
# line 33 "DEWeighted_List.pl"
float value; 
	    int number, how_many_outputs;
	    float dummy;
	    int liste[21];

};
#endif
