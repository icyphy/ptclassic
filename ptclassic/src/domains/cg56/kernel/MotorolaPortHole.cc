static const char file_id[] = "MotorolaConnect.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MotorolaConnect.h"

// allocate portholes in X memory by default
// this can be changed by specifying, say, P_YMEM
// in the attributes block in a star.
MotorolaPortHole::MotorolaPortHole() { setAttributes(P_XMEM);}

int InMotorolaPort :: isItInput() const { return TRUE; }
int OutMotorolaPort :: isItOutput() const { return TRUE; }
int MultiInMotorolaPort :: isItInput() const { return TRUE; }
int MultiOutMotorolaPort :: isItOutput() const { return TRUE; }

// Dummy
int MultiMotorolaPort :: someFunc() { return 1; }

PortHole& MultiInMotorolaPort :: newPort () {
        LOG_NEW; PortHole& p = *new InMotorolaPort;
        p.numberTokens = numberTokens;
        return installPort(p);
}

PortHole& MultiOutMotorolaPort :: newPort () {
	LOG_NEW; OutMotorolaPort* p = new OutMotorolaPort;
        p->numberTokens = numberTokens;
	forkProcessing(*p);
        return installPort(*p);
}
