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
/* $Header$ */

#ifndef ST_INCLUDED
#define ST_INCLUDED

#include "ansi.h"

extern char st_pkg_name[];

/* Fatal error codes */
#define ST_NO_MEM	0
#define ST_BAD_RET	1
#define ST_BAD_GEN	2

typedef struct _st_table_entry st_table_entry;
struct _st_table_entry {
    char *key;
    char *record;
    st_table_entry *next;
};

typedef struct _st_table st_table;
struct _st_table {
    int (*compare)();
    int (*hash)();
    int num_bins;
    int num_entries;
    int max_density;
    int reorder_flag;
    double grow_factor;
    st_table_entry **bins;
};

typedef struct _st_generator st_generator;
struct _st_generator {
    st_table *table;
    st_table_entry *entry;
    int indx;
};

#define st_is_member(table,key) st_lookup(table,key,(char **) 0)
#define st_count(table) ((table)->num_entries)

enum st_retval {ST_CONTINUE, ST_STOP, ST_DELETE};

EXTERN st_table *st_init_table_with_params
  ARGS((int (*compare)(const char *x, const char *y), int (*hash)(char *x, int modulus), int size, int density,
	double grow_factor, int reorder_flag));

EXTERN st_table *st_init_table
  ARGS((int (*compare)(const char *x, const char *y), int (*hash)(char *x, int modulus)));

EXTERN void st_free_table
  ARGS((st_table *table));

EXTERN int st_lookup
  ARGS((st_table *table, char *key, char **value));

EXTERN int st_insert
  ARGS((st_table *table, char *key, char *value));

EXTERN void st_add_direct
  ARGS((st_table *table, char *key, char *value));

EXTERN int st_find_or_add
  ARGS((st_table *table, char *key, char ***slot));

EXTERN int st_find
  ARGS((st_table *table, char *key, char ***slot));

EXTERN st_table *st_copy
  ARGS((st_table *old_table));

EXTERN int st_delete
  ARGS((st_table *table, char **keyp, char **value));

EXTERN int st_foreach
  ARGS((st_table *table, enum st_retval (*func)(), char *arg));

EXTERN int st_strhash
  ARGS((char *string, int modulus));

EXTERN int st_numhash
  ARGS((char *x, int size));

EXTERN int st_ptrhash
  ARGS((char *x, int size));

EXTERN int st_numcmp
  ARGS((const char *x, const char *y));

EXTERN int st_ptrcmp
  ARGS((const char *x, const char *y));

EXTERN st_generator *st_init_gen
  ARGS((st_table *table));

EXTERN int st_gen
  ARGS((st_generator *gen, char **key_p, char **value_p));

EXTERN void st_free_gen
  ARGS((st_generator *gen));

#define ST_DEFAULT_MAX_DENSITY 5
#define ST_DEFAULT_INIT_TABLE_SIZE 11
#define ST_DEFAULT_GROW_FACTOR 2.0
#define ST_DEFAULT_REORDER_FLAG 0

#define st_foreach_item(table, gen, key_p, value_p) \
    for(gen=st_init_gen(table); st_gen(gen,key_p,value_p) || (st_free_gen(gen),0);)

#endif /* ST_INCLUDED */
