#ifndef PARAMSTRUCTS_H
#define PARAMSTRUCTS_H 1

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
/* paramStructs.h  edg
Version identification:
$Id$
*/

#ifndef __cplusplus
#ifndef const
#define const /* nothing */
#endif
#endif

/* Data Structures */
struct ParamStruct {
    const char *name;
    const char *type;
    const char *value;
};
typedef struct ParamStruct ParamType;

struct ParamListStruct {
    int length;		/* length of array */
    ParamType *array;	/* points to first element */
};
typedef struct ParamListStruct ParamListType;

extern boolean PStrToPList();
extern char * PListToPStr();
extern void FreeFlatPList();

#endif /* PARAMSTRUCTS_H */

