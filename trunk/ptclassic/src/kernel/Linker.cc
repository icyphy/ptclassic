static const char file_id[] = "Linker.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This file implements support for incremental linking.  It works both
 for g++ and for Sun C++ (a port of AT&T cfront v2.1).  It should port
 easily to other Unix ports of cfront; all the implementation-specific
 stuff is in Linker.sysdep.h.

 The code can deal with two object-file formats: BSD-ish (Sun-3, Sparc, Vax)
 and COFF (mips).

 It takes an .o file, does an incremental link with respect to the
 running binary, links it in and calls the constructors for any
 global or file-static objects in the file.

 There are some restrictions:

 1. The linked-in code must not call any functions that are not already
 part of the executable program.  If other library functions are needed,
 then switches like -L and -l can be supplied explicitly to multilink.
 No libraries will be searched by default.

 2. The linked-in code must have at least one global or static object
 with a constructor.  Since the only access to the code that is loaded
 consists of calling these constructors, this is vital.

A future version will provide a "permalink" feature, allowing .o files
to become a permanent part of the system.

**************************************************************************/
#include "Linker.sysdep.h"
#include "Linker.h"
#include "Error.h"
#include "miscFuncs.h"
#include "StringList.h"
#include "pt_fstream.h"
#include <ctype.h>
#include "paths.h"

// Define DEBUG for dynamic linking debugging
// DEBUG should be undefined for shipping
//#define DEBUG
#ifdef DEBUG
void DebugMessage(char *str1, char *str2)
{
  char buf[2000];
  sprintf("%s %s\n",str1, str2);
  Error::message(buf);
}
#endif //DEBUG

// static data objects
int Linker::activeFlag = 0;
const char* Linker::ptolemyName = 0;
const char* Linker::symTableName = 0;
// by default, search math and C libraries
const char* Linker::myDefaultOpts = "-lm -lc";
char* Linker::memBlock = 0;
char* Linker::availMem = 0;

const size_t LINK_MEMORY = (size_t)(1024L * 1024L);

// This routine creates the memory block if needed, and aligns it
// to a page boundary.

void Linker::adjustMemory() {
	if (!memBlock) {
		memBlock = new char[LINK_MEMORY];
		availMem = memBlock;
	}
	// bump availMem up to the next page boundary
	// Note that now we always bump up to the next page boundary
	// Formerly we only bumped if we were not initializing memBlock
	size_t t = (size_t) availMem;
	size_t ps = getpagesize();
	size_t t2 = (t / ps) * ps;
	if (t2 < t) availMem = (char *) (t2 + ps);
}

void Linker::init (const char* myName) {
// locate the ptolemy binary that is currently running.
// main should call this with argv[0] as an argument.
// We believe argv[0] without checking if it begins with '/'
	ptolemyName = pathSearch (myName);
	if (!ptolemyName)
		Error::abortRun (
"Cannot locate the running binary! Incremental linking disabled");
// initially we use the executable as the symbol table
	symTableName = ptolemyName;
	return;
}

typedef void (*PointerToVoidFunction) ();

// This one is for backward compatibility.
int Linker::linkObj(const char* objName) {
	char* argv[3];
	argv[0] = "link";
	argv[1] = savestring(objName);
	argv[2] = 0;
	int status = multiLink(2, argv);
	LOG_DEL; delete [] argv[1];
	return status;
}

// class for use in generating a temporary file name that is cleaned
// up when the object is deleted.
class TFile {
public:
	TFile() : tname(tempFileName()) {}
	~TFile() {
		unlink(tname);
		LOG_DEL; delete [] tname;
	}
	operator const char*() const { return tname;}
private:
	char* tname;
};

