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
/*
 * Lbl
 *
 * David Harrison
 * University of California, Berkley
 * 1990
 *
 * Lbl is a subclass of the Athena Label widget that uses
 * a slightly different method to compute preferred geometry.
 * This new method returns the actual size of the widget when
 * the resize resource is set.  Everything else about the
 * class is inherited.
 *
 * These are the private definitions for the class.
 */

#ifndef _LblP_h
#define _LblP_h

#include "Lbl.h"
#include <X11/Xaw/LabelP.h>

/* No new class fields */
typedef struct {int foo;} LblClassPart;

/* Full class record declaration */
typedef struct _LblClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    LabelClassPart	label_class;
    LblClassPart	lbl_class;
} LblClassRec;

extern LblClassRec lblClassRec;

/* No new instance fields */
typedef struct {int foo;} LblPart;

/* Full instance record */
typedef struct _LblRec {
    CorePart	core;
    SimplePart	simple;
    LabelPart	label;
    LblPart	lbl;
} LblRec;

#endif /* _LblP_h */
