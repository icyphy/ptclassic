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
 * String manipulation macros
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * Unfortunately for C,  it has no true string type.  By string type,
 * I mean a type where the zero of the type exists.  So,  for the
 * near term,  I will patch it using the following functions.
 */

#include "port.h"
#include "str.h"

char *strnolen = "";

int str_compare(s1, s2)
register char *s1, *s2;
/*
 * Returns 0 if the strings are the same,  -1 if s1 is lexically less
 * than s2, and 1 if s1 is lexically greater than s2.  The zero string
 * (STRZERO) is considered the same as the string of no length STRNOLEN.
 */
{
    if (s1 && s2) return strcmp(s1, s2);
    else if (!s1) return (s2 ? strcmp(STRNOLEN, s2) : 0);
    else return strcmp(s1, STRNOLEN);
}


int str_ncompare(s1, s2, n)
register char *s1, *s2;
register int n;
/*
 * Returns 0 if the strings are the same,  -1 if s1 is lexically less
 * than s2, and 1 if s1 is lexically greater than s2.  The zero string
 * (STRZERO) is considered the same as the string of no length STRNOLEN.
 */
{
    if (s1 && s2) return strncmp(s1, s2, n);
    else if (!s1) return (s2 ? strncmp(STRNOLEN, s2, n) : 0);
    else return strncmp(s1, STRNOLEN, n);
}




/* Note: the old name stricmp() is not portable (it exists on the sun3 /lib/libc.a) */

int str_icmp(a, b)
register char *a, *b;
/*
 * This routine compares two strings disregarding case.
 */
{
    register int value = 0;

    if ((a == (char *) 0) || (b == (char *) 0)) {
	return a - b;
    }

    for ( /* nothing */;
	 ((*a | *b) &&
	  !(value = ((isupper(*a) ? *a - 'A' + 'a' : *a) -
		     (isupper(*b) ? *b - 'A' + 'a' : *b))));
	 a++, b++)
      /* Empty Body */;

    return value;
}



char *strlcase(a)
char *a;
/*
 * Makes all upper case letters lower case.  This is done
 * in place.
 */
{
    register char *i;

    if (a == (char *) 0) return a;

    for (i = a;  (*i != '\0');  i++) {
	if (isupper(*i)) {
	    *i = tolower(*i);
	}
    }
    return a;
}


char *strucase(a)
char *a;
/*
 * Makes all upper case letters lower case.  This is done
 * in place.
 */
{
    register char *i;

    if (a == (char *) 0) return a;

    for (i = a;  (*i != '\0');  i++) {
	if (islower(*i)) {
	    *i = toupper(*i);
	}
    }
    return a;
}
