/*
 * String manipulation macros
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * Unfortunately for C,  it has no true string type.  By string type,
 * I mean a type where the zero of the type exists.  So,  for the
 * near term,  I will patch it using the following macros.
 */

#ifndef	STRDEFNS
#define STRDEFNS

#include "port.h"

extern char *strlcase();
extern char *strucase();

typedef char *STR;

/*
 * STRZERO
 *
 * This represents the zero of the string type.
 */
#define STRZERO		((STR) 0)

/*
 * STRZEROLEN
 *
 * This macro points to one external variable where a single string
 * constant consisting of a pointer to one character which is zero.
 * This must be defined in the program somewhere.
 */
extern STR strnolen;

#define STRNOLEN	strnolen

/*
 * int STRLEN(s)
 *
 * Returns the length of s in characters.  Returns zero if s is STRZERO.
 */
#define STRLEN(s) \
  ((s) ? strlen(s) : 0)


/*
 * int STRCOMP(STR s1, STR s2)
 *
 * Compares the strings s1 and s2.  Returns 0 if equal,  -1 if
 * s1 is lexically less than s2,  and 1 if s1 is lexically greater
 * than s2.  See the function str_compare in str.c for how the
 * zero string is handled.
 */
extern int str_compare();

#define STRCOMP	str_compare


/*
 * int STRNCOMP(STR s1, STR s2, int n)
 *
 * Compares the first n characters of strings s1 and s2.  
 * Returns 0 if equal,  -1 if s1 is lexically less than s2,  
 * and 1 if s1 is lexically greater than s2.  See the function 
 * str_ncompare in str.c for how the zero string is handled.
 */
extern int str_ncompare();

#define STRNCOMP	str_ncompare

/*
 * int STRCLCOMP(a, b)
 *
 * Compares the two strings disregarding case.  Returns the same
 * values as STRCOMP and STRNCOMP.
 */
extern int stricmp();

#define STRCLCOMP	stricmp

/*
 * STR STRMOVE(STR s1, STR s2)
 *
 * Copies contents of s2 into s1.  s1 must be large enough to contain s2.
 * The current contents of s1 are lost.  If s2 is zero, s1 will be made
 * a zero length string.  Returns s1.
 */
#define STRMOVE(s1, s2) \
  ((s2) ? strcpy((s1), (s2)) : strcpy((s1), STRNOLEN))

/*
 * STR STRNMOVE(STR s1, STR s2, int n)
 *
 * Like STRMOVE except only the first n characters of s2 are copied into
 * s1.  s2 must be at least length n.  If s2 is zero,  no move will be
 * done.  Returns s1.
 */
#define STRNMOVE(s1, s2, n) \
  ((s2) ? strncpy((s1), (s2), n) : s1)

/*
 * STR STRCONCAT(STR s1, STR s2)
 *
 * Copies contents of s2 onto the end of  s1.  s1 is assumed large enough to
 * contain the additional characters.  Nothing is done if s2 is zero.
 * Returns s1.
 */
#define STRCONCAT(s1, s2) \
  ((s2) ? strcat((s1), (s2)) : s1)

/*
 * STR STRNCONCAT(STR s1, STR s2, int n)
 *
 * Copies the first n characters of s2 into s1.  s1 is not automatically
 * null terminated.  If s2 is zero,  no copying will be done.  Returns
 * s1.
 */
#define STRNCONCAT(s1, s2, n) \
  ((s2) ? strncat((s1), (s2), n) : s1)


/*
 * STR STRFIND(STR s, char c)
 *
 * Finds the first occurance of c in s and returns a pointer to that
 * character.  Returns STRZERO if it does not find the character or
 * if s is STRZERO.
 */
#define STRFIND(s, c) \
  ((s) ? strchr((s), c) : STRZERO)

/*
 * STR STRBFIND(STR s, char c)
 *
 * Finds the last occurance of c in s and returns a pointer to that
 * character.  Returns STRZERO if it does not find the character to
 * if s is STRZERO.
 */
#define STRBFIND(s, c) \
  ((s) ? strrchr(s, c) : STRZERO)

/*
 * STR STRDOWNCASE(STR s)
 *
 * Lower cases all characters in s in place and returns s.
 */
#define STRDOWNCASE(s) \
  ((s) ? strlcase(s) : STRZERO)


/*
 * STR STRUPCASE(STR s)
 *
 * Upper cases all characters in s in place and returns s.
 */
#define STRUPCASE(s) \
  ((s) ? strucase(s) : STRZERO)

#endif
