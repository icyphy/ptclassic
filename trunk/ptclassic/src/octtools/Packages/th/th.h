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
#ifndef TH_H
#define TH_H

#define TH_PKG_NAME "th"


EXTERN void thTechnology
  ARGS( (octObject *obj ));

EXTERN void thVerbose
  ARGS( (int flag ));

    
EXTERN int thGetPropInteger
  ARGS( ( char* name ) );
EXTERN double  thGetPropReal
  ARGS( ( char* name ) );
EXTERN char* thGetPropString
  ARGS( ( char* name ) );

#define  thCoordSize() thGetPropReal("COORD-SIZE")
#define  thLambda()    thGetPropInteger("LAMBDA")


EXTERN double thGetLayerPropReal
  ARGS((octObject* layer, char* name ));

EXTERN double thGetLayerLayerPropReal
  ARGS((octObject* layer1, octObject* layer2, char* name ));

EXTERN double thGetAreaCap
  ARGS((octObject* layer ));

EXTERN double thGetPerimCap
  ARGS((octObject* layer ));

EXTERN double thGetSheetResistance
  ARGS((octObject* layer ));

EXTERN double thGetSheetInductance
  ARGS((octObject* layer ));

EXTERN double thGetContactResistance
  ARGS((octObject* layer ));

EXTERN double thGetThickness
  ARGS((octObject* layer ));

EXTERN char*  thGetLayerClass
  ARGS((octObject* layer ));

#endif
