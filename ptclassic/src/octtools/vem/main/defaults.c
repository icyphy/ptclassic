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
/*LINTLIBRARY*/
/*
 * Default Parameter Package
 *
 * David Harrison
 * University of California, 1985, 1988
 *
 * This package provides a mechanism for storing and retrieving default
 * values for various display options provided by VEM.  Basically,  it
 * is a string based hash table keyed by parameter values of the form:
 *
 *	subname.subname ... .subname
 *
 * For example,  "background.color" is a parameter for determining
 * the background color of newly created graphics windows.  Generally,
 * these values are set when the program first starts up.  Most of these
 * values are read from a .Xdefaults file in the user's home directory.
 * If not specified explicitly by the user or in the .Xdefaults file,
 * it uses the default value supplied when the default is created.
 *
 * There are three purposes for this package:
 *   1. To provide an access function interface to global variables.
 *      This helps locate and document such values.
 *   2. Insure that no global value can be accessed without initializing
 *      it.  Unless the default has been created,  no value can be
 *      extracted from it.
 *   3. To provide a means of specifing the values in text form so
 *      they can be read at runtime and converted to the proper
 *      form in a uniform manner.
 */

#include "defaults.h"		/* Self declaration           */
#include <pwd.h>		/* Passwd file for tilde      */
#include <ctype.h>		/* Character types            */
#include <X11/Xutil.h>		/* X11 utilities              */
#include <X11/Intrinsic.h>	/* X Toolkit definitions      */
#include "st.h"			/* Hash table package         */
#include "message.h"		/* Message display system     */
#include "windows.h"		/* For toWnOption             */
#include "tap.h"		/* For default directory      */
#include "errtrap.h"		/* Error handling             */
#include "vemUtil.h"		/* General utilities          */
#include "xvals.h"		/* X values                   */

#define TILDE		'~'
#define MAXDIRSIZE	256
#define MAXINTENSITY	65535

#define ILLEGAL_PIXEL	-1

char *def_pkg_name = "defaults";

/*
 * For convenience,  the hard coded initial default for background
 * is defined once here (must assume B/W):
 */
#define BG	"white"
#define FG	"black"

static st_table *vemDefTable = NIL(st_table);
/* Default value hash table */

/* Types supported: */

#define	DEF_INTEGER	0
#define DEF_STRING	1
#define DEF_DOUBLE	2
#define DEF_COLOR	3
#define DEF_FONT	4
#define DEF_FILL	5
#define DEF_ATTR	6
#define DEF_FONTLIST	7

#define DEF_INVALID	0x100	/* Invalid bit */

typedef struct def_value_defn {
    STR textValue;		/* Text form of default   	      */
    int defType;		/* Default type (also invalid bit)    */
    union def_val {		/* Various forms of useful value      */
	int defInt;		/* DEF_INTEGER  */
	STR defString;		/* DEF_STRING   */
	double defDbl;		/* DEF_DOUBLE   */
	XColor *defColor;	/* DEF_COLOR    */
	XFontStruct *defFont;	/* DEF_FONT     */
	flPattern defFill;	/* DEF_FILL     */
	atrHandle defAttr;	/* DEF_ATTR     */
	lsList defFontList;	/* DEF_FONTLIST */
    } DV;
    vemStatus (*tranFunc)();	/* Transformation function             */
} def_value;

/*
 * Template for transformation functions can be found where dfNew
 * is defined.
 */



/*
 * Conversion Routines. 
 *
 * The routines for converting a text representation to a useful
 * representation are defined here.
 */

static vemStatus toInt(text, val)
STR text;
def_value *val;
/* Converts a string to an integer */
{
    int temp;

    if (sscanf(text, "%d", &temp) != 1) {
	vemMsg(MSG_A, "%s warning: cannot parse `%s' as an integer\n",
	       def_pkg_name, text);
	return VEM_CORRUPT;
    } else {
	val->DV.defInt = temp;
	return VEM_OK;
    }
}



static vemStatus toDouble(text, val)
STR text;
def_value *val;
/* Converts a string to a floating point number (single precision) */
{
    double temp;

    if (sscanf(text, "%lf", &temp) != 1) {
	/* Corrupt number */
	vemMsg(MSG_A, "%s warning: cannot parse `%s' as a double\n",
	       def_pkg_name, text);
	return VEM_CORRUPT;
    } else {
	val->DV.defDbl = temp;
	return VEM_OK;
    }
}

static vemStatus toString(text, val)
STR text;
def_value *val;
/* The identity transform (always works) */
{
    val->DV.defString = text;
    return VEM_OK;
}




/*
 * Text to X resource conversion routines
 */

