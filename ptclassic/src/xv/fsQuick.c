/* fsQuick.c 
*
* Quick and reasonably general halftoning code for taking grayscale-mapped
* inputs to bit-packed bitmap output.  Uses a variation on Floyd-Steinberg
* error diffusion that is much faster: propagate error to only two neighbors,
* half to next row and half to next column (processing in serpentine raster
* order).  Starts with some blue noise at the top edge.  Acts as if the top
* row were repeated a number of times above the picture to avoid edge
* effects at the top of the picture. At the end of each row, all error
* is propagated to first pixel processed in the next row (none is lost).
* Result is not much worse than standard F-S for most images I've tried,
* and is several times faster than popular implementations of F-S.
* Can be used for either possible black-white bit representation and either
* possible bit order in output bitmap.
*
* BUGS:
* o May be somewhat more prone to systematic artifacts in resulting bitmap than
*    standard Floyd-Steinberg.
* o Relies heavily on macros to avoid redundant code and runtime overhead; 
*    produces fast but not compact code; may not work if your C doesn't like 
*    very long lines of code after macro substitution.
*
* OTHER POSSIBLE IMPROVEMENTS:
* o The macro dorows() should really be a function.
* o Further optimization idea: remove postinc and predec of pointers and
*    shifting of outword in dither1pix. Instead supply specific offsets and
*    image pen mask as arguments to dither1pix. Will have to do this
*    separately for the two raster directions. Should save another 10-20%?
* o Should allow processing of one row per call, rather than whole image.
* o Should be made to work for non-packed bitmap output.
* o fsQTablesT should be made public so caller can have more flexibility to
*    to set this instead of calling fsQInit?  Hmmm...  maybe not.
*
*             -David B Rosen (currently rosen@cns.bu.edu) Jan-Feb 1991
*/

/* fsQuick Copyright (C) 1991 by David B. Rosen.  All Rights Reserved.  This
*  copyright notice applies only to fsQuick, not necessarily to the entire
*  work it is distributed with or used in.  See also accompanying license.
*/

#include <stdio.h>

#define fsQTablesTYPE /* this is the private type definition */ \
struct {  \
  /* bit lookup tables going up and down columns in row: */ \
  fsQBitmapT up[1<<fsQ_BITMAP_BITS], down[1<<fsQ_BITMAP_BITS]; \
}

#include "fsQuick.h"
/* Needed to pick up defn for random under Solaris2 etc (-DNO_RANDOM) */
#include "xv.h"

void FatalError fsQ_proto((char *message));

fsQ_RANDOM_DECL;

#ifndef NULL
#define NULL ((void *)0)
#endif /* NULL */

#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

#define fsQ_MAXVAL (fsQ_PIC_VALS-1)

fsQTablesT *fsQInit(black, IsLSBFirst)
     int black;
     int IsLSBFirst;
{
  register int flipbits= -(black&0x1);
  fsQBitmapT *bitTableLSBFirst=NULL, *bitTableMSBFirst=NULL;

  fsQTablesT *fsQTablesP;
  if (!(fsQTablesP = (void *)malloc(sizeof(fsQTablesT))))
    FatalError("not enough memory to fsQInit");

  if ((unsigned long)(fsQPicT)(fsQ_PIC_VALS-1) 
      != (unsigned long)(fsQ_PIC_VALS-1))
    FatalError("fsQInit: fsQ_PIC_VALS too large for type");

  if ((unsigned long)(fsQBitmapT)((((unsigned long)1)<<(fsQ_BITMAP_BITS))-1)
      != ((((unsigned long)1)<<(fsQ_BITMAP_BITS))-1))
    FatalError("fsQInit: fsQ_BITMAP_BITS too large for type");

#if fsQ_DEBUG
  fprintf(stderr, 
	  "fsQInit: black=%d, flipbits=%x\n", (int)black, (int)flipbits);
#endif /* fsQ_DEBUG */

  if (IsLSBFirst) {
    bitTableMSBFirst=fsQTablesP->up;
    bitTableLSBFirst=fsQTablesP->down;
  } else {
    bitTableLSBFirst=fsQTablesP->up;
    bitTableMSBFirst=fsQTablesP->down;
  }
  { register int i;
    for ( i=1<<fsQ_BITMAP_BITS; --i>=0; ) bitTableLSBFirst[i]= i^flipbits;
  }

  { register int i;
    for (i=(1<<fsQ_BITMAP_BITS); --i>=0; ){
      register int entry=0, ibit;
      for (ibit=fsQ_BITMAP_BITS; --ibit>=0; ) {
	/* set bit ((fsQ_BITMAP_BITS-1)-ibit) to bit ibit: */
	entry |= ( ((i>>ibit)&0x1) << ((fsQ_BITMAP_BITS-1)-ibit) );
      }
      bitTableMSBFirst[i] = entry^flipbits;
    }
  }
  return fsQTablesP;
}