int Linker::multiLink (const char* args, int perm) {
	// make copy, leaving room for "permlink " and null terminator.
	int l = strlen(args);
	LOG_NEW; char* buf = new char[l + 10];
	strcpy(buf, perm ? "permlink " : "templink ");
	strcat(buf, args);
	// max # of possible args is given if args has one-char args
	// separated by spaces: at most l/2 args.  + "permlink", + null at end.
	LOG_NEW; char** argv = new char *[l/2+2];
	int argc = 0;
	// now chop up the arglist.
	char *p = buf;
	while (*p) {
		argv[argc++] = p;
		while (*p && !isspace(*p)) p++;
		if (*p) {
			// null-terminate the string
			*p++ = 0;
			// skip any extra spaces.
			while (*p && isspace(*p)) p++;
		}
	}
	argv[argc] = 0;
	int status = multiLink(argc,argv);
	LOG_DEL; delete [] buf;
	LOG_DEL; delete [] argv;
	return status;
}

	
// link in multiple files.  Arglist is char** and not const char**
// to permit easy interfacing with Tcl.

int Linker::multiLink (int argc, char** argv) {
	if (linkingNotSupported) {
		Error::abortRun ("Sorry, incremental linking is not yet ",
				 "supported on this architecture");
		return FALSE;
	}
	else if (!symTableName) {
		Error::abortRun ("Incremental linking disabled");
		return FALSE;
	}
	TFile tname;

// round up to be on a page boundary
	adjustMemory();

// generate the loader command.  The switches mean:
// -A: incremental loading, using symbols from the ptolemy binary.
// LOC_OPT (-T or -R): specify starting address of text segment.
// Other switches are in LOADOPTS, from Linker.sysdep.h

	char command[512];
	// DYNLIB is hppa specific
	sprintf (command, "%s %s -A %s %s %x %s -o ",
		 LOADER, LOADOPTS, symTableName, LOC_OPT, availMem, DYNLIB);
#ifdef DEBUG
	Error::message(command);
#endif
	StringList cmd = command;
	cmd << tname;
	for (int i = 1; i < argc; i++) {
		const char* objName = expandPathName(argv[i]);
		cmd << " " << objName;
	}
	// these options go last so libraries will be searched properly.
	cmd << " " << myDefaultOpts;

	if (system (cmd)) {
		Error::abortRun("Error in linking file");
		return FALSE;
	}

	size_t size = readInObj(tname);

	if (size == 0) {
		return FALSE;
	}

// makeExecutable is a do-nothing on many systems.
	if (makeExecutable(size,pagsiz,availMem) != 0) {
		Error::error ("Linker: Can't make code executable");
		return FALSE;
	}

// flag for permanent-link or not
	int perm = (argv[0][0] == 'p');	// KLUDGE!

// invoke constructors in the newly read in code
	if (!perm) activeFlag = TRUE;	// indicate dynamically linked objects
	int nCalls = invokeConstructors(tname);
	int status = (nCalls >= 0);

	if (nCalls == 0 && !perm) {
		Error::abortRun("Linker: no constructors in linked-in code!");
		status = FALSE;
	}

	activeFlag = FALSE;	// turn off the indication.
	if (status) {
// we do not bump the memory pointer until now; this makes it official
		availMem += size;
// if this is a "permalink", make tfile the new symbol file
		if (perm) installTable(tname);
	}
	return status;
}

void Linker::installTable(const char* newTable) {
	if (ptolemyName == symTableName) {
		symTableName = getenv("PTOLEMY_SYM_TABLE");
		if (!symTableName) symTableName =
			hashstring(expandPathName("~/.pt_symtable"));
	}
	ifstream in(newTable);
	ofstream out(symTableName, ios::out, 0600);
	if (!in || !out) {
		Error::abortRun("Error installing new symbol table: ",
				"Further dynamic linking disabled");
		symTableName = 0;
	}
	else {
		char ch;
		while (in.get(ch)) out.put(ch);
	}
}

// This function scans the symbol table of the code that is to be
// read into Ptolemy, invoking any constructor functions that are
// in the section of code just read in (between bottom and top).
// The "nm" program is used to find the constructors.
// It returns the number of constructors called, -1 on error.

