/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

#include <stdio.h>

/*
 * Number conversion routines
 */

int *
FixToBinary(Fix, w, d, s)
float Fix;
int *w;
int *d;
int *s;
{
    int IntPart[10];
    int FractPart[10];
    int *Binary;
    int i, j, Number;
    float Fract;

    /* Start first with the integer part */

    if (Fix >= 0) {
	*s = 0;
    }
    else {
	*s = 1;
	Fix = - Fix;
    }
    Number = (int) Fix;
    Fract = Fix - (float) Number;
    i = 0;
    while (Number != 0 && i < 10) {
	IntPart[i++] = Number % 2;
	Number /= 2;
    }
    if (Number != 0) {
	printmsg(NULL, "FixToBinary : overflow\n");
	return (NULL);
    }

    /* Transform the fratcional part */

    j = 0;
    while (Fract != 0 && j < 10) {
	Fract *= 2;
	FractPart[j++] = (int) Fract;
	Fract -= (int) Fract;
    }

    *w = i + j;
    *d = j;
    Binary = (int *) calloc (*w, sizeof (int));

    for (j = 0; j < *d; j++)
	Binary[j] = FractPart[*d-j-1];
    for (j = 0; j < i; j++)
	Binary[*d+j] = IntPart[j];
    return (Binary);
}
