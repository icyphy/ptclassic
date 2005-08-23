/**************************************************************************
Version identification:
@(#)Linker.h	2.14 09/05/96

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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

class StringList;

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
// NOTE: for historical reasons, isActive lies about permlinked stars:
// it returns FALSE when permlinking.  doingDynLink returns TRUE during
// both perm and non-perm links.
	static int isActive();
	static int doingDynLink();

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
	static int invokeConstructors(const char* objName,
                                      void * dlhandle = (void *)NULL );

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

	// indicator that the linker is active and doing a non-perm link
	static int activeFlag;

	// indicator that the linker is active (either perm or non-perm link)
	static int dynLinkFlag;

	// default options to be added to links
	static const char* myDefaultOpts;

	// Below here, these slots are only used for dlopen() style linking

	// Generate a .so file from one or more .o or .so files
	static char *generateSharedObject( int argc, char **argv,
 					   char* objName, int maxsize); 

	// Link Sequence number.  Incremented with each link.
	static int linkSeqNum;

	// Names of all the files that have been permlinked
	static StringList permlinkFiles;
};
#endif
