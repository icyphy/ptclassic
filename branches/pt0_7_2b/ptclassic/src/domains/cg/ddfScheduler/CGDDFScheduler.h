/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer:  Soonhoi Ha

 macroScheduler for efficient and flexible parallel scheduler

*******************************************************************/

#ifndef _CGDDFScheduler_h
#define _CGDDFScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

#include "MacroScheduler.h"

/////////////////////////
// class CGDDFScheduler //
/////////////////////////

class CGDDFScheduler : public MacroScheduler {

public:
	CGDDFScheduler(MultiTarget* t, const char* log = 0) : 
		MacroScheduler(t,log) {}

protected:
	/* virtual */ CGMacroClusterGal* createClusterGal(Galaxy*, ostream*);

};


#endif
