/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This file implements support for incremental linking.
**************************************************************************/
// Dynamic linker!

#include "Linker.sysdep.h"
#include "Linker.h"
#include "Output.h"
#include "miscFuncs.h"
#include <sys/stat.h>

extern Error errorHandler;

void Linker::init (const char* myName) {
// locate the ptolemy binary that is currently running.
// main should call this with argv[0] as an argument.
// We believe argv[0] without checking if it begins with '/'
	if (*myName == '/') {
		ptolemyName = myName;
		return;
	}
	char path[BUFSIZ], tmpname[BUFSIZ];
	char* p;
	strcpy (path, getenv ("PATH"));
	struct stat statbuf;
	for (p = strtok (path, ":"); p; p = strtok (NULL, ":")) {
		sprintf (tmpname, "%s/%s", p, myName);
		if (access (tmpname, X_OK) != 0) continue;
		if (stat((char*)myName, &statbuf) != 0) continue;
		if ((statbuf.st_mode & S_IFMT) == S_IFDIR) continue;
		ptolemyName = savestring (tmpname);
		return;
	}
	errorHandler.error ("Cannot locate the running binary!");
	exit (1);
}

typedef void (*PointerToVoidFunction) ();

int Linker::linkObj (const char* objName) {
	errorHandler.error("Warning: 'link' doesn't quite work yet.");
// here, objName is the pathname of the .o file.
	char tname[20];
	strcpy (tname, "/tmp/hack.XXXXXX");
	mktemp (tname);

	int fd;
	if ((fd = open (objName, 2, 0)) < 0) {
		errorHandler.error("Can't open file ", objName);
		return FALSE;
	}

	exec header;

	if (read (fd, (void*) &header, sizeof(header)) <= 0) {
		errorHandler.error("Can't read header from ", objName);
		return FALSE;
	}
	close (fd);

	int size = header.a_text + header.a_data + header.a_bss;
	int pagsiz = getpagesize();

	if (size < pagsiz) size = pagsiz;
	void* codeBlock = new char[size+pagsiz];
	size_t init_fn = (size_t) codeBlock;

// round up to be on a page boundary
	init_fn += pagsiz-1;
	init_fn &= ~(pagsiz-1);

// generate the loader command.  The switches mean:
// -A: incremental loading, using symbols from the ptolemy binary.
// -T: specify starting address of text segment at init_fn.
// -N: do not make the text portion read-only.

	char command[512], *cmd = command;
	sprintf (cmd, "%s -N -A %s -T %x %s %s -o %s -lg++ -lc",
		 LDXX, ptolemyName, init_fn, PREFIX, objName, tname);

	if (system (cmd)) {
		errorHandler.error("Error in linking file");
		delete codeBlock;
		unlink (tname);
		return FALSE;
	}
	fd = open (tname, 2, 0);
	if (lseek (fd, sizeof (header), L_SET) < 0) {
		errorHandler.error("Error in temp file seek");
		delete codeBlock;
		unlink (tname);
		return FALSE;
	}
	read (fd, (char*) init_fn, size);
	close (fd);
	if (makeExecutable(size,pagsiz,init_fn) != 0) {
		errorHandler.error ("Can't make code executable -- mprotect");
		delete codeBlock;
		unlink (tname);
		return FALSE;
	}
	PointerToVoidFunction fn = (PointerToVoidFunction) init_fn;
	(*fn) ();
	return TRUE;
}

