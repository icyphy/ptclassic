/* Version Identification:
 * $Id$
 */
/* Copyright (c) 1990-1994 The Regents of the University of California.
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
#ifndef OCT_IO_PROCS_H
#define OCT_IO_PROCS_H
#include "ansi.h"

int oct_put_version
	ARGS((int version));
int oct_get_version
	ARGS((int *version));
int oct_put_type
	ARGS((int type));
int oct_get_type
	ARGS((int *type));
int oct_put_string
	ARGS((char *ptr));
int oct_get_string
	ARGS((char **string));

int oct_put_nl();

int oct_put_byte
	ARGS((int val));
int oct_get_byte
	ARGS((int *valp));
int oct_put_bytes
	ARGS((int32 length, char *bytes));
int oct_get_bytes
	ARGS((int32 *length, char **bytes));

int oct_put_double
	ARGS((double val));
int oct_get_double
	ARGS((double *val));
int oct_old_get_double
	ARGS((double *val));

int oct_get_32
	ARGS((int32 *valp));
int oct_put_32
	ARGS((int32 val));
int oct_get_box
	ARGS((struct octBox *box));
int oct_put_box
	ARGS((struct octBox box));
int oct_get_point
	ARGS((struct octPoint *point));
int oct_put_point
	ARGS((struct octPoint point));
int oct_put_points
	ARGS((int32 length, register struct octPoint *points));
int oct_get_points
	ARGS((int32 *length, register struct octPoint **points));

#endif
