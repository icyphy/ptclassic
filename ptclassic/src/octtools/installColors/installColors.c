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
/*
 * This program translates a textual VEM patterns file into 
 * a tap view.  It has been modified for use with Ptolemy,
 * and renamed "installColors" from pat2tap.
 *
 * David Harrison
 * University of California,  1987
 *
 * Modified by:
 * E. A. Lee
 * University of California,  1992
 */

#include "copyright.h"
#include "port.h"
#include <X11/Xlib.h>
#include "oct.h"
#include "tap.h"
#include "cleantap.h"
#include "process.h"

char *strnolen = "";

char errBuf[4096];
char *errMsgArea = errBuf;

#define strsave(s)	(strcpy(malloc((unsigned) (strlen(s)+1)), s))

#define INPUT_LEN	1024

#define PRINTF	(void) printf

char *prompt(pmt, def)
char *pmt;			/* User prompt   */
char *def;			/* Default value */
/* 
 * Returns a string read from standard input using the prompt `pmt'.
 * If no value is given,  `def' is returned.
 */
{
    static char pmt_buf[INPUT_LEN];

    if (def && (strlen(def) > 0)) {
	PRINTF("%s [%s]: ", pmt, def); /* fflush(stdout); */
	(void) gets(pmt_buf);
	if (strlen(pmt_buf) > 0) return strsave(pmt_buf);
	else if (def) return strsave(def);
	else return NULL;
    } else {
	PRINTF("%s: ", pmt);  /* fflush(stdout); */
	(void) gets(pmt_buf);
	return strsave(pmt_buf);
    }
}



int
main(argc, argv)
int argc;
char *argv[];
{
    char *pat_file;
    char *tech_spec;
    char *root_dir;
    char *out_tech;
    char *out_view;
    char *out_disp;
    char *disp_inp;
    char *disp_name;
    char *chr;
    char *yes_no;
    int chroma;
    Display *display;

    if (argc > 1) {
	PRINTF("%s is an interactive program.  You will be prompted for input.\n", argv[0]);
    }
    
    pat_file = prompt("Name of the color pattern file", "colors.pat");
    tech_spec = "schematic";
    if(getenv("PTOLEMY"))
        root_dir = prompt("Directory in which ptolemy technology is installed",
		"$PTOLEMY/lib/colors");
    else
        root_dir = prompt("Directory in which ptolemy technology is installed",
		"~ptolemy/lib/colors");
    out_tech = "ptolemy";
    out_view = tech_spec;
    if ( (disp_name = getenv("DISPLAY")) ) {
        yes_no = prompt("Do you wish to specify an X host and display other\n than the one in your DISPLAY variable?", "no");
        if (strcmp(yes_no, "no") != 0) {
            disp_inp = prompt("X host and display", disp_name);
            display = XOpenDisplay(disp_inp);
        } else {
            display = XOpenDisplay(disp_name);
        }
        out_disp = prompt("Output display type", ServerVendor(display));
	chr = prompt("Color output device",
              ((DisplayCells(display, DefaultScreen(display)) > 15) ? "yes" : "no"));
    } else {
	out_disp = prompt("Output display type", "GENERIC-COLOR");
	chr = prompt("Color output device", "yes");
    }
    if ((chr[0] == 'y') || (chr[0] == 'Y')) {
	chroma = TAP_COLOR;
    } else {
	chroma = TAP_BW;
    }
    process(pat_file, tech_spec, root_dir, out_tech, out_view, out_disp, chroma);
    cleantap("tap.views");
    return 1;
}



/*ARGSUSED*/
int vemMessage(str, directive)
char *str;
int directive;
/*
 * Quick replacement for vemMessage
 */
{
    PRINTF(str);
    return 1;
}



/* 
 * This following bit is pretty dumb.  Both OSF and Sun strings libraries
 * provide "strcasecmp()" which does the right thing.  Unfortunately, I
 * don't have access to the SGI or HP environments to do test a more
 * correctly engineered fix (i.e. implement strcasecmp() here for those
 * environments that don't have it, rather than stricmp(), and have the
 * functions in tech.c call strcasecmp()).
 *
 *				kkissell@acri.fr
 */

/* compat.h optionally defines NEED_STRICMP */

#ifdef NEED_STRICMP
int stricmp(a, b)
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
#endif /*NEED_STRICMP*/