static vemStatus toColor(text, val)
STR text;			/* New text value              */
def_value *val;		/* Old useful value (returned) */
/*
 * Text color specification to color structure.  See XParseColor
 * for the format of text color specification.  The display
 * need not be open before the routine runs. Note that unlike
 * previous efforts,  this DOES NOT translate to a pixel
 * value.  The actual extraction routine (one of dfGetPix or
 * defGetCol) decides what should be extracted.
 */
{
    XColor theColor;

    theColor.flags = DoRed | DoGreen | DoBlue;
    if (STRCLCOMP(text, "black") == 0) {
	theColor.red = theColor.green = theColor.blue = 0;
    } else if (STRCLCOMP(text, "white") == 0) {
	theColor.red = theColor.green = theColor.blue = MAXINTENSITY;
    } else {
	if (!XParseColor(xv_disp(), xv_cmap(), text, &theColor)) {
	    vemMsg(MSG_A, "%s warning: cannot parse `%s' as a color.\n",
		   def_pkg_name, text);
	    return VEM_CORRUPT;
	}
    }

    /* Set it's pixel value to illegal */
    theColor.pixel = ILLEGAL_PIXEL;

    /* Now put it in its place */
    if (val->DV.defColor != NIL(XColor)) {
	/* Get rid of the old one */
	VEMFREE(val->DV.defColor);
    }
    val->DV.defColor = VEMALLOC(XColor);
    *(val->DV.defColor) = theColor;
    return VEM_OK;
}


static vemStatus toFont(text, val)
STR text;			/* Font name            */
def_value *val;		/* Old font (modified)  */
/* Gets the specified font (frees the old one) */
{
    XFontStruct *newFont;
    char **font_name_list;
    int font_count;
    
    font_name_list = XListFonts(xv_disp(), text, 1, &font_count);
    if (font_count > 0) {
	do {
	    newFont = XLoadQueryFont(xv_disp(), font_name_list[0]);
	} while (newFont == (XFontStruct *) 0);
	XFreeFontNames(font_name_list);
	if (newFont == (XFontStruct *) 0) {
	    vemMsg(MSG_A, "%s warning: cannot load `%s' as a font\n",
		   def_pkg_name, text);
	    return VEM_CORRUPT;
	}
	/* Free the old one */
	if (val->DV.defFont != 0) {
	    XFreeFont(xv_disp(), val->DV.defFont);
	} 
	val->DV.defFont = newFont;
    } else {
	vemMsg(MSG_A, "%s warning: no font meets specification `%s'\n",
	       def_pkg_name, text);
	return VEM_CORRUPT;
    }
    return VEM_OK;
}




static vemStatus toFill(text, val)
STR text;			/* Fill pattern specification  */
def_value *val;		/* Old fill pattern (modified) */
/*
 * This routine takes a string of the form: %d,%d,%s where %s
 * is a string of binary digits and produces a fill pattern
 * for it.
 */
{
    char binarystr[1024];
    int width, height;
    flPattern newFill;

    if (text && (sscanf(text, "%d,%d,%s", &width, &height, binarystr) == 3)) {
	newFill = flNewPattern(width, height, binarystr);
	if (newFill != (flPattern) 0) {
	    /* All is well - free old and replace */
	    if (val->DV.defFill) {
		flFreePattern(val->DV.defFill);
	    }
	    val->DV.defFill = newFill;
	    return VEM_OK;
	} else {
	    vemMsg(MSG_A, "%s warning: cannot interpret `%s' as a fill pattern.\n",
		   def_pkg_name, text);
	    return VEM_CORRUPT;
	}
    } else {
	vemMsg(MSG_A, "%s warning: cannot parse `%s' as a fill pattern.\n",
	       def_pkg_name, text);
	return VEM_CORRUPT;
    }
}



