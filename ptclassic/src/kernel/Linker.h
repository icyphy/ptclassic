/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This class implements support for incremental linking.
**************************************************************************/
#ifndef _Linker_h
#define _Linker_h 1
#ifdef __GNUG__
#pragma interface
#endif

class Linker {
public:
// initialization function.  Must be called first.
	static void init(const char* myName);

// link in a new star.
	static int linkObj(const char* objName);

// check whether linker is active (so objects can be marked as dynamically
// linked).  Actually it's set while constructors or other functions that
// have just been linked are being run.
	static int isActive() { return activeFlag;}

// see whether the linker is enabled.
	static int enabled() { return ptolemyName != 0;}

// get name of executable image file (for make-like checking)
	static const char* imageFileName() { return ptolemyName;}
private:
	static char* genHeader(const char*);
	static const char* ptolemyName;
	static int pid;
	static int activeFlag;
};
#endif
