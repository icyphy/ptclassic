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
#ifndef PQ_H
#define PQ_H

#include "da.h"
#include "utility.h"
#include "ansi.h"

#define PQ_NAME "pq"

typedef struct heapstruct heap_t;

struct heapstruct {
    dary data;
    int (*compare)();
    int num_elements;
};

typedef struct pq_struct pq_t;

struct pq_struct {
    heap_t *heap;
    int num_keys;
    int num_entries;
};

typedef struct pq_q_entry pq_entry_t;

struct pq_q_entry {
    char *key;
    dary records;
    int num_records;
    int cur_record;
};

typedef struct pq_internal_struct pq_int_t;

struct pq_internal_struct {
    char *key;
    char *data;
};
	

typedef struct pq_gen_struct pq_gen_t;

struct pq_gen_struct {
    pq_entry_t entry;
    int continue_loop, keynum, recordnum;
    pq_int_t *heapcopy;
};


#define pq_foreach(queue, gen, nextkey, value) \
    for(pq_initgen(queue, &gen, (char **)&nextkey, &value);\
	gen.continue_loop;\
	pq_nextgen(queue, &gen, (char **)&nextkey, &value))

#define pq_init1(queue, gen, nextkey, value) \
    (gen.heapcopy = pq_queue_to_array(queue),\
     gen.continue_loop = 1,\
     gen.keynum = queue -> num_entries,\
     gen.recordnum = -1,\
     pq_next1(&gen, &nextkey, &value))

#define pq_foreach1(queue, gen, nextkey, value) \
    for(pq_init1(queue, gen, nextkey, value);\
	gen.continue_loop;\
	pq_next1(&gen, (char **)&nextkey, (char **)&value))

#define pq_break(gen) \
    { \
	gen.continue_loop = 0; \
	break; \
    }
    

int pq_numcmp
	ARGS((char *x, char *y));
int pq_ptrcmp
	ARGS((char *x, char *y));
int pq_rev_numcmp
	ARGS((char *x, char *y));
int pq_rev_ptrcmp
	ARGS((char *x, char *y));
int pq_rev_strcmp
	ARGS((char *x, char *y));

pq_t *pq_init_queue
	ARGS((int (*compare)()));
pq_t *pq_copy
	ARGS((pq_t *old_queue));

void pq_free_queue
	ARGS((pq_t *queue));

int pq_find_entry
	ARGS((register pq_t *queue, register char *key, char *value));

dary pq_find_key
	ARGS((register pq_t *queue, register char *key));

/* Not really exported.  Supports the foreach macro */
pq_int_t * pq_queue_to_array
	ARGS((pq_t *queue));


int pq_insert
	ARGS((register pq_t *queue, register char *key, char *value));
int pq_insert_unique
	ARGS((register pq_t *queue, register char *key, char *value));
int pq_delete
	ARGS((pq_t *queue, char *key, char *value));
int pq_delete_key
	ARGS((pq_t *queue, char *key));

void pq_insert_fast
	ARGS((register pq_t *queue, register char *key, char *value));

int pq_count
	ARGS((pq_t *queue));
int pq_count_under_key
	ARGS((pq_t *queue, char *key));
int pq_empty
	ARGS((pq_t *queue));

int pq_pop
	ARGS((register pq_t *queue, char **result));
int pq_maximal_key
	ARGS((register pq_t *queue, char **key));

void pq_initgen
	ARGS((pq_t *queue, pq_gen_t *gen, char **key, char **data));
void pq_nextgen
	ARGS((pq_t *queue, pq_gen_t *gen, char **key, char **data));

int pq_next1
	ARGS((pq_gen_t *gen, char **key, char **value));

#endif
