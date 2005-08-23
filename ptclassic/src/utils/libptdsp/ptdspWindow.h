#ifndef _ptdspWindow_h
#define _ptdspWindow_h 1

/*
Copyright (c) 1990-1996 The Regents of the University of California.
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

Programmer: Kennard White and Brian Evans
Version: @(#)ptdspWindow.h	1.4	7/23/96
*/

#ifdef __cplusplus
extern "C" {
#endif

#define PTDSP_WINDOW_TYPE_NULL			0
#define PTDSP_WINDOW_TYPE_RECTANGLE		1
#define PTDSP_WINDOW_TYPE_BARTLETT		2
#define PTDSP_WINDOW_TYPE_HANNING		3
#define PTDSP_WINDOW_TYPE_HAMMING		4
#define PTDSP_WINDOW_TYPE_BLACKMAN		5
#define PTDSP_WINDOW_TYPE_STEEPBLACKMAN		6
#define PTDSP_WINDOW_TYPE_KAISER		7

extern int Ptdsp_WindowNumber(const char* windowstr);
extern int Ptdsp_Window(double* windowTaps, int length,
		       int windowNumber, double* parameters);

#ifdef __cplusplus
}
#endif

#endif
