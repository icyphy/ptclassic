#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "io_internal.h"
#include "io.h"
#include "io_procs.h"
#include "oct_utils.h"

#define IS_SPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\n')
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')

extern IOFILE *oct_facet_file;
extern int oct_binary_format;

static char *io_write_failed = "IO error: %s";
static char *io_corrupt_facet = "Corrupt facet";

/*
 *  UNSAFE MACRO to output an integer.  
 *  fp should be a "register IOFILE *" for best performance.
 *  Unsafe to pass v, byte0, byte1, byte2, byte3, cnt, negative, l, temp !!
 */

#define OUTPUT_INT_MACRO(fp, value) {\
    if (oct_binary_format) {\
	register int byte0, byte1, v;\
	int negative, byte2, byte3, cnt;\
	negative = value < 0;\
	v = negative ? -value : value;\
	byte3 = ((unsigned) v >> 24) & 255;\
	byte2 = ((unsigned) v >> 16) & 255;\
	byte1 = ((unsigned) v >> 8) & 255;\
	byte0 = v & 255;\
	if (byte3 != 0) cnt = 4; else if (byte2 != 0) cnt = 3;\
	else if (byte1 != 0) cnt = 2; else if (byte0 != 0) cnt = 1;\
	else cnt = 0;\
	if (negative) {IOputc(cnt+5, fp);} else {IOputc(cnt, fp);}\
	switch(cnt) {\
	    case 4: IOputc(byte3, fp);\
	    case 3: IOputc(byte2, fp);\
	    case 2: IOputc(byte1, fp);\
	    case 1: IOputc(byte0, fp);\
	}\
    } else {\
	register int l, div, v;\
	int negative, temp[60];\
	negative = value < 0;\
	if (negative) IOputc('-', fp);\
	v = negative ? -value : value;\
	l = 0;\
	do {div = v/10; temp[l++] = (v-div*10)+'0'; v = div;} while (v>0);\
	while (--l >= 0) IOputc(temp[l], fp);\
	IOputc(' ', fp);\
    }\
    if (IOferror(fp)) {\
	oct_error(io_write_failed, sys_msg());\
	return 0;\
    }\
}
/*	l = 0; do temp[l++] = (v % 10) + '0'; while ((v /= 10) > 0);*/

/*
 *  UNSAFE MACRO to input an integer
 *  fp should be a "register IOFILE *" for best performance.
 *  Unsafe to pass v, negative, c !!
 */

#define INPUT_INT_MACRO(fp, valp) {\
    if (oct_binary_format) {\
	register int v = 0;\
	switch(IOgetc(fp)) {\
	    case 9: v = IOgetc(fp);\
	    case 8: v = (v << 8) + IOgetc(fp);\
	    case 7: v = (v << 8) + IOgetc(fp);\
	    case 6: v =  - ((v << 8) + IOgetc(fp));\
		break;\
	    case 4: v = IOgetc(fp);\
	    case 3: v = (v << 8) + IOgetc(fp);\
	    case 2: v = (v << 8) + IOgetc(fp);\
	    case 1: v = (v << 8) + IOgetc(fp);\
	    case 0: break;\
	    default: oct_error(io_corrupt_facet);\
		return 0;\
	}\
	*valp = v;\
    } else {\
	register int c, v = 0;\
	int negative;\
	do c = IOgetc(fp); while (IS_SPACE(c));\
	negative = c == '-';\
	if (negative) c = IOgetc(fp);\
	while (IS_DIGIT(c)) {\
	    v = 10*v + c - '0';\
	    c = IOgetc(fp);\
	}\
	*valp = negative ? -v : v;\
    }\
    if (IOfeof(fp)) {\
	oct_error(io_corrupt_facet);\
	return 0;\
    }\
}

#ifdef PROFILE
OUTPUT_INT(fp, value)
register IOFILE *fp;
int value;
{    OUTPUT_INT_MACRO(fp, value);}


