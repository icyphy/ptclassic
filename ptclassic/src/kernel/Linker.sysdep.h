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

 Programmers:  J. Buck, Christopher Hylands
 Date of creation: 7/30/90

 This file defines the system-dependent cruft needed for incremental
 linking.

**************************************************************************/

// get size_t definition
#include <sys/types.h>

#if defined(__sparc) && defined(__svr4__)
#define SOL2
#endif

#if defined(__sparc) && ! defined (__svr4__) && defined(__GNUG__)
#define SUN4
#endif

#if defined(__alpha) || defined(linux)
#define LINKING_NOT_SUPPORTED
#endif

// Is linking supported?
const int linkingNotSupported =
#ifdef LINKING_NOT_SUPPORTED
 1;
#else
 0;
#endif

// There are two styles of incremental linking:
// 1. Use a 4.3/Sun-style loader with the -A flag.  Usually, binaries
// that are to be dynamically linked must be built with the -N option
// 2. Use the System V Release 4 dlopen() style call.  SunOS4.1.x,
// Solaris2.x and Irix5.x support this.
//
// dlopen() style linking works under sun4, but if you try and load a
// star that has undefined symbols, pigiRpc or ptcl will exit.

#if defined(__sgi) || defined(SOL2)
#include <dlfcn.h>
#include <sys/stat.h>
#define USE_DLOPEN

// Don't use RTLD_GLOBAL, or if you reload a star, you won't see the changes
#if defined(__sgi)
// Under sgi, don't use sgidladd, or you won't see changes
#define DLOPEN dlopen
#define DLOPEN_FLAGS RTLD_NOW
#endif // __sgi

#if defined(SOL2) || defined(SUN4)
#define DLOPEN dlopen
#if defined(SOL2)
#define DLOPEN_FLAGS RTLD_NOW
#else
// defined(sun)
#define DLOPEN_FLAGS RTLD_LAZY
#endif // SOL2
#endif // SOL2 || SUN4

#if defined(__GNUG__)
#if defined(SOL2) || defined (sgi)
#define SHARED_OBJECT_COMMAND "g++ -shared -o"
#else
#if defined(SUN4)
#define SHARED_OBJECT_COMMAND "ld -z -o"
#endif // SUN4
#endif // SOL2
#else // __GNUG__
#define SHARED_OBJECT_COMMAND "CC -G -o"
#endif // __GNUG__
#endif

// The loader should do incremental linking; use a 4.3/Sun-style loader
// or use the Gnu loader.
#if defined(__sgi) || defined (sgi)
// For USE_DLOPEN, we need ld so we can process .o files into .so files
#define LOADER "/usr/bin/ld"
#else
#if defined(SOL2)
#define LOADER "/usr/ccs/bin/ld"
#else
#define LOADER "/bin/ld"
#endif // SOL2
#endif // sgi

// Full pathname of the "nm" program; it reads symbol names from a .o
// file.  Do NOT use a "demangling" version such as gnu nm.

#if defined(__sgi) || defined (sgi)
#define NM_PROGRAM "/usr/bin/nm"
#else
#if defined(SOL2)
#define NM_PROGRAM "/usr/ccs/bin/nm"
#else
#define NM_PROGRAM "/bin/nm"
#endif // SOL2
#endif // sgi

// Options to give the loader.  We also give it "-T hex-addr" to specify
// the starting address and "-A ptolemy-name" to get the symbols for the
// running executable.
#ifdef hpux
#define LOADOPTS "-N -x -a archive"
#define LOC_OPT "-R"
#else  // !hpux
#define LOC_OPT "-T"
#ifdef mips
#define __mips 1
#define LOADOPTS "-N -x -G 0"
#else // mips
#define LOADOPTS "-N -x"
#endif // !mips
#endif // !hpux

#ifdef hpux
#define DYNLIB "/lib/dyncall.o"
#else // !hpux
#define DYNLIB ""
#endif // hpux

// The assembler to use
#define ASSEMBLER "as"

// This directive should go to the text section
#define TEXT_DIR ".text"

// This directive should declare an address word
#if defined(mips) || defined(hpux)
#define ADDR_WORD ".word"
#else
#define ADDR_WORD ".long"
#endif

