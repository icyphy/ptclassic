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
 * Test program for string(3) routines.
 * 
 * Note that at least one Bell Labs implementation of the string
 * routines flunks a couple of these tests -- the ones which test
 * behavior on "negative" characters.
 */

#include "port.h"

#define	STREQ(a, b)	(strcmp((a), (b)) == 0)

char *it = "<UNSET>";		/* Routine name for message routines. */
int waserror = 0;		/* For exit status. */

char uctest[] = "\004\203";	/* For testing signedness of chars. */
int charsigned;			/* Result. */

/*
 - check - complain if condition is not true
 */
void
check(thing, number)
int thing;
int number;			/* Test number for error message. */
{
	if (!thing) {
		(void) fprintf(stderr, "%s flunked test %d\n", it, number);
		waserror = 1;
	}
}

/*
 - equal - complain if first two args don't strcmp as equal
 */
void
equal(a, b, number)
char *a;
char *b;
int number;			/* Test number for error message. */
{
	check(a != NULL && b != NULL && STREQ(a, b), number);
}

char one[50];
char two[50];

#ifdef UNIXERR
#define ERR 1
#endif
#ifdef BERKERR
#define ERR 1
#endif
#ifdef ERR
int f;
extern char *sys_errlist[];
extern int sys_nerr;
extern int errno;
#endif

void first();
void second();

/* ARGSUSED */
int
main(argc, argv)
int argc;
char *argv[];
{
	(void) fprintf(stderr, "*** EXPECT A FEW OF THE PORT TESTS TO FAIL, DO NOT BE ALARMED ***\n");
	/*
	 * First, establish whether chars are signed.
	 */
	if (uctest[0] < uctest[1])
		charsigned = 0;
	else
		charsigned = 1;

	/*
	 * Then, do the rest of the work.  Split into two functions because
	 * some compilers get unhappy about a single immense function.
	 */
	first();
	second();

	return ((waserror) ? 1 : 0);
}

