/******************************************************************
Version identification:
@(#)Linker.sysdep.h	2.69 01/05/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

 This file defines the system-dependent cruft needed for incremental
 linking.

**************************************************************************/

// get size_t definition
#include <sys/types.h>
#include "compat.h"		/* Pickup PTSOL2 etc. */
#include <sys/stat.h>		/* Pickup chmod for hppa */

#ifdef PT_NT4VC
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

#if defined(PTIRIX5) || defined(PTSOL2)  || defined(PTALPHA) || defined(PTLINUX_ELF) || defined(PTSVR4) || defined(PTNT) && !defined(PT_NT4VC)
#define PTSVR4_STYLE_LINKING
#include <dlfcn.h>
#include <sys/stat.h>
#define USE_DLOPEN

// Don't use RTLD_GLOBAL, or if you reload a star, you won't see the changes
#ifdef PTIRIX5
// Under sgi, don't use sgidladd, or you won't see changes
#define DLOPEN dlopen
#define DLOPEN_FLAGS RTLD_NOW
#endif // PTIRIX5

#if defined(PTSVR4_STYLE_LINKING)
#define DLOPEN dlopen
#if defined(PTSOL2) || defined(PTALPHA)
#define DLOPEN_FLAGS RTLD_NOW
#else
#if defined(PTLINUX) && defined(__ELF__)
// Neal Becker says:
//   I am pleased to note that on linux-ELF systems the RTLD_GLOBAL flag is
//   now supported.  You need the package ld.so-1.7.12 or later.
// #define DLOPEN_FLAGS RTLD_LAZY|RTLD_GLOBAL
// RTLD_GLOBAL flag is removed because it could cause problems with 
// temporary linking and permanent linking.
#define DLOPEN_FLAGS RTLD_LAZY
#else
// defined(sun)
#define DLOPEN_FLAGS RTLD_LAZY
#endif // PTLINUX
#endif // PTSOL2 || PTALPHA
#endif // PTSVR4_STYLE_LINKING

#ifdef __GNUG__
#if defined(PTSOL2) || defined(PTIRIX5) || defined(PTLINUX_ELF) || defined (PTSVR4) || defined(PTNT)
#define SHARED_OBJECT_COMMAND "g++ -shared -o"
#else
#ifdef PTSUN4
#define SHARED_OBJECT_COMMAND "ld -z -o"
#else // PTSUN4
#ifdef PTALPHA
#define SHARED_OBJECT_COMMAND "g++ -shared -Wl,-expect_unresolved,'*' -o"
//#define SHARED_OBJECT_COMMAND "ld -shared -expect_unresolved '*' -soname"
#endif // PTALPHA
#endif // PTSUN4

#endif // PTSOL2
#else // __GNUG__
#if defined(PTIRIX5_CFRONT)
#define SHARED_OBJECT_COMMAND "CC -shared -o"    //-G  doesn't work at al
#else
#define SHARED_OBJECT_COMMAND "CC -G -o"
#endif // PTIRIX5_CFRONT
#endif // __GNUG__
#endif

// The loader should do incremental linking; use a 4.3/Sun-style loader
// or use the Gnu loader.
#if defined(PTIRIX5) || defined(PTLINUX) || defined(PTNBSD_386) || defined(PTFREEBSD)
// For USE_DLOPEN, we need ld so we can process .o files into .so files
#define LOADER "/usr/bin/ld"
#else
#if defined(PTSOL2) || defined(PTSVR4)
#define LOADER "/usr/ccs/bin/ld"
#else
#define LOADER "/bin/ld"
#endif // PTSOL2
#endif // PTIRIX5

#if defined(PTHPPA) &&  __GNUG__ > 1 && defined(USE_SHLLOAD)
// Define USE_SHLLOAD for Neal Becker's HPPA shl_load() style
// dynamic linking.  You can add -DUSE_SHLLOAD to MISCCFLAGS in 
// $PTOLEMY/mk/config-hppa.mk
// To get this to work, you'll need a linker patch
// As of 4/95 the linker patch was PHSS_5083* for hpux9.x
// gcc-2.7.2 under HPUX10.01 cannot create shared libraries, so this
// might not work.
#if defined(PT_EGCS)
#define SHARED_OBJECT_COMMAND "g++ -shared -fPIC -nostdlib -Xlinker +s -lstdc++ -o"
#else
#define SHARED_OBJECT_COMMAND "g++ -shared -fPIC -nostdlib -Xlinker +s -lg++ -lstdc++ -o"
#endif //PT_EGCS
// HPPA really, really wants shared library permissions to be 555.
#define READONLY_SHAREDLIBS
#include <dl.h>
#include <errno.h>
#endif

// Full pathname of the "nm" program; it reads symbol names from a .o
// file.  Do NOT use a "demangling" version such as gnu nm.

