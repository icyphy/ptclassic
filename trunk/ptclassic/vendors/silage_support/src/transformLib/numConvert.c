/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

#include <stdio.h>
#define LENGTH 20

#define invert(x) \
    ((x == -1) ? (x = 1) : ((x == 1) ? (x = -1) : (x = 0)))

/*
 * Number conversion routines
 */

int *
FixToBinary(Fix, w, d)
float Fix;
int w;
int d;
{
    int *IntPart;
    int *FractPart;
    int *Binary;
    int i, j, sign, Number;
    float Fract;

    /* Start first with the integer part */

    if (Fix >= 0) {
	sign = 0;
    }
    else {
	sign = 1;
	Fix = - Fix;
    }
    Number = (int) Fix;
    Fract = Fix - (float) Number;

    /* Transform the integer part */

    IntPart =  (int *) calloc(w-d, sizeof(int));
    for (i = 0; i < w-d; i++) {
	IntPart[i] = Number % 2;
	Number /= 2;
    }
    if (Number != 0) {
	printmsg(NULL, "FixToBinary : overflow\n");
	return (NULL);
    }

    /* Transform the fractional part */

    FractPart = (int *) calloc(d, sizeof(int));
    for (j = 0; j < d; j++) {
	Fract *= 2;
	FractPart[j] = (int) Fract;
	Fract -= (int) Fract;
    }

    Binary = (int *) calloc (w, sizeof (int));

    for (j = 0; j < d; j++)
	Binary[j] = (sign == 0) ? FractPart[d-j-1] :
				  invert(FractPart[d-j-1]);
    for (j = 0; j < w - d; j++)
	Binary[d+j] = (sign == 0) ? IntPart[j] :
				  invert(IntPart[j]);

    cfree(IntPart);
    cfree(FractPart);

    /* Check if binary is different from zero */
    for (j = 0; j < w; j++)
        if (Binary[j] != 0)
            break;
 
    if (j == w) {
	cfree(Binary);
        return (NULL);
    }

    return (Binary);
}

int *
BinaryToCSD(Binary, w, d, MaxNonZeroBits)
int *Binary;
int *w, *d;
int MaxNonZeroBits;
{
    int *Result;
    int *NewBinary;
    int i,j,k;
    int NonZeroes;

    /* Transform binary number into representation with minimal CSD notation */
    /* The Maximum Number of Non-ZeroBits is limited by MaxNonZeroBits 
       (if set) */

    Result = (int *) calloc(*w+1, sizeof(int));

    for (i = 0; i < *w; i++)
	Result[i] = Binary[i];
    Result[*w] = 0;
    cfree(Binary);

    i = 0;
    while (i < *w) {
	if (Result[i] == 0 || Result[i+1] == 0) {
	    i++;
	    continue;
	}
	/* find first different bit */
	for (j = i + 1; j <= *w; j++)
	    if (Result[j] != Result[i])
		break;
	if (Result[j] == 0) {
	    Result[j] = Result[i];
	    Result[i] = invert(Result[i]);
	    for (k = i + 1; k < j; k++)
		Result[k] = 0;
	    i = j;
	}
	else {
	    Result[i] = Result[j];
	    for (k = i + 1; k <= j; k++)
		Result[k] = 0;
	    i = j + 1;
	}
    }

    /* Clear redundant non-zero-bits (as set by MaxNonZeroBits) */
    if (MaxNonZeroBits == -1) {
	*w++;
	return (Result);
    }

    NonZeroes = 0;
    for (i = *w; i >= 0; i--) {
	if (Result[i] == 0)
	    continue;
	if (NonZeroes >= MaxNonZeroBits)
	    Result[i] = 0;
	else
	    NonZeroes++;
    }
    *w++;

    return (Result);
}
