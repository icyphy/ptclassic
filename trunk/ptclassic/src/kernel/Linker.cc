static const char file_id[] = "Linker.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
 part of the executable program.

 2. The linked-in code must have at least one global or static object
 with a constructor.  Since the only access to the code that is loaded
 consists of calling these constructors, this is vital.

**************************************************************************/
#ifndef hpux
#include "Linker.sysdep.h"
#endif
#include "Linker.h"
#include "Error.h"
#include "miscFuncs.h"
#include "StringList.h"
#include "streamCompat.h"

// static data objects
int Linker::activeFlag = 0;
int Linker::pid = 0;
const char* Linker::ptolemyName = 0;

const char*
pathSearch (const char* name, const char* path) {
// if name has slashes it must be exact
	if (strchr (name, '/')) {
		return access(name, 0) == 0 ? hashstring(name) : 0;
	}
	char nameBuf[512];
// build the next candidate name
	while (*path) {
		char* q = nameBuf;
		while (*path && *path != ':') *q++ = *path++;
		*q++ = '/';
		strcpy (q, name);
		if (access (nameBuf, 0) == 0) return hashstring(nameBuf);
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
#ifdef hpux
	Error::abortRun ("Incremental linking not yet supported for HP-UX");
	return FALSE;
}
#else
	if (!ptolemyName)
		Error::abortRun ("Incremental linking disabled");
// here, objName is the pathname of the .o file.
	objName = expandPathName (objName);
	char tname[20];
	strcpy (tname, "/tmp/hack.XXXXXX");
	mktemp (tname);

// first, read the object file
	int fd;
	if ((fd = open (objName, O_RDONLY, 0)) < 0) {
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
	LOG_NEW; void* codeBlock = new char[size+pagsiz];
	size_t init_fn = (size_t) codeBlock;

// round up to be on a page boundary
	init_fn += pagsiz-1;
	init_fn &= ~(pagsiz-1);
	int availSpace = size + pagsiz - (init_fn - (size_t)codeBlock);

// generate the header.  The first word is a pointer to the initialization
// function, the second to the termination function.
	char* headerName = genHeader (objName);
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
		LOG_DEL; delete codeBlock;
		unlink (tname);
		return FALSE;
	}
	else {
		// clean up
		unlink (headerName);
		LOG_DEL; delete (char*)headerName;
	}
	fd = open (tname, O_RDWR, 0);
// unix lets us do this: the file actually disappears when closed.
	unlink (tname);

#ifdef COFF
	if (fd < 0 || read (fd, (char*) &h1, sizeof h1) <= 0 ||
	    read (fd, (char*) &h2, sizeof h2) <= 0) {
		Error::abortRun("Can't read header from incremental link output");
		LOG_DEL; delete codeBlock;
		close (fd);
		return FALSE;
	}
	long seekpos = N_TXTOFF(h1, h2);
	lseek (fd, seekpos, 0);
#else
	if (fd < 0 || read (fd, (char*) &header, sizeof(header)) <= 0) {
		Error::abortRun("Can't read header from incremental link output");
		LOG_DEL; delete codeBlock;
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
		LOG_DEL; delete codeBlock;
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

// find the first occurrence of subString in line.
static char* match(char* line, const char* subString) {
	if (*subString == 0) return line;
	char* p = line;
	int ltail = strlen(subString) - 1;
	while (1) {
		p = index(p, *subString);
		if (p == 0) return 0;
		if (strncmp (p+1, subString+1, ltail) == 0)
			return p;
		p++;
	}
}

const int LINELEN = 128;

// This function contains a bit of hackery.  It scans an object file
// looking for a global symbol of a particular form, and creates a
// small assembly language model that refers to that name.  This
// technique of providing a hook to call the constructor works for
// g++ and for Sun's port of cfront.  For compilers that use a
// completely different method, you'll need to reimplement this.

// The nm command is used to search for a symbol of the right form.

char*
Linker::genHeader (const char* objName) {
// Open a pipe to "nm" to get symbol information
	char command[256];
	char* sym = 0;
	sprintf (command, "%s -g %s", NM_PROGRAM, objName);
	FILE* fd = popen (command, "r");
	if (fd == 0) {
		Error::abortRun("Linker: can't exec '/bin/nm' command");
		return 0;
	}
// read the output from nm, searching for GLOBAL
	char line[LINELEN];
	while (fgets (line, LINELEN, fd) &&
	       (sym = match (line, CONS_PREFIX)) == 0)
		;
	pclose (fd);
	if (sym == 0) {
		Error::abortRun("Linker::genHeader: no constructors in ",
				objName);
		return 0;
	}
// trim trailing whitespace from sym
	char* p = sym + strlen(sym) - 1;
	while (*p <= ' ') p--;
	*(p+1) = 0;

	char aname[32], oname[32];
	sprintf (aname, "/tmp/ptlink.%d.s", pid);
	sprintf (oname, "/tmp/ptlink.%d.o", pid);
	fd = fopen (aname, "w");
// output some assembler stuff
	fprintf (fd, "\t%s\n", TEXT_DIR);
	fprintf (fd, "\t%s %s\n", ADDR_WORD, sym);
	sym[ID_POS] = DEST_CHAR;
	fprintf (fd, "\t%s %s\n", ADDR_WORD, sym);
	fclose (fd);
// assemble the file

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

#endif /* hp-ux */