#if defined(PTAIX)       
#define NM_PROGRAM "/usr/ucb/nm"
#else
#if defined(PTIRIX5) || defined(PTLINUX) || defined(PTNBSD_386) || defined(PTFREEBSD)
#define NM_PROGRAM "/usr/bin/nm"
#else
#if defined(PTSOL2) || defined(PTSVR4)
#define NM_PROGRAM "/usr/ccs/bin/nm"
#else
#define NM_PROGRAM "/bin/nm"
#endif // PTSOL2
#endif // PTIRIX5 || PTLINUX || PTNBSD_386 || PTFREEBSD
#endif // PTAIX

// Options to give the loader.  We also give it "-T hex-addr" to specify
// the starting address and "-A ptolemy-name" to get the symbols for the
// running executable.
// linux note: this should be OK for binutils-1.9, but new versions will
// use different flags :( neal@ctd.comsat.com ):
#ifdef PTHPPA
#define LOADOPTS "-N -x -a archive"
#define LOC_OPT "-R"
#else  // !PTHPPA
#define LOC_OPT "-T"
#ifdef mips
#define __mips 1
#define LOADOPTS "-N -x -G 0"
#else // mips
#define LOADOPTS "-N -x"
#endif // !mips
#endif // !PTHPPA

#ifdef PTHPPA
#define DYNLIB "/lib/dyncall.o"
#else // !PTHPPA
#define DYNLIB ""
#endif // PTHPPA

// The assembler to use
#define ASSEMBLER "as"

// This directive should go to the text section
#define TEXT_DIR ".text"

// This directive should declare an address word
#if defined(PTULTRIX) || defined(PTHPPA)
#define ADDR_WORD ".word"
#else
#define ADDR_WORD ".long"
#endif

// hp-ux does not provide this function; may need to be changed
#if defined(PTHPPA)
#if ! defined(PTHPUX10)
// Under HPUX-10.20, getpagesize is defined in /usr/include/sys/unistd.h
inline size_t getpagesize() { return 4096;}
#endif
#else
#if defined(PTLINUX) || defined(PTALPHA) || defined(PTNBSD_386) || defined(PTFREEBSD)
#include <unistd.h>
#else /* PTLINUX || PTALPHA || PTNBSD_386 || PTFREEBSD */
#ifdef __GNUG__
#ifdef PTIRIX5
#if defined(__SYSTYPE_SVR4) || defined(SYSTYPE_SVR4)
/* SGI irix5 */
extern "C" int getpagesize(void);
#endif /* SVR4 */
#else
#ifdef PTSOL2_6
extern "C" int getpagesize(void);
#else
extern "C" size_t getpagesize(void);
#endif
#endif
#endif
#endif
#endif

#if defined(PTSOL2) || defined(PTLINUX_ELF)
// Don't call InvokeConstructors if gcc-2.5.8 has Ron Guilmette's SVR4
// patches, or if we are using CC.  Right now we only use this code
// with CC. gcc-2.6.0 seems to require that InvokeConstructors be run.
// but gcc-2.7.2.1 does not requre it
//
// This is still experimental.  If this works, then there will be no
// need to call dlsym().
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
#ifdef __GNUG__
#if defined(mips) || defined(PTHPPA) || defined(PTLINUX_ELF)
#if (defined(PTIRIX5) && __GNUC_MINOR__ < 7  ) || defined(PTLINUX_ELF)
#define CONS_PREFIX "_GLOBAL_.I."
#define CONS_LENGTH 11
#else
#define CONS_PREFIX "_GLOBAL_$I$"
#define CONS_LENGTH 11
#endif // PTIRIX5 || PTAIX
#else /* g++, nonmips & nonPTHPPA */
#if defined(PTSOL2) || defined (PTAIX) || defined(PTSVR4)
#define CONS_PREFIX "_GLOBAL_.I."
#define CONS_LENGTH 11
#else // PTSOL2 || PTAIX
#if defined(PTALPHA)
#define CONS_PREFIX "_GLOBAL_$I$"
#define CONS_LENGTH 11
#else // PTALPHA
#define CONS_PREFIX "__GLOBAL_$I$"
#define CONS_LENGTH 12
#endif //PTALPHA
#endif //PTSOL2 || PTAIX
#endif
#else /* not __GNUG__ (i.e. cfront) */
#if defined(PTHPPA) || defined(PTSOL2) || defined(PTIRIX5_CFRONT)

// These definitions also work with Sun Solaris2.3 CC3.0, but not
// with CC4.0.  Sol2 CC4.0 uses ctv as a constructor suffix
#define CONS_PREFIX "__sti"
#define CONS_LENGTH 5
#else // PTHPPA || sol2
#define CONS_PREFIX "___sti"
#define CONS_LENGTH 6
#endif // PTHPPA || SOL2
#endif
#endif // NO_INVOKECONSTRUCTORS

#if defined(PTHPPA) || defined(PTIRIX5) || defined(PTSOL2)
// -p print easily parsable, terse output
// -h don't print the output heading data
// -x print in hex rather than decimal
#define NM_OPTIONS "-phx"
#else
#ifdef PTLINUX
#define NM_OPTIONS "-g --no-cplus"
#else
#ifdef PTALPHA
#define NM_OPTIONS "-B"
#else
#define NM_OPTIONS "-g"
#endif // PTALPHA
#endif
#endif

