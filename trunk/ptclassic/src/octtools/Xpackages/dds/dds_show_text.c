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
 * Dialog Definition System
 * Show Text Items
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#include <X11/Xaw/Simple.h>
#include <X11/Xaw/AsciiText.h>


#define AVE_CHAR_STR	"n"
#define MAXLINE		4096
#define START_SIZE	500
#define CARET_SIZE	2

static ddsHandle new_showtext();
static void get_showtext();
static void set_showtext();
static void del_showtext();
static Widget parent_showtext();
static void place_showtext();

ddsClass _dds_show_text = {
    DDS_SHOW_TEXT, new_showtext, get_showtext, set_showtext, del_showtext,
    parent_showtext, place_showtext
};

static char *read_from_file();
static void find_size();
static void format_str();



static ddsHandle new_showtext(parent, class, data)
ddsInternal *parent;		/* Should be DDS_TOP or DDS_COMPOSITE */
ddsClass *class;		/* Should be DDS_SHOW_TEXT            */
ddsData data;			/* Should be (ddsEditText *)          */
/*
 * Makes a new non-editable text component.  This is simply a scrollable
 * text output area where editing has been turned off.  It is possible
 * to specify that the text should be drawn from a file.
 */
{
    ddsFullShowText *item;
    ddsInternal *same;
    XFontStruct *font;
    int top, bottom, right, left, char_width, char_height;
    Widget pw;

    if (parent) {
	item = ALLOC(ddsFullShowText, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->user_spec = *((ddsShowText *) data);
	if (!item->user_spec.text) item->user_spec.text = DDS_SHOWTEXT_NAME;
	item->user_spec.text = util_strsav(item->user_spec.text);
	if (item->user_spec.where == DDS_FROM_FILE) {
	    item->real_buf = read_from_file(item->user_spec.text);
	} else {
	    item->real_buf = item->user_spec.text;
	}
	if (item->user_spec.compute_size_p == DDS_TRUE) {
	    find_size(item);
	} else {
	    item->real_rows = item->user_spec.rows;
	    item->real_cols = item->user_spec.cols;
	}
	if (item->user_spec.word_wrap_p == DDS_TRUE) {
	    format_str(item);
	    if (item->user_spec.compute_size_p == DDS_TRUE) {
		find_size(item);
	    }
	}
	pw = (*parent->base.class->_dds_parent_widget)(parent, same);
	item->text =
	  XtVaCreateManagedWidget(DDS_SHOWTEXT_NAME, asciiTextWidgetClass, pw,
				  XtNeditType, XawtextRead,
				  XtNscrollVertical, XawtextScrollWhenNeeded,
				  XtNstring, item->real_buf,
				  XtNtype, XawAsciiString,
				  XtNuseStringInPlace, True,
				  XtNdisplayCaret, False,
				  NULL);
	(*parent->base.class->_dds_place)(parent, same, 1, &(item->text),
					  item->user_spec.rows);

	dds_add_cursor_widget(item->text, XtNcursor);
	XtAddCallback(item->text, XtNdestroyCallback, _dds_destroy_callback,
		      (XtPointer) item);

	/* Here is yet another text widget size hack */
	top = bottom = left = right = 0;
	XtVaGetValues(item->text, XtNfont, &font, 
		      XtNtopMargin, &top, XtNbottomMargin, &bottom,
		      XtNrightMargin, &right, XtNleftMargin, &left,
		      NULL);
	char_height = font->ascent + font->descent;
	char_width = XTextWidth(font, AVE_CHAR_STR, strlen(AVE_CHAR_STR));
	XtVaSetValues(item->text,
		      XtNheight, (char_height*item->real_rows)+top+bottom+CARET_SIZE,
		      XtNwidth, (char_width*item->real_cols)+right+left,
		      NULL);
	return (ddsHandle) item;
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "DDS_SHOW_TEXT components must have a parent");
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}


static char *read_from_file(filename)
char *filename;
/*
 * Reads text from the given file.  We could just use the feature of text
 * display in the asciiTextWidget but this doesn't give us the capability
 * of full file expansion or of nice error reporting.  We can also now
 * format text on demand whether it is a string or a file.  Disadvantage:
 * fixed line length.
 */
{
    FILE *fp;
    char err[MAXLINE];
    char *result, *ptr;
    int alloc, temp, ch;

    alloc = START_SIZE;
    result = ALLOC(char, alloc);
    ptr = result;
    if ( (fp = fopen(util_file_expand(filename), "r")) ) {
	while ((ch = getc(fp)) != EOF) {
	    if ((ptr-result) >= alloc) {
		temp = ptr - result;
		alloc *= 2;
		result = REALLOC(char, result, alloc);
		ptr = result+temp;
	    }
	    *ptr++ = (char) ch;
	}
	if ((ptr-result) >= alloc) {
	    temp = ptr - result;
	    alloc *= 2;
	    result = REALLOC(char, result, alloc);
	    ptr = result+temp;
	}
	*ptr = '\0';
	return result;
    } else {
	(void) sprintf(err, "Can't open file `%s'\n", filename);
	return util_strsav(err);
    }
}



static void find_size(item)
ddsFullShowText *item;
/*
 * Attempts to compute the size of the item.  This is done by scanning
 * the current string looking for end-of-line characters.  The number
 * of columns will be lesser of the maximum number of columns found and
 * the user specified number of columns.  Similarly, the number of
 * rows will be the lesser of the lines found and the user specified
 * number of rows.
 */
{
    char *ptr;
    int col_count;

    item->real_cols = 0;
    item->real_rows = 0;
    col_count = 0;
    for (ptr = item->real_buf;  *ptr;  ptr++) {
	if (*ptr == '\n') {
	    if (col_count > item->real_cols) {
		if (col_count <= item->user_spec.cols) {
		    item->real_cols = col_count;
		} else {
		    item->real_cols = item->user_spec.cols;
		}
	    }
	    col_count = 0;
	    if (item->real_rows < item->user_spec.rows) {
		item->real_rows += 1;
	    }
	} else {
	    col_count++;
	}
    }
    if (*(ptr-1) != '\n') {
	/* Handle last line */
	if (col_count > item->real_cols) {
	    if (col_count <= item->user_spec.cols) {
		item->real_cols = col_count;
	    } else {
		item->real_cols = item->user_spec.cols;
	    }
	    col_count = 0;
	}
	if (item->real_rows < item->user_spec.rows) {
	    item->real_rows += 1;
	}
    }
}

/* format stuff */
typedef struct len_str_defn {
    int len;			/* Length */
    char *str;			/* String */
} len_str;

static int getline(str, ostr)
String *str;
len_str *ostr;
/*
 * Finds the next line in `str'.  Returns the length.  The string
 * itself is placed in `ostr'.  `str' is modified to point past
 * the line just found.  The string will not contain the line termination
 * character.  Returns -1 if there are no more lines.
 */
{
    ostr->len = -1;
    ostr->str = *str;
    if (*str) {
	*str = strchr(ostr->str, '\n');
	if (*str) {
	    ostr->len = *str - ostr->str;
	    (*str)++;
	} else {
	    ostr->len = strlen(ostr->str);
	    *str = (String) 0;
	}
    }
    return ostr->len;
}

static int getword(istr, ostr)
len_str *istr;			/* Input string (modified) */
len_str *ostr;			/* Ouput string */
/*
 * Scans for the next word in `istr'.  Returns the length of
 * the result (-1 if there are no more words).
 */
{
    while ((istr->len > 0) && isspace(*(istr->str))) {
	istr->str += 1;
	istr->len -= 1;
    }
    if (istr->len > 0) {
	ostr->len = 0;
	ostr->str = istr->str;
	while ((istr->len > 0) && !isspace(*(istr->str))) {
	    istr->str += 1;
	    istr->len -= 1;
	    ostr->len += 1;
	}
	return ostr->len;
    } else {
	ostr->len = -1;
	ostr->str = istr->str;
    }
    return ostr->len;
}

static String DoFormat(str, dest_col)
String str;			/* Input string         */
int dest_col;			/* Destination columns  */
/*
 * This is a very simple formatting routine that formats `str'
 * into paragraphs of filled text that are word broken.  A blank
 * line separates paragraphs.  Normally,  it tries to fill
 * to `dest_col' columns.  
 */
{
    int max_word, line_len, advance;
    len_str line, word;
    String idx;
    String result;
    int endptr;
    int res_num = 0;
    int res_alloc = 128;

    /* Pass one - determine maximum word size */
    max_word = 0;
    idx = str;
    while (getline(&idx, &line) >= 0) {
	while (getword(&line, &word) >= 0) {
	    if (word.len > max_word) max_word = word.len;
	}
    }

    /* Pass two fill to the proper column */
    result = ALLOC(char, res_alloc);
    endptr = 0;
    idx = str;
    line_len = 0;
    while (getline(&idx, &line) >= 0) {
	if (line.len > 0) {
	    while (getword(&line, &word) >= 0) {
		if ((res_num + word.len + 1) >= res_alloc) {
		    res_alloc = res_alloc + res_alloc + word.len + 1;
		    result = REALLOC(char, result, res_alloc);
		}
		if (line_len + (line_len ? 1 : 0) + word.len < dest_col) {
		    /* Insert it on this line */
		    advance = 0;
		    if (line_len) {
			*(result+endptr) = ' ';
			advance = 1;
		    }
		    (void) strncpy(result+endptr+advance, word.str, word.len);
		    advance += word.len;
		} else {
		    /* Insert it on next line */
		    *(result+endptr) = '\n';
		    line_len = 0;
		    (void) strncpy(result+endptr+1, word.str, word.len);
		    advance = word.len + 1;
		}
		endptr += advance;
		res_num += advance;
		line_len += advance;
	    }
	} else {
	    /* Forced blank line */
	    if (res_num + 2 >= res_alloc) {
		res_alloc = res_alloc + res_alloc;
		result = REALLOC(char, result, res_alloc);
	    }
	    *(result+endptr) = '\n';
	    *(result+endptr+1) = '\n';
	    endptr += 2;
	    res_num++;
	    line_len = 0;
	}
    }
    /* Null terminate */
    if (res_num + 1 >= res_alloc) {
	res_alloc = res_alloc + res_alloc;
	result = REALLOC(char, result, res_alloc);
    }
    *(result+endptr) = '\0';
    endptr++;
    res_num++;
    return result;
}

static void format_str(item)
ddsFullShowText *item;
/*
 * Causes the item text to be filled into lines.  See DoFormat
 * for the formatting algorithm.
 */
{
    char *old_buf = item->real_buf;

    item->real_buf = DoFormat(item->real_buf, item->real_cols);
    if (item->user_spec.text != old_buf) {
	FREE(old_buf);
    }
}



static void get_showtext(item, data)
ddsInternal *item;		/* DDS_SHOW_TEXT   */
ddsData data;			/* (ddsShowText *) */
/*
 * Copies out the current state of the show text item.  The
 * text string is owned by the package and should not be
 * modified by the application.
 */
{
    ddsShowText *actual = (ddsShowText *) data;

    *actual = item->show_text.user_spec;
}

static void set_showtext(item, data)
ddsInternal *item;		/* DDS_SHOW_TEXT   */
ddsData data;			/* (ddsShowText *) */
/*
 * Sets the current state of the show text item.  If
 * the state differs, the appropriate widget is updated.
 * The type of the item may not be changed.
 */
{
    ddsShowText *actual = (ddsShowText *) data;
    ddsFullShowText new_state;
    XFontStruct *font;
    int top, bottom, right, left, char_width, char_height;

    new_state.user_spec = *actual;
    if (!new_state.user_spec.text) new_state.user_spec.text = DDS_SHOWTEXT_NAME;
    new_state.user_spec.text = util_strsav(new_state.user_spec.text);
    if (new_state.user_spec.where == DDS_FROM_FILE) {
	new_state.real_buf = read_from_file(new_state.user_spec.text);
    } else {
	new_state.real_buf = new_state.user_spec.text;
    }
    if (new_state.user_spec.compute_size_p == DDS_TRUE) {
	find_size(&new_state);
    } else {
	new_state.real_rows = new_state.user_spec.rows;
	new_state.real_cols = new_state.user_spec.cols;
    }
    if (new_state.user_spec.word_wrap_p == DDS_TRUE) {
	format_str(&new_state);
	if (new_state.user_spec.compute_size_p == DDS_TRUE) {
	    find_size(&new_state);
	}
    }
    font = (XFontStruct *) 0;
    top = bottom = right = left = 0;
    XtVaGetValues(item->show_text.text, XtNfont, &font, 
		  XtNtopMargin, &top, XtNbottomMargin, &bottom,
		  XtNrightMargin, &right, XtNleftMargin, &left,
		  NULL);
    char_height = font->ascent + font->descent;
    char_width = XTextWidth(font, AVE_CHAR_STR, strlen(AVE_CHAR_STR));

    if (item->show_text.real_buf != item->show_text.user_spec.text) {
	FREE(item->show_text.real_buf);
    }
    FREE(item->show_text.user_spec.text);

    /* set fields */
    item->show_text.user_spec.where = new_state.user_spec.where;
    item->show_text.user_spec.text = new_state.user_spec.text;
    item->show_text.user_spec.rows = new_state.user_spec.rows;
    item->show_text.user_spec.cols = new_state.user_spec.cols;
    item->show_text.user_spec.compute_size_p = new_state.user_spec.compute_size_p;
    item->show_text.user_spec.word_wrap_p = new_state.user_spec.word_wrap_p;
    item->show_text.real_buf = new_state.real_buf;

    XtVaSetValues(item->show_text.text,
		  XtNheight, (char_height*item->show_text.user_spec.rows)+top+bottom+CARET_SIZE,
		  XtNwidth, (char_width*item->show_text.user_spec.cols)+right+left,
		  XtNstring, item->show_text.real_buf,
		  NULL);
}



static void del_showtext(item)
ddsInternal *item;		/* DDS_SHOW_TEXT */
/*
 * Releases non-widget related data associated with a show text component.
 */
{
    if (item->show_text.real_buf != item->show_text.user_spec.text) {
	FREE(item->show_text.real_buf);
    }
    dds_remove_cursor_widget(item->show_text.text, XtNcursor);
    FREE(item->show_text.user_spec.text);
    FREE(item);
}



/*ARGSUSED*/
static Widget parent_showtext(parent, child)
ddsInternal *parent;		/* DDS_CONTROL */
ddsInternal *child;		/* New child   */
/*
 * The show text component cannot have children.
 */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_SHOW_TEXT components may not have child components");
    /*NOTREACHED*/
    return (Widget)NULL;
}

/*ARGSUSED*/
static void place_showtext(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_CONTROL              */
ddsInternal *child;		/* New component            */
int nw;				/* Number of widgets        */
WidgetList wlist;		/* Widgets themselves       */
int vspan;
/*
 * This is an error.  The control component cannot have children.
 */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_SHOW_TEXT components may not have child components");
    /*NOTREACHED*/
}


