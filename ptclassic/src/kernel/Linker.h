/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This class implements support for incremental linking.
**************************************************************************/

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
private:
	static const char* genHeader(const char*);
	static const char* ptolemyName;
	static int pid;
	static int activeFlag;
};
