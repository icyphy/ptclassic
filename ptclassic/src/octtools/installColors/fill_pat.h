/*
 * Fill Pattern Manipulation Header
 *
 * David Harrison
 * University of California, 1985, 1987
 *
 * This file contains declarations for use of the fill pattern manipulation
 * package implemented in fillPat.c.  Other files which should be included
 * before this one:
 */

#ifndef FILLPAT
#define FILLPAT

#include "general.h"

/* Can be typedef'ed to void if supported */
typedef struct fill_dummy_defn {
    int dummy;
} fill_dummy;

typedef fill_dummy *flPattern;	/* Internal representation of a fill pattern */

extern flPattern flNewPattern();
  /* Create a new fill pattern from external form */

extern vemStatus flSize();
  /* Returns the current size of a fill pattern */

extern vemStatus flIsSolid();
  /* Returns VEM_OK if the pattern is a solid fill,  VEM_FALSE otherwise */

extern vemStatus flXBitmap();
  /* Returns pattern in format suitable for making X bitmaps and pixmaps */

extern vemStatus flFreePattern();
  /* Releases the resources associated with a pattern created by vemNewPattern */

extern flPattern flChangeSize();
  /* Sizes a pattern to a specified size using clipping and replication */

extern vemStatus flEqual();
  /* Compares two fill patterns to see if they are equal */

#endif
