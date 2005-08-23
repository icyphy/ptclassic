#ifndef _DEDynForkBase_h
#define _DEDynForkBase_h 1
// header file generated from DEDynForkBase.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997-1998 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "DEStar.h"
# line 34 "DEDynForkBase.pl"
// we need a list which contains the addresses of all blocks 
        // and their portholes
        struct receiverAddress {
            int address;
            PortHole *myPort_p;
            receiverAddress *next;
        };

class DEDynForkBase : public DEStar
{
public:
	DEDynForkBase();
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	virtual void removePortHole (PortHole *delPort_p) = 0;
	virtual PortHole* createPortHole (int myName) = 0;

protected:
};
#endif
