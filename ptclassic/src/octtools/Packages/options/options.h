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
#ifndef OPT_H
#define OPT_H

#include "ansi.h"

#define OPT_PKG_NAME	"options"

typedef struct {
    char *optionLetters;
    char *argName;
    char *description;
} optionStruct;

/* dummy `optionLetters' values for arguments following options */
extern char OPT_RARG[];		/* required argument */
extern char OPT_OARG[];		/* optional argument */
extern char OPT_ELLIP[];	/* flag to put ellipsis in summary */
extern char OPT_DESC[];		/* general description */
extern char OPT_CONT[];		/* continues description */

extern optionStruct optionList[];
extern int optind;
extern char *optarg;
extern char *optProgName;

EXTERN int optGetOption
	ARGS((int argc, char **argv));
EXTERN void optUsage
	NULLARGS;
EXTERN void optInit
	ARGS((char *progName, optionStruct options[], int rtnBadFlag));
EXTERN void optAddOptions
	ARGS((optionStruct options[]));
EXTERN void optAddUnivOptions
	NULLARGS;
EXTERN char *optUsageString
	NULLARGS;

#endif /* OPT_H */
