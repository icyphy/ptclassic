/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This file defines the system-dependent cruft needed for incremental
 linking.
**************************************************************************/

// The loader should do incremental linking; use a 4.3/Sun-style loader
// or use the Gnu loader.
#define LOADER "ld"

// Options to give the loader.  We also give it "-T hex-addr" to specify
// the starting address and "-A ptolemy-name" to get the symbols for the
// running executable.
#ifdef mips
#define __mips 1
#define LOADOPTS "-N -S -G 0"
#else
#define LOADOPTS "-N -S"
#endif

// The assembler to use
#define ASSEMBLER "as"

// This directive should go to the text section
#define TEXT_DIR ".text"

// This directive should declare an address word
#ifdef mips
#define ADDR_WORD ".word"
#else
#define ADDR_WORD ".long"
#endif

// Prefix for constructor-calling symbols
// This is an attempt to support both g++ and cfront.
#ifdef __GNUG__
#define DEST_CHAR 'D'
#ifdef mips
#define CONS_PREFIX "_GLOBAL_$I$"
#define ID_POS 9
#else /* g++, nonmips */
#define CONS_PREFIX "__GLOBAL_$I$"
#define ID_POS 10
#endif
#else /* not __GNUG__ (i.e. cfront) */
#define CONS_PREFIX "___sti"
#define ID_POS 5
#define DEST_CHAR 'd'
#endif

// g++ has to see the following before it sees the declaration of
// the flock function in std.h, or it will give a warning message.
// (in C++ there is a namespace collision).
struct flock;

extern "C" {
#ifdef mips
#define __LANGUAGE_C
#define COFF
#include <aouthdr.h>
#endif
#include <a.out.h>
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



