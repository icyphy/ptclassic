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

#include "stdio.h"		/* for __filbuf */
#include "port.h"
#include "cp.h"

#define CP_PKG_NAME "cp"

/*typedef enum {CP_FALSE = 0, CP_TRUE} boolean;*/

typedef int type_type;

#define BLANK 1
#define UPPER_CHAR 2
#define DIGIT 4
#define SPECIAL 8
#define SEP (BLANK | UPPER_CHAR)

#define MAX_INT 077777777

#define IS(type,char) ( (type) & char_type[(int) (char)] )

extern type_type char_type[];

extern char current_char;
extern int current_int;
extern FILE *infile;

#define CP_GET_CHAR ( ((current_int = getc(infile)) == EOF) ?\
			error("Unexpected end of file"),(char) 0 :\
			((current_char = (char) current_int) == '\n' ?\
			cp_line_number++, current_char : current_char))

#define CP_UNGET_CHAR(c, file) {\
    if (c == '\n') cp_line_number--;\
    (void) ungetc(c, file);\
}

void error();

