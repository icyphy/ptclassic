/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 12/18/90

 The routines in this file support preprocessing and compilation of
 Ptolemy star definitions, and make calls to the Linker class to
 dynamically link new stars into the system.

 We attempt to avoid compilation unless the .o file is "out of date."

**************************************************************************/

#include <fcntl.h>
#include <std.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Linker.h"
#include "Domain.h"
#include "miscFuncs.h"
#include "StringList.h"
#include "Output.h"
#include <ctype.h>

#ifdef __sun__
#ifdef __mc68000__
#define ARCH "sun3"
#endif
#ifdef __sparc__
#define ARCH "sun4"
#endif
#endif

#ifdef vax
#define ARCH "vax"
#endif
#ifdef mips
#define ARCH "mips"
#endif

// pigi funcs to call
extern "C" void PrintDebug(const char*);
extern "C" void DirName(char*);
extern "C" char* BaseName(const char*);
extern "C" int IconFileToSourceFile (const char*, char*, char*);

static void strcpyLC (char* out, const char* in) {
	char c;
	while ((c = *in++) != 0) {
		if (isupper (c)) c = tolower(c);
		*out++ = c;
	}
	*out = 0;
	return;
}

static int noPermission (const char* msg, const char* file) {
	StringList sl = msg;
        sl += file;
	sl += ": ";
	sl += sys_errlist[errno];
	Error::abortRun (sl);
	return FALSE;
}

inline static int exists (const char* file) {
	return access(file, 0) == 0;
}

static int isYounger (const char* fileA, const char* fileB) {
	struct stat bufA, bufB;
	if (stat ((char*)fileA, &bufA) < 0 || stat ((char*)fileB, &bufB) < 0)
		return FALSE;
	return bufA.st_mtime > bufB.st_mtime;
}

static int compile (const char* name, const char* idomain, const char* srcDir,
		    const char* objDir)
{
	char domain[32], cmd[512];
	const char* PTOLEMY = getenv ("PTOLEMY");
	if (PTOLEMY == 0) PTOLEMY = expandPathName ("~ptolemy");
	strcpyLC (domain, idomain);
	sprintf (cmd, "cd %s; g++ -c -I %s/src/domains/%s/kernel "
		 "-I %s/src/kernel -I %s %s/%s%s.cc", objDir, PTOLEMY,
		 domain, PTOLEMY, srcDir, srcDir, idomain, name);
	PrintDebug (cmd);
	if (system (cmd)) {
		Error::abortRun ("Loader: errors in compilation");
		return FALSE;
	}
	return TRUE;
}

static char* genObjDir (const char* src) {
	char buf[512];
	char* p = buf;

	while (*src) {
		if (*src == '/' && strncmp (src, "/src/", 5) == 0) {
			strcpy (p, "/obj.");
			strcpy (p + 5, ARCH);
			strcat (p, "/");
			p += 6 + strlen (ARCH);
			src += 5;
		}
		else *p++ = *src++;
	}
	return savestring (buf);
}

// Function to initialize the loader
extern "C" void
KcLoadInit (const char* argv0) {
	Linker::init (argv0);
}

		


// Here is the function that loads in a star!
// name = username of the star
// idomain = domain of the star
// srcDir = star source directory
int
compileAndLink (const char* name, const char* idomain, const char* srcDir) {
	char* objDir = genObjDir (srcDir);
	char plName[512], oName[512], ccName[512], cmd[512];
// debug statement
	sprintf (cmd, "compileAndLink(%s,%s,%s)", name, idomain, srcDir);
	PrintDebug (cmd);
	sprintf (cmd, "objDir = %s", objDir);
	PrintDebug (objDir);

// form the source file name
	sprintf (plName, "%s/%s%s.pl", srcDir, idomain, name);
// check existence of file.
	int fd = open (plName, O_RDONLY);
	if (fd < 0) return noPermission ("Loader: can't open ", plName);
	close (fd);
	sprintf (oName, "%s/%s%s.o", objDir, idomain, name);
	if (exists (oName) && isYounger (oName, plName))
		return Linker::linkObj (oName);
	sprintf (ccName, "%s/%s%s.cc", srcDir, idomain, name);
// preprocess if need be.
	if (!exists (ccName) || isYounger (plName, ccName)) {
		sprintf (cmd, "cd %s; ptlang %s%s.pl",
			 srcDir, idomain, name);
		PrintDebug (cmd);
		if (system (cmd)) {
			Error::abortRun ("Loader: preprocessor error");
			return FALSE;
		}
		// make sure we now have the .cc file
		if (!exists (ccName)) {
			StringList msg = "Loader: ";
			msg += ccName;
			msg += " still doesn't exist:\ncheck the file ";
			msg += plName;
			Error::abortRun (msg);
			return FALSE;
		}
	}
// now compile.
	if (!compile (name, idomain, srcDir, objDir)) return FALSE;
// finally incremental link.
	return Linker::linkObj (oName);
}


// Here is the pigi interface to the loader.  fullName is the name
// of the icon for the star.


extern "C" int
KcLoad (const char* iconName) {
	char codeName[512], base[128], domain[64], dir[512];

	iconName = expandPathName (iconName);

	if (!IconFileToSourceFile (iconName, codeName, domain))
		return FALSE;
	strcpy (dir, codeName);
	DirName(dir);		// strip off the tail
	char* b = BaseName(codeName);
// now basename is the domain concatenated with the star name.
// check that domain is prefix of basename.
	int l = strlen(domain);
	if (strncmp(b, domain, l) != 0) {
		StringList msg = "Loader: ";
		msg += domain;
		msg += " is not a prefix of ";
		msg += b;
		Error::abortRun(msg);
		return FALSE;
	}
// b has a suffix ".pl"; zap it.
	strcpy (base, b + l);
	char* p = strrchr (base, '.');
	if (p) *p = 0;
	return compileAndLink (base, domain, dir);
}
