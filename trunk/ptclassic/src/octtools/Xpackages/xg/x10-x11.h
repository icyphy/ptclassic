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
#ifndef X10X11_H
#define X10X11_H

#ifdef X11
#define BLACKPIXEL(display)		BlackPixel(display,DefaultScreen(display))
#define WHITEPIXEL(display)		WhitePixel(display,DefaultScreen(display))
#define DEFAULTDEPTH(display)   DefaultDepth(display,DefaultScreen(display))
#define DISPLAYPLANES(display)	DisplayPlanes(display,DefaultScreen(display))
#define XPENDING(display)		XPending(display)
#define XNEXTEVENT(display,event)	XNextEvent(display,event)
#define XMAPWINDOW(display,window)	XMapWindow(display,window)
#define XUNGRABPOINTER(display)	XUngrabPointer(display,CurrentTime)
#define XLOADQUERYFONT(display,name)	XLoadQueryFont(display,name)
#define	XCLEARWINDOW(display,window)	XClearWindow(display,window)
#define XGETDEFAULT(display,prog,opt)	XGetDefault(display,prog,opt)
#define XDEFINECURSOR(display,win,cursor)	XDefineCursor(display,win,cursor)
#define XFLUSH(display)			XFlush(display)
#define XDESTROYWINDOW(display,window)		XDestroyWindow(display,window)
#define XSTORENAME(display,win,name)		XStoreName(display,win,name)
#define XFONTWIDTH(font)\
	(font->max_bounds.rbearing - font->min_bounds.lbearing)
#define XFONTHEIGHT(font)\
	(font->max_bounds.ascent + font->max_bounds.descent)
typedef XPoint			XPOINT;
typedef unsigned long	PIXEL;
typedef XFontStruct		XFONTSTRUCT;
#else
#define BLACKPIXEL(display)					BlackPixel
#define WHITEPIXEL(display)					WhitePixel
#define DISPLAYPLANES(display)				DisplayPlanes()
#define XPENDING(display)					XPending()
#define XNEXTEVENT(display,event)			XNextEvent(event)
#define XMAPWINDOW(display,window)			XMapWindow(window)
#define XUNGRABPOINTER(display)				XUngrabMouse()
#define XLOADQUERYFONT(display,name)		XOpenFont(name)
#define	XCLEARWINDOW(display,window)		XClear(window)
#define XGETDEFAULT(display,prog,opt)		XGetDefault(prog,opt)
#define XDEFINECURSOR(display,win,cursor)	XDefineCursor(win,cursor)
#define XDESTROYWINDOW(display,window)		XDestroyWindow(window)
#define XFLUSH(display)						XFlush()
#define XSTORENAME(display,win,name)		XStoreName(win,name)
#define	ConnectionNumber(display)			dpyno()
#define XAnyEvent							XEvent
#define ButtonRelease						ButtonReleased
#define ButtonPress							ButtonPressed
#define MotionNotify						MouseMoved
#define Expose								ExposeRegion
#define KeyPress							KeyPressed
#define Button1								LeftButton
#define Button2								MiddleButton
#define Button3								RightButton
#define XFONTWIDTH(font)					font->width
#define XFONTHEIGHT(font)					font->height
typedef Vertex		XPOINT;
typedef int			PIXEL;
typedef FontInfo	XFONTSTRUCT;
#endif

#endif
