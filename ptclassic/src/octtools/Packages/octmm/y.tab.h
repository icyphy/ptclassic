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

typedef union  {
    int intval;
    double dblval;
    char *strval;
    geo_t layer;
} YYSTYPE;
extern YYSTYPE yylval;
# define PLUS 257
# define MINUS 258
# define TIMES 259
# define XOR 260
# define NAME 261
# define NUMBER 262
# define REAL 263
# define LPAREN 264
# define RPAREN 265
# define SEMI 266
# define INVALID 267
# define EQUAL 268
# define COMMA 269
# define SLASH 270
# define GROW 271
# define SQUARES 272
# define DELETE 273
# define FRAME 274
# define SCALE 275
# define PROPERTY 276
# define OPTIONS 277
# define LABELS 278
# define BOUNDBOX 279
# define GROWX 280
# define GROWY 281
# define FRAMEX 282
# define FRAMEY 283
# define FLATTEN 284
# define SUBCELL 285
