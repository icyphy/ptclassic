#ifndef _AsmArch_h
#define _AsmArch_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Basic assembly language code architecture for a single processor.
 Either an SDFScheduler or a LoopScheduler object can be used as
 the scheduler.  More specialized architectures (for a specific
 processor, etc) can be derived from this.

*******************************************************************/

#include "Architecture.h"
#include "SDFScheduler.h"

class ProcMemory;
class AsmStar;

class AsmArch : public Architecture {
private:
	const char* starClass;
protected:
	ProcMemory& mem;
public:
	AsmArch(char*n, char* d, const char* stype, ProcMemory& m) :
		Architecture(n,d,stype), mem(m) {}
	void printCode ();
	virtual int allocMem(AsmStar&);
	int performAllocation();
	virtual void headerCode();
	virtual int loadCode();
	virtual int runCode();
};

class AsmArchSched : public ArchSched {
public:
	AsmArchSched(AsmArch* a,SDFScheduler* sch) : ArchSched(a), sched(sch){}
protected:
	int setup(Block& galaxy);
	int run (Block& galaxy) {return sched->run(galaxy);}
	void setStopTime(float t) { sched->setStopTime(t);}
	AsmArch* asmArch() const { return (AsmArch*)arch;}
private:
	SDFScheduler* sched;
};
#endif
