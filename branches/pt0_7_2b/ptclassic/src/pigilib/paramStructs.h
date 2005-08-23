#ifndef _PARAMSTRUCTS_H
#define _PARAMSTRUCTS_H 1

/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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

/* paramStructs.h  edg
Version identification:
@(#)paramStructs.h	1.13	4/26/96
*/

#include "compat.h"

/* Data Structures */
struct ParamStruct {
    char* name;
    char* type;
    char* value;
};

typedef struct ParamStruct ParamType;

struct ParamListStruct {
    int length;			/* length of array */
    ParamType* array;		/* points to first element */
    char* dynamic_memory;	/* pointer to allocated string memory */
    int flat_plist_flag;	/* indicates whether it's a flat param list */
};
typedef struct ParamListStruct ParamListType;

/* Function prototypes */

extern void FreeFlatPList ARGS((ParamListType *pListPtr));
extern boolean PStrToPList ARGS((const char *pStr, ParamListType *pListPtr));
extern char* PListToPStr ARGS((ParamListType *pListPtr));
extern boolean CopyFlatPList ARGS((ParamListType* srcPtr,
				   ParamListType* destPtr));

#endif  /* _PARAMSTRUCTS_H */
