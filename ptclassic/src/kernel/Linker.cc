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
	ptolemyName = pathSearch (myName, getenv("PATH"));
	if (!ptolemyName)
		Error::abortRun (
"Cannot locate the running binary! Incremental linking disabled");
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
#ifdef COFF
	filehdr h1;
	aouthdr h2;
	if (read (fd, (char*) &h1, sizeof h1) <= 0 ||
	    read (fd, (char*) &h2, sizeof h2) <= 0) {
		Error::error("Linker: Can't read header from ", objName);
		return FALSE;
	}
	int size = h2.tsize + h2.dsize + h2.bsize + HEADSIZE;
#else
	exec header;

	if (read (fd, (char*) &header, sizeof(header)) <= 0) {
		Error::error("Linker: Can't read header from ", objName);
		return FALSE;
	}
	int size = header.a_text + header.a_data + header.a_bss + HEADSIZE;
#endif
	close (fd);

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
		delete (char*)headerName;
	}
	fd = open (tname, 2, 0);
// unix lets us do this: the file actually disappears when closed.
	unlink (tname);

#ifdef COFF
	if (fd < 0 || read (fd, (char*) &h1, sizeof h1) <= 0 ||
	    read (fd, (char*) &h2, sizeof h2) <= 0) {
		Error::abortRun("Can't read header from incremental link output");
		delete codeBlock;
		close (fd);
		return FALSE;
	}
	long seekpos = N_TXTOFF(h1, h2);
	lseek (fd, seekpos, 0);
#else
	if (fd < 0 || read (fd, (char*) &header, sizeof(header)) <= 0) {
		Error::abortRun("Can't read header from incremental link output");
		delete codeBlock;
		close (fd);
		return FALSE;
	}

	// we should not need this checking but better safe than sorry
	int nsize = header.a_text + header.a_data + header.a_bss;
	if (nsize > size && nsize > availSpace) {
		// code will not fit
		Error::abortRun ("Linker: code size unexpectedly large");
		close (fd);
		return FALSE;
	}
	size = nsize;
#endif

	read (fd, (char*) init_fn, size);
	close (fd);

// makeExecutable is a do-nothing on many systems.
	if (makeExecutable(size,pagsiz,init_fn) != 0) {
		Error::error ("Linker: Can't make code executable");
		delete codeBlock;
		return FALSE;
	}
// get the first word, use it as a function pointer and call it.

	PointerToVoidFunction fn = *(PointerToVoidFunction*)init_fn;
	activeFlag = TRUE;	// indicate dynamically linked objects

// the following line calls the constructors in the dynamically loaded
// routine.  activeFlag tells the KnownBlock class that any objects added
// to the known list are dynamically linked.

	(*fn) ();

	activeFlag = FALSE;	// turn off the indication.
	return TRUE;
}

// This function contains major hackery and is very G++-specific.
// It is known to work on Sun-3, Sun-4, and Vax with g++ 1.37.1.
// It also works on DecStations (with the Mips chip) using the OSF
// (based on g++ 1.37.2) compiler.

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
#ifdef mips
	fprintf (fd, "\t%s %s_$I$_$_%s\n", ADDR_WORD, CONS_PREFIX, name);
	fprintf (fd, "\t%s %s_$D$_$_%s\n", ADDR_WORD, CONS_PREFIX, name);
#else
	fprintf (fd, "\t%s %s_$I$__%d%s\n", ADDR_WORD, CONS_PREFIX, strlen(name), name);
	fprintf (fd, "\t%s %s_$D$__%d%s\n", ADDR_WORD, CONS_PREFIX, strlen(name), name);
#endif
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

