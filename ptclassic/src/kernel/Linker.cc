/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This file implements support for incremental linking.  It is quite
 g++-specific.  It takes an .o file and loads it; the code it generates
 to call constructors will only work if the .o file is a definition of
 a Ptolemy star (using ptlang, preferably).

 The linked-in code must not call any functions that are not already
 part of the executable program.

**************************************************************************/

#include "Linker.sysdep.h"
#include "Linker.h"
#include "Error.h"
#include "miscFuncs.h"
#include "StringList.h"
#include <sys/stat.h>

// static data objects
int Linker::activeFlag = 0;
const char* ptolemyName = 0;

const char*
pathSearch (const char* name, const char* path) {
// if name has slashes it must be exact
	if (strchr (name, '/')) {
		return access(name, 0) == 0 ? savestring(name) : 0;
	}
	char nameBuf[512];
// build the next candidate name
	while (*path) {
		char* q = nameBuf;
		while (*path && *path != ':') *q++ = *path++;
		*q++ = '/';
		strcpy (q, name);
		if (access (nameBuf, 0) == 0) return savestring(nameBuf);
		if (*path == ':') path++;
	}
	return 0;
}

void Linker::init (const char* myName) {
// locate the ptolemy binary that is currently running.
// main should call this with argv[0] as an argument.
// We believe argv[0] without checking if it begins with '/'
	pid = getpid();
#ifdef mips
// not yet implemented
	ptolemyName = 0;
#else
	ptolemyName = pathSearch (myName, getenv("PATH"));
	if (!ptolemyName)
		Error::abortRun (
"Cannot locate the running binary! Incremental linking disabled");
#endif
	return;
}

typedef void (*PointerToVoidFunction) ();
const int HEADSIZE = 2 * sizeof (PointerToVoidFunction);

int Linker::linkObj (const char* objName) {
	if (!ptolemyName)
		Error::abortRun ("Incremental linking disabled");
// here, objName is the pathname of the .o file.
	objName = expandPathName (objName);
	char tname[20];
	strcpy (tname, "/tmp/hack.XXXXXX");
	mktemp (tname);

// first, read the object file
	int fd;
	if ((fd = open (objName, 0, 0)) < 0) {
		Error::error("Linker: Can't open file ", objName);
		return FALSE;
	}

	exec header;

	if (read (fd, (void*) &header, sizeof(header)) <= 0) {
		Error::error("Linker: Can't read header from ", objName);
		return FALSE;
	}
	close (fd);

	int size = header.a_text + header.a_data + header.a_bss + HEADSIZE;
	int pagsiz = getpagesize();

	if (size < pagsiz) size = pagsiz;
	void* codeBlock = new char[size+pagsiz];
	size_t init_fn = (size_t) codeBlock;

// round up to be on a page boundary
	init_fn += pagsiz-1;
	init_fn &= ~(pagsiz-1);
	int availSpace = size + pagsiz - (init_fn - (size_t)codeBlock);

// generate the header.  The first word is a pointer to the initialization
// function, the second to the termination function.
	const char* headerName = genHeader (objName);
	if (!headerName) return FALSE;

// generate the loader command.  The switches mean:
// -A: incremental loading, using symbols from the ptolemy binary.
// -T: specify starting address of text segment at init_fn.
// Other switches are in LOADOPTS, from Linker.sysdep.h

	char command[512], *cmd = command;
	sprintf (cmd, "%s %s -A %s -T %x %s %s -o %s",
		 LOADER, LOADOPTS, ptolemyName, init_fn,
		 headerName, objName, tname);

	if (system (cmd)) {
		Error::abortRun("Error in linking file");
		delete codeBlock;
		unlink (tname);
		return FALSE;
	}
	else {
		// clean up
		unlink (headerName);
		delete headerName;
	}
	fd = open (tname, 2, 0);
//	if (lseek (fd, sizeof (header), L_SET) < 0)
	if (read (fd, (void*) &header, sizeof(header)) <= 0) {
		Error::abortRun("Can't read header from incremental link output");
		delete codeBlock;
		unlink (tname);
		return FALSE;
	}
	// we should not need this checking but better safe than sorry
	int nsize = header.a_text + header.a_data + header.a_bss;
	if (nsize > size && nsize > availSpace) {
		// code will not fit
		Error::abortRun ("Linker: code size unexpectedly large");
		return FALSE;
	}

	read (fd, (char*) init_fn, nsize);
	close (fd);
	if (makeExecutable(size,pagsiz,init_fn) != 0) {
		Error::error ("Linker: Can't make code executable");
		delete codeBlock;
		unlink (tname);
		return FALSE;
	}
// get the first word, use it as a function pointer and call it.
	PointerToVoidFunction fn = *(PointerToVoidFunction*)init_fn;
	activeFlag = TRUE;	// indicate dynamically linked objects
	(*fn) ();
	activeFlag = FALSE;	// turn off the indication.
	return TRUE;
}

// This function contains major hackery and is very G++-specific.
// It is known to work on Sun-3's and Sun-4's with g++ 1.37.1.
// it should really be more like collect; for now we'll assume there
// is always one startup function to be called, and that the names
// generated always have the form generated by the printfs below.
const char*
Linker::genHeader (const char* objName) {
	char name[64];
// form the basename for the file.
	const char* bname = strrchr (objName, '/');
	if (!bname) bname = objName;
	else bname++;		// skip the '/'
	strcpy (name, bname);
// eliminate the ".o" suffix, if present.
	char* p = strrchr (name, '.');
	if (p) *p = 0;

	char aname[32], oname[32];
	sprintf (aname, "/tmp/ptlink.%d.s", pid);
	sprintf (oname, "/tmp/ptlink.%d.o", pid);
	FILE* fd = fopen (aname, "w");
// output some assembler stuff
	fprintf (fd, "\t%s\n", TEXT_DIR);
	fprintf (fd, "\t%s %s_$I$__%d%s\n", ADDR_WORD, CONS_PREFIX, strlen(name), name);
	fprintf (fd, "\t%s %s_$D$__%d%s\n", ADDR_WORD, CONS_PREFIX, strlen(name), name);
	fclose (fd);
// assemble the file
	char command[256];
	sprintf (command, "%s %s -o %s", ASSEMBLER, aname, oname);
	int status = system (command);
	unlink (aname);
	if (status) {
		Error::abortRun ("Linker::genHeader: error in dynamic link");
		unlink (oname);
		return 0;
	}
	return savestring(oname);
}

