/* Version Identification:
 * $Id$
 */
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#ifndef OCT_FSYS_H
#define OCT_FSYS_H

#include "io_internal.h"

extern char *fsys_error_message;

#define FSYS_EXISTS	0x01
#define FSYS_CREATABLE	0x02
#define FSYS_LOCKED	0x04
#define FSYS_READABLE	0x10
#define FSYS_WRITABLE	0x20

#define FSYS_CHILD	0x01
#define FSYS_SIBLING	0x02

void oct_init_facet_key_table();

extern int fsys_commit();
extern int fsys_lock();
extern int fsys_unlock();
extern void fsys_print_key();
extern IOFILE *fsys_open();
extern void fsys_close();
extern int fsys_make_key();
extern int fsys_resolve();
extern int fsys_save_key();
extern char *fsys_full_name();
extern int fsys_free_key();
extern int fsys_compare_key();
extern int fsys_hash_key();

#endif /* OCT_FSYS_H */
