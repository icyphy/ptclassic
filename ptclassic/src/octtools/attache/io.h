/* Version Identification:
 * $Id$
 */
/*
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
*/
#ifndef ATTACHE_IO_H
#define ATTACHE_IO_H
#include "ansi.h"

extern char eraseChar;
extern char wordEraseChar;
extern char lineKillChar;
extern int LINES;
extern int COLS;

extern void IOinit();
extern void IOend();
extern int IOgetchar();
extern void IOputchar	ARGS((int ch));
extern void IOputs	ARGS((char *str));
extern void IOwrap	ARGS((char *str));
extern void IOputcharSee	ARGS((int ch));
extern void IOputsSee		ARGS((char *str));
extern void IOhome();
extern void IObotHome();
extern void IOmove		ARGS((int y, int x));	
extern void IOgetyx		ARGS((int *yPtr, int *xPtr));	
extern void IOstandout();
extern void IOstandend();
extern void IOclearScreen();
extern void IOforceClear();
extern void IOclearLine();
extern void IOrefresh();

#define CNTRL(ch)	((ch) ^ 0100)

#endif /* ATTACHE_IO_H */
