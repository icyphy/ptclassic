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
#ifndef PQ_H
#define PQ_H

#include "da.h"
#include "utility.h"

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
    


int pq_numcmp(), pq_ptrcmp(), pq_rev_numcmp(), pq_rev_ptrcmp(), pq_rev_strcmp();

pq_t *pq_init_queue(), *pq_copy();
void pq_free_queue();

int pq_find_entry();

dary pq_find_key();

pq_int_t *pq_queue_to_array();

int pq_insert(), pq_insert_unique(), pq_delete(), pq_delete_key();

void pq_insert_fast();

int pq_count(), pq_count_under_key(), pq_empty();

int  pq_pop(), pq_maximal_key();

void pq_initgen(), pq_nextgen();

#endif