void
first()
{
	/*
	 * Test strcmp first because we use it to test other things.
	 */
	it = "strcmp";
	check(strcmp("", "") == 0, 1);		/* Trivial case. */
	check(strcmp("a", "a") == 0, 2);	/* Identity. */
	check(strcmp("abc", "abc") == 0, 3);	/* Multicharacter. */
	check(strcmp("abc", "abcd") < 0, 4);	/* Length mismatches. */
	check(strcmp("abcd", "abc") > 0, 5);
	check(strcmp("abcd", "abce") < 0, 6);	/* Honest miscompares. */
	check(strcmp("abce", "abcd") > 0, 7);
	check(strcmp("a\203", "a") > 0, 8);	/* Tricky if char signed. */
	if (charsigned)				/* Sign-bit comparison. */
		check(strcmp("a\203", "a\003") < 0, 9);
	else
		check(strcmp("a\203", "a\003") > 0, 9);

	/*
	 * Test strcpy next because we need it to set up other tests.
	 */
	it = "strcpy";
	check(strcpy(one, "abcd") == one, 1);	/* Returned value. */
	equal(one, "abcd", 2);			/* Basic test. */

	(void) strcpy(one, "x");
	equal(one, "x", 3);			/* Writeover. */
	equal(one+2, "cd", 4);			/* Wrote too much? */

	(void) strcpy(two, "hi there");
	(void) strcpy(one, two);
	equal(one, "hi there", 5);		/* Basic test encore. */
	equal(two, "hi there", 6);		/* Stomped on source? */

	(void) strcpy(one, "");
	equal(one, "", 7);			/* Boundary condition. */

	/*
	 * strcat
	 */
	it = "strcat";
	(void) strcpy(one, "ijk");
	check(strcat(one, "lmn") == one, 1);	/* Returned value. */
	equal(one, "ijklmn", 2);		/* Basic test. */

	(void) strcpy(one, "x");
	(void) strcat(one, "yz");
	equal(one, "xyz", 3);			/* Writeover. */
	equal(one+4, "mn", 4);			/* Wrote too much? */

	(void) strcpy(one, "gh");
	(void) strcpy(two, "ef");
	(void) strcat(one, two);
	equal(one, "ghef", 5);			/* Basic test encore. */
	equal(two, "ef", 6);			/* Stomped on source? */

	(void) strcpy(one, "");
	(void) strcat(one, "");
	equal(one, "", 7);			/* Boundary conditions. */
	(void) strcpy(one, "ab");
	(void) strcat(one, "");
	equal(one, "ab", 8);
	(void) strcpy(one, "");
	(void) strcat(one, "cd");
	equal(one, "cd", 9);

	/*
	 * strncat - first test it as strcat, with big counts, then
	 * test the count mechanism.
	 */
	it = "strncat";
	(void) strcpy(one, "ijk");
	check(strncat(one, "lmn", 99) == one, 1);	/* Returned value. */
	equal(one, "ijklmn", 2);		/* Basic test. */

	(void) strcpy(one, "x");
	(void) strncat(one, "yz", 99);
	equal(one, "xyz", 3);			/* Writeover. */
	equal(one+4, "mn", 4);			/* Wrote too much? */

	(void) strcpy(one, "gh");
	(void) strcpy(two, "ef");
	(void) strncat(one, two, 99);
	equal(one, "ghef", 5);			/* Basic test encore. */
	equal(two, "ef", 6);			/* Stomped on source? */

	(void) strcpy(one, "");
	(void) strncat(one, "", 99);
	equal(one, "", 7);			/* Boundary conditions. */
	(void) strcpy(one, "ab");
	(void) strncat(one, "", 99);
	equal(one, "ab", 8);
	(void) strcpy(one, "");
	(void) strncat(one, "cd", 99);
	equal(one, "cd", 9);

	(void) strcpy(one, "ab");
	(void) strncat(one, "cdef", 2);
	equal(one, "abcd", 10);			/* Count-limited. */

	(void) strncat(one, "gh", 0);
	equal(one, "abcd", 11);			/* Zero count. */

	(void) strncat(one, "gh", 2);
	equal(one, "abcdgh", 12);		/* Count and length equal. */

	/*
	 * strncmp - first test as strcmp with big counts, then test
	 * count code.
	 */
	it = "strncmp";
	check(strncmp("", "", 99) == 0, 1);	/* Trivial case. */
	check(strncmp("a", "a", 99) == 0, 2);	/* Identity. */
	check(strncmp("abc", "abc", 99) == 0, 3);	/* Multicharacter. */
	check(strncmp("abc", "abcd", 99) < 0, 4);	/* Length unequal. */
	check(strncmp("abcd", "abc", 99) > 0, 5);
	check(strncmp("abcd", "abce", 99) < 0, 6);	/* Honestly unequal. */
	check(strncmp("abce", "abcd", 99) > 0, 7);
	check(strncmp("a\203", "a", 2) > 0, 8);	/* Tricky if '\203' < 0 */
	if (charsigned)				/* Sign-bit comparison. */
		check(strncmp("a\203", "a\003", 2) < 0, 9);
	else
		check(strncmp("a\203", "a\003", 2) > 0, 9);
	check(strncmp("abce", "abcd", 3) == 0, 10);	/* Count limited. */
	check(strncmp("abce", "abc", 3) == 0, 11);	/* Count == length. */
	check(strncmp("abcd", "abce", 4) < 0, 12);	/* Nudging limit. */
	check(strncmp("abc", "def", 0) == 0, 13);	/* Zero count. */

	/*
	 * strncpy - testing is a bit different because of odd semantics
	 */
	it = "strncpy";
	check(strncpy(one, "abc", 4) == one, 1);	/* Returned value. */
	equal(one, "abc", 2);			/* Did the copy go right? */

	(void) strcpy(one, "abcdefgh");
	(void) strncpy(one, "xyz", 2);
	equal(one, "xycdefgh", 3);		/* Copy cut by count. */

	(void) strcpy(one, "abcdefgh");
	(void) strncpy(one, "xyz", 3);		/* Copy cut just before NUL. */
	equal(one, "xyzdefgh", 4);

	(void) strcpy(one, "abcdefgh");
	(void) strncpy(one, "xyz", 4);		/* Copy just includes NUL. */
	equal(one, "xyz", 5);
	equal(one+4, "efgh", 6);		/* Wrote too much? */

	(void) strcpy(one, "abcdefgh");
	(void) strncpy(one, "xyz", 5);		/* Copy includes padding. */
	equal(one, "xyz", 7);
	equal(one+4, "", 8);
	equal(one+5, "fgh", 9);

	(void) strcpy(one, "abc");
	(void) strncpy(one, "xyz", 0);		/* Zero-length copy. */
	equal(one, "abc", 10);	

	(void) strncpy(one, "", 2);		/* Zero-length source. */
	equal(one, "", 11);
	equal(one+1, "", 12);	
	equal(one+2, "c", 13);

	(void) strcpy(one, "hi there");
	(void) strncpy(two, one, 9);
	equal(two, "hi there", 14);		/* Just paranoia. */
	equal(one, "hi there", 15);		/* Stomped on source? */

	/*
	 * strlen
	 */
	it = "strlen";
	check(strlen("") == 0, 1);		/* Empty. */
	check(strlen("a") == 1, 2);		/* Single char. */
	check(strlen("abcd") == 4, 3);		/* Multiple chars. */

	/*
	 * strchr
	 */
	it = "strchr";
	check(strchr("abcd", 'z') == NULL, 1);	/* Not found. */
	(void) strcpy(one, "abcd");
	check(strchr(one, 'c') == one+2, 2);	/* Basic test. */
	check(strchr(one, 'd') == one+3, 3);	/* End of string. */
	check(strchr(one, 'a') == one, 4);	/* Beginning. */
	check(strchr(one, '\0') == one+4, 5);	/* Finding NUL. */
	(void) strcpy(one, "ababa");
	check(strchr(one, 'b') == one+1, 6);	/* Finding first. */
	(void) strcpy(one, "");
	check(strchr(one, 'b') == NULL, 7);	/* Empty string. */
	check(strchr(one, '\0') == one, 8);	/* NUL in empty string. */

	/*
	 * strrchr
	 */
	it = "strrchr";
	check(strrchr("abcd", 'z') == NULL, 1);	/* Not found. */
	(void) strcpy(one, "abcd");
	check(strrchr(one, 'c') == one+2, 2);	/* Basic test. */
	check(strrchr(one, 'd') == one+3, 3);	/* End of string. */
	check(strrchr(one, 'a') == one, 4);	/* Beginning. */
	check(strrchr(one, '\0') == one+4, 5);	/* Finding NUL. */
	(void) strcpy(one, "ababa");
	check(strrchr(one, 'b') == one+3, 6);	/* Finding last. */
	(void) strcpy(one, "");
	check(strrchr(one, 'b') == NULL, 7);	/* Empty string. */
	check(strrchr(one, '\0') == one, 8);	/* NUL in empty string. */
}

