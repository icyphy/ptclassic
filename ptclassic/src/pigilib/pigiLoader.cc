static const char file_id[] = "pigiLoader.cc";
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

#include <std.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Linker.h"
#include "Domain.h"
#include "miscFuncs.h"
#include "StringList.h"
#include "Interpreter.h"
#include <ctype.h>
#include "streamCompat.h"

// choose compiler to use
#ifdef __GNUG__
#define CPLUSPLUS "g++"
#else
#define CPLUSPLUS "CC"
#endif

// architecture-specific stuff
#if defined(__sun__) || defined(sun)
#ifdef __mc68000__
#define ARCH "sun3"
#endif
#if defined(__sparc__) || defined(sparc)
#define ARCH "sun4"
#endif
#endif

#ifdef vax
#define ARCH "vax"
#endif

#ifdef mips
#define ARCH "mips"
#define EXTRAOPTS "-G 0"
#else
#define EXTRAOPTS ""
#endif

#ifdef __hppa__
#define ARCH "snake"
#endif

// root of Ptolemy source/lib directory
static const char* ptolemyRoot;
// a temporary file name for compiler errors
static const char* tmpFileName;

// pigi funcs to call
extern "C" {
	void PrintDebug(const char*);
	void DirName(char*);
	char* BaseName(const char*);
	int IconFileToSourceFile (const char*, char*, char*);
	int util_csystem (const char*);
	void win_msg(const char*);
	void ErrAdd(const char*);
	void KcLog(const char*);
	int KcIsCompiledInStar(const char*);
	int KcIsKnown(const char*);
	const char* curDomainName();
	int KcSetKBDomain(const char*);
};

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
	ErrAdd (sl);
	return FALSE;
}

inline static int exists (const char* file) {
	return access(file, 0) == 0;
}

static int isYounger (const char* fileA, const char* fileB) {
	struct stat bufA, bufB;
	if (stat (fileA, &bufA) < 0 || stat (fileB, &bufB) < 0)
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
	ErrAdd ("Compilation failed");
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
	sprintf (cmd, "cd %s; %s %s -c -I%s/src/domains/%s/kernel "
		 "-I%s/src/domains/%s/stars -I%s/src/kernel "
		 "-I%s %s/%s%s.cc >& %s", objDir, CPLUSPLUS,
		 EXTRAOPTS, ptolemyRoot, domain,
		 ptolemyRoot, domain, ptolemyRoot,
		 srcDir, srcDir, idomain, name, tmpFileName);
	PrintDebug (cmd);
	if (util_csystem (cmd)) {
		reportErrors ("errors in compilation");
		return FALSE;
	}
	unlink(tmpFileName);
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
	if (ptolemyRoot == 0)
		ptolemyRoot = hashstring(expandPathName("~ptolemy"));
	tmpFileName = tempFileName();
	Linker::init (argv0);
}

// Load an object file (local only)
static int
linkObject (const char* ofile) {
	char buf[512];
	sprintf (buf, "link %s\n", ofile);
	KcLog (buf);
	return Linker::linkObj (ofile);
}

// tables for suffixes and preprocessors
const int N_PREPROCS = 3;
const int INTERP_FILE = 3;

static const char *preprocSuffix[] = { "", "pl", "chdl", "pt" };
static const char *preprocProg[] = { "", "ptlang", "pepp", 0 };

// Here is the function that loads in a star!
// name = username of the star
// idomain = domain of the star
// srcDir = star source directory
static int
compileAndLink (const char* name, const char* idomain, const char* srcDir,
		  int preproc) {
	srcDir = expandPathName (srcDir);
	char* objDir = genObjDir (srcDir);
	char plName[512], oName[512], ccName[512], cmd[512];

// form the source file name
	sprintf (plName, "%s/%s%s.%s", srcDir, idomain, name,
		 preprocSuffix[preproc]);
	sprintf (ccName, "%s/%s%s.cc", srcDir, idomain, name);
	char *sourceFile = preproc ? plName : ccName;
// check existence of file.
	int fd = open (sourceFile, O_RDONLY);
	if (fd < 0) return noPermission ("Loader: can't open ", plName);
	close (fd);
	sprintf (oName, "%s/%s%s.o", objDir, idomain, name);
// Special case hack: Interpreter file.  We simply load the file into
// an Interpreter object and run the command router.  We always return
// success.
	if (preproc == INTERP_FILE) {
		Interpreter intObj(sourceFile);
		intObj.commandRouter();
		return TRUE;
	}
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
		unlink(tmpFileName);
		return linkObject (oName);
	}
