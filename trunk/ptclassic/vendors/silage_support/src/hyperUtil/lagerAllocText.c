/* $Id$ */

/* Copyright (C) 1988 Regents of the University of California
 * All rights reserved.
 */
/* lagerAllocText.c */

/*LINTLIBRARY*/

/*
 *
 *  EXTERNAL FUNCTIONS:
 *  char *
 *  lagerAllocText( char * ): allocates and stores the string of
 *      characters and returns a pointer to the new string.
 *
 *  lagerFreeText( char * ): de-allocates a piece of text.
 */

#include "port.h"

char *
lagerAllocText( text )
    char *text;
{
    char *malloc( ), *strcpy( );
    char *ptr;

    if ( text == NULL ) return( lagerAllocText( "\0" ) );

    ptr = malloc( (unsigned) (1 + strlen(text) * sizeof(char)  ) );

    strcpy( ptr,text );

    return( ptr );

} /* lagerAllocText( ) */


char *
lagerFreeText( text )
    char *text;
{
    free( text );

    return( NULL );

} /* lagerFreeText() */
