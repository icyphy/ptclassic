#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
#include "copyright.h"
#include "port.h"
#include "st.h"
#include "internal.h"
#include "str.h"

/*#define STRINGSTATS*/

static st_table *stringTable;

#define SAVE(string)	strcpy(ALLOC(char, strlen(string) + 1), string)

struct string {
    char *ptr;
    int cnt;
#ifdef STRINGSTATS
    int max_cnt;
#endif /* STRINGSTATS */
};


void
oct_str_create()
{
    stringTable = st_init_table(strcmp, st_strhash);
}

#ifdef STRINGSTATS
static int strCount = 0;
static int nonZero = 0;
#endif

/*ARGSUSED*/
static enum st_retval
delstring(key, value, arg)
char *key, *value, *arg;
{
#ifdef STRINGSTATS
    strCount++;
    if (((struct string *) value)->cnt > 0) {
	nonZero++;
    }
    (void) fprintf(stderr, "%s (%d/%d)\n", key,
		   ((struct string *) value)->cnt,
		   ((struct string *) value)->max_cnt);
#endif
    FREE(key);
    FREE(value);
    return ST_CONTINUE;
}


void
oct_str_destroy()
{
#ifdef STRINGSTATS
    (void) fprintf(stderr, "String Table:\n");
#endif
    (void) st_foreach(stringTable, delstring, (char *) 0);
    st_free_table(stringTable);
#ifdef STRINGSTATS
    (void) fprintf(stderr, "\ntotal number of strings = %d\n", strCount);
    (void) fprintf(stderr, "\ntotal number of non-freed strings = %d\n", nonZero);
#endif
}


char *
strsave(ptr)
char *ptr;
{
    return(SAVE(ptr));
}

    
char *
oct_str_intern(ptr)
char *ptr;
{
    struct string *item;
    char *dummy;
    
    if (ptr != NIL(char)) {
	if (!st_lookup(stringTable, ptr, &dummy)) {
	    item = ALLOC(struct string, 1);
	    item->ptr = SAVE(ptr);
	    item->cnt = 1;
#ifdef STRINGSTATS
	    item->max_cnt = 1;
#endif /* STRINGSTATS */
	    (void) st_add_direct(stringTable, item->ptr, (char *) item);
	} else {
	    item = (struct string *) dummy;
	    item->cnt++;
#ifdef STRINGSTATS
	    item->max_cnt++;
#endif /* STRINGSTATS */
	}
	return(item->ptr);
    }
    return(NIL(char));
}


void
oct_str_free(ptr)
char *ptr;
{
    char *dummy;
    struct string *item;
    
    if (ptr != NIL(char)) {
	if (!st_lookup(stringTable, ptr, &dummy)) {
	    errRaise(OCT_PKG_NAME, OCT_ERROR, "freeing an unallocated string");
	    /*NOTREACHED*/
	}

	item = (struct string *) dummy;
	if (item->cnt-- <= 0) {
	    errRaise(OCT_PKG_NAME, OCT_ERROR, "freeing an already freed string");
	    /*NOTREACHED*/
	}
    }
}


