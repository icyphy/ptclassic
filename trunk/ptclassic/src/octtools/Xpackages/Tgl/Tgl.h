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
 * Tgl - New toggle widget
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 */

#ifndef _XtTgl_h
#define _XtTgl_h

/* Resources:

Name		Class		Type			Description
--------------------------------------------------------------------------------
accelerators	Accelerators	TranslationTable NULL
background	Background	Pixel		XtDefaultBackground
backgroundPixmap Pixmap		Pixmap		XtUnspecifiedPixmap
border		BorderColor	Pixel		XtDefaultForeground
borderPixmap	Pixmap		Pixmap		XtUnspecifiedPixmap
borderWidth	BorderWidth	Dimension	1
callback	Callback	Pointer		NULL
colormap	Colormap	Pointer		Parent's colormap
current		Current		Widget		NULL
cursor		Cursor		Cursor		None
depth		Depth		int		Parent's depth
destroyCallback Callback	Pointer		NULL
foreground	Foreground	Pixel		XtDefaultForeground
height		Height		Dimension	text height
hisetPixmap	Pixmap		TgPixmap	HighSetCheckMark
hiunsetPixmap	Pixmap		TgPixmap	HighUnSetCheckMark
insensitiveBorder Insensitive	Pixmap		Gray
justify	        Justify		XtJustify	XtJustifyCenter
leader		Leader		Widget		NULL
mappedWhenManaged MappedWhenManaged Boolean	True
screen		Screen		Pointer		Parent's Screen
sensitive	Sensitive	Boolean		True
setPixmap	Pixmap		TgPixmap	SetCheckMark
state		State		Boolean		False
translations	Translations	TranslationTable See below
unsetPixmap	Pixmap		TgPixmap	UnsetCheckMark
width		Width		Dimension	text width
x	     	Position	Position	0
y	     	Position	Position	0

*/

#define XtNstate		"state"
#define XtNhisetPixmap		"hisetPixmap"
#define XtNhiunsetPixmap	"hiunsetPixmap"
#define XtNsetPixmap		"setPixmap"
#define XtNunsetPixmap		"unsetPixmap"
#define XtNleader		"leader"
#define XtNcurrent		"current"

#define XtCState		"State"
#define XtCLeader		"Leader"
#define XtCCurrent		"Current"

/*
 * This should really be defined in StringDefs.h but isn't
 */
#ifndef XtRWidget
#define XtRWidget		"Widget"
#endif

#define XtRTgPixmap	"tgPixmap"
typedef struct _TgPixmap {
    unsigned int width, height;
    Pixmap map;
} TgPixmap, *TgPixmapPtr;

/* Actually PixDataPtr */
#define XtRPixdata	"pixData"

typedef struct _PixData {
    unsigned int width, height;
    char *data;
} PixData, *PixDataPtr;

extern WidgetClass tglWidgetClass;

typedef struct _TglClassRec   *TglWidgetClass;
typedef struct _TglRec        *TglWidget;

/*
 * Public Access Routines
 */

extern void XtTgResetLeader();
  /* Widget w; */

#endif /* _XtTgl_h */
