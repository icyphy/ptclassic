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
 * Fill Pattern Manipulation
 *
 * David Harrison
 * University of California, 1985, 1987
 *
 * This package implements a set of operations on a "fill pattern".  A
 * fill pattern is defined as a rectangular region of bits (0 or 1).
 * In VEM,  fill patterns can exist in one of three forms:
 *
 *    - Fill pattern specification in technology file (loosely formatted
 *	text representation)
 *    - Internal technology representation (more organized version
 *	of the above with the fluff removed)
 *    - A binary data array (with associated width and length) used
 *	for constructing Bitmaps and Pixmaps (very dense, X specific
 *	format).
 *
 * Outside VEM,  fill patterns live in the first state.  Inside VEM,
 * all fill patters are always maintained in the second form.  Finally,
 * for display purposes,  the fill pattern is changed into the third
 * form.  This package implements the routines for converting between
 * these forms and performing some simple operations on them.
 */

#include "fill_pat.h"		/* Self declaration        */
#include <X11/Xutil.h>		/* X11 Utility functions   */

typedef struct fill_record {
    int rows, cols;		/* Size of fill pattern           */
    char **pattern;		/* Dense character representation */
} pat_internal;

/*
 * flPattern is a hidden pointer to an array of character pointers
 * (pat_internal).  Basically,  this is a two dimensional array of
 * characters where each character is either '0' or '1'.  flNewPattern
 * basically removes white space and imposes the two dimensional
 * array structure.
 */




flPattern flNewPattern(width, height, dataStr)
int width, height;		/* Size of pattern in columns and rows */
STR dataStr;			/* Outside representation of fill pat. */
/*
 * This routine generates an internal representation of an externally
 * specified fill pattern.  'dataStr' is assumed to be string consisting
 * of five characters: '0', '1', ' ', '\n', and '\t'.  The routine
 * reads the string,  makes sure it agrees with the size data,  and
 * converts it into internal format.  If successful,  a handle for
 * the pattern is returned.  If unsuccessful,   it returns NULL.  It
 * generates an error message if there were problems.
 */
{
    pat_internal *newPat;
    int idx, rows, cols;
    short c;

    /* Allocate new pattern */
    newPat = VEMALLOC(pat_internal);
    newPat->pattern = VEMARRAYALLOC(char *, height);
    for (idx = 0;  idx < height;  idx++)
      newPat->pattern[idx] = VEMARRAYALLOC(char, width);

    /* Strip the input string and place in array */
    rows = 0;  cols = 0;
    for (idx = 0;  (c = dataStr[idx]) != '\0';  idx++) {
	if ((c != ' ') && (c != '\t') && (c != '\n')) {
	    if (cols >= width) {
		cols = 0;  rows++;
		if (rows >= height) break;
	    }
	    (newPat->pattern[rows])[cols++] = c;
	}
    }

    if (cols >= width) rows++;

    /* Sanity check to make sure string agrees with size */
    if ((rows != height) || (cols != width)) {
	return(0);
    } else {
	newPat->rows = height;
	newPat->cols = width;
	return((flPattern) newPat);
    }
}


void flSize(pat, width, height)
flPattern pat;			/* External handle to pattern */
int *width, *height;		/* Size of fill pattern       */
/*
 * Returns the size of the fill pattern `pat' in `width' and `height'.
 * `width' represents the number of columns,  and `height' represents
 * the number of rows.
 */
{
    pat_internal *real_pat = (pat_internal *) pat; 

    *width = real_pat->cols;
    *height = real_pat->rows;
}



vemStatus flIsSolid(pat)
flPattern pat;			/* External handle to pattern */
/*
 * This routine returns VEM_OK if the fill pattern `pat' is a solid fill
 * pattern (defined as all ones for any size).  It returns VEM_FALSE
 * if it does stipple.  Note the routine returns zero if there is
 * no fill pattern (width and height zero).
 */
{
    pat_internal *real_pat = (pat_internal *) pat;
    int row, col;

    if (!real_pat) return VEM_FALSE;
    if ((real_pat->rows == 0) || (real_pat->cols == 0)) return VEM_FALSE;
    for (row = 0;  row < real_pat->rows;  row++) {
	for (col = 0;  col < real_pat->cols;  col++) {
	    if (real_pat->pattern[row][col] == '0') {
		return VEM_FALSE;
	    }
	}
    }
    return VEM_OK;
}