void fsQuick(picture, gamma, nRows, nCols,
	       bitmap, bitTables, wordsPerRow)
     const fsQPicT *picture;
     const fsQPicT *gamma;
     int nRows;
     int nCols;
     fsQBitmapT *bitmap;
     const fsQTablesT *bitTables;
     int wordsPerRow;
{
  int *errRow;
  /* output words having all bits in image */
  int nWholeWords=nCols/fsQ_BITMAP_BITS; 
  
  /* having bits in image < fsQ_BITMAP_BITS*/
  int nExtraWords=wordsPerRow-nWholeWords; 

  register int err=0;

  /*These go up the cols of one row, down the next:*/
  register fsQBitmapT *bitP = bitmap;

  register const fsQPicT *picP = picture;
  /*This is just 1 row so it just goes up and back down again: */

  register int *errP;
  register const fsQBitmapT *bitTable;

/* MACROS: */
#define dither1pix(condition, predec, postinc)                            \
  if (condition) { /* if-else optimized away with constant condition*/    \
	/* Now err is residual error from previous pixel processed,       \
           and * predec errP is resid. error from pixel above this one.*/ \
	                                                                  \
    if (fsQ_DEBUG>2) {                                                    \
      int * errPtmp = errP;	                                          \
      fprintf(stderr,                                                     \
                "dither1pix: rowsleft=%d, err=%d, * predec errP=%d",      \
                (int)rowsleft, (int)err, (int) * predec errPtmp);         \
    }                                                                     \
                                                                          \
    err += * predec errP;                                                 \
    /* Now err is the total error coming into this pixel */               \
                                                                          \
    { register int val = * predec picP postinc; /*intersperse loads*/     \
      outword<<=1; /* shift the output image word */                      \
      if (fsQ_DEBUG>2) {                                                  \
        fprintf(stderr, ", pix val=%d", (int)gamma[val]);                 \
      }                                                                   \
      err += gamma[val];                                                  \
    }                                                                     \
                                                                          \
    /* Now err stores net value for this pixel (not the error) */         \
                                                                          \
    if (fsQ_DEBUG>2) {                                                    \
      fprintf(stderr, ", net val=%d\n", (int)err);                        \
    }                                                                     \
                                                                          \
    if (err >= fsQ_THRESH_VAL) { /* compare net value to thresh */        \
      /* this pixel will be white */                                      \
      err -= fsQ_MAXVAL;                                                  \
      outword |= 1;                                                       \
    }                                                                     \
                                                                          \
    /* Now err is total residual error sent out from this pixel. */       \
                                                                          \
    *errP postinc = (err>>=1); /* half over, half down */                 \
      /* Now * predec errP is resid. error sent to pixel below this one   \
	 and err is residual error sent to next pixel processed */        \
  } else {                                                                \
    predec errP postinc; /* just keep track of column this way */         \
    outword<<=1; /* shift the output image word */                        \
  }


    /* The following macro performs the halftoning for fsQ_BITMAP_BITS pixels 
       for each of nWords words in output bitmap. It propagates half of the 
       error over to the next column (in err) and the rest down to the next 
       row (in *errP).
       This is much faster than using the Floyd-Steinberg coefficients and
       propagating to four neighbors as is customary. It proceeds in
       serpentine fashion (left-right, right-left, etc....) and at the end of
       each row, _all_ of the error is propagated to the first pixel
       processed in the next row, instead of throwing away the half that
       would go off the edge. The innermost loop is fairly well optimized
       when condition is constant, which it will be for almost all pixels.
       
       This macro references: 
       gamma, err, errP, picP, bitP, bitTable, fsQ_MAXVAL, fsQ_THRESH_VAL
       */

#define ditherword(nWords, condition, predec, postinc)                   \
{ register int iword;                                                    \
  for (iword=nWords; --iword>=0; ) {                                     \
    register int outword=0; /* output image word (offset in bitTable) */ \
      /* Loop over fsQ_BITMAP_BITS columns, postincrementing or          \
         predecrementing the input                                       \
         pointer picP and error pointer errP and shifting outword: */    \
                                                                         \
    if (fsQ_BITMAP_BITS == 8) {                                          \
      dither1pix(condition, predec, postinc);                            \
      dither1pix(condition, predec, postinc);                            \
      dither1pix(condition, predec, postinc);                            \
      dither1pix(condition, predec, postinc);                            \
      dither1pix(condition, predec, postinc);                            \
      dither1pix(condition, predec, postinc);                            \
      dither1pix(condition, predec, postinc);                            \
      dither1pix(condition, predec, postinc);                            \
    } else {                                                             \
      { register int ibit;                                               \
        for (ibit=fsQ_BITMAP_BITS; --ibit>=0; )                          \
          dither1pix(condition, predec, postinc);                        \
      }                                                                  \
    }                                                                    \
                                                                         \
  * predec bitP postinc = bitTable[outword];   /* convert output word */ \
  }                                                                      \
}


/* row loop macro: */
#define dorows(nRows, nextRow) \
{ \
    int rowsleft = nRows; \
    while (TRUE) { \
      if (--rowsleft<0) break; \
      if ((rowsleft&63) == 0) WaitCursor();    /* JHB */ \
      /* left to right with postincrement: */ \
\
      bitTable=bitTables->up; \
      /* ditherword() with TRUE so if (condition)... is optimized away: */ \
      ditherword(nWholeWords,               TRUE,   , ++); \
      ditherword(nExtraWords,  errP<errRow+nCols,   , ++); \
      /* now err + *--errP is residual error sent to highest col of next row*/\
\
      nextRow; \
\
      if (--rowsleft<0) break; \
      /* right to left with predecrement: */ \
\
      bitTable=bitTables->down; \
      ditherword(nExtraWords, errP<=errRow+nCols, --,   ); \
      /* ditherword() with TRUE so if (condition)... is optimized away: */ \
      ditherword(nWholeWords,               TRUE, --,   ); \
      /* now err + *errP is residual error sent to lowest col of next row */ \
\
      nextRow; \
    } \
}

/* END OF MACROS.  BEGIN: */

  /* allocate vector of errors for columns in row: */
  errRow = (int *) malloc(nCols * sizeof(int));
  if (! errRow) FatalError("out of memory in fsQuick");

  errP= errRow; /* initialize pointer */

  { register int col;
    for (col=nCols; --col>=0; ) {
      if (fsQ_DEBUG>2) {
	errRow[col]= 0;
      } else {
	/*Start w/ small random errors to avoid some weird systematic effects.*/
	errRow[col]= (fsQ_RANDOM % (fsQ_MAXVAL+1) - fsQ_THRESH_VAL) /8;
      }
    }
  }

#ifdef fsQ_NOSHORTCUT
    /* This is a reality check that does the condition test for every pixel
     as if we weren't sure that pixels in nWholeWords are all in the image. */
    nWholeWords=0;
    nExtraWords=wordsPerRow;
#endif

/* To avoid edge effects at the top of the picture, extend the picture upwards
   by replicating the top row several times.
   Implement this by simply processing the top row several times: */
dorows(fsQ_FIRST_ROW_EXTRAS, );

/* now process the whole actual image: */
dorows(nRows,  bitP += wordsPerRow;  picP += nCols; );

#undef dorows
#undef ditherword
#undef dither1pix

  free(errRow);
}

void fsQFree(fsQTablesP)
     fsQTablesT *fsQTablesP;
{
  free(fsQTablesP);
}