static vemStatus toAttr(text, val)
STR text;			/* List of defaults to make up attribute */
def_value *val;		/* Returned value                        */
/*
 * This routine parses a string of the form: 
 *   <color>,<fill>,<lstyle>,<brwidth>
 * where <color> is the name of a color default,  <fill> is the
 * name of a fill pattern default,  <lstyle> is the name of a
 * line pattern default,  and <brwidth> is the name
 * of integer default that represents brush width.
 * These fields are translated into attributes using the attribute
 * module.  Any of the fields may be omitted by not specifing a name
 * between the commas.  Selection attributes are denoted by a leading
 * exclamation point.  Here are two examples:
 *   !select.color,select.pattern,,,,
 *   background.color,,,,,
 * Attributes can be set ONLY ONCE.  If the attribute is already
 * allocated,  the routine returns VEM_INUSE.
 */
{
    static char colstr[64], fillstr[64], patstr[64], wstr[64], hstr[64];
    static char *fields[5] = { colstr, fillstr, patstr, wstr, hstr };
    char *tmp, *spot;
    XColor theColor;
    flPattern theFill = (flPattern) 0;
    flPattern theStyle = (flPattern) 0;
    int theWidth = 0;
    int selflag, size, idx;
    unsigned long mask;
    atrHandle newAttr;

    if (val->DV.defAttr != (atrHandle) 0) return VEM_INUSE;

    if (!text) return VEM_CORRUPT;
    if (text[0] == '!') {
	selflag = 1;
	text++;
    } else {
	selflag = 0;
    }

    tmp = text;
    for (idx = 0;  idx < 4;  idx++) {
	if ( (spot = strchr(tmp, ',')) ) {
	    size = spot - tmp;
	    STRNMOVE(fields[idx], tmp, size);
	    fields[idx][size] = '\0';
	    tmp += (size+1);
	} else {
	    vemMsg(MSG_A, "%s warning: cannot parse `%s' as an attribute specification.\n",
		   def_pkg_name, text);
	    return VEM_CORRUPT;
	}
    }
    STRMOVE(fields[4], tmp);

    /* Begin translation by looking up the defaults */
    theColor.pixel = theColor.red = theColor.blue = theColor.green = 0;
    if (strlen(colstr) > 0) {
	XColor *lookUpColor;

	dfGetCol(colstr, &lookUpColor);
	theColor = *lookUpColor;
    }
    if (strlen(fillstr) > 0) {
	dfGetFill(fillstr, &theFill); 
    }
    if (strlen(patstr) > 0) {
	dfGetFill(patstr, &theStyle);
    }
    if (strlen(wstr) > 0) {
	dfGetInt(wstr, &theWidth);
    }

    /* Make the attribute */
    if (selflag) {
	VEM_CKRVAL(atrSelect(&newAttr, &mask, &theColor, theFill,
			     theStyle, theWidth) >= VEM_OK,
		   "Cannot allocate selection set.  See vem.planes and vem.colors",
		   VEM_RESOURCE);
    } else {
	VEM_CKRVAL(atrNew(&newAttr, &theColor, theFill,
			  theStyle, theWidth) >= VEM_OK,
		   "Too few colors defined.  Try setting vem.colors",
		   VEM_RESOURCE);
    }
    val->DV.defAttr = newAttr;
    return VEM_OK;
}


/*
 * Text specification to window option conversion
 */


static vemStatus toWnOption(text, val)
STR text;			/* New option values           */
def_value *val;		/* Old useful value (modified) */
/*
 * This routine is exported to the defaults module only.  It
 * reads a string of the form c..-c.. where each `c' is
 * a window option character.  Those before the minus sign
 * are turned on,  those after are turned off.  The option characters are
 * summarized below:
 *  g: display grid
 *  t: display title bar
 *  e: show window expanded
 *  d: show difference coordinates in title bar
 *  a: show absolute coordinates in title bar
 *  b: show status bits in title bar
 *  f: show edit cell name in title bar
 *  c: show context cell name in title bar
 *  r: show rpc application name
 *  i: show instance bounding boxes
 *  m: manhattan line arguments
 *  G: terminal/path gravity
 *  A: show actual terminal frames and names
 *  D: display grid as dots (not lines)
 * The routine modifies the bits value in `val' to reflect
 * the changes.
 */
{
    char *spot;
    int bits, on_bits = 0, m_flag;

    spot = &(text[0]);
    bits = m_flag = 0;
    while (*spot) {
	switch (*spot) {
	case 'g':
	    bits |= VEM_GRIDLINES;
	    break;
	case 't':
	    bits |= VEM_TITLEDISP;
	    break;
	case 'e':
	    bits |= VEM_EXPAND;
	    break;
	case 'x':
	    bits |= VEM_EXPERT;
	    break;
	case 'd':
	    bits |= VEM_SHOWDIFF;
	    break;
	case 'a':
	    bits |= VEM_SHOWABS;
	    break;
	case 'b':
	    bits |= VEM_SHOWBITS;
	    break;
	case 'c':
	    bits |= VEM_SHOWCON;
	    break;
	case 'r':
	    bits |= VEM_SHOWRPC;
	    break;
	case 'f':
	    bits |= VEM_SHOWEDIT;
	    break;
	case 'i':
	    bits |= VEM_SHOWBB;
	    break;
	case 'm':
	    bits |= VEM_MANLINE;
	    break;
	case 'G':
	    bits |= VEM_GRAVITY;
	    break;
	case 'A':
	    bits |= VEM_SHOWAT;
	    break;
	case 'D':
	    bits |= VEM_DOTGRID;
	    break;
	case '-':
	    on_bits = bits;
	    bits = 0;
	    m_flag = 1;
	    break;
	default:
	    /* Bad character - ignore */
	    break;
	}
	spot++;
    }
    if (!m_flag) {
	on_bits = bits;
	bits = 0;
    }
    /* Got the necessary information */
    val->DV.defInt |= on_bits;
    val->DV.defInt &= ~bits;
    return VEM_OK;
}




static vemStatus toPath(text, val)
STR text;			/* New option value    */
def_value *val;		/* Useful option value */
/*
 * Converts 'text' into a full directory name.  'text' may contain
 * tilde notation.  At some future point,  this might also check
 * to make sure the path exists.
 */
{
    /* val->DV.defString = VEMSTRCOPY(util_file_expand(text)); */
    val->DV.defString = VEMSTRCOPY(text); /* Do not expand now. */
    return VEM_OK;
}


#define MAXFONTS	500

#define FS(x)	((XFontStruct *) (x))

