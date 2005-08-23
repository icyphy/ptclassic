/*
 * Header file for Tk canvas-based picture display facility
 * Author:Luis Gutierrez (based on ptkPlot )
 * @(#)ptkPicture_defs.h	1.1  06/25/97
 */
#ifndef _ptkPicture_defs_h
#define _ptkPicture_defs_h 1

/*
Copyright (c) 1990-1997 The Regents of the University of California.
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

#include <string.h>
#include <stdlib.h>
#include "tk.h"

typedef struct Picture {  
  char* name;
  char* title;
  Tk_PhotoImageBlock photoblock;
  Tk_PhotoHandle photohandle;
  int scaled_height;
  int scaled_width;
  int downsmplx, upsmplx;
  int downsmply, upsmply;
  int palette;
} Picture;

#endif