void
second()
{
	/*
	 * strpbrk - somewhat like strchr
	 */
	it = "strpbrk";
	check(strpbrk("abcd", "z") == NULL, 1);	/* Not found. */
	(void) strcpy(one, "abcd");
	check(strpbrk(one, "c") == one+2, 2);	/* Basic test. */
	check(strpbrk(one, "d") == one+3, 3);	/* End of string. */
	check(strpbrk(one, "a") == one, 4);	/* Beginning. */
	check(strpbrk(one, "") == NULL, 5);	/* Empty search list. */
	check(strpbrk(one, "cb") == one+1, 6);	/* Multiple search. */
	(void) strcpy(one, "abcabdea");
	check(strpbrk(one, "b") == one+1, 7);	/* Finding first. */
	check(strpbrk(one, "cb") == one+1, 8);	/* With multiple search. */
	check(strpbrk(one, "db") == one+1, 9);	/* Another variant. */
	(void) strcpy(one, "");
	check(strpbrk(one, "bc") == NULL, 10);	/* Empty string. */
	check(strpbrk(one, "") == NULL, 11);	/* Both strings empty. */
#ifdef notdef
	/*
	 * strstr - somewhat like strchr
	 */
	it = "strstr";
	check(strstr("abcd", "z") == NULL, 1);	/* Not found. */
	check(strstr("abcd", "abx") == NULL, 2);	/* Dead end. */
	(void) strcpy(one, "abcd");
	check(strstr(one, "c") == one+2, 3);	/* Basic test. */
	check(strstr(one, "bc") == one+1, 4);	/* Multichar. */
	check(strstr(one, "d") == one+3, 5);	/* End of string. */
	check(strstr(one, "cd") == one+2, 6);	/* Tail of string. */
	check(strstr(one, "abc") == one, 7);	/* Beginning. */
	check(strstr(one, "abcd") == one, 8);	/* Exact match. */
	check(strstr(one, "abcde") == NULL, 9);	/* Too long. */
	check(strstr(one, "de") == NULL, 10);	/* Past end. */
	check(strstr(one, "") == one+4, 11);	/* Finding empty. */
	(void) strcpy(one, "ababa");
	check(strstr(one, "ba") == one+1, 12);	/* Finding first. */
	(void) strcpy(one, "");
	check(strstr(one, "b") == NULL, 13);	/* Empty string. */
	check(strstr(one, "") == one, 14);	/* Empty in empty string. */
	(void) strcpy(one, "bcbca");
	check(strstr(one, "bca") == one+2, 15);	/* False start. */
	(void) strcpy(one, "bbbcabbca");
	check(strstr(one, "bbca") == one+1, 16);	/* With overlap. */
#endif /* notdef */

	/*
	 * strspn
	 */
	it = "strspn";
	check(strspn("abcba", "abc") == 5, 1);	/* Whole string. */
	check(strspn("abcba", "ab") == 2, 2);	/* Partial. */
	check(strspn("abc", "qx") == 0, 3);	/* None. */
	check(strspn("", "ab") == 0, 4);	/* Null string. */
	check(strspn("abc", "") == 0, 5);	/* Null search list. */

	/*
	 * strcspn
	 */
	it = "strcspn";
	check(strcspn("abcba", "qx") == 5, 1);	/* Whole string. */
	check(strcspn("abcba", "cx") == 2, 2);	/* Partial. */
	check(strcspn("abc", "abc") == 0, 3);	/* None. */
	check(strcspn("", "ab") == 0, 4);	/* Null string. */
	check(strcspn("abc", "") == 3, 5);	/* Null search list. */

	/*
	 * strtok - the hard one
	 */
	it = "strtok";
	(void) strcpy(one, "first, second, third");
	equal(strtok(one, ", "), "first", 1);	/* Basic test. */
	equal(one, "first", 2);
	equal(strtok((char *)NULL, ", "), "second", 3);
	equal(strtok((char *)NULL, ", "), "third", 4);
	check(strtok((char *)NULL, ", ") == NULL, 5);
	(void) strcpy(one, ", first, ");
	equal(strtok(one, ", "), "first", 6);	/* Extra delims, 1 tok. */
	check(strtok((char *)NULL, ", ") == NULL, 7);
	(void) strcpy(one, "1a, 1b; 2a, 2b");
	equal(strtok(one, ", "), "1a", 8);	/* Changing delim lists. */
	equal(strtok((char *)NULL, "; "), "1b", 9);
	equal(strtok((char *)NULL, ", "), "2a", 10);
	(void) strcpy(two, "x-y");
	equal(strtok(two, "-"), "x", 11);	/* New string before done. */
	equal(strtok((char *)NULL, "-"), "y", 12);
	check(strtok((char *)NULL, "-") == NULL, 13);
	(void) strcpy(one, "a,b, c,, ,d");
	equal(strtok(one, ", "), "a", 14);	/* Different separators. */
	equal(strtok((char *)NULL, ", "), "b", 15);
	equal(strtok((char *)NULL, " ,"), "c", 16);	/* Permute list too. */
	equal(strtok((char *)NULL, " ,"), "d", 17);
	check(strtok((char *)NULL, ", ") == NULL, 18);
	check(strtok((char *)NULL, ", ") == NULL, 19);	/* Persistence. */
	(void) strcpy(one, ", ");
	check(strtok(one, ", ") == NULL, 20);	/* No tokens. */
	(void) strcpy(one, "");
	check(strtok(one, ", ") == NULL, 21);	/* Empty string. */
	(void) strcpy(one, "abc");
	equal(strtok(one, ", "), "abc", 22);	/* No delimiters. */
	check(strtok((char *)NULL, ", ") == NULL, 23);
	(void) strcpy(one, "abc");
	equal(strtok(one, ""), "abc", 24);	/* Empty delimiter list. */
	check(strtok((char *)NULL, "") == NULL, 25);
	(void) strcpy(one, "abcdefgh");
	(void) strcpy(one, "a,b,c");
	equal(strtok(one, ","), "a", 26);	/* Basics again... */
	equal(strtok((char *)NULL, ","), "b", 27);
	equal(strtok((char *)NULL, ","), "c", 28);
	check(strtok((char *)NULL, ",") == NULL, 29);
	equal(one+6, "gh", 30);			/* Stomped past end? */
	equal(one, "a", 31);			/* Stomped old tokens? */
	equal(one+2, "b", 32);
	equal(one+4, "c", 33);

	/*
	 * memcmp
	 */
	it = "memcmp";
	check(memcmp("a", "a", 1) == 0, 1);	/* Identity. */
	check(memcmp("abc", "abc", 3) == 0, 2);	/* Multicharacter. */
	check(memcmp("abcd", "abce", 4) < 0, 3);	/* Honestly unequal. */
	check(memcmp("abce", "abcd", 4) > 0, 4);
	check(memcmp("alph", "beta", 4) < 0, 5);
	if (charsigned)				/* Sign-bit comparison. */
		check(memcmp("a\203", "a\003", 2) < 0, 6);
	else
		check(memcmp("a\203", "a\003", 2) > 0, 6);
	check(memcmp("abce", "abcd", 3) == 0, 7);	/* Count limited. */
	check(memcmp("abc", "def", 0) == 0, 8);	/* Zero count. */

	/*
	 * memchr
	 */
	it = "memchr";
	check(memchr("abcd", 'z', 4) == NULL, 1);	/* Not found. */
	(void) strcpy(one, "abcd");
	check(memchr(one, 'c', 4) == one+2, 2);	/* Basic test. */
	check(memchr(one, 'd', 4) == one+3, 3);	/* End of string. */
	check(memchr(one, 'a', 4) == one, 4);	/* Beginning. */
	check(memchr(one, '\0', 5) == one+4, 5);	/* Finding NUL. */
	(void) strcpy(one, "ababa");
	check(memchr(one, 'b', 5) == one+1, 6);	/* Finding first. */
	check(memchr(one, 'b', 0) == NULL, 7);	/* Zero count. */
	check(memchr(one, 'a', 1) == one, 8);	/* Singleton case. */
	(void) strcpy(one, "a\203b");
	check(memchr(one, 0203, 3) == one+1, 9);	/* Unsignedness. */

	/*
	 * memcpy
	 *
	 * Note that X3J11 says memcpy must work regardless of overlap.
	 * The SVID says it might fail.
	 */
	it = "memcpy";
	check(memcpy(one, "abc", 4) == one, 1);	/* Returned value. */
	equal(one, "abc", 2);			/* Did the copy go right? */

	(void) strcpy(one, "abcdefgh");
	(void) memcpy(one+1, "xyz", 2);
	equal(one, "axydefgh", 3);		/* Basic test. */

	(void) strcpy(one, "abc");
	(void) memcpy(one, "xyz", 0);
	equal(one, "abc", 4);			/* Zero-length copy. */

	(void) strcpy(one, "hi there");
	(void) strcpy(two, "foo");
	(void) memcpy(two, one, 9);
	equal(two, "hi there", 5);		/* Just paranoia. */
	equal(one, "hi there", 6);		/* Stomped on source? */

	(void) strcpy(one, "abcdefgh");
	(void) memcpy(one+1, one, 9);
	equal(one, "aabcdefgh", 7);		/* Overlap, right-to-left. */

	(void) strcpy(one, "abcdefgh");
	(void) memcpy(one+1, one+2, 7);
	equal(one, "acdefgh", 8);		/* Overlap, left-to-right. */

	(void) strcpy(one, "abcdefgh");
	(void) memcpy(one, one, 9);
	equal(one, "abcdefgh", 9);		/* 100% overlap. */

	/*
	 * memccpy - first test like memcpy, then the search part
	 *
	 * The SVID, the only place where memccpy is mentioned, says
	 * overlap might fail, so we don't try it.  Besides, it's hard
	 * to see the rationale for a non-left-to-right memccpy.
	 */
	it = "memccpy";
	check(memccpy(one, "abc", 'q', 4) == NULL, 1);	/* Returned value. */
	equal(one, "abc", 2);			/* Did the copy go right? */

	(void) strcpy(one, "abcdefgh");
	(void) memccpy(one+1, "xyz", 'q', 2);
	equal(one, "axydefgh", 3);		/* Basic test. */

	(void) strcpy(one, "abc");
	(void) memccpy(one, "xyz", 'q', 0);
	equal(one, "abc", 4);			/* Zero-length copy. */

	(void) strcpy(one, "hi there");
	(void) strcpy(two, "foo");
	(void) memccpy(two, one, 'q', 9);
	equal(two, "hi there", 5);		/* Just paranoia. */
	equal(one, "hi there", 6);		/* Stomped on source? */

	(void) strcpy(one, "abcdefgh");
	(void) strcpy(two, "horsefeathers");
	check(memccpy(two, one, 'f', 9) == two+6, 7);	/* Returned value. */
	equal(one, "abcdefgh", 8);		/* Source intact? */
	equal(two, "abcdefeathers", 9);		/* Copy correct? */

	(void) strcpy(one, "abcd");
	(void) strcpy(two, "bumblebee");
	check(memccpy(two, one, 'a', 4) == two+1, 10);	/* First char. */
	equal(two, "aumblebee", 11);
	check(memccpy(two, one, 'd', 4) == two+4, 12);	/* Last char. */
	equal(two, "abcdlebee", 13);
	(void) strcpy(one, "xyz");
	check(memccpy(two, one, 'x', 1) == two+1, 14);	/* Singleton. */
	equal(two, "xbcdlebee", 15);

	/*
	 * memset
	 */
	it = "memset";
	(void) strcpy(one, "abcdefgh");
	check(memset(one+1, 'x', 3) == one+1, 1);	/* Return value. */
	equal(one, "axxxefgh", 2);		/* Basic test. */

	(void) memset(one+2, 'y', 0);
	equal(one, "axxxefgh", 3);		/* Zero-length set. */

	(void) memset(one+5, 0, 1);
	equal(one, "axxxe", 4);			/* Zero fill. */
	equal(one+6, "gh", 5);			/* And the leftover. */

	(void) memset(one+2, 010045, 1);
	equal(one, "ax\045xe", 6);		/* Unsigned char convert. */

#ifdef ERR
	/*
	 * strerror - VERY system-dependent
	 */
	it = "strerror";
	f = open("/", 1);	/* Should always fail. */
	check(f < 0 && errno > 0 && errno < sys_nerr, 1);
	equal(strerror(errno), sys_errlist[errno], 2);
#ifdef UNIXERR
	equal(strerror(errno), "Is a directory", 3);
#endif
#ifdef BERKERR
	equal(strerror(errno), "Permission denied", 3);
#endif
#endif
}