static int size_cmp(a, b)
lsGeneric a, b;
{
    return (FS(b)->ascent + FS(b)->descent) - (FS(a)->ascent+ FS(a)->descent);
}

static vemStatus toFList(text, val)
STR text;			/* New option value    */
def_value *val;		/* Useful option value */
/*
 * This routine parses a string of comma seperated font
 * names.  These names are placed in a list of fonts
 * for later use.  Fonts that can't be opened are
 * omitted from the list.  If none of the fonts can
 * be opened,  the routine returns VEM_CORRUPT.
 */
{
    XFontStruct *font;
    char fontname[1024], *p;
    char **font_name_list;
    int font_count, i;

    /* Eliminate old fonts if necessary */
    if (val->DV.defFontList != 0) {
	while (lsDelBegin(val->DV.defFontList, (lsGeneric *) &font) == LS_OK) {
	    XFreeFont(xv_disp(), font);
	}
	lsDestroy(val->DV.defFontList, (void (*)()) 0);
    }
    /* Make new list and read in fonts */
    val->DV.defFontList = lsCreate();
    while (text && *text) {
	p = strchr(text, ',');
	if (p) {
	    (void) strncpy(fontname, text, p - text);
	    fontname[p-text] = '\0';
	} else {
	    (void) strcpy(fontname, text);
	}
	/* Multiple expansion of fonts */
	font_name_list = XListFonts(xv_disp(), fontname, MAXFONTS,
				    &font_count);
	for (i = 0;  i < font_count;  i++) {
	    font = XLoadQueryFont(xv_disp(), font_name_list[i]);
	    if (font) {
		lsNewEnd(val->DV.defFontList, (lsGeneric) font, LS_NH);
	    }
	}
	XFreeFontNames(font_name_list);
	text = p;
	if (text) text++;
    }
    if (lsLength(val->DV.defFontList) > 0) {
	lsSort(val->DV.defFontList, size_cmp);
	return VEM_OK;
    } else {
	vemMsg(MSG_A, "%s warning: Cannot parse `%s' as a font list.\n",
	       def_pkg_name, text);
	return VEM_CORRUPT;
    }
}
    


vemStatus dfNew(name, initValue, type, tranFunc)
STR name;			/* Name of default value        */
STR initValue;			/* Initial text form of default */
int type;			/* Type of default              */
vemStatus (*tranFunc)();	/* Transformation function      */
/*
 * Allocates a new default record of type 'type'.  It applies the 
 * transformation function to the initial value to obtain the initial 
 * useful value,  and inserts the record into the defaults hash table.  
 * The transformation function should be defined as follows:
 *   vemStatus tranFunc(newVal, oldVal)
 *   STR newVal;
 *   def_value *oldVal;
 * If the specified text value is appropriate,  the routine should
 * modify 'oldVal' to its new value and return VEM_OK.  If the
 * text value is not appropriate,  it should return VEM_CORRUPT
 * and leave 'oldVal' unchanged.
 */
{
    def_value *newDefault;

    newDefault = VEMALLOC(def_value);
    newDefault->textValue = initValue;
    newDefault->defType = type;
    newDefault->tranFunc = tranFunc;
    /* Set to zero of the type */
    switch (type) {
    case DEF_INTEGER:
	newDefault->DV.defInt = 0;
	break;
    case DEF_STRING:
	newDefault->DV.defString = 0;
	break;
    case DEF_DOUBLE:
	newDefault->DV.defDbl = 0.0;
	break;
    case DEF_COLOR:
	newDefault->DV.defColor = (XColor *) 0;
	break;
    case DEF_FONT:
	newDefault->DV.defFont = (XFontStruct *) 0;
	break;
    case DEF_FILL:
	newDefault->DV.defFill = (flPattern) 0;
	break;
    case DEF_ATTR:
	newDefault->DV.defAttr = (atrHandle) 0;
	break;
    case DEF_FONTLIST:
	newDefault->DV.defFontList = (lsList) 0;
	break;
    }

    if ((*tranFunc)(initValue, newDefault) == VEM_OK) {
	/* Put it in the hash table */
	if (st_insert(vemDefTable, name, (Pointer) newDefault) == 1)
	  return VEM_DUPLICATE;
	else
	  return VEM_OK;
    } else {
	VEMFREE(newDefault);
	return VEM_CORRUPT;
    }
}


#define MAX_DB_SPEC	512

static void up_each_word(spec)
STR spec;			/* Resource specification */
/*
 * Upcases each word in the specification.  Modifies the string in
 * place.
 */
{
    char *now, *next;

    now = spec;
    do {
	next = strchr(now, '.'); /* Was strrchr()!! AC May 3 91 */
	if (*now != '.') {
	    *now = (char) toupper(*now);
	}
	now = next;
	if (now) now++;
    } while (now && (*now != '\0'));
}

