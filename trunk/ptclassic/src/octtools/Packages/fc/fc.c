#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
/*
 * fc - file name completion package
 *
 * Wayne A. Christopher
 * David Harrison
 * University of California, Berkeley
 * 1988
 */

#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include <sys/types.h>

#ifdef USE_DIRENT_H		/* compat.h declares USE_DIRENT_H */
#include <dirent.h>
#else
#include <sys/dir.h>
#endif /* USE_DIRENT_H */

#include <pwd.h>

#define FC_TIL		'~'
#define MAXUNAME	15

static char *copy(str)
char *str;
/*
 * Makes an allocated copy of `str'.
 */
{
    if (str) {
	return strcpy(malloc((unsigned) (strlen(str)+1)), str);
    } else {
	return (char *) 0;
    }
}

static int prefix(dest, src)
char *dest;			/* String to check */
char *src;			/* Full check      */
/*
 * Returns a non-zero status if `dest' is a prefix of `src'.
 */
{
    return strncmp(dest, src, strlen(dest)) == 0;
}

#define COMP_INIT	10
static int comp_alloc = 0;
static int comp_size = 0;
static char **comp_ary = (char **) 0;

static void zero_comp(start)
int start;
/*
 * Resets all items in the comp array to zero
 */
{
    int i;

    for (i = start;  i < comp_alloc;  i++) {
	comp_ary[i] = (char *) 0;
    }
}

static void free_comp()
/*
 * Frees all allocated strings in comp array.
 */
{
    int i;

    for (i = 0;  i < comp_alloc;  i++) {
	if (comp_ary[i]) free(comp_ary[i]);
    }
}

static void reset_comp()
/*
 * Resets completion array.
 */
{
    if (comp_alloc == 0) {
	/* Allocate initially */
	comp_alloc = COMP_INIT;
	comp_ary = (char **) malloc((unsigned) (sizeof(char *) * comp_alloc));
	zero_comp(0);
    }
    free_comp();
    zero_comp(0);
    comp_size = 0;
}

static void add_comp(new)
char *new;			/* New completion                 */
/*
 * Adds a new completion to the array `comps'.
 */
{
    if (comp_size >= comp_alloc) {
	/* Make it bigger */
	comp_ary = (char **)
	  realloc((char *) comp_ary,
		  (unsigned) (sizeof(char *) * 2 * comp_alloc));
	comp_alloc *= 2;	   /* The array is big now. */
	zero_comp(comp_alloc / 2); /* Zero the second half of the array. */
    }
    comp_ary[comp_size] = copy(new);
    comp_size++;
}

static int compare(a, b)
char **a, **b;
{
    return strcmp(*a, *b);
}

static void sort_comp()
/*
 * Sorts the current list of words the the completion array.
 */
{
    qsort((char *) comp_ary, comp_size, sizeof(char *), compare);
}

static int common_comp()
/*
 * Returns the maximum number of characters that are common in 
 * the current comp array.
 */
{
    int result, idx, cspot, stspot;
    char *cmpstr, *teststr;

    result = 0;
    if (comp_size > 0) {
	cmpstr = comp_ary[0];
	result = strlen(cmpstr);
	for (idx = 1;  idx < comp_size;  idx++) {
	    teststr = comp_ary[idx];
	    stspot = strlen(teststr);
	    if (result < stspot) stspot = result;
	    for (cspot = 0;  cspot < stspot;  cspot++) {
		if (cmpstr[cspot] != teststr[cspot]) break;
	    }
	    if (cspot < result) result = cspot;
	}
    }
    return result;
}

static char **get_comp()
/*
 * Returns current completion array.  Adds a null termination at the end.
 */
{
    add_comp((char *) 0);
    return comp_ary;
}

char **fc_complete(buf, start, span)
char *buf;			/* String to complete     */
int *start;			/* Index into completions */
int *span;			/* Number of common chars */
/*
 * Attempts to interpret `buf' as a filename.  All possible
 * completions are returned in a null terminated array of
 * file names.  The file names do not include the prefix
 * directory.  If `start' and `span' are non-zero they
 * are filled with an index and a length of common characters
 * among completions beyond the end of `buf' (this is used
 * for adding significant characters in interactive interfaces).
 * File names with tilde expansions are allowed.  The returned 
 * array is locally owned and should not be freed.  The strings 
 * returned in the array will be freed the next time fc_complete 
 * is called.
 */
{
    DIR *wdir;
    char *lcomp, *dir;
#if defined(aiws) || defined(_IBMR2) || defined(SYSV)
    struct dirent *de;
#else
    struct direct *de;
#endif /* aiws */
    struct passwd *pw;
    int spot, lspn;
    int i;

    reset_comp();
    ERR_IGNORE( buf = util_logical_expand(buf) );
    buf = copy( buf );		/* Don't mangle anything... */

    /* Allow expansion for cell:view:facet forms */
    i = 0;
    while ( buf[i] != '\0' ) {
	if ( buf[i] == ':' ) buf[i] = '/';
	i++;
    }

    lcomp = strrchr(buf, '/');
    if (lcomp == NULL) {
	dir = ".";
	lcomp = buf;
	if (*buf == FC_TIL) {	/* User name completion... */
	    char uname[MAXUNAME];

	    spot = strlen(buf);
	    buf++;
	    (void) setpwent();
	    while ( (pw = getpwent()) ) {
		if (prefix(buf, pw->pw_name)) {
		    uname[0] = FC_TIL;
		    uname[1] = '\0';
		    (void) strcat(uname, pw->pw_name);
		    add_comp(uname);
		}
	    }
	    (void) endpwent();
	    sort_comp();
	    lspn = common_comp() - spot;
	    if (start) *start = spot;
	    if (span) *span = lspn;
	    return get_comp();
	}
    } else {
	dir = buf;
	*lcomp = '\0';
	lcomp++;
	if (*dir == '\0') {
	    /* Slash relative */
	    dir = "/";
	} else if (*dir == FC_TIL) {
	    dir = util_tilde_expand(dir);
	    if (dir == NULL) return (char **) 0;
	}
    }
    if (!(wdir = opendir(dir))) return (char **) 0;
    spot = strlen(lcomp);
    while ( (de = readdir(wdir)) ) {
	if ((prefix(lcomp, de->d_name)) &&
	    (*lcomp || (*de->d_name != '.'))) {
	    add_comp(de->d_name);
	}
    }
    (void) closedir(wdir);
    sort_comp();
    lspn = common_comp() - spot;
    free(buf);
    if (start) *start = spot;
    if (span) *span = lspn;
    return get_comp();
}