#ifdef PTHPPA
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
#ifdef PTHPPA
#include <a.out.h>
#else
#ifdef mips
#define COFF
#include <aouthdr.h>
#endif
#if !defined(SOL2) && !defined(PTNT)
#include <a.out.h>
#endif /*SOL2*/
#endif
}

#if defined(PTHPPA) || defined(PTSOL2) || defined(SYSV) || defined(SVR4)
#include <fcntl.h>
#endif

#if defined(PTSOL2) || defined(PTSVR4)
#include <unistd.h>
#endif

// Sun4: don't include nlist.h after a.out.h, or nlist will be
// multiply defined.
#ifdef PTHPPA
#include <nlist.h>
#include <stdlib.h>
#ifndef PTHPUX10
// HPUX10.01 gets this right, HPUX9.x needs nlist declared.
extern "C" {int nlist(char *, struct nlist *);};
#endif
#endif //PTHPPA

#include <std.h>
#if !defined(mips) && !defined(PT_NT4VC)
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

#ifdef PTHPPA
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

#ifdef PTHPPA
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

#endif /* PTHPPA */

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

#if defined(PTSUN4) || defined(PTSOL2) || defined(vax)
#if !defined(USE_DLOPEN)
#define STRUCT_DEFS exec header
#define READHEAD_FAIL (read (fd, (char*) &header, sizeof(header)) <= 0)
#define OBJ_READ_SIZE ((size_t)(header.a_text + header.a_data))
#define READOBJ_FAIL (read (fd, availMem, OBJ_READ_SIZE) < OBJ_READ_SIZE)
#define OBJ_SIZE (size_t)(header.a_text + header.a_data + header.a_bss)
#endif
#endif

#if defined(PTLINUX) || defined(PTNBSD_386) || defined(PTFREEBSD)
#define STRUCT_DEFS exec header
#define READHEAD_FAIL (read (fd, (char*) &header, sizeof(header)) <= 0)
#define OBJ_READ_SIZE ((size_t)(header.a_text + header.a_data))
#define READOBJ_FAIL \
 (lseek(fd, N_TXTOFF(header), 0) < 0 \
   || read (fd, availMem, OBJ_READ_SIZE) < OBJ_READ_SIZE)
#define OBJ_SIZE (size_t)(header.a_text + header.a_data + header.a_bss)
#endif


#if defined(LINKING_NOT_SUPPORTED) || defined(PTSOL2)
#define STRUCT_DEFS
#define READHEAD_FAIL 1
#define READOBJ_FAIL 1
#define OBJ_SIZE 0
#define READOBJ_FAIL 1
#endif // LINKING_NOT_SUPPORTED

// alpha stuff
#ifdef PTALPHA
inline int getpagesize() { return 4096;}
// ugly hack to get around a bug in the popen prototype for Alpha/OSF
#define ALPHAFIX (char*)(const char*)
#else
#define ALPHAFIX
#endif

#ifdef PTAIX
#define N_TXTOFF(f, a) 1024
#define STRUCT_DEFS filehdr h1; aouthdr h2
#define READHEAD_FAIL \
(read (fd, (char*) &h1, sizeof h1) <= 0 ||\
 read (fd, (char*) &h2, sizeof h2) <= 0)
#define OBJ_READ_SIZE ((size_t)(h2.tsize + h2.dsize))
#define READOBJ_FAIL \
 (lseek(fd, N_TXTOFF(h1,h2), 0) < 0 \
   || read (fd, availMem, OBJ_READ_SIZE) < OBJ_READ_SIZE)
#define OBJ_SIZE (size_t)(h2.tsize + h2.dsize + h2.bsize)
#endif

#ifdef PTAIX_XLC
extern "C" int getpagesize(void);
#endif

#ifdef PTSUN4
/* The sun4 port of Ptolemy requires a static pigiRpc for incremental linking
 * to work.  The pigiRpc binary can have _no_ shared libraries, or
 * the bsd style ld incremental linking will fail with a message like
 * ld: -ldl: No such file or directory
 * However, Tcl7.6 permits incremental linking of shared objects, so
 * it has calls to dlsym, dlopen and dlerror.  However, we cannot
 * link to -ldl, so we provide dummy functions here.  
 */
extern "C" {
static void ptsun4dlerror() {
     fprintf(stderr,
             "Linker: The sun4 port of Ptolemy does not support dl* calls\n");
} 
void *dlopen(char * /*path*/, int /*mode*/) {
	ptsun4dlerror();
	return (void *)NULL;
}
void *dlsym(void * /*handle*/, char * /*symbol*/) {
	ptsun4dlerror();
	return (void *)NULL;
}
char *dlerror() {
	ptsun4dlerror();
	return (char *)NULL;
}
int dlclose(void * /*handle*/) {
	ptsun4dlerror();
	return 0;
}

} /* extern "C" */
#endif /* PTSUN4 */