#ifdef DEBUG
// Function to print out debugging info
static void debugInvokeConstructors(char *symbol,long addr, const
				    char* objName) 
{
  	  char buf[256];
	  sprintf(buf, "Pipe found: %s at 0x%x", symbol, addr);
	  Error::message(buf);

// Here we violate the principal of keeping architecture dependencies
// out of this file.  These defines are for debugging.
#if defined(hppa)

	  // Verify the address using nlist() ...
	  char tmpname[1024];
	  struct nlist nl[2];

	  nl[0].n_name = symbol;
	  nl[1].n_name = (char *)NULL;

	  strcpy(tmpname,objName);

	  if (nlist(tmpname,nl))
          {
	     Error::abortRun("nlist failed");
	  } else {
	     addr = nl[0].n_value;
	     sprintf(buf, "Nlist found: %s at 0x%x", symbol, addr);
	     Error::message(buf);
	  }
#endif //hppa
}
#endif

int
Linker::invokeConstructors (const char* objName) {
// Open a pipe to "nm" to get symbol information
	StringList command = NM_PROGRAM;
	command << " " << NM_OPTIONS << " " << objName;

#ifdef DEBUG
	{
	  char buf[256];
	  sprintf(buf, "NM Pipe: %s", (const char *)command);
	  Error::message(buf);
	}
#endif

// ALPHAFIX is needed because ALPHA/OSF provides a bogus prototype for popen.
	FILE* fd = popen (ALPHAFIX command, "r");
	if (fd == 0) {
		Error::abortRun("Linker: can't exec ", NM_PROGRAM);
		return -1;
	}
// read the output from nm, searching for GLOBAL symbols with
// value > base.
	int nCalls = 0;
	long addr;
	char symbol[256], type[20];
	size_t memoryEnd = (size_t)memBlock + LINK_MEMORY;


	while (fscanf(fd, "%lx %s %s", &addr, type, symbol) == 3) {
		if (addr >= (size_t)availMem && addr <= memoryEnd &&
		    strncmp(symbol, CONS_PREFIX, CONS_LENGTH) == 0) {
			// it is a constructor, call it:
#ifdef DEBUG
		  debugInvokeConstructors(symbol, addr, objName);
#endif
		  	PointerToVoidFunction fn = (PointerToVoidFunction)addr;
			fn();
			nCalls++;
		}
	}
	pclose (fd);
	return nCalls;
}

size_t Linker::readInObj(const char* objName) {
	STRUCT_DEFS;		// macro, defines sys-dependent header structs
	size_t size;

	int fd = open(objName, O_RDWR, 0);
	if (fd < 0) return 0;

// the following macro reads headers into the structures defined above,
// returning true if there is a failure.
	if (READHEAD_FAIL) {
		Error::abortRun("Linker: Can't read header from incremental link output");
		close (fd);
		return 0;
	}
// a macro to set the size.
	size = OBJ_SIZE;
	if (availMem + size > memBlock + LINK_MEMORY) {
		Error::abortRun("Linker: insufficient space to read in new code");
		size = 0;
	} else {
		// clear out all the existing memory (mainly for bss)
		memset( availMem, 0, size);
// this macro does whatever seeks and reads are necessary to read in the code.
		if (READOBJ_FAIL) {
			Error::abortRun("Linker: Error reading object file!");
			size = 0;
		}
	}

#ifdef DEBUG
#ifdef hpux
        {
	  char buf[1024];
	  sprintf(buf,
	    "availmem: 0x%x\ntextseg starts at 0x%x, size: 0x%x\ndataseg starts at 0x%x, size: 0x%x\n",
	     availMem,
	     h2.exec_tmem, h2.exec_tsize, h2.exec_dmem, h2.exec_dsize);
	  Error::message(buf);
	}
#endif
#endif /* DEBUG */
	INITIALIZE();
	FLUSH_CACHE();

	close(fd);
	return size;
}

// function to set default options: uses hashstring to manage memory
void Linker::setDefaultOpts(const char* arg) {
	myDefaultOpts = hashstring(arg);
}

// helper class to clean things up -- frees memory and zaps the
// symbol table file.

class Linker_Cleanup {
public:
	Linker_Cleanup() {}
	~Linker_Cleanup() {
		delete [] Linker::memBlock;
		if (Linker::ptolemyName != Linker::symTableName)
			unlink(Linker::symTableName);
	}
};

static Linker_Cleanup linkerCleanup;
