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
// change the following if ld++ and crt1+.o are in non-standard directories

#ifndef LOADER
#define LOADER "/usr/gnu/lib/gcc-ld"
#endif

// The standard assember "as" may work as well.
#ifndef ASSEMBLER
#define ASSEMBLER "/usr/gnu/lib/gcc-as"
#endif

// This directive should go to the text section
#ifndef TEXT_DIR
#define TEXT_DIR ".text"
#endif

// This directive should declare an address word
#ifndef ADDR_WORD
#define ADDR_WORD ".long"
#endif

extern "C" {
#include <a.out.h>
}

// g++ has to see the following before it sees the declaration of
// the flock function in std.h, or it will give a bogus warning message.
struct flock;

#include <std.h>
#include <sys/file.h>
#if defined(MIPSEL) || defined(USG) 
#if !defined(hpux)
#define exec aouthdr
#define a_text text_start
#define a_data data_start
#endif
#endif

// the following code may also be needed on other platforms.
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



