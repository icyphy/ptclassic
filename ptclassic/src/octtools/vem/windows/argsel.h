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
 * Argument Selection Set Header
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989
 *
 * This file contains header definitions for the argument list selection
 * module implemented in argsel.c.  This package exports an interface
 * for displaying argument types in all appropriate windows.
 *
 * IMPORTANT:  all routines assume that the selection attributes desired
 * have already been set by appropriate calls to the attribute module.
 */

#ifndef ARGSEL_HEADER
#define ARGSEL_HEADER

#include "ansi.h"
#include "commands.h"		/* Dispatch module */

extern void argOn
  ARGS((vemOneArg *arg));
  /* Turns on the selection of an argument */

extern void argOff
  ARGS((vemOneArg *arg));
  /* Turns off selection of an argument */

extern void argType
  ARGS((vemOneArg *arg, int type));
  /* Sets the type of the arg set */

extern void argAddPoint
  ARGS((vemOneArg *arg, vemPoint *pnt));
  /* Adds a point to the current set */

extern void argChgPoint
  ARGS((vemOneArg *arg, int idx, vemPoint *pnt));
  /* Changes an existing point */

extern vemStatus argDelPoint
  ARGS((vemOneArg *arg, int idx));
  /* Removes an existing point */

extern void argInsPoint
  ARGS((vemOneArg *arg, int idx, vemPoint *pnt));
  /* Inserts a new point */

#endif