// hp-ux does not provide this function; may need to be changed
#ifdef hpux
inline size_t getpagesize() { return 4096;}
#else
#ifdef __GNUG__
#if defined(__sgi) || defined(sgi)
#if defined(__SYSTYPE_SVR4) || defined(SYSTYPE_SVR4)
/* SGI irix5 */
extern "C" int getpagesize(void);
#endif /* SVR4 */
#else
extern "C" size_t getpagesize(void);
#endif
#endif
#endif

#if defined(SOL2) 
#if ! defined(__GNUG__)
// Don't call InvokeConstructors if gcc-2.5.8 has Ron Guilmette's SVR4
// patches, or if we are using CC.  Right now we only use this code
// with CC.
// This is still experimental.  If this works, then there will be no
// need to call dlsym().
//
// Joe Buck's g++ FAQ says:
// `If you want to build shared libraries from g++ compiled code on any
// sort of SVr4 system (including Solaris) you should contact Ron Guilmette
// <rfg@netcom.com>.  He has patches that insure that any file-scope static-
// storage objects within such libraries will be properly initialized when
// the libraries are first attached to your running process. (The patches
// are for g++ 2.5.8.  They *might* be incorporated into the g++ 2.6 standard
// distribution, but that's not certain yet.)'
//
#define NO_INVOKECONSTRUCTORS
// NO_INVOKECONSTRUCTORS only works with dlopen() style linking
#define USE_DLOPEN
// If NO_INVOKECONSTRUCTORS is defined, the USE_NM_GREP is not used
#undef USE_NM_GREP
#undef NM_GREP_STRING
// If NO_INVOKECONSTRUCTORS is defined, then nm is not called
#undef NM_PROGRAM
#undef NM_OPTIONS
#endif

#if ! defined(NO_INVOKECONSTRUCTORS)
// Prefix for constructor-calling symbols
// This is an attempt to support both g++ and cfront.
#if defined(__GNUG__)
#if defined(mips) || defined(hpux)
#if defined(__sgi) || defined(sgi)
#define CONS_PREFIX "_GLOBAL_.I."
#define CONS_LENGTH 11
#else
#define CONS_PREFIX "_GLOBAL_$I$"
#define CONS_LENGTH 11
#endif // sgi
#else /* g++, nonmips & nonhpux */
#if defined(SOL2)
#define CONS_PREFIX "_GLOBAL_.I."
#define CONS_LENGTH 11
#else /*SOL2*/
#define CONS_PREFIX "__GLOBAL_$I$"
#define CONS_LENGTH 12
#endif/*SOL2*/
#endif
#else /* not __GNUG__ (i.e. cfront) */
#if defined(hpux) || defined(SOL2)

// These definitions also work with Sun Solaris2.3 CC3.0, but not
// with CC4.0.  Sol2 CC4.0 uses ctv as a constructor suffix
#define CONS_PREFIX "__sti"
#define CONS_LENGTH 5
#else // hpux || sol2
#define CONS_PREFIX "___sti"
#define CONS_LENGTH 6
#endif // hpux || SOL2
#endif
#endif // NO_INVOKECONSTRUCTORS

#if defined(hpux) || defined(__sgi) || defined(sgi) || defined(SOL2)
// -p print easily parsable, terse output
// -h don't print the output heading data
// -x print in hex rather than decimal
#define NM_OPTIONS "-phx"
#else
#define NM_OPTIONS "-g"
#endif

#if defined(hpux)
// On the hppa a symbol can have the same name in the code and an
// entry segments.  nm -p cannot differentiate between the two, so we
// define a separate method of checking for symbols
#define USE_NM_GREP
#define NM_GREP_STRING "..*|extern|entry'"
#endif

// Optionally skip '0x' the first 2 characters of nm output.
// The Solaris2.3 sscanf barfs on numbers that begin with 0x
#ifdef SOL2
#define NM_ADDR_OFFSET 2
#else
#define NM_ADDR_OFFSET 0
#endif

// g++ has to see the following before it sees the declaration of
// the flock function in std.h, or it will give a warning message.
// (in C++ there is a namespace collision).
struct flock;

extern "C" {
#ifdef hpux
#include <a.out.h>
#else
#ifdef mips
#define COFF
#include <aouthdr.h>
#endif
#ifndef SOL2
#include <a.out.h>
#endif /*SOL2*/
#endif
}

#if defined(hpux) || defined(SOL2) || defined(SYSV) || defined(SVR4)
#include <fcntl.h>
#endif

#if defined(SOL2)
#include <unistd.h>
#endif

