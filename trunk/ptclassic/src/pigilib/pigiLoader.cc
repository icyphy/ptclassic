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

// root of Ptolemy source/lib directory
static const char* ptolemyRoot;
// a temporary file name for compiler errors
static const char* tmpFileName;

// pigi funcs to call
extern "C" void PrintDebug(const char*);
extern "C" void DirName(char*);
extern "C" char* BaseName(const char*);
extern "C" int IconFileToSourceFile (const char*, char*, char*);
extern "C" int util_csystem (const char*);
extern "C" void win_msg(const char*);

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

// make a window with the stuff in the file.
static void reportErrors (const char* text) {
	const int BUFLEN = 2048;
	char buf[BUFLEN];
	sprintf (buf, "Loader: %s\n", text);
	int l = strlen (buf);
	char* p = buf + l;
	FILE* fd = fopen (tmpFileName, "r");
	if (fd == 0) {
		strcpy (p, "Can't open error file!");
	}
	else {
		int c;
		while ((c = getc (fd)) != EOF && p < buf + BUFLEN - 1)
			*p++ = c;
		*p = 0;
	}
	win_msg (buf);
	unlink (tmpFileName);
}

// Default compile statement.  We include the kernel directory, the domain-
// specific kernel and stars directories, and the source file directory.
// If others are required as well, you must use a makefile.

static int compile (const char* name, const char* idomain, const char* srcDir,
		    const char* objDir)
{
	char domain[32], cmd[512];
	strcpyLC (domain, idomain);
	sprintf (cmd, "cd %s; g++ -c -I %s/src/domains/%s/kernel "
		 "-I %s/src/domains/%s/stars -I %s/src/kernel "
		 "-I %s %s/%s%s.cc >& %s", objDir, ptolemyRoot, domain,
		 ptolemyRoot, domain, ptolemyRoot,
		 srcDir, srcDir, idomain, name, tmpFileName);
	PrintDebug (cmd);
	if (util_csystem (cmd)) {
		reportErrors ("errors in compilation");
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
	*p = 0;
	return savestring (buf);
}

// Function to initialize the loader
extern "C" void
KcLoadInit (const char* argv0) {
	ptolemyRoot = getenv ("PTOLEMY");
	if (ptolemyRoot == 0) ptolemyRoot = "~ptolemy";
	tmpFileName = tempFileName();
	Linker::init (argv0);
}

// Here is the function that loads in a star!
// name = username of the star
// idomain = domain of the star
// srcDir = star source directory
extern "C" int
KcCompileAndLink (const char* name, const char* idomain, const char* srcDir) {
	srcDir = expandPathName (srcDir);
	char* objDir = genObjDir (srcDir);
	char plName[512], oName[512], ccName[512], cmd[512];
// debug statement
	sprintf (cmd, "compileAndLink(%s,%s,%s)", name, idomain, srcDir);
	PrintDebug (cmd);
	sprintf (cmd, "objDir is %s", objDir);
	PrintDebug (objDir);

// form the source file name
	sprintf (plName, "%s/%s%s.pl", srcDir, idomain, name);
// check existence of file.
	int fd = open (plName, O_RDONLY);
	if (fd < 0) return noPermission ("Loader: can't open ", plName);
	close (fd);
	sprintf (oName, "%s/%s%s.o", objDir, idomain, name);
// if there is a makefile, use make.
	char makeFile[512];
	sprintf (makeFile, "%s/%s", objDir, "Makefile");
	if (!exists (makeFile)) {
		sprintf (makeFile, "%s/%s", objDir, "makefile");
	}
	if (exists (makeFile)) {
		sprintf (cmd, "cd %s; make %s%s.o >& %s",
			 objDir, idomain, name, tmpFileName);
		PrintDebug (cmd);
		if (util_csystem (cmd)) {
			reportErrors ("errors from make");
			return FALSE;
		}
		return Linker::linkObj (oName);
	}
// No makefile.  If object is younger than source, assume it's good.
	if (exists (oName) && isYounger (oName, plName))
		return Linker::linkObj (oName);
	sprintf (ccName, "%s/%s%s.cc", srcDir, idomain, name);
// Preprocess if need be.
	if (!exists (ccName) || isYounger (plName, ccName)) {
		sprintf (cmd, "cd %s; ptlang %s%s.pl >& %s",
			 srcDir, idomain, name, tmpFileName);
		PrintDebug (cmd);
		if (util_csystem (cmd)) {
			reportErrors (cmd);
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
	return KcCompileAndLink (base, domain, dir);
}
