/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This class implements support for incremental linking.  There are two
 versions: linkObj, which links in a single new module, and multiLink,
 which permits arbitrary linker options to be supplied.

**************************************************************************/
#ifndef _Linker_h
#define _Linker_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include <std.h>

class Linker {
	friend class Linker_Cleanup;
public:
// initialization function.  Must be called first.
	static void init(const char* myName);

// link in a new star.
	static int linkObj(const char* objName);

// link in multiple files, search libraries, etc.

// first form: args is the linker argument list, permanent is a boolean
// that chooses whether link is permanent or temporary.

// second form ("Tcl form"): argv[0], if begins with "p", specifies
// a permanent link, otherwise temporary; remaining args are linker args.

	static int multiLink(const char* args, int permanent);
	static int multiLink(int argc, char** argv);

// check whether linker is active (so objects can be marked as dynamically
// linked).  Actually it's set while constructors or other functions that
// have just been linked are being run.
	static int isActive() { return activeFlag;}

// see whether the linker is enabled.
	static int enabled() { return ptolemyName != 0;}

// get name of executable image file (for make-like checking)
	static const char* imageFileName() { return ptolemyName;}

// return current value of defaultOpts
	static const char* defaultOpts() { return myDefaultOpts;}

// set new defaultOpts value
	static void setDefaultOpts(const char* newValue);
private:
	// invoke global constructors in the object file
	static int invokeConstructors(const char* objName);

	// read the object file into memory (at availMem)
	static size_t readInObj(const char* objName);

	// install a new symbol table for future dynamic links
	static void installTable(const char* newTable);

	// align availMem on a page boundary
	static void adjustMemory();

	// the full path of the current executable
	static const char* ptolemyName;

	// the full path of the file being used as the symbol table
	static const char* symTableName;

	// the block of memory available for loading into
	static char* memBlock;

	// pointer to the current position in memBlock
	static char* availMem;

	// indicator that the linker is active
	static int activeFlag;

	// default options to be added to links
	static const char* myDefaultOpts;
};
#endif
