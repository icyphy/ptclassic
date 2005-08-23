#ifndef _Picture_h
#define _Picture_h 1
/**************************************************************************
Version identification:
@(#)Picture.h	1.4  09/01/99

Author: Luis Gutierrez
( based on code for XYPlot written by Edward A. Lee and D. Niehaus)

Defines and maintains a TK XY plot.
It uses the C functions defined in $PTOLEMY/src/ptklib/ptkPlot.c,
but puts an object-oriented wrapper around them.

Copyright (c) 1990-1999 The Regents of the University of California.
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

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

/* Do the right thing for sol2 boolean defs.  sol2compat.h must be included
 * first so sys/types.h is included correctly.  See octtools/include/port.h
 */
/*#include "sol2compat.h"*/

#include "Block.h"
#include "InfString.h"
extern "C" {
#include "ptk.h"
#include "ptkPicture_defs.h"
#include "ptkPicture.h"
}

// picObj will be instantiated by each star that uses a Tk image

class picObj {
public:
	// constructor and destructor
	picObj ();
	~picObj ();

	// Create an image
	// Returns FALSE if setup fails, TRUE otherwise.
	int setup (Block* star,		// The star I am in
		   char* title,         // Title for the display window
		   int height,          // height of picture
		   int width,           // width of picture
		   int palette,        
		   /* specifies the max amount of colors
		      that the widget will try to use to display
		      the image(max is 256)
		   */
		   u_char* buffer,
		   int numinputs);      // 1 = greyscale, 3 = RGB

	// Redraw picture. used when contents of buffer change or picture needs
	// to be redrawn
	int updatePicture (void);

	int resizePicture(int height, int width, u_char* buffer);

	// Window name to use
	InfString winName;
protected:
	Block* myStar;

	// A static variable used to create names that are
	// guaranteed to be unique.  After each use, it should
	// be incremented.
	static int unique;

	// A unique string for each instance of this object identifies
	// the star within which it sits for the benefit of Tcl routines.
	InfString starID;

	//Stores info needed to display the image.
	Picture picture;
private:
	int okFlag;
};
#endif
