#ifndef PALETTE_H
#define PALETTE_H
/* palette.h  edg
Version identification:
$Id$
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

#ifdef __cplusplus
#include "oct.h" /* for octObject */

extern boolean MkPalIconInPal(octObject *facetPtr, char *dir, char *palName);
extern boolean MkUnivIconInPal(octObject *facetPtr, char *dir, char *palName);
extern boolean MkGalIconInPal(octObject *galFacetPtr, char *dir, char *palName);
extern boolean MkStarIconInPal(char *starName, char *dir, char *palName);
#else
extern boolean MkPalIconInPal();
extern boolean MkUnivIconInPal();
extern boolean MkGalIconInPal();
extern boolean MkStarIconInPal();
#endif /*__cplusplus*/
#endif /* PALETTE_H */
