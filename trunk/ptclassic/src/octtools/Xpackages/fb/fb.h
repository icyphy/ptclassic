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
 * Top level interface for graphic file browser
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * These are the external definitions for the file browser.
 * This is an X toolkit library and requires the athena
 * widget set.
 */

#ifndef _BROWSER_H_
#define _BROWSER_H_

#include "ansi.h"
#include <X11/Intrinsic.h>

typedef struct fb_item_defn {
    String class;		/* Defines the image          */
    String name;		/* Name displayed under image */
    caddr_t data;		/* User specific data         */
} fb_item;

extern Widget fb_create
  ARGS((Widget parent, int num_items, fb_item items[],
	void (*callback) ARGS((String class, String label,
			       caddr_t item_data, caddr_t global_data)),
	caddr_t data));
  /* Creates a new file browser */

extern void fb_update
  ARGS((Widget fb, int num_items, fb_item items[]));
  /* Updates an existing browser */

#endif /* _BROWSER_H_ */
