#ifndef _CG56Target_h
#define _CG56Target_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "AsmTarget.h"
#include "ProcMemory.h"

class CG56Memory : public DualMemory {
public:
	CG56Memory(unsigned x_addr,	// start of x memory
		   unsigned x_len,	// length of x memory
		   unsigned y_addr,	// start of y memory
		   unsigned y_len	// length of y memory
		   );
};

class CG56Target : public AsmTarget {
public:
	CG56Target (const char* nam, const char* desc,
		    unsigned x_addr, unsigned x_len,
		    unsigned y_addr, unsigned y_len);
	Block* clone() const;
	void headerCode();
	void wrapup();
	StringList beginIteration(int repetitions, int depth);
	StringList endIteration(int repetitions, int depth);
	~CG56Target();

private:
	unsigned xa, xl, ya, yl;
};

#endif