void flXBitmap(disp, vis, pat, width, height, data)
Display *disp;			/* X Display                           */
Visual *vis;			/* X Visual                            */
flPattern pat;			/* External handle to fill pattern     */
int *width, *height;		/* Returned size of bitmap             */
char **data;			/* Returned data array                 */
/*
 * This routine converts a pattern in internal format into a pattern
 * suitable for building an X bitmap or pixmap (in XY format).  The
 * data is allocated in this routine and should be freed by the caller.
 */
{
    pat_internal *real_pat = (pat_internal *) pat;
    char *ldata;
    XImage *image;
    int pad, length, rows, cols;

    *width = real_pat->cols;
    *height = real_pat->rows;
/*    pad = BitmapPad(disp); */
    pad = 8;
    length = (((real_pat->cols + pad-1)/pad)*pad)/sizeof(char) * real_pat->rows;
    ldata = VEMARRAYALLOC(char, length);
    image = XCreateImage(disp, vis, 1, XYBitmap, 0, ldata,
			 real_pat->cols, real_pat->rows, pad, 0);
    for (rows = 0;  rows < real_pat->rows;  rows++) {
	for (cols = 0;  cols < real_pat->cols;  cols++) {
	    if ((real_pat->pattern[rows])[cols]-'0') {
		/* Bit is on */
		XPutPixel(image, cols, rows, ~0L);
	    } else {
		XPutPixel(image, cols, rows, 0);
	    }
	}
    }
    *data = VEMARRAYALLOC(char, length);
    (void) memcpy(*data, ldata, length);
    XDestroyImage(image);
}



void flXLineStyle(pat, offset, len, list)
flPattern pat;			/* Pattern to examine      */
int *offset;			/* Returned offset         */
int *len;			/* Returned length of list */
char **list;			/* Returned pattern list   */
/*
 * This routine interprets `pat' as a line style.  The first
 * row of the pattern is changed into a form suitable for
 * use as an X11 line style.  The offset, len, and list
 * arguments are as described for XSetDashes() in the Xlib
 * documentation.  The returned buffer is allocated in this
 * routine and should be freed by the caller.
 */
{
    pat_internal *real_pat = (pat_internal *) pat;
    int i, count, spot;
    char lastchar;

    /* Offset is always zero */
    *offset = 0;

    /* Pass one determines the size */
    *len = 0;
    lastchar = '\0';
    for (i = 0;  i < real_pat->cols;  i++) {
	if ((real_pat->pattern[0])[i] != lastchar) {
	    *len += 1;
	    lastchar = (real_pat->pattern[0])[i];
	}
    }
    *list = VEMARRAYALLOC(char, *len);

    /* Second pass fills in the list */
    lastchar = (real_pat->pattern[0])[0];
    count = spot = 0;
    for (i = 0;  i < real_pat->cols;  i++) {
	if ((real_pat->pattern[0])[i] != lastchar) {
	    (*list)[spot++] = (char) count;
	    count = 1;
	    lastchar = (real_pat->pattern[0])[i];
	} else {
	    count++;
	}
    }
    (*list)[spot++] = (char) count;
}



void flFreePattern(fillPat)
flPattern fillPat;		/* Pattern to destroy */
/*
 * This routine frees all resources consumed by 'fillPat'.  It should
 * never be referenced again.
 */
{
    pat_internal *realPat = (pat_internal *) fillPat;
    int idx;

    for (idx = 0;  idx < realPat->rows;  idx++)
      VEMFREE((realPat->pattern)[idx]);
    VEMFREE(realPat->pattern);
    VEMFREE(realPat);
}

flPattern flCopyPattern(pat)
flPattern pat;			/* Pattern */
/*
 * Returns a freshly allocated copy of `pat'.
 */
{
    pat_internal *realPat = (pat_internal *) pat;
    pat_internal *newPat;
    int i, j;

    newPat = VEMALLOC(pat_internal);
    newPat->rows = realPat->rows;
    newPat->cols = realPat->cols;
    newPat->pattern = VEMARRAYALLOC(char *, realPat->rows);
    for (i = 0;  i < realPat->rows;  i++) {
	newPat->pattern[i] = VEMARRAYALLOC(char, realPat->cols);
	for (j = 0;  j < realPat->cols;  j++) {
	    (newPat->pattern[i])[j] = (realPat->pattern[i])[j];
	}
    }
    return (flPattern) newPat;
}


