static const char file_id[] = "Linker.cc";
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

 Programmers:  J. Buck, Christopher Hylands
 Date of creation: 7/30/90

 This file implements support for incremental linking.  It works both
 for g++ and for Sun C++ (a port of AT&T cfront v2.1).  It should port
 easily to other Unix ports of cfront; all the implementation-specific
 stuff is in Linker.sysdep.h.


 There are two styles of incremental linking:

 1. Use a 4.3/Sun-style loader with the -A flag to load in .o files.
 Usually, binaries that are to be dynamically linked must be built with
 the -N option.  The code can deal with two object-file formats:
 BSD-ish (Sun-3, Sparc, Vax) and COFF (mips).  


 2. Use the System V Release 4 dlopen() style call to load in shared
 objects (.so files).  SunOS4.1.x, Solaris2.x and Irix5.x support this
 style of dynamic linking.  In Ptolemy0.5.1, only the sol2 and irix5
 ARCHs support dynamic linking of shared objects.


 Shared Objects
 --------------
 There are several ways to specify the path to a shared object.

 1) Using just a file name 'link foo.so' will not work unless
 LD_LIBRARY_PATH includes the directory where foo.so resides.
 The man pages for dlopen() and ld discuss LD_LIBRARY_PATH

 Interestingly, using putenv() to set LD_LIBRARY_PATH from within ptcl
 has no effect on the runtime loader.
	
 2) If the file name begins with './', then the current directory is
 searched.  'link ./foo.so' should work, as will 'link ./mydir/foo.so'

 3) If the file name is an absolute pathname, then the shared object
 will be loaded. 'link /tmp/foo.so' should work.

 4) Dynamic programs can have a run path specified at link time.  The
 run path is the path searched at runtime for shared object.  (Under
 Solaris2.3, the -R option to ld controls the run path.  Under Irix5.2,
 the -rpath option to ld controls the run path).
 If ptcl or pigiRpc has been compiled with a run path built in, and the
 shared object is in that path, then the shared object will be found.

 The Sun Linker Manual says:
	To locate the shared object foo.so.1, the runtime linker will
	use any LD_LIBRARY_PATH definition presently in effect,
	followed by any runpath specified during the link-edit of prog
	and finally, the default location /usr/lib.  If the file name
	had been specified ./foo.so.1, then the runtime linker would
	have searched for the file only in the present working
	directory.


 Non-Shared linking
 ------------------
 The code takes an .o file, does an incremental link with respect to
 the running binary, links it in and calls the constructors for any 
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
#ifdef __GNUG__
#pragma implementation
#endif

#include "Linker.sysdep.h"
#include "Linker.h"
#include "Error.h"
#include "miscFuncs.h"
#include "StringList.h"
#include "pt_fstream.h"
#include <ctype.h>
#include <stdio.h>
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
// Use this macro for debugging.
#define D(x) x
#else //DEBUG
#define D(x)
#endif //DEBUG

// static data objects
int Linker::activeFlag = 0;
const char* Linker::ptolemyName = 0;
const char* Linker::symTableName = 0;

// by default, search math and C libraries
#if defined(USE_SHLLOAD)
const char* Linker::myDefaultOpts = "/lib/libm.sl";
#else
const char* Linker::myDefaultOpts = "-lm -lc";
#endif

// If USE_DLOPEN is defined, then we don't use these vars
char* Linker::memBlock = 0;
char* Linker::availMem = 0;

/// This is only used if USE_DLOPEN is defined
int Linker::linkSeqNum = 0;

StringList Linker::permlinkFiles;
const size_t LINK_MEMORY = (size_t)(1024L * 1024L);

// This routine creates the memory block if needed, and aligns it
// to a page boundary.

int Linker::isActive() { return activeFlag;}

