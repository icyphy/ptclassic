/* $Id$ */

/* Copyright (C) 1989 Regents of the University of California
 * All rights reserved.
 */
/* obnoxious stringswitch macros. If you don't like the sight of this,
 *                                remember that you are happier than I am,
 *				  since I wrote this
 */
#define STRINGSWITCH(A) {char *AbCdEfGh = A; 
#define FIRSTCASE(B)   if (strcmp(AbCdEfGh, B) == 0)
#define CASE(B)   else if (strcmp(AbCdEfGh, B) == 0)
#define TWOCASE(B,C) if ((strcmp(AbCdEfGh, B) == 0) ||\
			 (strcmp(AbCdEfGh, C) == 0))
#define THREECASE(B,C,D) if ((strcmp(AbCdEfGh, B) == 0) ||\
			     (strcmp(AbCdEfGh, C) == 0) ||\
			     (strcmp(AbCdEfGh, D) == 0))
#define ENDSWITCH }

