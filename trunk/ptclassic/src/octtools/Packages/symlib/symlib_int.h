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

#define RAISE_ERROR	errRaise(SYMLIB_PKG_NAME, SYM_ERROR, octErrorString())
  
#define OCT_CHECK(funct) \
  if (funct < OCT_OK) {  \
      RAISE_ERROR;       \
  }

#define SYM_CHECK(funct) \
  if (funct < SYM_OK) {  \
      RAISE_ERROR;	 \
  }

typedef int symStatus;

extern void sym_error_handler();
extern void sym_actual_formal_merge();
extern void sym_merge_nets(/* octObject *net1, *net2, *newnet */);
extern char sym_buffer[4096];

#define SYM_FTERMP(obj)  ((obj)->contents.term.instanceId == oct_null_id)

#define SYM_AVEX(bb) (((bb).lowerLeft.x + (bb).upperRight.x) / 2)
#define SYM_AVEY(bb) (((bb).lowerLeft.y + (bb).upperRight.y) / 2)

#define SYM_ABS(a)   ((a) > 0 ? (a) : -(a))

#define SYM_START	errPushHandler(sym_error_handler)
#define SYM_END		errPopHandler()