void Linker::adjustMemory() {
#if defined(USE_DLOPEN) || defined(USE_SHLLOAD)
  char buf[1024];
  sprintf(buf,
    "Linker: adjustMemory() not used on architectures that support dlopen()");
  Error::abortRun(buf);
#else // USE_DLOPEN || USE_SHLLOAD
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
#endif // USE_DLOPEN || USE_SHLLOAD
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


#if defined(USE_DLOPEN) || defined(USE_SHLLOAD)

#define PATHNAME_LENGTH 1024
// Create a .so file from one or more .o or .so files
// argv can also contain linker options from the multilink tcl call
// Really, we should call make here, and have better error messages
//
// Here we implement Joe Buck's idea, which is to create .so files
// with temporary names, where the name is /tmp__ptlink%d_%d.so (The first
// %d is the pid, the second is a sequence number which is incremented 
// after each link.  Note that we need to trash these temporary files
// up deconstruction.

// sprintf format string that will name the temporary file
#ifdef USE_SHLLOAD
#define DLOPEN_TMP_FILE_FORMAT "/tmp/__ptlink%d_%d.sl"
#else
#define DLOPEN_TMP_FILE_FORMAT "/tmp/__ptlink%d_%d.so"
#endif

char *
Linker::generateSharedObject(int argc, char **argv, char* objName,
			     int /*maxsize*/)
{
  StringList command = SHARED_OBJECT_COMMAND;
  
  // flag for permanent-link or not
  int perm = (argv[0][0] == 'p');	// KLUDGE!

  // Generate a temporary name
  sprintf(objName, DLOPEN_TMP_FILE_FORMAT, (int)getpid(), linkSeqNum++); 


  // Create the command to produce the shared object
  command << " " << objName;

#if defined (USE_SHLLOAD) || defined (__linux__) && defined  (USE_DLOPEN)
  // I  don't think we need all this (neal@ctd.comsat.com)
  for (int i = 1; i < argc; i++) {
    const char* fullObjName = expandPathName(argv[i]);
    command << " " << fullObjName;
    delete [] fullObjName;
  }
#else // USE_SHLLOAD

  for (int i = 1; i < argc; i++) {
    const char* fullObjName = argv[i];

    // If this is a permlink, then save the filenames for later
    if ( permlinkFiles.length() == 0 ) {
      // We have not yet done any permlinks, so we need not check
      permlinkFiles << " " << fullObjName;
    } else {
      // Don't add the file if it is already in the list.
      // -- Parse permlinkFiles w/ strtok_r, multithread-safe version of strtok
      // -- Use a copy of permlinkFiles because strtok_r alters string
      char* tmpString = savestring(permlinkFiles);
      char* lasts = 0;
      char* p = strtok_r(tmpString, " ", &lasts);
      while (p) {
	if ( strcmp(p, fullObjName) == 0 ) {
	  break;
	}
	p = strtok_r((char *)NULL, " ", &lasts);
      }
      // If we made it all the way through the list, and p is NULL,
      // then add our element to the list of files to be permlinked.
      if (perm && p == (char*)NULL)
	permlinkFiles << " " << fullObjName;

      // This is not a permlink, and our file is not a permlinked
      // file, so add it to the command line.
      if (!perm && p == (char*)NULL)
	command << " " << fullObjName;
      delete [] tmpString;
    } 
  } // for loop

  // If we have already done a permlink, then add all of the files
  // thus far permlinked to the command line.

  if ( permlinkFiles.length() > 0 )
    command << " " << permlinkFiles;
#endif //USE_SHLLOAD
#ifdef DEBUG
  {
    char buf[4*PATHNAME_LENGTH];
    sprintf(buf, "command to create shared object: %s",
	    (const char *)command);
    Error::message(buf);
  }
#endif // DEBUG

  if (system (command)) {
    Error::abortRun(
		    "Error in creating shared object, command failed: ",
		    command);
    return (char *) NULL;
  }

  return objName;
}


// Attempt to unlink all the temporary .so files
static void cleanupSharedObjects(int linkSeqNum)
{
  char fileName[PATHNAME_LENGTH];
  int i;
  for(i=0;i<linkSeqNum;i++) {
    sprintf(fileName, DLOPEN_TMP_FILE_FORMAT,(int)getpid(), i);
#ifdef DEBUG    
    {
      char buf[4*PATHNAME_LENGTH];
      sprintf(buf, "Linker cleanup: about to remove: %s", fileName);
      Error::message(buf);
    }
#endif
    unlink(fileName);
  }
}

#else // USE_DLOPEN
char *
Linker::generateSharedObject(int argc, char **argv, char* objName, int maxsize)
{
  return (char *)NULL;
}
#endif

// link in multiple files.  Arglist is char** and not const char**
// to permit easy interfacing with Tcl.

int Linker::multiLink (int argc, char** argv) {

	if (linkingNotSupported) {
		Error::abortRun ("Sorry, incremental linking is not yet ",
				 "supported on this architecture");
		return FALSE;
	}else if (!symTableName) {
	  Error::abortRun ("Incremental linking disabled");
	  return FALSE;
	}else if (argc < 1) {
	  Error::abortRun ("multiLink called with less than one arg");
	  return FALSE;
	}
	TFile tname;
	
#ifdef USE_DLOPEN
	void *dlhandle;

	char objName[PATHNAME_LENGTH];
	// 'permlink foo.o' allows us to load a star (foo.o) to be derived
	// from later, say with 'link bar.o'.  bar.pl would have the line
	// derivedfrom {foo} in it, allowing bar.pl to access foo's methods.
	
	// Under dlopen() style linking, we need to create a .so file that
	// has all the permlinked stars in it so that the star we are trying
	// to link in has access to the permlinked stars.  If we try to
	// load the permlinked stars in one .so and our new star in another
	// .so file, we will not see the symbols in the permlinked .so file.

	// We can simulate this behaviour by using multilink to link in
	// the permlinked star and the new star at once.

	// If we are doing a permlink, we create an extra special
	// .so file which has all the permlinked stars in it.  Subsequent
	// links will include the permlink .so file in the creation of
	// of the temporary .so file.  If we do further permlinking, we will
	// add to the permlinked .so file.

	// multlink can have one or more .o or .so filenames, combined
	// with linker options.

	// For our purposes, a filename is a word that does not start with a -
	// a linker option must start with a -

	// If there is only one arg, and it is a filename that ends in
	// .so, we assume that the user knows what's going on and we
	// do a dlopen() on that file.  (Do we copy to a temporary name first?)
	// The user could be using a makefile to generate a .so file.

	// If there is only one arg, and it ends in .o, we call the linker
	// .so file
	// If there is more than one arg, we call the linker to produce
	// a temporary .so file.  In this way, we can collect up the permlink
	// .so files etc.

	if ( argc > 2 ||
	     (argc == 2 && strncmp(argv[1]+strlen(argv[1])-3, ".so", 3) != 0))
	  if (generateSharedObject(argc, argv, objName, PATHNAME_LENGTH) ==
	      (char *)NULL)
	    return FALSE;
	// BTW - Don't do dlclose() to workaround the reloading of stars,
	// as we could have "block that reference the
	// old code might be around (if you have multiple universes
	// built, say) So if the user loads
	// a definition for star "Foo" and then reloads it, it is possible
	// to end up with stars that reference both the old and the new code." 
	
	// Read in the object now, make the symbols available to all
	// for access.  Without RTLD_GLOBAL, the symbols from one
	// dlopen() would not be visible in a subsequent dlopen()
	// The sgi uses sgidlopen() which does not need the
	// RTLD_GLOBAL flag.
	  
	if ( (dlhandle = DLOPEN(objName, DLOPEN_FLAGS)) == NULL) {
	  char buf[1024];
	  sprintf(buf,"Error linking file %s: dlopen: %s", objName,dlerror());
	  Error::abortRun(buf);
	  return FALSE;
	}

// flag for permanent-link or not
	int perm = (argv[0][0] == 'p');	// KLUDGE!

// invoke constructors in the newly read in code

	// activeFlag is checked by the KnownBlock constructors, and a flag
	// inside KnownBlock is saved.  This is how we can later tell
	// if a Block was dynamically linked in, or if it was compiled-in.
	if (!perm) activeFlag = TRUE;	
				// Call with objName, not tname
	int nCalls = invokeConstructors(objName, dlhandle);

#else //USE_DLOPEN
#ifdef USE_SHLLOAD
	shl_t lib_handle;

	char objName[PATHNAME_LENGTH];

	if ( argc > 2 ||
	     (argc == 2 && strncmp(argv[1]+strlen(argv[1])-3, ".sl", 3) != 0))
	  if (generateSharedObject(argc, argv, objName, PATHNAME_LENGTH) ==
	      (char *)NULL)
	    return FALSE;
	  
	if ( (lib_handle = shl_load(objName, BIND_DEFERRED | DYNAMIC_PATH | BIND_VERBOSE  | BIND_FIRST, 0)) == NULL) {
	  char buf[1024];
	  sprintf(buf,"Error linking file %s: dlopen: %s", objName, strerror (errno));
	  Error::abortRun(buf);
	  return FALSE;
	}

// flag for permanent-link or not
	int perm = (argv[0][0] == 'p');	// KLUDGE!

// invoke constructors in the newly read in code

	// activeFlag is checked by the KnownBlock constructors, and a flag
	// inside KnownBlock is saved.  This is how we can later tell
	// if a Block was dynamically linked in, or if it was compiled-in.
	if (!perm) activeFlag = TRUE;	
				// Call with objName, not tname
	int nCalls = invokeConstructors(objName, (void*)lib_handle);

#else //USE_SHLLOAD

// round up to be on a page boundary
	adjustMemory();

// generate the loader command.  The switches mean:
// -A: incremental loading, using symbols from the ptolemy binary.
// LOC_OPT (-T or -R): specify starting address of text segment.
// Other switches are in LOADOPTS, from Linker.sysdep.h

	char command[512];
	// DYNLIB is hppa specific
	sprintf (command, "%s %s -A %s %s %lx %s -o ",
		 LOADER, LOADOPTS, symTableName, LOC_OPT,
		 (unsigned long)availMem, DYNLIB);
	D( Error::message(command); )
	StringList cmd = command;
	cmd << tname;
	for (int i = 1; i < argc; i++) {
		const char* objName = expandPathName(argv[i]);
		cmd << " " << objName;
		delete [] objName;
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
	// activeFlag is checked by the KnownBlock constructors, and a flag
	// inside KnownBlock is saved.  This is how we can later tell
	// if a Block was dynamically linked in, or if it was compiled-in.
	if (!perm) activeFlag = TRUE;	
				// Call with tname, not objName
	int nCalls = invokeConstructors(tname, (void *)NULL);
#endif //USE_SHLLOAD
#endif //USE_DLOPEN


	int status = (nCalls >= 0);

	if (nCalls == 0 && !perm) {
		Error::abortRun("Linker: no constructors in linked-in code!");
		status = FALSE;
	}

	activeFlag = FALSE;	// turn off the indication.
	if (status) {
// we do not bump the memory pointer until now; this makes it official
#if !defined(USE_DLOPEN) && !defined(USE_SHLLOAD)
		availMem += size;
#endif
// if this is a "permalink", make tfile the new symbol file
		if (perm) installTable(tname);
	}
	return status;
}

#if !defined(USE_DLOPEN) && !defined(USE_SHLLOAD)
void Linker::installTable(const char* newTable) {

	if (ptolemyName == symTableName) {
		symTableName = getenv("PTOLEMY_SYM_TABLE");
		if (!symTableName) {
			const char *symtablefile =
				expandPathName("~/.pt_symtable");
			symTableName = hashstring(symtablefile);
			delete [] symtablefile;
		}
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
#else
void Linker::installTable(const char* /*newTable*/) {
}
#endif // !USE_DLOPEN && !USE_SHLLOAD


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
	  sprintf(buf, "Pipe found: %s at 0x%lx", symbol, (unsigned long) addr);
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
Linker::invokeConstructors (const char* objName, void * dlhandle) {
#ifdef NO_INVOKECONSTRUCTORS
        return 1;
#else //NO_INVOKECONSTRUCTORS  
	int nCalls = 0;
#ifdef USE_SHLLOAD
// we need to get the basename of the objfile
	char* lastslash = strrchr (objName, '/');
	char* basestart;
	if (lastslash == NULL) basestart = objName;
	else basestart = lastslash + 1;
	char ConsName[256];
	strcpy (ConsName, "_GLOBAL__FI_");
	strcat (ConsName, basestart);
// get rid of ".sl" extension
	char* baseend = strrchr (ConsName, '.');
	*baseend = '\0';
// add "_sl"
	strcat (ConsName, "_sl");
// Now we know what symbol to look for
#ifdef DEBUG
        {
	  char buf[256];
	  sprintf(buf, "Constructor Symbol: %s", ConsName);
	  Error::message(buf);
	}
#endif
	shl_t handle = (shl_t)dlhandle;
	void (*A_ptr)();
	if (shl_findsym (&handle, ConsName, TYPE_PROCEDURE, &A_ptr))
	  Error::abortRun ("shl_findsym failed");

	(*A_ptr)();
	nCalls++;
#else // USE_SHLLOAD	
#ifdef USE_NM_GREP
// On the hppa a symbol can have the same name in the code and
// an entry segments.  nm -p cannot differentiate between the
// two, so we define a separate method of checking for symbols.

// NOTE: If you change the pipe description, remember to modify the
//       fscanf() format string !
	StringList grepPattern = "";
	grepPattern << "'" << CONS_PREFIX << NM_GREP_STRING;

	StringList command = NM_PROGRAM;
	command << " " << objName << " | grep " << grepPattern;
#else //USE_NM_GREP
	char instring[BUFSIZ];
// Open a pipe to "nm" to get symbol information
	StringList command = NM_PROGRAM;
	command << " " << NM_OPTIONS << " " << objName;
#endif //USE_NM_GREP

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
	long addr;
#ifdef USE_NM_GREP
 	char line[1024], *symbol, *p_addr;
#else
	char symbol[256], type[20];
#endif //USE_NM_GREP


#ifndef USE_DLOPEN
	size_t memoryEnd = (size_t)memBlock + LINK_MEMORY;
#endif //USE_DLOPEN

#ifdef USE_NM_GREP
	char* lasts = 0;
	while (fscanf(fd, "%s%*s", line) == 1) {

          // Parse line with strtok_r, multithread-safe version of strtok
          symbol = strtok_r(line, "|", &lasts);
          p_addr = strtok_r(NULL, "|", &lasts);

	  addr = atol(p_addr);

	  D( debugInvokeConstructors(symbol, addr, objName); )

	  if (addr >= (size_t) availMem && addr <= memoryEnd)
	  {

	    PointerToVoidFunction fn = (PointerToVoidFunction)addr;
	    fn();
	    nCalls++;
	  }
	}

#else //USE_NM_GREP

        while( fgets(instring, BUFSIZ,fd) == instring ) {
	  // D( printf("%s",instring); )
	    /* Optionally skip a few characters */
	    if (sscanf(instring + NM_ADDR_OFFSET,
		       "%lx %s %s", &addr, type, symbol) == 3) {
#ifdef USE_DLOPEN
	      if ( strncmp(symbol, CONS_PREFIX, CONS_LENGTH) == 0) {
		// it is a constructor, call it:
		D( debugInvokeConstructors(symbol, addr, objName);)
		PointerToVoidFunction fn;
		if ((fn =
		     (PointerToVoidFunction)dlsym(dlhandle,symbol))
		    == NULL) {
		  char buf[1024];
		  sprintf(buf,"Error getting address of symbol %s\n\tdlsym:%s",
			  symbol,  dlerror());
		  Error::abortRun(buf);
		  return 0;
		}
		D( printf("debug: InvokeConstructors: fn=0x%lx\n",(unsigned long) fn); )
		fn();
		nCalls++;
	      }
#else // USE_DLOPEN
	      if (addr >= (size_t)availMem && addr <= memoryEnd &&
		  strncmp(symbol, CONS_PREFIX, CONS_LENGTH) == 0) {
		// it is a constructor, call it:
		D( debugInvokeConstructors(symbol, addr, objName);)
		PointerToVoidFunction fn = (PointerToVoidFunction)addr;
		fn();
		nCalls++;
	      }
#endif // USE_DLOPEN
	    }
	}
#endif //USE_NM_GREP
	pclose (fd);
#endif //USE_SHLLOAD
	return nCalls;
#endif //NO_INVOKECONSTRUCTORS
}

size_t Linker::readInObj(const char* objName) {
#if defined (USE_DLOPEN) || defined (USE_SHLLOAD)
  char buf[1024];
  sprintf(buf,"Linker: readInObj(%s) not used on architectures that support dlopen()",objName);
  Error::abortRun(buf);
  return (size_t) 0;
#else
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
#endif /* USE_DLOPEN */
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
#if defined (USE_DLOPEN) || defined (USE_SHLLOAD)
		cleanupSharedObjects(Linker::linkSeqNum);
#endif
	}
};

static Linker_Cleanup linkerCleanup;