// Sun4: don't include nlist.h after a.out.h, or nlist will be
// multiply defined.
#ifdef hpux
#include <nlist.h>
#include <stdlib.h>
extern "C" {int nlist(char *, struct nlist *);};
#endif //hpux

#include <std.h>
#ifndef mips
#include <sys/file.h>
#endif

// gcc/libg++ 2.5 is missing a prototype for pclose, though there is one
// for popen.  Go figure.
#ifdef __GNUG__
extern "C" int pclose(FILE *);
#endif

// the following code may also be needed on other platforms.
// No, this has not been tested on a Pyramid; it comes from libg++
// example code that suggests that this is the thing to do.
#ifdef __pyr__
#include <sys/mman.h>

extern "C" {
  int
  mprotect (char *, int, int);
}
inline void makeExecutable(size_t size, int pagsiz, char* init_fn) {
	void* roundedSize = (void*) ((size+pagsiz-1)&~(pagsiz-1));
	return mprotect(init_fn, roundedSize,
			       PROT_READ|PROT_EXEC|PROT_WRITE);
}
#else
#define makeExecutable(size,pagsiz,init_fn) 0 /* nothing */
#endif

// macros to read in the object file -- too tightly coupled with
// variables in readInObj, but...

#ifdef hpux
extern "C" { void flush_cache(void *, size_t);};
#define INITIALIZE() \
  (memset((void *) (h2.exec_dmem + h2.exec_dsize), 0, (size_t) (h2.exec_bsize)))
#define FLUSH_CACHE()  \
  (flush_cache(availMem, (size_t) ((h2.exec_dmem - h2.exec_tmem) \
			+ h2.exec_dsize + h2.exec_bsize)))
#else
#define INITIALIZE()  /* nothing */
#define FLUSH_CACHE() /* nothing */
#endif

#ifdef hpux
#define READOBJ_FAIL \
(lseek (fd, (off_t) h2.exec_tfile, SEEK_SET) < 0 ||\
 read (fd, (void *) h2.exec_tmem, (size_t) h2.exec_tsize) < h2.exec_tsize ||\
 lseek (fd, (off_t) h2.exec_dfile, SEEK_SET) < 0 ||\
 read (fd, (void *) h2.exec_dmem, (size_t) h2.exec_dsize) < h2.exec_dsize)

#define STRUCT_DEFS FILHDR h1; AOUTHDR h2

#define READHEAD_FAIL \
(read (fd, (void *) &h1, sizeof h1) <= 0 ||\
read (fd, (void *) &h2, sizeof h2) <= 0)

#define OBJ_SIZE (size_t)((h2.exec_dmem - h2.exec_tmem) \
			 + h2.exec_dsize + h2.exec_bsize)

#endif /* hpux */

#ifdef COFF
#define STRUCT_DEFS filehdr h1;	aouthdr h2
#define READHEAD_FAIL \
(read (fd, (char*) &h1, sizeof h1) <= 0 ||\
 read (fd, (char*) &h2, sizeof h2) <= 0)
#define OBJ_READ_SIZE ((size_t)(h2.tsize + h2.dsize))
#define READOBJ_FAIL \
 (lseek(fd, N_TXTOFF(h1,h2), 0) < 0 \
   || read (fd, availMem, OBJ_READ_SIZE) < OBJ_READ_SIZE)
#define OBJ_SIZE (size_t)(h2.tsize + h2.dsize + h2.bsize)

#endif

#if defined(sun) || defined(vax)
#if !defined(USE_DLOPEN)
#define STRUCT_DEFS exec header
#define READHEAD_FAIL (read (fd, (char*) &header, sizeof(header)) <= 0)
#define OBJ_READ_SIZE ((size_t)(header.a_text + header.a_data))
#define READOBJ_FAIL (read (fd, availMem, OBJ_READ_SIZE) < OBJ_READ_SIZE)
#define OBJ_SIZE (size_t)(header.a_text + header.a_data + header.a_bss)
#endif
#endif


#if defined(LINKING_NOT_SUPPORTED) || defined(SOL2)
#define STRUCT_DEFS
#define READHEAD_FAIL 1
#define READOBJ_FAIL 1
#define OBJ_SIZE 0
#define READOBJ_FAIL 1
#endif // LINKING_NOT_SUPPORTED

// alpha stuff
#if defined(__alpha)
inline int getpagesize() { return 4096;}
// ugly hack to get around a bug in the popen prototype for Alpha/OSF
#define ALPHAFIX (char*)(const char*)
#else
#define ALPHAFIX
#endif
