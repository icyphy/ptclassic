/* 
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
#endif

#include "topFixup.h"

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"

void
topVecSort( TOPVector *pVec, TOPCompareFunc *compFunc) {
    qsort( pVec->Data, pVec->Num, pVec->ObSize, compFunc);
}

int _topVecSortInt( const void *a, const void *b) {
    int		i = *(int*)a;
    int		j = *(int*)b;
    return (i - j);
}

int _topVecSortDbl( const void *a, const void *b) {
    double	i = *(double*)a;
    double	j = *(double*)b;

    if (i > j)	return 1;
    if (i < j)	return -1;
    return 0;
}
