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
/* $XConsortium: LImageP.h,v 1.2 88/10/25 17:37:59 swick Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#ifndef _LImageP_h
#define _LImageP_h

#include "LImage.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRSqPixmap		"sqPixmap"
typedef struct _SqPixmap {
    unsigned int width, height;
    Pixmap map;
} SqPixmap, *SqPixmapPtr;

/*
 * Class Record
 */

typedef struct {
    int empty;
} LImageClassPart;

/*
 * Class Hierarchy
 */

typedef struct _LImageClassRec {
    CoreClassPart	core_class;
    LImageClassPart	lImage_class;
} LImageClassRec;

extern LImageClassRec lImageClassRec;

typedef struct {
    String line;
    int count;
    int width;
} LILabelLine;

typedef struct {
    /* resources */
    Pixel foreground;
    XFontStruct *font;
    String label;
    SqPixmap image;
    Boolean select;
    XtCallbackList callbacks;
    Dimension padding;
    int click_thres;
    /* private state */
    int num_lines;
    LILabelLine *line_array;
    int max_line_width;
    int all_height;
    Time last_ms;
    int click_count;
} LImagePart;

/*
 * Instance hierarchy
 */

typedef struct _LImageRec {
    CorePart		core;
    LImagePart		lImage;
} LImageRec;

#endif  /* _LImageP_h */
