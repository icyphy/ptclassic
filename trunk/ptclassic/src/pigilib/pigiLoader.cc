static const char file_id[] = "pigiLoader.cc";

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
 Date of creation: 12/18/90

 The routines in this file support preprocessing and compilation of
 Ptolemy star definitions, and make calls to the Linker class to
 dynamically link new stars into the system.

 We attempt to avoid compilation unless the .o file is "out of date."

**************************************************************************/

#if !defined(freebsd)
#define NEED_SYS_ERRLIST	// compat.h uses this.
#endif

// Include standard include files to prevent conflict with
// the type definition Pointer used by "rpc.h". BLE
#include "compat.h"
#include "sol2compat.h"
#include <std.h>
#include <sys/stat.h>
#include <ctype.h>

#include "Linker.h"
#include "Error.h"
#include "Domain.h"
#include "miscFuncs.h"
#include "StringList.h"
#include "pt_fstream.h"
#include "ptk.h"

#if defined(PTHPPA_CFRONT) || defined(PTSOL2) || defined(SVR4) || defined(SYSV) || !defined(PTIRIX5_CFRONT)
#include <fcntl.h>		// For open().
#endif /*hppa GNUG SOL2*/

// If a makefile is present in the directory where the star source files
// are located, then make is called, and many of the values in this file
// are ignored.
// Note that a user can create a makefile based on $PTOLEMY/mk/userstars.mk
// that can override the defaults in this file.

// choose compiler to use
#ifdef __GNUG__
#define CPLUSPLUS "g++"
#else
#define CPLUSPLUS "CC"
#endif

// define extra compiler options EXTRAOPTS
#ifdef PTSVR4
#ifdef __GNUG__
#define EXTRAOPTS "-fPIC"
#else
#define EXTRAOPTS "-KPIC"
#endif /* __GNUG__ */
#endif /* PTSVR4 */

#ifdef mips
#define EXTRAOPTS "-G 0"
#else
#ifndef __GNUG__
// Then we are using cfront
#define EXTRAOPTS " -DPOSTFIX_OP="
#else
#if __GNUC_MINOR__ >= 7

#ifdef PTSOL2
#define EXTRAOPTS "-fpic"
#else
#ifdef freebsd
#define EXTRAOPTS "-Dfreebsd"
#else
#define EXTRAOPTS ""
#endif /* freebsd */
#endif /* PTSOL2 */

#else  /* __GNUC_MINOR__ >= 7 */
#define EXTRAOPTS ""
#endif
#endif /* ! __GNUG__ */
#endif /* mips */

// root of Ptolemy source/lib directory
static const char* ptolemyRoot = 0;

// a temporary file name for compiler errors
static char* tmpFileName = 0;

extern "C" {
#define Pointer screwed_Pointer		/* rpc.h and type.h define Pointer */
#include "utility.h"			/* define util_csystem */

#include "vemInterface.h"		/* define PrintDebug */
#include "util.h"			/* define DirName and BaseName */
#include "icon.h"			/* define IconFileToSourceFile */
#include "xfunctions.h"			/* define win_msg */
#include "err.h"			/* define ErrAdd */

#include "kernelCalls.h"		/* define functions prefixed by Kc */
#undef Pointer
};

/* Even though pigiLoader.cc is a C++ file, pigiLoader.h is a C include file */
extern "C" {
#include "pigiLoader.h"
}

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
	sl << file << ": " << sys_errlist[errno];
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
	StringList buf = "Loader: ";
	buf << text << "\n";
	FILE* fp = fopen(tmpFileName, "r");
	if (fp == 0) {
		buf << "Can't open error file!";
	}
	else {
		const int LINELEN = 80;
		char line[LINELEN + 1];
		while ( ! feof(fp) ) {
		   line[0] = 0;
		   char *retval = fgets(line, LINELEN, fp);
		   if ( retval == 0 ) {
		     buf << "** error reading " << tmpFileName << " **";
		     break;
		   }
		   line[LINELEN] = 0;
		   buf << line;
		}
	}
	win_msg(buf);
	ErrAdd("Compilation failed");
	unlink(tmpFileName);
}

