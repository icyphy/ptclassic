/* Version Identification:
 * $Id$
 */
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/* This file must be parsable by a regular C compiler.  */
#ifndef VOV_H
#define VOV_H

/* #pragma once */   /* This is not portable yet (June 91) */

#include "port.h"		/* Portability package. */
#include "ansi.h"		/* This defines the macro ARGS(). */


/*
**  Macros to refer to inputs and/or to outputs of nodes. 
*/
#define VOV_INPUT_MASK   0x1
#define VOV_OUTPUT_MASK  0x2
#define VOV_IOS_MASK     (VOV_INPUT_MASK | VOV_OUTPUT_MASK)


/*
**  TYPES OF PLACES KNOWN TO VOV.
*/
typedef enum _vovPlaceType {
    VOV_FIRST_PLACE_TYPE,	/* Sentinel value. */
    VOV_UNIX_FILE,
    VOV_OCT_FACET,
    VOV_PROGRAM_OPTION,
    VOV_EXECUTABLE,
    VOV_EXIT_STATUS,
    VOV_BOOLEAN,
    VOV_MEASURE,		
    VOV_LAST_PLACE_TYPE		/* Sentinel value. */
  } VovPlaceType;



/*
**  EXPORTED FUNCTION:
*/
EXTERN int   VOVbegin
  ARGS( ( int argc, char* argv[] ) );

EXTERN int   VOVaffinity 
  ARGS( (char* affinity ) );

EXTERN int   VOVinput
  ARGS ( ( VovPlaceType type , char* name ) );

EXTERN int   VOVoutput
  ARGS( ( VovPlaceType type , char* name ) );

EXTERN int   VOVdelete
  ARGS( ( VovPlaceType type , char* name ) );

EXTERN void  VOVmeasure
  ARGS( ( int placeId, char*  name, char* unit, int value ) );

EXTERN void  VOVend
  ARGS( ( int status ) );	/* Calls exit!! */

EXTERN void  _VOVend
  ARGS( ( int status ) );	/* Does not exit */

EXTERN int   VOVdisable
  ARGS( () );			/* Disable all tracing fuctions for this process. */

EXTERN void  VOVredirect
  ARGS( (int f ) );		/* Set redirect flag to control stdout/stderr  */

EXTERN FILE* VOVfopen
  ARGS((char* name,char* mode)); /* Utility function, good to replace most fopen() */



#ifdef OCT_H
/* These functions are useful only if you use oct.h */
EXTERN int   VOVinputFacet
  ARGS( ( octObject* facet ) );

EXTERN int   VOVoutputFacet
  ARGS ( ( octObject* facet ) );
#endif 



#ifdef __cplusplus
//
// The C++ library provides the same functions as the C library,
// but the prefix VOV__ is used in place of VOV.
//
//  Extern class declaration make sense only in C++.
extern class String;

extern int   VOV__begin ARGS( ( int argc, char* argv[] ) );
extern int   VOV__affinity ARGS( ( char* affinity ) );
extern int   VOV__input ARGS ( ( VovPlaceType type , char* name ) );
extern int   VOV__output ARGS( ( VovPlaceType type , char* name ) );
extern int   VOV__delete ARGS( ( VovPlaceType type , char* name ) );
extern void  VOV__measure ARGS( ( int placeId, char*  name, char* unit, int value ) );
extern void  VOV__end ARGS( ( int status ) ); 
extern void  _VOV__end ARGS( ( int status ) );
extern int   VOV__disable();
extern void  VOV__redirect ARGS( (int f ));
extern FILE* VOV__fopen ARGS( (char* name, char* mode ) );

#ifdef OCT_H
extern int   VOV__inputFacet ARGS( ( octObject* facet ) );
extern int   VOV__outputFacet ARGS ( ( octObject* facet ) );
#endif

#endif				/* C++ functions. */




#endif				/* VOV_H */



