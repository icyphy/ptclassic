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

#include "port.h"
#include "general.h"		/* General definitions     */
#include "message.h"		/* Message handling system */
#include "fill_pat.h"		/* Self declaration        */

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
    int index, rows, cols;
    short c;

    /* Allocate new pattern */
    newPat = VEMALLOC(pat_internal);
    newPat->pattern = VEMARRAYALLOC(char *, height);
    for (index = 0;  index < height;  index++)
      newPat->pattern[index] = VEMARRAYALLOC(char, width);

    /* Strip the input string and place in array */
    rows = 0;  cols = 0;
    for (index = 0;  (c = dataStr[index]) != '\0';  index++) {
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


vemStatus flSize(pat, width, height)
flPattern pat;			/* External handle to pattern */
int *width, *height;		/* Size of fill pattern       */
/*
 * Returns the size of the fill pattern `pat' in `width' and `height'.
 * `width' represents the number of columns,  and `height' represents
 * the number of rows.
 */
{
    pat_internal *real_pat = (pat_internal *) pat; 

    if (real_pat) {
	*width = real_pat->cols;
	*height = real_pat->rows;
	return VEM_OK;
    } else {
	return VEM_CORRUPT;
    }
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





vemStatus flXBitmap(pat, width, height, data)
flPattern pat;			/* External handle to fill pattern     */
int *width, *height;		/* Returned size of bitmap             */
unsigned short **data;		/* Returned data array                 */
/*
 * This routine converts a pattern in internal format into a pattern
 * suitable for building an X bitmap or pixmap (in XY format).  
 */
{
    pat_internal *real_pat = (pat_internal *) pat;
    int length, index, rows, cols, pos;

    length = (((real_pat->cols+15)/16) * real_pat->rows);
    *data = VEMARRAYALLOC(unsigned short, length);
    (void) memset((Pointer) *data, 0, length);
    index = 0;
    for (rows = 0;  rows < real_pat->rows;  rows++) {
	pos = 0;
	for (cols = 0;  cols < real_pat->cols;  cols++) {
	    (*data)[index] |= (((real_pat->pattern[rows])[cols]-'0') << pos++);
	    if (pos > 15) {
		pos = 0;  index++;
	    }
	}
	if (pos > 0) index++;
    }
    *width = real_pat->cols;
    *height = real_pat->rows;
    return VEM_OK;
}



vemStatus flFreePattern(fillPat)
flPattern fillPat;		/* Pattern to destroy */
/*
 * This routine frees all resources consumed by 'fillPat'.  It should
 * never be referenced again.
 */
{
    pat_internal *realPat = (pat_internal *) fillPat;
    int index;

    for (index = 0;  index < realPat->rows;  index++)
      VEMFREE((realPat->pattern)[index]);
    VEMFREE(realPat->pattern);
    VEMFREE(realPat);

    return VEM_OK;
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
    int index, rows, cols;
    int oldHeight, oldWidth;

    /* Allocate return fill pattern */
    newPat = VEMALLOC(pat_internal);
    newPat->rows = new_height;
    newPat->cols = new_width;
    newPat->pattern = VEMARRAYALLOC(char *, new_height);
    for (index = 0;  index < new_height;  index++)
      newPat->pattern[index] = VEMARRAYALLOC(char, new_width);

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




static int gcd(a, b)
int a, b;
/* Returns the Greatest Common Divisor between a and b */
{
    if (a == b) {
	return a;
    } else if (a > b) {
	return gcd(a - b, b);
    } else {
	return gcd(b - a, a);
    }
}


static int lcm(a, b)
int a, b;
/* Returns the least common multiple of a and b */
{
    return (a * b) / gcd(a, b);
}


extern vemStatus flEqual(pat1, pat2)
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



vemStatus flOutput(pat, out_buf)
flPattern pat;			/* External pattern */
char *out_buf;			/* Output array     */
/*
 * This routine outputs to a buffer a stream of '0's and '1's
 * which represent the pattern specified by `pat'.
 */
{
    pat_internal *real_pat = (pat_internal *) pat;
    int i, j;

    if (real_pat) {
	for (i = 0;  i < real_pat->rows;  i++) {
	    for (j = 0;  j < real_pat->cols;  j++) {
		out_buf[i*real_pat->cols+j] =
		  (real_pat->pattern)[i][j];
	    }
	}
    }
    out_buf[real_pat->rows*real_pat->cols] = '\0';
    return VEM_OK;
}