INPUT_INT(fp, valuep)
register IOFILE *fp;
int *valuep;
{    INPUT_INT_MACRO(fp, valuep);}

#else

#define OUTPUT_INT	OUTPUT_INT_MACRO
#define INPUT_INT	INPUT_INT_MACRO
#endif


int
oct_put_version(version)
int version;
{
    if (IOfprintf(oct_facet_file, "%06d\n", version) == EOF) {
	oct_error(io_write_failed, sys_msg());
        return 0;
    }
    return 1;
}

int
oct_get_version(version)
int *version;
{
    int c;

    c = IOgetc(oct_facet_file);
    if (IS_DIGIT(c)) {
	(void) IOungetc(c,oct_facet_file);
	if (IOfscanf(oct_facet_file, "%d\n", version) != 1) {
	    oct_error(io_write_failed, sys_msg());
	    return 0;
	}
#if defined(aiws) && !defined(_IBMR2)
	/* AIX has a bug and does not eat the newline */
	(void) IOgetc(oct_facet_file);
#endif
	return 1;
    } else if (c == 'O') {
	/* hack - this may be an old (version 0) ascii oct file) */
	*version = 0;
	(void) IOungetc(c,oct_facet_file);
        return 1;
    }
    return 0;
}

int
oct_put_type(type)
int type;
{
    if (oct_binary_format) {
	(void) IOputc(type, oct_facet_file);
    } else {
	(void) IOfputs(oct_type_names[type], oct_facet_file);
	(void) IOputc(' ', oct_facet_file);
    }
    if (IOferror(oct_facet_file)) {
	oct_error(io_write_failed, sys_msg());
	return 0;
    }
    return 1;
}

int
oct_get_type(type)
int *type;
{
    register IOFILE *fp = oct_facet_file;
    register int count, c;
    static int last = OCT_UNDEFINED_OBJECT;
    static char buffer[128];

    if (oct_binary_format) {
	*type = IOgetc(fp);
	if (*type <= OCT_MAX_TYPE && *type >= OCT_UNDEFINED_OBJECT) {
	    return 1;
	}
    } else {
	/* skip leading white space */
	while ((c = IOgetc(fp)) != EOF && IS_SPACE(c)) {
	}
	if (c != EOF) {
	    /* fetch the field until a space seperator */
	    buffer[0] = c;
	    count = 1;
	    while ((c = IOgetc(fp)) != EOF && ! IS_SPACE(c)) {
		buffer[count++] = c;
	    }
	    buffer[count] = '\0';
	    if (c != EOF) {
		if (strcmp(oct_type_names[last]+4, buffer+4) == 0) {
		    *type = last;
		    return 1;
		}
		for(*type=0; oct_type_names[*type] != NIL(char); (*type)++) {
		    if (strcmp(oct_type_names[*type]+4, buffer+4) == 0) {
			last = *type;
			return 1;
		    }
		}
	    }
	}
    }
    *type = -1;
    oct_error(io_corrupt_facet);
    return 0;
}

int
oct_put_string(ptr)
char *ptr;
{
    if (ptr == NIL(char)) ptr = "";
    if (oct_binary_format) {
	if (! oct_put_32((int32) strlen(ptr))) {
	   return 0;
	}
	if (ptr[0] != '\0') {
	    (void) IOfputs(ptr, oct_facet_file);
	}
    } else {
	(void) IOputc('"', oct_facet_file);
	(void) IOfputs(ptr, oct_facet_file);
	(void) IOputc('"', oct_facet_file);
	(void) IOputc(' ', oct_facet_file);
    }
    if (IOferror(oct_facet_file)) {
	oct_error(io_write_failed, sys_msg());
	return 0;
    }
    return 1;
}

