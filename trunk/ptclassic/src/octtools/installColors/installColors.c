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
    root_dir = prompt("Directory in which ptolemy technology is installed",
		"~ptolemy/lib/colors");
    out_tech = "ptolemy";
    out_view = tech_spec;
    if (disp_name = getenv("DISPLAY")) {
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



int stricmp(a, b)
register char *a, *b;
/*
 * This routine compares two strings disregarding case.
 */
{
    register int value;

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
