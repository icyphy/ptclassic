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
 * Table - Forms-based composite widget/geometry manager for the X Toolkit
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains the Table public declarations.
 */
 
#ifndef _Table_h
#define _Table_h

/*
 * Table Widget Parameters
 *
 * Name			Class		RepType		Default Value
 *
 * background		Background	Pixel		XtDefaultBackground
 * border		BorderColor	Pixel		XtDefaultForeground
 * borderWidth		BorderWidth	Dimension	0
 * cursor		Cursor		Cursor		None
 * x			Position	Position	0
 * y			Position	Position	0
 * width		Width		Dimension	(computed)
 * height		Height		Dimension	(computed)
 * mappedWhenManaged	MappedWhenManaged Boolean	True
 * sensitive		Sensitive	Boolean		True
 * layout		Layout		String		NULL
 * internalHeight	Height		Dimension	0
 * internalWidth	Width		Dimension	0
 * columnSpacing	Spacing		Dimension	0
 * rowSpacing		Spacing		Dimension	0
 */

#define XtNlayout		"layout"
#define XtNcolumnSpacing        "columnSpacing"
#define XtNrowSpacing           "rowSpacing"
#define XtNdefaultOptions	"defaultOptions"
#ifndef XtNcursor
#define XtNcursor		"cursor"
#endif

#define XtCLayout		"Layout"
#ifndef XtCSpacing
#define XtCSpacing		"Spacing"
#endif
#define XtCOptions		"Options"

#define XtROptions		"Options"

/*
 * Option masks
 */
#define TBL_LEFT	(1<<0)
#define TBL_RIGHT	(1<<1)
#define TBL_TOP		(1<<2)
#define TBL_BOTTOM	(1<<3)
#define TBL_SM_WIDTH	(1<<4)
#define TBL_SM_HEIGHT	(1<<5)	
#define TBL_LK_WIDTH	(1<<6)
#define TBL_LK_HEIGHT	(1<<7)

#define TBL_DEF_OPT	-1

typedef int XtTblMask;

/*
 * Opaque class and instance records
 */

typedef struct _TableClassRec	*TableWidgetClass;
typedef struct _TableRec	*TableWidget;

extern WidgetClass tableWidgetClass;

/*
 * Public access routines
 */

extern caddr_t XtTblParseLayout();
  /* String layout; */

extern void XtTblPosition();
  /* 
   * Widget w;
   * Position col, row;
   */

extern void XtTblResize();
  /*
   * Widget w;
   * Dimension h_span, v_span;
   */

extern void XtTblOptions();
  /*
   * Widget w;
   * XtTblMask opt;
   */

extern void XtTblConfig();
  /* 
   * Widget w;
   * Position col, row;
   * Dimension h_span, v_span;
   * XtTblMask opt;
   */
 
#endif /* _Table_h */