int
oct_get_string(string)
char **string;
{
    register int c;
    register IOFILE *fp = oct_facet_file;
    register char *ptr;
    int32 count;
    static char *string_buffer = NIL(char);
    static unsigned string_buffer_size = 0;
    
    if (oct_binary_format) {
	if (! oct_get_32(&count)) {
	    return 0;
	}
	if (count + 1 > string_buffer_size) {
	    string_buffer_size = count + 100;
	    FREE(string_buffer);
	    string_buffer = ALLOC(char, string_buffer_size);
	}
	
	for(c = 0; c < count; c++) {
	    string_buffer[c] = IOgetc(fp);
	}
	string_buffer[count] = '\0';
	*string = oct_str_intern(string_buffer);
    } else {

	/* skip leading white space, must start with quote */
	while ((c = IOgetc(fp)) != EOF && IS_SPACE(c)) {
	}
	if (c != '"') {
	    oct_error(io_corrupt_facet);
	    return 0;
	}
	
	c = IOgetc(fp);
	if (c == '"') {
	    *string = NIL(char);
	} else {
	    if (string_buffer_size == 0) {
		string_buffer_size = 128;
		string_buffer = ALLOC(char, string_buffer_size+1);
	    }
	    
	    count = string_buffer_size;
	    ptr = string_buffer;
	    while (c != EOF && c != '"') {
		if (--count < 0) {
		    count = string_buffer_size;
		    string_buffer_size += count;
		    string_buffer =
		      realloc(string_buffer, (unsigned) (string_buffer_size+1));
		    ptr = string_buffer + string_buffer_size;
		}
		*ptr++ = c;
		c = IOgetc(fp);
	    }
	    
	    if (c == EOF) {
		oct_error(io_corrupt_facet);
		return 0;
	    }

	    *ptr = '\0';
	    *string = oct_str_intern(string_buffer);
	}
    }
    return 1;
}

int
oct_put_nl()
{
    if (! oct_binary_format) {
	IOputc('\n', oct_facet_file);
	if (IOferror(oct_facet_file)) {
	    oct_error(io_write_failed, sys_msg());
	    return 0;
	}
    }
    return 1;
}

int
oct_put_32(val)
int32 val;
{
    register IOFILE *fp = oct_facet_file;
    OUTPUT_INT(fp, val);
    return 1;
}

int
oct_get_32(valp)
int32 *valp;
{
    register IOFILE *fp = oct_facet_file;
    INPUT_INT(fp, valp);
    return 1;
}

int
oct_put_double(val)
double val;
{
    if (IOfprintf(oct_facet_file, "%gG", val) == EOF) {
	oct_error(io_write_failed, sys_msg());
	return 0;
    }
    return 1;
}

int
oct_old_get_double(val)
double *val;
{
    if (IOfscanf(oct_facet_file, "%lf%*1[ ]", val) != 1) {
	oct_error(io_corrupt_facet);
	return 0;
    }
    return 1;
}

int
oct_get_double(val)
double *val;
{
    if (IOfscanf(oct_facet_file, "%lfG", val) != 1) {
	oct_error(io_corrupt_facet);
	return 0;
    }
    return 1;
}

int
oct_put_byte(val)
int val;
{
    register IOFILE *fp = oct_facet_file;
    if (oct_binary_format) {
	IOputc(val, fp);
	if (IOferror(fp)) {
	    oct_error(io_write_failed, sys_msg());
	    return 0;
	}
    } else {
	OUTPUT_INT(fp, val);
    }
    return 1;
}

int
oct_get_byte(valp)
int *valp;
{
    register IOFILE *fp = oct_facet_file;
    if (oct_binary_format) {
	*valp = IOgetc(fp);
    } else {
	INPUT_INT(fp, valp);
    }
    return 1;
}

int
oct_put_bytes(length, bytes)
int32 length;
char *bytes;
{
    int i;
    
    if (! oct_put_32(length)) {
	oct_error(io_write_failed, sys_msg());
	return 0;
    }
    for(i = 0; i < length; i++) {
	if (! oct_put_byte(bytes[i])) {
	    oct_error(io_write_failed, sys_msg());
	    return 0;
	}
    }
    return 1;
}


