/* Version Identification:
 * $Id$
 */
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*
 * Tgl - subclass of Command for supporting toggle buttons
 *
 * This file contains the private declarations for Toggle.  It
 * should only be used if you are subclassing this widget.
 */

#ifndef _XtTglP_h
#define _XtTglP_h

#include <X11/Xaw/SimpleP.h>
#include "Tgl.h"

/*
 * Local Structures
 */

/*
 * Class Record
 */

typedef struct {
    int dummy;  	/* not used */
} TglClassPart;

/*
 * Class hierarchy
 */

typedef struct _TglClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    TglClassPart	tgl_class;
} TglClassRec;

extern TglClassRec tglClassRec;

/*
 * Information in instance record
 */

typedef struct _TglPart {
    /* Resources */
    Pixel foreground;		/* Foreground color  */
    XtCallbackList callbacks;	/* Callback functions */
    Boolean state;		/* Current set state */
    Widget leader;		/* Tgl leader     */
    /* All depth one */
    TgPixmap hisetPixmap;	/* Highlight and set */
    TgPixmap hiunsetPixmap;	/* Highlight and unset */
    TgPixmap setPixmap;		/* Set pixmap        */
    TgPixmap unsetPixmap;	/* Unset pixmap      */
    /* Drawing state */
    Boolean highlight;		/* Highlight flag    */
    Boolean prev_state;		/* Previous state    */
    XtJustify justify;		/* Justification     */
    /* Just for leader */
    Widget current;		/* Current set widget */
    Widget prev_current;	/* Previous current   */
} TglPart;

/*
 * Instance hierarchy
 */

typedef struct _TglRec {
    CorePart         core;
    SimplePart	     simple;
    TglPart	     tgl;
} TglRec;

#endif /* _XtCommandP_h */


