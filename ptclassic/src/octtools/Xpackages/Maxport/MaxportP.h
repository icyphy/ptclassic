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
 * Maxport
 *
 * David Harrison
 * University of California, Berkley
 * 1990
 *
 * Maxport is a subclass of the Athena Viewport widget that allows
 * the caller to specify a maximum width and height.  This is useful
 * if you have a child widget that starts out very large and you
 * would like to force the viewport to scroll initially.
 *
 * These are the private definitions for subclassing the class.
 */

#ifndef _MaxportP_h
#define _MaxportP_h

#include "Maxport.h"
#include <X11/Xaw/ViewportP.h>
#include <X11/Xaw/FormP.h>

typedef struct {int empty;} MaxportClassPart;

typedef struct _MaxportClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ConstraintClassPart	constraint_class;
    FormClassPart	form_class;
    ViewportClassPart	viewport_class;
    MaxportClassPart	maxport_class;
} MaxportClassRec;

extern MaxportClassRec maxportClassRec;

typedef struct _MaxportPart {
    /* resources */
    Dimension max_width;	/* Maximum width  */
    Dimension max_height;	/* Maximum height */
    /* private state */
} MaxportPart;

typedef struct _MaxportRec {
    CorePart	core;
    CompositePart	composite;
    ConstraintPart	constraint;
    FormPart		form;
    ViewportPart	viewport;
    MaxportPart		maxport;
} MaxportRec;

#endif /* _MaxportP_h */