// Default compile statement.  We include the kernel directory, the domain-
// specific kernel and stars directories, and the source file directory.
// If others are required as well, you must use a makefile.

static int compile (const char* name, const char* idomain, const char* srcDir,
		    const char* objDir)
{
 	char domain[32];
  	strcpyLC(domain, idomain);

	StringList ptSrcDir = ptolemyRoot;
	ptSrcDir << "/src";

	StringList ptDomainDir = ptSrcDir;
	ptDomainDir << "/domains/" << domain;

	// Unix command to compile the file "name" 
	// it has the form
	//   cd $objDir; $compile $options $includepath $ccfile >& $tmpFileName
	// where ccfile = $srcDir/$idomain$name.cc

	// 1. Initialize command, add "cd $objDir; $compile $options"
	StringList cmd;
	cmd.initialize();
	cmd << "cd " << objDir << "; ";
	cmd << CPLUSPLUS << " " << EXTRAOPTS << " -c ";

	// 2. build up the include path
	//    -- don't forget to add a space after each -I directive
        //    -- if running under cfront, include -I$PTOLEMY/src/compat/cfront 
#ifndef __GNUG__
	cmd << "-I" << ptSrcDir << "/compat/cfront ";
#endif
	cmd << "-I" << ptDomainDir << "/kernel ";
	cmd << "-I" << ptDomainDir << "/stars ";
	cmd << "-I" << ptDomainDir << "/dsp/stars ";
	cmd << "-I" << ptDomainDir << "/image/stars ";
	cmd << "-I" << ptDomainDir << "/tcltk/stars ";
	cmd << "-I" << ptDomainDir << "/matrix/stars ";

	//    -- stars in code generation domains rely on includes in CG kernel
	if ( strcmp(domain, "cg") != 0 ) {
	  cmd << "-I" << ptSrcDir << "/cg/kernel ";
	}

	//    -- stars in dataflow domains rely on includes in SDF kernel
	if ( strcmp(domain, "sdf") != 0 ) {
	  cmd << "-I" << ptSrcDir << "/sdf/kernel ";
	}

	cmd << "-I" << ptSrcDir << "/kernel ";
	cmd << "-I" << srcDir << " ";

	// 3. Add the C++ file name to be compiled
	cmd << srcDir << "/" << idomain << name << ".cc";

	// 4. Send the output of the compilation to the temporary file
	cmd << " >& " << tmpFileName;

	// Evaluate the Unix command and report results
	PrintDebug(cmd);
	if ( util_csystem(cmd) ) {
		reportErrors ("errors in compilation");
		return FALSE;
	}

	// Delete temporary file on successful compilation
	unlink(tmpFileName);
	return TRUE;
}

// Replace the last occurrence of the "src" sub-directory with "obj.$PTARCH"
// in the directory srcDirStr (note that srcDirStr is not a path name)
static char* genObjDir (const char* srcDirStr) {
	int len = strlen(srcDirStr);
	char* srccopy = savestring(srcDirStr);
	char* objDirStr = srccopy;

	// Search for a sub-directory named "src"
	if ( len >= 4 ) {

	  // 1. Check for src sub-directory at end of directory name
	  int i = len - 4;
	  char* srcloc = &srccopy[i];
	  int found = ( strcmp(srcloc, "/src") == 0 );

	  // 2. If not found, check for src sub-directory in directory name
	  if ( ! found ) {
	    while ( i-- ) {
	      if ( *srcloc == '/' && ( strncmp(srcloc, "/src/", 5) == 0 ) ) {
		found = TRUE;
		break;
	      }
	      srcloc--;
	    }
	  }

	  // 3. If found, substitute "/src" subdirectory with "/obj.$PTARCH"
	  if ( found ) {
	    char tmpChar = *srcloc;
	    *srcloc = 0;		// temporarily null terminate srcloc
	    StringList temp = srccopy;
	    *srcloc = tmpChar;
 	    const char* ptarch = getenv("PTARCH");
	    if ( ptarch == (const char *)NULL ) {
	      ErrAdd("PTARCH environment variable not set");
	      return (char *)NULL;
	    }
 	    temp << "/obj." << ptarch;	// replace "/src" with "/obj.$PTARCH"

	    temp << &srcloc[4];		// copy rest of source directory
	    if ( exists(temp) ) {
		objDirStr = savestring(temp);
		delete [] srccopy;
	    }
	  }
	}

	return objDirStr;
}