static char *getDB(disp, prog, name)
Display *disp;			/* Connection to X  */
STR prog;			/* Program name     */
STR name;			/* Name for default */
/*
 * This routine attempts to extract the default `name' from
 * the resource database constructed by the toolkit.  The
 * program name is concatenated onto the specification before
 * retrieval.  The class specification is derived directly 
 * from the original name by capitalizing each item down
 * the hierarchy.
 */
{
    static char full_spec[MAX_DB_SPEC], class_spec[MAX_DB_SPEC];
    XrmValue value;
    char *rep_str;

    STRMOVE(full_spec, prog);
    STRCONCAT(full_spec, ".");
    STRCONCAT(full_spec, name);
    STRMOVE(class_spec, "Vem");
    STRCONCAT(class_spec, ".");
    STRCONCAT(class_spec, name);
    up_each_word(class_spec);
    if (XrmGetResource(XtDatabase(disp), full_spec, class_spec,
		       &rep_str, &value)) {
	return (char *) value.addr;
    } else {
	return (char *) 0;
    }
}


/* For convenience: */

#define NEWDEFAULT(name, init, type, conv) \
if ((fromDB = getDB(disp, programName, name)) != (char *) 0) { \
    (void) dfNew(name, fromDB, type, conv);       \
} else {                                          \
    (void) dfNew(name, init, type, conv);         \
}

extern char *mktemp();

#define CAD_PATH(pth) \
VEMSTRCOPY(strcat(strcat(strcpy(buf, ""), "$OCTTOOLS"), pth))

#define BDS(bloc) \
CAD_PATH(strcat(strcpy(buf2, bloc), CUR_VER))

static char logFileName[] = "/tmp/vem.log.XXXXXX";

