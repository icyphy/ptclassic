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

#include "utility.h"
      /* Boolean type */

typedef char bool;
#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL 0
#endif

      /* Allocation */

/*
 * NEW(type)
 *     A pointer to a new (heap) object of given type.
 */

#define NEW(type) ((type *) malloc(sizeof(type)))

/*
 * NEWVEC(type,n)
 *     A pointer to a new (heap) vector of n objects of given type.
 */

#define NEWVEC(type,n) ((type *) calloc((unsigned) (n), sizeof(type)))

    /* Infinite loop macro */

#define loop for(;;)


    /* Copying structures and arrays */

#define COPY(to,from) (void) (memcpy(&(to), &(from), sizeof(from)))
#define COPYVEC(to,from,n) (void) (memcpy((to), (from), \
					 sizeof((from)[0])*(n)))