int
oct_get_bytes(length, bytes)
int32 *length;
char **bytes;
{
    int i, intval;
    char *ptr;
    
    if (! oct_get_32(length)) {
	oct_error(io_corrupt_facet);
	return 0;
    }
    if (*length == 0) {
	*bytes = NIL(char);
	return 1;
    }
    if (*length < 0) {
	oct_error("negative length in oct_get_bytes");
	return 0;
    }
    *bytes = ALLOC(char, *length);
    ptr = *bytes;
    for(i = 0; i < *length; i++) {
	if (! oct_get_byte(&intval)) {
	    oct_error(io_corrupt_facet);
	    return 0;
	}
	*ptr++ = intval;
    }
    return 1;
}

int
oct_put_points(length, points)
int32 length;
register struct octPoint *points;
{
    int i;
    register IOFILE *fp = oct_facet_file;
    
    OUTPUT_INT(fp, length);
    for(i = 0; i < length; i++) {
        OUTPUT_INT(fp, points[i].x);
        OUTPUT_INT(fp, points[i].y);
    }
    return 1;
}


int 
oct_get_points(length, points)
int32 *length;
register struct octPoint **points;
{
    int i;
    register IOFILE *fp = oct_facet_file;
    
    INPUT_INT(fp, length);
    if (*length == 0) {
	*points = NIL(struct octPoint);
	return 1;
    }
    
    *points = ALLOC(struct octPoint, *length);
    for(i = 0; i < *length; i++) {
	INPUT_INT(fp, &((*points)[i].x));
	INPUT_INT(fp, &((*points)[i].y));
    }
    return 1;
}

#define SMALL
#ifdef SMALL
int
oct_put_box(box)
struct octBox box;
{
    return oct_put_32(box.lowerLeft.x) &&
      oct_put_32(box.lowerLeft.y) &&
      oct_put_32(box.upperRight.x) &&
      oct_put_32(box.upperRight.y);
}

int
oct_get_box(box)
struct octBox *box;
{
    return oct_get_32(&(box->lowerLeft.x)) &&
      oct_get_32(&(box->lowerLeft.y)) &&
      oct_get_32(&(box->upperRight.x)) &&
      oct_get_32(&(box->upperRight.y));
}


int
oct_put_point(point)
struct octPoint point;
{
    return oct_put_32(point.x) &&
      oct_put_32(point.y);
}

int
oct_get_point(point)
struct octPoint *point;
{
    return oct_get_32(&(point->x)) &&
      oct_get_32(&(point->y));
}
#else
int
oct_put_box(box)
struct octBox box;
{
    register IOFILE *fp = oct_facet_file;
    OUTPUT_INT(fp, box.lowerLeft.x);
    OUTPUT_INT(fp, box.lowerLeft.y);
    OUTPUT_INT(fp, box.upperRight.x);
    OUTPUT_INT(fp, box.upperRight.y);
    return 1;
}


oct_get_box(box)
struct octBox *box;
{
    register IOFILE *fp = oct_facet_file;
    INPUT_INT(fp, &(box->lowerLeft.x));
    INPUT_INT(fp, &(box->lowerLeft.y));
    INPUT_INT(fp, &(box->upperRight.x));
    INPUT_INT(fp, &(box->upperRight.y));
    return 1;
}


int
oct_put_point(point)
struct octPoint point;
{
    register IOFILE *fp = oct_facet_file;
    OUTPUT_INT(fp, point.x);
    OUTPUT_INT(fp, point.y);
    return 1;
}

oct_get_point(point)
struct octPoint *point;
{
    register IOFILE *fp = oct_facet_file;
    INPUT_INT(fp, &(point->x));
    INPUT_INT(fp, &(point->y));
    return 1;
}
#endif
