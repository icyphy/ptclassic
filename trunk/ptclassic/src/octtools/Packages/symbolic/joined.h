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
/*
 * Joined Terminals
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file contains utility routines for dealing with joined
 * terminals.  Joined terminals are separate terminals that
 * are electrically equivalent.  The routines included here
 * test to see if a terminal is joined with others and
 * allow the user to find out what other terminals are
 * joined.
 */

#ifndef _JOINED_H_
#define _JOINED_H_

#include "ansi.h"
#include "oct.h"

extern int symNumJoined
  ARGS((octObject *term));
  /* Returns number of electrically equivalent terminals */

#define SYM_LIST_TOO_SMALL	-1

extern int symListJoined
  ARGS((octObject *term, int num, octObject joined[]));
  /* Fills in a list of electrically equivalent terminals */

#endif /* _JOINED_H_ */