// Function to initialize the loader
extern "C" void
KcLoadInit (const char* argv0) {
	ptolemyRoot = hashstring(getenv("PTOLEMY"));
	if (ptolemyRoot == 0) {
		char* expandedName = expandPathName("~ptolemy");
		ptolemyRoot = hashstring(expandedName);
		delete [] expandedName;
	}
	delete [] tmpFileName;
	tmpFileName = tempFileName();
	Linker::init (argv0);
	// look for a file specifying modules to be permanently linked in
	// this relies on pigiRpc starting in the home directory.
}

// Function to read in a file and hand its contents as arguments
// to the permlink function.  The file has one line in it, which
// specifies arguments for the loader to be permanently linked in.

extern "C" void
KcDoStartupLinking() {
	ifstream lstream(".pigilink");
	if (lstream) {
		const int BUFLEN = 2048;
		char buf[BUFLEN], *p;
		strcpy(buf, "permlink ");
		p = buf + strlen(buf);
		PrintDebug("processing .pigilink file");
		lstream.getline(p, BUFLEN-10);
		PrintDebug(buf);
		if (Linker::multiLink(p, 1))
			PrintDebug("link successful");
		else
			PrintDebug("link failed");
	}
}

// Load an object file (local only)
static int
linkObject (const char* ofile, int permB, const char *linkArgs) {
	// Record ptcl command: $link $ofile $linkArgs
	StringList ptclbuf = permB ? "permlink " : "link ";
	if (ofile) ptclbuf << ofile << " ";
	if (linkArgs) ptclbuf << linkArgs;
	KcLog (ptclbuf);

	// Dynamically link the file: $ofile $linkArgs
	StringList linkbuf;
	if (ofile) linkbuf << ofile << " ";
	if (linkArgs) linkbuf << linkArgs;

	return Linker::multiLink(linkbuf, permB);
}

// tables for suffixes and preprocessors
const int N_PREPROCS = 5;

static const char *preprocSuffix[] = { "", "pl", "chdl", "is", "std", "html" };
static const char *preprocProg[] = { "", "ptlang", "pepp", "islang", "" };
static const char *preprocTclFun[] = {"", "", "", "", "ptkTychoLoadFSM", "" };
static const int domSpec[] = { 1, 1, 1, 1, 0, 0 };