void dfInit(disp, programName)
Display *disp;
char *programName;
/*
 * This routine initializes the defaults hash table and defines
 * the various default values.  First,  it creates the defaults
 * setting up initial values.  This routine assumes the connection
 * to the display has been made and sets the hardwired defaults
 * for color or B/W (depending on the number of planes in the display).
 * Before moving on to the next default,  it checks the .Xdefaults 
 * file to see if the user has a preference for the value.  All new 
 * defaults should be coded here.  This routine should ALWAYS be called
 * before any defaults are used or changed.  All of the conversion
 * routines can be found above.
 */
{
    int STRCOMP(), hashIt(), temp, depth;
    STR fromDB;
    char buf[1024], buf2[1024];

    /* Allocate hash table */
    if (vemDefTable != NIL(st_table)) {
	/* Already defined - no initialization necessary */
	errRaise(def_pkg_name, VEM_DUPLICATE,
		 "Defaults package is already initialized");
    } else {
	vemDefTable = st_init_table(strcmp, st_strhash);

	/* First,  the non-color specific ones */
	NEWDEFAULT("defaultsVersion", "8-2", DEF_STRING, toString);
	NEWDEFAULT("initialBrowser", "0", DEF_INTEGER, toInt);
	NEWDEFAULT("expert", "0", DEF_INTEGER, toInt);
	NEWDEFAULT("lambda", "20", DEF_INTEGER, toInt);
	NEWDEFAULT("snap", dfGetText("lambda"), DEF_INTEGER, toInt);
	NEWDEFAULT("technology", tapGetDefaultRoot(), DEF_STRING, toString);
	NEWDEFAULT("label.fonts",
		   "*-helvetica-medium-r-normal-*-iso8859-*",
		   DEF_FONTLIST, toFList);
	NEWDEFAULT("grid.pixels", "8", DEF_INTEGER, toInt);
	NEWDEFAULT("grid.base", "0", DEF_INTEGER, toInt);
	NEWDEFAULT("grid.minbase", "1", DEF_INTEGER, toInt);
	NEWDEFAULT("gravity", "10", DEF_INTEGER, toInt);
	NEWDEFAULT("schem.layer", "WIRING", DEF_STRING, toString);
	dfGetInt("grid.base", &temp);
	if (temp > 0) {
	    /* Infinite grid */
	    NEWDEFAULT("grid.major.units", "10", DEF_INTEGER, toInt);
	    NEWDEFAULT("grid.minor.units", "2", DEF_INTEGER, toInt);
	} else {
	    /* Normal grid */
	    NEWDEFAULT("grid.major.units", "100", DEF_INTEGER, toInt);
	    NEWDEFAULT("grid.minor.units", "20", DEF_INTEGER, toInt);
	}
	NEWDEFAULT("boundbox.thres", "2.0", DEF_DOUBLE, toDouble);
	NEWDEFAULT("boundbox.font", "*-medium-r-*-100-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("boundbox.min", "8", DEF_INTEGER, toInt);
	NEWDEFAULT("interface", "interface", DEF_STRING, toString);
	NEWDEFAULT("version", "", DEF_STRING, toString);
	NEWDEFAULT("bindings", BDS("/lib/vem/vem.bindings."), DEF_STRING, toPath);
	NEWDEFAULT("border.width", "2", DEF_INTEGER, toInt);
	NEWDEFAULT("logfile", mktemp(logFileName), DEF_STRING, toPath);
	NEWDEFAULT("inst.priority", "1", DEF_INTEGER, toInt);
	NEWDEFAULT("argument.pixels", "20", DEF_INTEGER, toInt);
	NEWDEFAULT("solid.threshold", "12", DEF_INTEGER, toInt);
	NEWDEFAULT("abstraction", "0", DEF_INTEGER, toInt);
	NEWDEFAULT("dialog.freeze", "1", DEF_INTEGER, toInt);
	NEWDEFAULT("label.boundbox", "1", DEF_INTEGER, toInt);
	NEWDEFAULT("nolayer.fill", "1,1,1", DEF_FILL, toFill);
	NEWDEFAULT("grid.major.size", "1", DEF_INTEGER, toInt);
	NEWDEFAULT("grid.minor.size", "1", DEF_INTEGER, toInt);
	NEWDEFAULT("menu.delay", "0", DEF_INTEGER, toInt);
	NEWDEFAULT("deftechname", "scmos", DEF_STRING, toString);
	NEWDEFAULT("text.font.norm", "*-medium-r-*-100-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("text.font.bold", "*-medium-r-*-100-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("text.font.italic", "*-medium-r-*-100-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("title.font", "*-medium-r-*-100-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("menu.title.font", "*-bold-r-*-140-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("menu.item.font", "*-medium-r-*-120-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("menu.key.font", "*-medium-r-*-100-*-iso8859-*", DEF_FONT, toFont);
	NEWDEFAULT("viewtype", "physical", DEF_STRING, toString);
	NEWDEFAULT("editstyle", "NONE", DEF_STRING, toString);
	NEWDEFAULT("autosave", "1000", DEF_INTEGER, toInt);

	/* Symbolic editing defaults */
	NEWDEFAULT("symbolic.reconnect", "1", DEF_INTEGER, toInt);

	/* Now,  the color vs. B/W defaults */
	depth = xv_depth();
	if (depth < 4) {
	    /* Black and white display */
	    NEWDEFAULT("window.options", "gtedabcrfAD-imG", DEF_INTEGER, toWnOption);
	    NEWDEFAULT("select.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("select.style", "1,8,00001111", DEF_FILL, toFill);
	    NEWDEFAULT("select.fill", "4,4,0000011001100000",
		       DEF_FILL, toFill);
	    NEWDEFAULT("console.bngd", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("console.fgnd", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("console.cursor", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.norm", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.bold", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.italic", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("background.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("cursor.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("border.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.zero.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.zero.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("grid.zero.size", "3", DEF_INTEGER, toInt);
	    NEWDEFAULT("grid.major.line.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.major.dot.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.major.line.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("grid.minor.line.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.minor.dot.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.minor.line.style", "1,2,01", DEF_FILL, toFill);
	    NEWDEFAULT("title.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("title.backgr", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("boundbox.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("formal.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("formal.style", "1,4,0111", DEF_FILL, toFill);
	    NEWDEFAULT("actual.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("actual.style", "1,8,11111100", DEF_FILL, toFill);
	    NEWDEFAULT("inst.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("inst.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("nolayer.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("nolayer.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("menu.title.foreground", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.title.background", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.item.foreground", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.item.background", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.cursor.color", "black", DEF_COLOR, toColor);
	} else if (depth < 8) {
	    /* Color defaults for low color machines */
	    NEWDEFAULT("window.options", "gtedabcrfAD-imG", DEF_INTEGER, toWnOption);
	    NEWDEFAULT("select.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("select.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("select.fill",
	 "8,8,0000000000000000000000000001100000011000000000000000000000000000",
		       DEF_FILL, toFill);
	    NEWDEFAULT("console.bngd", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("console.fgnd", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("console.cursor", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.norm", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.bold", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.italic", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("background.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("cursor.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("border.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.zero.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.zero.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("grid.zero.size", "1", DEF_INTEGER, toInt);
	    NEWDEFAULT("grid.major.line.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.major.dot.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.major.line.style", "1,2,01", DEF_FILL, toFill);
	    NEWDEFAULT("grid.minor.line.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.minor.dot.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.minor.line.style", "1,2,01", DEF_FILL, toFill);
	    NEWDEFAULT("title.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("title.backgr", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("boundbox.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("formal.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("formal.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("actual.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("actual.style", "1,3,001", DEF_FILL, toFill);
	    NEWDEFAULT("inst.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("inst.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("nolayer.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("nolayer.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("menu.title.foreground", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.title.background", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.item.foreground", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.item.background", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.cursor.color", "black", DEF_COLOR, toColor);
	} else {
	    /* Full color defaults */
	    NEWDEFAULT("window.options", "gtedabcrfA-imGD", DEF_INTEGER, toWnOption);
	    NEWDEFAULT("select.color", "cyan", DEF_COLOR, toColor);
	    NEWDEFAULT("select.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("select.fill",
	 "8,8,0000000000000000000000000001100000011000000000000000000000000000",
		       DEF_FILL, toFill);
	    NEWDEFAULT("console.bngd", "#eee", DEF_COLOR, toColor);
	    NEWDEFAULT("console.fgnd", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("console.cursor", "red", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.norm", "VioletRed", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.bold", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("text.color.italic", "blue", DEF_COLOR, toColor);
	    NEWDEFAULT("background.color", "LightGray", DEF_COLOR, toColor);
	    NEWDEFAULT("cursor.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("border.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.zero.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.zero.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("grid.zero.size", "1", DEF_INTEGER, toInt);
	    NEWDEFAULT("grid.major.line.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.major.dot.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.major.line.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("grid.minor.line.color", "#888", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.minor.dot.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("grid.minor.line.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("title.color", "goldenrod", DEF_COLOR, toColor);
	    NEWDEFAULT("title.backgr", "#602", DEF_COLOR, toColor);
	    NEWDEFAULT("boundbox.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("formal.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("formal.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("actual.color", "#00ffff", DEF_COLOR, toColor);
	    NEWDEFAULT("actual.style", "1,2,01", DEF_FILL, toFill);
	    NEWDEFAULT("inst.color", "white", DEF_COLOR, toColor);
	    NEWDEFAULT("inst.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("nolayer.color", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("nolayer.style", "1,1,1", DEF_FILL, toFill);
	    NEWDEFAULT("menu.title.foreground", "yellow", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.title.background", "brown", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.item.foreground", "black", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.item.background", "#aaa", DEF_COLOR, toColor);
	    NEWDEFAULT("menu.cursor.color", "red", DEF_COLOR, toColor);
	}
	/*
	 * Now the attributes.  Attributes are a combination of
	 * other defaults.  Normally,  they are never set in the
	 * X defaults file or by dfStore.
	 */
	dfNew("Background", "background.color,,,,", DEF_ATTR, toAttr);
	dfNew("select", "!select.color,select.fill,select.style,,",
	      DEF_ATTR, toAttr);
	dfNew("grid.zero",
	      "grid.zero.color,,grid.zero.style,grid.zero.size,grid.zero.size",
	      DEF_ATTR, toAttr);
	dfNew("grid.major.line",
	  "grid.major.line.color,,grid.major.line.style,,",
	  DEF_ATTR, toAttr);
	dfNew("grid.major.dot",
	  "grid.major.dot.color,,,grid.major.size,grid.major.size",
	  DEF_ATTR, toAttr);
	dfNew("grid.minor.line",
	  "grid.minor.line.color,,grid.minor.line.style,,",
	  DEF_ATTR, toAttr);
	dfNew("grid.minor.dot",
	  "grid.minor.dot.color,,,grid.minor.size,grid.minor.size",
	  DEF_ATTR, toAttr);
	dfNew("formal", "formal.color,,formal.style,,", DEF_ATTR, toAttr);
	dfNew("actual", "actual.color,,actual.style,,", DEF_ATTR, toAttr);
	dfNew("inst", "inst.color,,inst.style,,", DEF_ATTR, toAttr);
	dfNew("nolayer", "nolayer.color,nolayer.fill,nolayer.style,,",
	      DEF_ATTR, toAttr);
    }
}




vemStatus dfStore(name, value)
STR name;			/* Name for default value     */
STR value;			/* Text form of default value */
/*
 * Stores a parameter value in the default system.  The default must
 * have been created using dfNew.  The routine automatically updates
 * the useful value of the parameter.  Diagnostics:  VEM_CANTFIND
 * (parameter not defined).  WARNING:  dfInit should be called once
 * before calling this routine.  Note:  you cannot store a new
 * value into an attribute.
 */
{
    def_value *curValue;
    vemStatus retCode;

    if (st_lookup(vemDefTable, name, (Pointer *) &curValue) == 1) {
	/* Found it */

	if ((retCode = (*(curValue->tranFunc))(value, curValue)) == VEM_OK) {
	    curValue->textValue = value;
	    curValue->defType &= (~DEF_INVALID);
	    return VEM_OK;
	} else {
	    curValue->defType |= DEF_INVALID;
	    return retCode;
	}
    } else return VEM_CANTFIND;
}


/*
 * Routines for getting the useful value of a default start on this page
 */


void dfGetInt(name, value)
STR name;			/* Name of default */
int *value;			/* Returned value  */
/*
 * Returns a default value of type DEF_INTEGER.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.
 */
{
    def_value *theVal;

    *value = 0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_INTEGER) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not an integer", name);
    }
    *value = theVal->DV.defInt;
}


void dfGetString(name, value)
STR name;			/* Name of default */
STR *value;			/* Returned value  */
/*
 * Returns a default value of type DEF_STRING.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.  The string is owned by the
 * defaults module and should not be changed.
 */
{
    def_value *theVal;

    *value = "";
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_STRING) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not a string", name);
    }
    *value = theVal->DV.defString;
}


void dfGetDouble(name, value)
STR name;			/* Name of default */
double *value;			/* Returned value  */
/*
 * Returns a default value of type DEF_DOUBLE.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.
 */
{
    def_value *theVal;

    *value = 0.0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_DOUBLE) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not a float", name);
    }
    *value = theVal->DV.defDbl;
}



void dfGetCol(name, value)
STR name;			/* Name of default */
XColor **value;			/* Returned value  */
/*
 * Returns a default value of type DEF_COLOR.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.  This routine simply returns
 * the structure for the color.  See dfGetPix for the
 * routine that also allocates a pixel.
 */
{
    def_value *theVal;

    *value = (XColor *) 0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_COLOR) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not a color", name);
    }
    *value = theVal->DV.defColor;
}

void dfGetPixel(name, value)
STR name;			/* Name of default */
XColor **value;			/* Returned value  */
/*
 * Returns a default value of type DEF_COLOR.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.  This routine goes further than
 * dfGetCol and attempts to get the pixel value associated 
 * with the color using XAllocColor. The display must be open 
 * for this to work.
 */
{
    def_value *theVal;

    *value = (XColor *) 0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,

		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_COLOR) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not a color", name);
    }
    *value = theVal->DV.defColor;
    if ((*value)->pixel == ILLEGAL_PIXEL) {
	/* Attempt to resolve - short cut if black or white */
	if (xv_depth() > 1) {
	    if (STRCLCOMP(theVal->textValue, "white") == 0) {
		(void) vuWhite(*value);
	    } else if (STRCLCOMP(theVal->textValue, "black") == 0) {
		(void) vuBlack(*value);
	    } else {
		/* Color display station */
		if (!XAllocColor(xv_disp(), xv_cmap(), *value)) {
		    errRaise(def_pkg_name, VEM_RESOURCE,
			     "Cannot allocate requested pixel for `%s'\n",
			     name);
		}
	    }
	} else {
	    /* Its black and white */
	    if (STRCLCOMP(theVal->textValue, "white") == 0) {
		/* Return WhitePixel */
		(void) vuWhite(*value);
	    } else {
		/* Return BlackPixel */
		(void) vuBlack(*value);
	    }
	}
    }
}


void dfGetFont(name, value)
STR name;			/* Name of default */
XFontStruct **value;		/* Returned value  */
/*
 * Returns a default value of type DEF_FONT.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.
 */
{
    def_value *theVal;

    *value = (XFontStruct *) 0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_FONT) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not a font", name);
    }
    *value = theVal->DV.defFont;
}



void dfGetFill(name, value)
STR name;			/* Name of fill default */
flPattern *value;		/* Fill pattern         */
/*
 * Returns a default value of type DEF_FILL.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.
 */
{
    def_value *theVal;

    *value = (flPattern) 0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_FILL) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not a fill pattern", name);
    }
    *value = theVal->DV.defFill;
}


void dfGetAttr(name, value)
STR name;			/* Name of fill default */
atrHandle *value;		/* Attribute handle     */
/*
 * Returns a default value of type DEF_ATTR.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.
 */
{
    def_value *theVal;

    *value = (atrHandle) 0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_ATTR) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not an attribute", name);
    }
    *value = theVal->DV.defAttr;
}



STR dfGetText(name)
STR name;			/* Name for default value */
/*
 * Gets the text form of a default value.  Returns the null
 * string if there is no value defined for the given name.
 * This is usually used for editing default values while the
 * program is running.
 */
{
    def_value *theVal;

    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_INTEGER) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not an integer", name);
    }
    VEM_CKRVAL(st_lookup(vemDefTable, name, (Pointer *) &theVal),
	       "Unknown default", (char *) 0);
    /* Got him */
    return theVal->textValue;
}


void dfGetFontList(name, value)
STR name;			/* Name of default */
lsList *value;			/* Returned value  */
/*
 * Returns a default value of type DEF_FONTLIST.  The package
 * raises an error if the paramter cannot be found or the
 * type is not appropriate.
 */
{
    def_value *theVal;

    *value = (lsList) 0;
    if (!st_lookup(vemDefTable, name, (Pointer *) &theVal)) {
	errRaise(def_pkg_name, VEM_CANTFIND,
		 "Parameter `%s' not defined", name);
    }
    if (theVal->defType & DEF_INVALID) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' does not have a valid value", name);
    }
    if (theVal->defType != DEF_FONTLIST) {
	errRaise(def_pkg_name, VEM_TYPECONF,
		 "Parameter `%s' is not a font list", name);
    }
    *value = theVal->DV.defFontList;
}




static enum st_retval genFunc(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * Generator function for passing to st_foreach.
 */
{
    def_value *theVal = (def_value *) value;
    int *option = (int *) arg;

    vemMsg(*option, "  %s = `%s'\n", key, theVal->textValue);
    return ST_CONTINUE;
}


void dfDump(option)
int option;			/* VEM message option word */
/*
 * This routine dumps all the default text values using
 * vemMessage.  'option' is passed directly to vemMsg.
 */
{
    vemMsg(option, "Default values:\n");
    st_foreach(vemDefTable, genFunc, (char *) &option);
}

