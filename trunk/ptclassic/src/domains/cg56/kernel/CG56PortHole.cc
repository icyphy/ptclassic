static const char file_id[] = "CG56Connect.cc";
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

#include "CG56Connect.h"

int InCG56Port :: isItInput() const { return TRUE; }
int OutCG56Port :: isItOutput() const { return TRUE; }
int MultiInCG56Port :: isItInput() const { return TRUE; }
int MultiOutCG56Port :: isItOutput() const { return TRUE; }

PortHole& MultiInCG56Port :: newPort () {
        LOG_NEW; PortHole& p = *new InCG56Port;
        p.numberTokens = numberTokens;
        return installPort(p);
}

PortHole& MultiOutCG56Port :: newPort () {
	LOG_NEW; OutCG56Port* p = new OutCG56Port;
        p->numberTokens = numberTokens;
	forkProcessing(*p);
        return installPort(*p);
}
