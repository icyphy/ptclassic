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
 * Message Managment Package Header
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains declarations required for using the VEM
 * message display facility.  This include file requires:
 *  - general.h
 */

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "ansi.h"
#include "general.h"

extern char *msg_pkg_name;

/* Fatal errors */
#define MSG_NOFILE	-1	/* Bad log file */

#define MSG_NOLOG	0x01	/* Don't log message to file */
#define MSG_DISP	0x02	/* Display message to user   */
#define MSG_MOREMD	0x04	/* Use more mode             */

/* New - more terse flags */

#define MSG_C		0x01		/* To console  */
#define MSG_L		0x02		/* To log file */
#define MSG_A		(MSG_C|MSG_L)	/* To both    */

extern char *errMsgArea;
  /* Area for building error messages */

extern void vemMsgSetCons
  ARGS((Pointer conData, void (*conWriteFunc)()));
  /* Sets the current console window */

extern vemStatus vemMsgFile
  ARGS((STR fileName));
  /* Sets the current log file */

extern void vemMessage
  ARGS((char *msg, int opt));
  /* Writes out a message      */

#ifdef MLINT

extern void vemMsg();

#else /* MLINT */
#ifdef lint

/*ARGSUSED*/
/*VARARGS1*/
void vemMsg(where, fmt)
int where;
char *fmt;
{
   /* Leave definition out */
}

#else /* lint */

extern void vemMsg
  ARGS((int flags, char *fmt, ...));
  /* Self-parsing version of message function */

#endif /* lint */
#endif /* MLINT */

#endif /* _MESSAGE_H_ */
