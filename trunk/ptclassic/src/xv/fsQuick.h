/* fsQuick Copyright (C) 1991 by David B. Rosen.  All Rights Reserved.  This
*  copyright notice applies only to fsQuick, not necessarily to the entire
*  work it is distributed with or used in.  See also accompanying license.
*/

#include "fsQParams.h"

#ifdef __STDC__
#ifndef fsQ_proto
#define fsQ_proto(a) a
#endif /* fsQ_proto */
#else /* __STDC__ */
#ifndef fsQ_proto
#define fsQ_proto(a) ()
#endif /* fsQ_proto */
#undef const
#define	const
#endif /* __STDC__ */

typedef fsQ_BITMAP_TYPE fsQBitmapT;
typedef fsQ_PIC_TYPE fsQPicT;

#ifndef fsQTablesTYPE
#define fsQTablesTYPE void /* none of your business what this is a pointer to */
#endif /* fsQTablesTYPE */
typedef fsQTablesTYPE fsQTablesT;

extern fsQTablesT *fsQInit fsQ_proto((
	int black   /* bit 0 of this is the value of a black bit in bitmap */
	, int IsLSBFirst  /* boolean: is least-significant bit first column? */
));

extern void fsQuick fsQ_proto((
	const fsQPicT *picture /* input picture */
	, const fsQPicT *gamma  /* grayscale map for picture */
	, int nRows, int nCols   /* in picture (row num is most-sig. index) */
	, fsQBitmapT *bitmap  /* output */
	, const fsQTablesT *bitTables  /* output map */
	, int wordsPerRow /* in bitmap ("word" is defined in fsQParams.h) */
));
