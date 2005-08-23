/* 
Functions that Display a file

Version identification:
@(#)displayFile.h	1.2 9/4/96

Copyright (c) 1996 The Regents of the University of California.
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
Author: Christopher Hylands 9/3/96
*/

/* Please keep this file free of an include <tcl.h> so that we
 * call it from anywhere
 */

/* Start tycho if it is not already running.
 */
void startTycho();


/*
 * Display a file 
 * We must do the right thing in the following situations:
 * If ptcl is running, then we should check the DISPLAY variable, and
 * if it is set, exec tycho <filename>.
 * If we are inside pigi, we may need to start tycho.
 *  
 * Currently, this function is called from pigilib/icon.c and from
 * cg/kernel/CGUtilties.cc
 *
 * fileName	- The name of the file to open
 * debugFuncPtr - A function that takes a const char * that will print
 *		  out status information about what is going on.  This
 *		  is useful if there is a slight delay in getting things
 *		  started
 * errFuncPtr	- A functino that takes a const char * that will print
 *		  an error message if something goes wrong.
 * If debugFuncPtr or errFuncPtr are NULL, then they are not called.
 */
int displayFile(const char *fileName,
		void (*debugFuncPtr)(const char *),
		void (*errFuncPtr)(const char *));

