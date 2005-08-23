#ifndef _SRCGCPrinter_h
#define _SRCGCPrinter_h 1
// header file generated from SRCGCPrinter.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCStar.h"
#include "StringState.h"

class SRCGCPrinter : public SRCGCStar
{
public:
	SRCGCPrinter();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock openfile;
	/* virtual */ int myExecTime();
	/* virtual */ void wrapup();
	/* virtual */ void initCode();
	MultiInSRCGCPort input;

protected:
	/* virtual */ void setup();
	/* virtual */ void tick();
	StringState fileName;
	StringState prefix;

private:
# line 93 "SRCGCPrinter.pl"
int fileOutput;

};
#endif