// No makefile.  If object is younger than source, assume it's good.
// It also must be younger than the Ptolemy image.
	if (exists (oName) && isYounger (oName, sourceFile) &&
	    isYounger (oName, Linker::imageFileName()))
		return linkObject (oName);
// Preprocess if need be.
	if (preproc && (!exists (ccName) || isYounger (plName, ccName))) {
		sprintf (cmd, "cd %s; %s %s%s.%s >& %s",
			 srcDir, preprocProg[preproc], idomain, name,
			 preprocSuffix[preproc], tmpFileName);
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
			ErrAdd ("Loading file didn't create the star");
			return FALSE;
		}
	}
// now compile.
	if (!compile (name, idomain, srcDir, objDir)) return FALSE;
// finally incremental link.
	return linkObject (oName);
}

// This function searches for a source file, first looking for preprocessor
// source, then .cc source.
// The return value is 0 if only a .cc is found, a + integer for a preprocessor
// source file, and -1 if nothing is found.  The final argument points to
// a buffer that is filled with the full filename (the .cc filename if no
// match is found).

extern "C" int
FindStarSourceFile(const char* dir,const char* dom,const char* base,
		   char* buf) {
	for (int i = 1; i <= N_PREPROCS; i++) {
		sprintf (buf, "%s/%s%s.%s", dir, dom, base,
			 preprocSuffix[i]);
		if (exists(buf)) return i;
	}
	sprintf (buf, "%s/%s%s.cc", dir, dom, base);
	return (exists(buf) ? 0 : -1);
}

// Here is the pigi interface used by make-star.  It looks for a .pl
// or a .cc file (or other preprocessor file) and does what's needed.

extern "C" int
KcCompileAndLink (const char* name, const char* idomain, const char* srcDir) {
	if (!Linker::enabled()) {
		ErrAdd ("Loader disabled");
		return FALSE;
	}
// form the source file name
	const char* eDir = expandPathName (srcDir);
	char fName[512];
	int preproc = FindStarSourceFile(eDir, idomain, name, fName);
	if (preproc < 0) {
		StringList msg = "Loader: can't find ";
		msg += fName;
		msg += " or .*";
		ErrAdd (msg);
		return FALSE;
	}
	return compileAndLink (name, idomain, srcDir, preproc);
}



// Here is the pigi interface to the loader.  fullName is the name
// of the icon for the star.


extern "C" int
KcLoad (const char* iconName) {
	if (!Linker::enabled()) {
		ErrAdd ("Loader disabled");
		return FALSE;
	}
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
		ErrAdd(msg);
		return FALSE;
	}
// b has a suffix ".pl", ".chdl", or ".cc"; zap it; tell KcCompileAndLink
// which one.
	strcpy (base, b + l);
	char* p = strrchr (base, '.');
	int preproc = 0;
	if (p) {
		for (int i = 1; i <= N_PREPROCS; i++) {
			if (strcmp(p+1, preprocSuffix[i]) == 0) {
				preproc = i;
				break;
			}
		}
	}
	if (p && preproc == 0 && strcmp (p, ".cc") != 0) {
		StringList msg = "Loader: unknown file type: ";
		msg += base;
		ErrAdd (msg);
		return FALSE;
	}
	*p = 0;
// see if the star is already loaded on a different domain list
	const char* curdom = curDomainName();
	if (strcmp(curdom, domain) != 0) {
		KcSetKBDomain(domain);
		if (KcIsCompiledInStar(base)) {
			StringList msg = "star '";
			msg += base;
			msg += "' belongs to an incompatible domain.";
			msg += "\nCheck the galaxy's domain using edit-domain";
			ErrAdd (msg);
			return FALSE;
		}
	}
	if (!compileAndLink (base, domain, dir, preproc))
		return FALSE;
	int status = KcIsKnown(base);
	if (!status)
		ErrAdd("loader ran successfully, but star is not defined!");
	KcSetKBDomain(curdom);
	return status;
}
