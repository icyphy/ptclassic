#ifndef _DEDynForkInt_h
#define _DEDynForkInt_h 1
// header file generated from DEDynForkInt.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "DEDynForkBase.h"

class DEDynForkInt : public DEDynForkBase
{
public:
	DEDynForkInt();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ ~DEDynForkInt();
	InDEPort in;
	MultiOutDEPort out;
	PortHole* createPortHole (int myName);
	void removePortHole (PortHole *delPort_p);

protected:
	/* virtual */ void go();
private:
# line 63 "DEDynForkInt.pl"
int myName;
        receiverAddress *receiverAddress_p, *myList_h, *myList_t;

};
#endif