flPattern flChangeSize(pat, new_width, new_height)
flPattern pat;			/* Pattern to resize   */
int new_width, new_height;	/* New size of pattern */
/*
 * This routine resizes the specified pattern and returns the new
 * pattern.  The routine can both shrink and expand the pattern.
 * Shrinking is accomplished by a straightforward clip of the pattern.
 * The upper-right most portion of the bitmap is saved.  Expansion
 * is carried out by a combination of replication and shrinking.
 * The pattern is replicated vertically and horizontally until the
 * pattern is larger than the target size.  It is then trimmed using
 * the same mechanism for shrinking given above.  The routine returns
 * NULL if the sizing fails.
 */
{
    pat_internal *realPat = (pat_internal *) pat;
    pat_internal *newPat;
    int idx, rows, cols;
    int oldHeight, oldWidth;

    /* Allocate return fill pattern */
    newPat = VEMALLOC(pat_internal);
    newPat->rows = new_height;
    newPat->cols = new_width;
    newPat->pattern = VEMARRAYALLOC(char *, new_height);
    for (idx = 0;  idx < new_height;  idx++)
      newPat->pattern[idx] = VEMARRAYALLOC(char, new_width);

    /* Transfer appropriate characters */
    oldHeight = realPat->rows;
    oldWidth = realPat->cols;
    for (rows = 0;  rows < new_height;  rows++) {
	for (cols = 0;  cols < new_width;  cols++) {
	    (newPat->pattern[rows])[cols] =
	      (realPat->pattern[rows % oldHeight])[cols % oldWidth];
	}
    }

    return (flPattern) newPat;
}




static int gcd(m, n)
int m, n;
/* Returns greatest common divisor (Euclid's algorithm) */
{
    int r;

    if (m < n) {
	r = m; m = n; n = r;
    }
    while ( (r = m % n) ) {
	m = n;
	n = r;
    }
    return n;
}

static int lcm(a, b)
int a, b;
/* Returns the least common multiple of a and b */
{
    return (a * b) / gcd(a, b);
}


vemStatus flEqual(pat1, pat2)
flPattern pat1, pat2;		/* Two patterns to compare */
/*
 * This routine compares two fill patterns and returns VEM_OK if
 * the two patterns are isomorphic.  If they differ,  the routine
 * returns VEM_FALSE.
 */
{
    pat_internal *p1 = (pat_internal *) pat1;
    pat_internal *p2 = (pat_internal *) pat2;
    int lcm_rows, lcm_cols;
    int row_idx, col_idx;

    lcm_rows = lcm(p1->rows, p2->rows);
    lcm_cols = lcm(p1->cols, p2->cols);
    for (row_idx = 0;  row_idx < lcm_rows;  row_idx++) {
	for (col_idx = 0;  col_idx < lcm_cols;  col_idx++) {
	    if ((p1->pattern[row_idx % p1->rows])[col_idx % p1->cols]
		!= (p2->pattern[row_idx % p2->rows])[col_idx % p2->cols])
	      return VEM_FALSE;
	}
    }
    return VEM_OK;
}


void flOutStr(pat, width, height, str)
flPattern pat;			/* Incoming pattern          */
int *width, *height;		/* Outgoing width and height */
char **str;			/* Outgoing string           */
/*
 * This routine outputs a textual representation of a pattern.
 * The string is null-terminated.
 */
{
    pat_internal *real_pat = (pat_internal *) pat;
    int i, j;

    *width = real_pat->cols;
    *height = real_pat->rows;
    *str = VEMARRAYALLOC(char, (real_pat->cols*real_pat->rows)+1);
    for (i = 0;  i < real_pat->rows;  i++) {
	for (j = 0;  j < real_pat->cols;  j++) {
	    (*str)[(i*real_pat->cols) + j] = (real_pat->pattern[i])[j];
	}
    }
    (*str)[real_pat->rows * real_pat->cols] = '\0';
}
