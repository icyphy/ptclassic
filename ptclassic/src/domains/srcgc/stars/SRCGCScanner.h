#ifndef _SRCGCScanner_h
#define _SRCGCScanner_h 1
// header file generated from SRCGCScanner.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */
#include "SRCGCStar.h"
#include "StringState.h"

class SRCGCScanner : public SRCGCStar
{
public:
	SRCGCScanner();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	static CodeBlock openfile;
	/* virtual */ int myExecTime();
	/* virtual */ void wrapup();
	/* virtual */ void initCode();
	OutSRCGCPort output;

protected:
	/* virtual */ void go();
	StringState fileName;

private:
# line 80 "SRCGCScanner.pl"
int fileInput;

};
#endif