// Here is the function that loads in a star!
// name = username of the star
// idomain = domain of the star
// srcDirStr = star source directory
static int
compileAndLink (const char* name, const char* idomain, const char* srcDirStr,
		int preproc, int permB, const char* linkArgs) {

	if (!domSpec[preproc])  {
	  // If not domain-specific, don't check domain.
	  const int BUFLEN = 512;
	  char buf[BUFLEN];

	  if (strcmp(preprocTclFun[preproc],"")) {
	    // With tcl function, use it to compile.
	    sprintf (buf,"%s {%s/%s.%s}",preprocTclFun[preproc],
		     srcDirStr,name,preprocSuffix[preproc]);
	    if (Tcl_GlobalEval(ptkInterp, buf) != TCL_OK) {
	      sprintf(buf, "Cannot compile in '%s'", name);
	      ErrAdd(buf);
	      return (FALSE);
	    }
	    return (TRUE);

	  } else {
	    // No processor specified.
	    return (FALSE);
	  }
	}

	// Determine and save the source and object directory names
	char* expandedSrcDir = expandPathName(srcDirStr);
	char* expandedObjDir = genObjDir(expandedSrcDir);
	if ( expandedObjDir == (char *)NULL) {
	    ErrAdd("Cannot expand object directory name");
	    return (FALSE);
	}

	StringList srcDir = expandedSrcDir;
	StringList objDir = expandedObjDir;
	delete [] expandedSrcDir;
	delete [] expandedObjDir;

	// Form path name of the source files
	// -- the template file $srcDir/$idomain$name.$suffix
	// -- the C++ file $srcDir/$idomain$name.cc
	StringList plName = srcDir;
	plName << "/" << idomain << name << "." << preprocSuffix[preproc];
	StringList ccName = srcDir;
	ccName << "/" << idomain << name << ".cc";
	const char *sourceFile = (preproc ? plName : ccName);

	// Check existence of source file.
	int fd = open(sourceFile, 0);
	if (fd < 0) {
		return noPermission ("Loader: can't open ", sourceFile);
	}
	close (fd);

	// Form path name of the object file, $objDir/$idomain$name.o
	StringList oName = objDir;
	oName << "/" << idomain << name << ".o";

	// If a makefile exists in the object directory, use make.
	StringList makeFile = objDir;
	makeFile << "/Makefile";
	if (!exists(makeFile)) {
		makeFile = objDir;
		makeFile << "/makefile";
	}
	if (exists(makeFile)) {
		// cd $objDir; make $idomain$name.o >& $tmpFileName
		StringList cmd = "cd ";
		cmd << objDir << "; make " << idomain << name << ".o >& ";
		cmd << tmpFileName;
		PrintDebug(cmd);
		if (util_csystem(cmd)) {
			reportErrors ("errors from make");
			return FALSE;
		}
		unlink(tmpFileName);
		return linkObject(oName, permB, linkArgs);
	}

	// No makefile.  If object is younger than source, assume it's good.
	// It also must be younger than the Ptolemy image.
	if (exists (oName) && isYounger (oName, sourceFile) &&
	    isYounger (oName, Linker::imageFileName())) {
		return linkObject (oName, permB, linkArgs);
	}

	// Preprocess if need be.
	if (preproc && (!exists (ccName) || isYounger (plName, ccName))) {
		// cd $srcDir; $preproc $idomain$name.$suffix >& $tmpFileName
		StringList cmd = "cd ";
		cmd << srcDir << "; " << preprocProg[preproc] << " ";
		cmd << idomain << name << "." << preprocSuffix[preproc];
		cmd << " >& " << tmpFileName;
		PrintDebug(cmd);
		if (util_csystem(cmd)) {
			reportErrors (cmd);
			return FALSE;
		}
		// make sure we now have the .cc file
		if (!exists (ccName)) {
			StringList msg = "Loader: ";
			msg += ccName;
			msg += " still doesn't exist:\ncheck the file ";
			msg += plName;
			Error::abortRun(msg);
			ErrAdd("Loading file didn't create the star");
			return FALSE;
		}
	}

	// now compile
	if (!compile(name, idomain, srcDir, objDir)) return FALSE;

	// finally incremental link.
	return linkObject(oName, permB, linkArgs);
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

		// Check if it's non domain-specific.
		sprintf (buf, "%s/%s.%s", dir, base, preprocSuffix[i]);
		if (exists(buf)) return i;
	}
	sprintf (buf, "%s/%s%s.cc", dir, dom, base);
	return (exists(buf) ? 0 : -1);
}

// Here is the pigi interface used by make-star.  It looks for a .pl
// or a .cc file (or other preprocessor file) and does what's needed.

