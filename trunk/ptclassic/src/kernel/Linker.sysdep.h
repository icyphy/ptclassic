/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This file defines the system-dependent cruft needed for incremental
 linking.

 The stuff for HP is not yet completed, hence linkingNotSupported is
 true.

**************************************************************************/

// Is linking supported?  Currently not complete for HP/UX.
const int linkingNotSupported =
#if defined(hpux) || defined(__alpha)
 1;
#else
 0;
#endif

// The loader should do incremental linking; use a 4.3/Sun-style loader
// or use the Gnu loader.
#define LOADER "/bin/ld"

// Full pathname of the "nm" program; it reads symbol names from a .o
// file.  Do NOT use a "demangling" version such as gnu nm.

#define NM_PROGRAM "/bin/nm"

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
#else
#define LOADOPTS "-N -x"
#endif // !mips
#endif // !hpux

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
inline int getpagesize() { return 4096;}
#endif

// Prefix for constructor-calling symbols
// This is an attempt to support both g++ and cfront.
#ifdef __GNUG__
#if defined(mips) || defined(hpux)
#define CONS_PREFIX "_GLOBAL_$I$"
#define CONS_LENGTH 11
#else /* g++, nonmips & nonhpux */
#define CONS_PREFIX "__GLOBAL_$I$"
#define CONS_LENGTH 12
#endif
#else /* not __GNUG__ (i.e. cfront) */
#define CONS_PREFIX "___sti"
#define CONS_LENGTH 6
#endif

#ifdef hpux
#define NM_OPTIONS "-e -p -x"
#else
#define NM_OPTIONS "-g"
#endif

// g++ has to see the following before it sees the declaration of
// the flock function in std.h, or it will give a warning message.
// (in C++ there is a namespace collision).
struct flock;

extern "C" {
#ifdef hpux
#include <a.out.h>
#include <fcntl.h>
#else
#ifdef mips
#define COFF
#include <aouthdr.h>
#endif
#include <a.out.h>
#endif
}

#include <std.h>
#ifndef mips
#include <sys/file.h>
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
#define READOBJ_FAIL \
(lseek (fd, h2.exec_tfile, SEEK_SET) < 0 ||\
 read (fd, availMem, h2.exec_tsize) < h2.exec_tsize ||\
 lseek (fd, h2.exec_dfile, SEEK_SET) < 0 ||\
 read (fd, availMem+h2.exec_tsize, h2.exec_dsize) < h2.exec_dsize)

#define STRUCT_DEFS FILHDR h1; AOUTHDR h2

#define READHEAD_FAIL \
(read (fd, (char*) &h1, sizeof h1) <= 0 ||\
read (fd, (char*) &h2, sizeof h2) <= 0)

#define OBJ_SIZE (size_t)(h2.exec_tsize + h2.exec_dsize + h2.exec_bsize)

#endif

#ifdef COFF
#define STRUCT_DEFS filehdr h1;	aouthdr h2
#define READHEAD_FAIL \
(read (fd, (char*) &h1, sizeof h1) <= 0 ||\
 read (fd, (char*) &h2, sizeof h2) <= 0)
#define READOBJ_FAIL \
 (lseek(fd, N_TXTOFF(h1,h2), 0) < 0 || read (fd, availMem, size) < size)
#define OBJ_SIZE (size_t)(h2.tsize + h2.dsize + h2.bsize)

#endif

#if defined(sun) || defined(vax)
#define STRUCT_DEFS exec header
#define READHEAD_FAIL (read (fd, (char*) &header, sizeof(header)) <= 0)
#define READOBJ_FAIL (read (fd, availMem, size) < size)
#define OBJ_SIZE (size_t)(header.a_text + header.a_data + header.a_bss)
#endif

// alpha stuff
#ifdef __alpha
#define STRUCT_DEFS
#define READHEAD_FAIL 1
#define READOBJ_FAIL 1
#define OBJ_SIZE 0
#define READOBJ_FAIL 1
inline int getpagesize() { return 4096;}
// ugly hack to get around a bug in the popen prototype for Alpha/OSF
#define ALPHAFIX (char*)(const char*)
#else
#define ALPHAFIX
#endif
