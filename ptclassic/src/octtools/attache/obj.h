/* Version Identification:
 * $Id$
 */
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "copyright.h"
#define NILPF		((char *(*)()) 0)
#define NILGF		((int (*)()) 0)

struct enumTemplate {
    char *name;
    int value;
    struct fieldTemplate *subFields;
    void (*initFn)();
};

#define DFLT_ENUM_VAL	-12345

struct fieldTemplate {
    char *text;
    int offset;
    char *(*printFn)();
    int (*getFn)();
    struct enumTemplate *enumList;
};

#define OFS(field)	( (char *) &((octObject *) 0)->contents.field - \
			    (char *) 0 )
#define TYPE_OFS	( (char *) &((octObject *) 0)->type - (char *) 0 )

extern struct fieldTemplate summaryTemplate[];
extern struct fieldTemplate detailedTemplate[];
extern struct enumTemplate detailedObjEnum[];

#define RESET_FIELD	0
#define KEEP_FIELD	1