extern "C" int
KcCompileAndLink (const char* name, const char* idomain, const char* srcDir,
  int permB, const char* linkArgs) {
	if (!Linker::enabled()) {
		ErrAdd ("Loader disabled");
		return FALSE;
	}
	// form the source file name
	char* eDir = expandPathName(srcDir);
	char fName[512];
	int preproc = FindStarSourceFile(eDir, idomain, name, fName);
	delete [] eDir;
	if (preproc < 0) {
		StringList msg = "Loader: can't find ";
		msg += fName;
		msg += " or .*";
		ErrAdd (msg);
		return FALSE;
	}
	return compileAndLink (name, idomain, srcDir, preproc, permB, linkArgs);
}



// Here is the pigi interface to the loader.  fullName is the name
// of the icon for the star.

extern "C" int
KcLoad (const char* iconNameStr, int permB, const char* linkArgs) {
	if (!Linker::enabled()) {
		ErrAdd ("Loader disabled");
		return FALSE;
	}
	// base, domain, dir and preproc are needed to call compileAndLink
	char codeName[512], base[128], domain[64], dir[512];
	int preproc = 0;

	char* iconName = expandPathName(iconNameStr);
	int retval = IconFileToSourceFile(iconName, codeName, domain);
	delete [] iconName;
	if ( ! retval ) {
		return FALSE;
	}

	strcpy (dir, codeName);
	DirName(dir);		// strip off the tail
	char* b = BaseName(codeName);

	char* suffix = strrchr (b, '.');
	preproc = 0;
	if (suffix) {
	  // Find the preproc # for the suffix.
	  for (int i = 1; i <= N_PREPROCS; i++) {
	    if (strcmp(suffix+1, preprocSuffix[i]) == 0) {
	      preproc = i;
	      break;
	    }
	  }
	}

	if (!domSpec[preproc])  {
	  // If not domain-specific, don't check domain.

	  // b has a suffix ".std"; zap it;
	  *suffix = 0;
	  // Get the base name.
	  strcpy(base,b);

	  // Suppose the domain of it is current domain because it's 
	  // not domain-specific.
	  strcpy(domain,curDomainName());

	} else {
	  // now basename is the domain concatenated with the star name.
	  // check that domain is prefix of basename.
	  int domainlen = strlen(domain);
	  if (strncmp(b, domain, domainlen) != 0) {
	    StringList msg = "Loader: ";
	    msg += domain;
	    msg += " is not a prefix of ";
	    msg += b;
	    ErrAdd(msg);
	    return FALSE;
	  }

	  // b has a suffix ".pl", ".chdl", or ".cc"; zap it;
	  // tell KcCompileAndLink which one.
	  strcpy (base, b + domainlen);
	  char* p = strrchr (base, '.');
	  if (p && preproc == 0 && strcmp (p, ".cc") != 0) {
	    StringList msg = "Loader: unknown file type: ";
	    msg += base;
	    ErrAdd (msg);
	    return FALSE;
	  }
	  *p = 0;
	}

	const char* curdom = curDomainName();

	// If the star's domain is different from the current domain, then
	// set the current domain to the star's domain in order for the
	// KcIsCompiledInStar and KcIsKnown calls below to work.
	if (strcmp(curdom, domain) != 0) {
		KcSetKBDomain(domain);
	}
	if (domSpec[preproc] && KcIsCompiledInStar(base)) {
		StringList msg = "star '";
		msg << base
		    << "' is a compiled-in star of domain "
		    << domain
		    << ".\nCannot dynamically load a compiled-in star class."
		    << "\nA possible solution is to change the domain of the"
		    << "\nof the universe to "
		    << domain;
		ErrAdd(msg);
		// restore the original current domain
		KcSetKBDomain(curdom);
		return FALSE;
	}
	if (!compileAndLink (base, domain, dir, preproc, permB, linkArgs)) {
		// restore the original current domain
		KcSetKBDomain(curdom);
		return FALSE;
	}
	int status = KcIsKnown(base);
	if (!status) {
		ErrAdd("loader ran successfully, but star is not defined!");
	}

	// restore the original current domain
	KcSetKBDomain(curdom);
	return status;
}
