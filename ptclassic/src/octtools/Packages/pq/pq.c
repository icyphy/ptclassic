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
#include "da.h"
#include "utility.h"
#include "errtrap.h"
#include "pq.h"

#define PQ_NUMCMP(x,y) (((int) x < (int) y) ? -1:(x == y ? 0 : 1))
#define NIL_FN(type) ((type (*)()) 0)
    
/*    
#define EQUAL(func, x, y) \
    ((((func) == pq_numcmp) || ((func) == pq_rev_numcmp) || \
      ((func) == pq_rev_numcmp) || ((func) == pq_ptrcmp)) ?\
      (PQ_NUMCMP((x),(y)) == 0) : ((*func)((x), (y)) == 0))

#define LT(func, x, y) \
    ((((func) == pq_numcmp) || ((func) == pq_ptrcmp)) ?\
      (PQ_NUMCMP((x),(y)) < 0) : \
     ((((func) == pq_rev_numcmp) || ((func) == pq_rev_ptrcmp)) ?\
      (PQ_NUMCMP((x),(y)) > 0) :  ((*func)((x), (y)) < 0)))

*/
#define EQUAL(func, x, y) ((*func)((x), (y)) == 0)
#define LT(func, x, y) ((*func)((x), (y)) < 0)

#define SANITY_CHECK(queue, fname) \
    if(queue == NIL(pq_t)) { \
	errRaise(PQ_NAME, 0, "Error: function fname passed a null queue");\
    }

int pq_numcmp(), pq_ptrcmp(), pq_rev_numcmp(), pq_rev_ptrcmp(), pq_rev_strcmp();

static pq_entry_t * pq_dup_entry();

static void free_queue_entry();

#define HEAP_SET(heap, j, value) (*daSet(pq_entry_t, (heap), (j)) = (value))
#define HEAP_GET(heap, j) (*daGet(pq_entry_t, (heap), (j)))
#define HEAP_APPEND(heap, value) (*daSetLast(pq_entry_t, (heap)) = (value))


/* Routines which manipulate the basic underlying data structure, a heap.
   A heap is an array of N elements s.t. for each a[k] in the heap, a[k] >
   a[2k] and a[k] > a[2k + 1].  To make the computations simple, we do not
   use a[0] (since 2*0 = 0).  We use the array package to permit the array
   to grow dynamically */

static heap_t *
newheap(compare)
int (*compare)();
{
    heap_t *result;

    result = ALLOC(heap_t, 1);
    result->compare = compare;


    result->data = daAlloc(pq_entry_t, 0);
    result->num_elements = 0;

    return result;
}

/* Fix a heap, starting at position k.  The rest of the heap is guaranteed
   to satisfy the heap condition, but a[k] might be < either of its sons.  So
   the idea is to push a[k] down to the leaves or until it satisfies the heap
   condition by exchanging it with the larger of its sons. */

static void
downheap(heap, k)
heap_t *heap;
int k;
{
    int j, N;
    dary heap_data;
    pq_entry_t value, rightson, leftson;

    heap_data = heap -> data;
    N = heap -> num_elements;

    value = HEAP_GET(heap_data, k);

    while(k <= N/2) {
	j = 2*k;
	if (j < N) {
	    rightson = HEAP_GET(heap_data, j);
	    ++j;
	    leftson = HEAP_GET(heap_data, j);
	    if(LT(heap->compare, rightson.key, leftson.key))
		rightson = leftson;
	    else --j;
	}
	else rightson = HEAP_GET(heap_data, j);
	if(LT(heap->compare, value.key, rightson.key)) {
	    HEAP_SET(heap_data, k, rightson);
	    k = j;
	}
	else break;
    }
    HEAP_SET(heap_data, k, value);
}
		
	

/* Fix a heap, starting at position k.  The rest of the heap is guaranteed
   to satisfy the heap condition, but a[k] might be > its parent.  So
   the idea is to push a[k] up to the root or until it satisfies the heap
   condition by exchanging it with its parent. */

static void
upheap(heap, k)
heap_t *heap;
int k;
{
    int j;
    dary heap_data;
    pq_entry_t value, parent;

    heap_data = heap -> data;

    value = HEAP_GET(heap_data, k);

    while(k > 1) {
	j = k/2;
	parent = HEAP_GET(heap_data, j);
	if(LT(heap->compare, value.key, parent.key)) break;
	HEAP_SET(heap_data, k, parent);
	k = j;
    }
    HEAP_SET(heap_data, k, value);
}


/* Now that the basic heap manipulation routines are done, we can write the
   higher-level routines which support the heap client application.  Heaps are
   not exported, so we can save a lot of the error checking by presuming that
   this is done by the queue package */

/* Insert an item into the heap */

static void
heap_insert(heap, entry)
heap_t *heap;
pq_entry_t entry;
{
    HEAP_SET(heap->data, ++(heap->num_elements), entry);
    upheap(heap, heap->num_elements);
}

/* Remove item k from the heap.  Only really used for k = 1 */

static void
heap_remove(heap, k)
heap_t *heap;
int k;
{
    pq_entry_t *entry;

    entry = &HEAP_GET(heap->data, k);

    daFree(entry->records, NIL_FN(void));
    entry = &HEAP_GET(heap->data, heap->num_elements);
    HEAP_SET(heap->data, k, *entry);
    entry = &HEAP_GET(heap->data, heap->num_elements);
    entry->records = (dary)0;
    --(heap->num_elements);
    downheap(heap, k);
}

/* Find the item with key key on the heap, starting at item k */

static int
heap_get_index(heap, key, k)
heap_t *heap;
char *key;
int k;
{
    pq_entry_t value;
    int result;

    if( k > heap->num_elements) return 0;

    value = HEAP_GET(heap->data, k);

    if(EQUAL(heap->compare, value.key, key)) return k;
    if(LT(heap->compare, value.key, key)) return 0;
    k *= 2;
    if((result = heap_get_index(heap, key, k)) != 0)
	return result;
    ++k;
    return heap_get_index(heap, key, k);
}

static pq_entry_t *
heap_find(heap, key)
heap_t *heap;
char *key;
{
    int k;

    if( (k = heap_get_index(heap, key, 1)) ) 
	return(&HEAP_GET(heap->data, k));
    else return NIL(pq_entry_t);
}

/* return a[1] */

static pq_entry_t *
heap_top(heap)
heap_t *heap;
{
    if((heap->num_elements) == 0) return NIL(pq_entry_t);
    return &HEAP_GET(heap->data, 1);

}

/* Free the heap */

static void
free_heap(heap)
heap_t *heap;
{
    daFree(heap->data, free_queue_entry);
    FREE(heap);
}

static heap_t *
heap_copy(heap)
heap_t *heap;
{
    heap_t *newheap;
    int i;
    pq_entry_t *entry;

    newheap = ALLOC(heap_t, 1);
    *newheap = *heap;

    if((newheap->data = daDup(heap->data)) == (dary) 0) {
	FREE(newheap);
	return NIL(heap_t);
    }
    else {
	for(i = 1; i <= heap->num_elements; ++i) {
	    entry = pq_dup_entry(HEAP_GET(heap->data, i));
	    HEAP_SET(newheap->data, i, *entry);
	    FREE(entry);
	}
	return newheap;
    }
}

    


/* Queue manipulation package. */

pq_t *pq_init_queue(compare)
int (*compare)();
{
    pq_t *new;

    new = ALLOC(pq_t, 1);
    new->num_entries = 0;
    new->num_keys = 0;
    new->heap = newheap(compare);
    return new;
}

static void
free_queue_entry(ptr)
pq_entry_t *ptr;
{
    if(ptr->records != (dary)0){
	daFree(ptr->records, NIL_FN(void));
	ptr->records = (dary)0;
    }
}

static pq_entry_t *
pq_dup_entry(ptr)
pq_entry_t ptr;
{
    pq_entry_t *result;

    result = ALLOC(pq_entry_t, 1);

    *result = ptr;

    result->records = daDup(ptr.records);

    return result;
}
   
void
pq_free_queue(queue)
pq_t *queue;
{
    SANITY_CHECK(queue, pq_free_queue);
    free_heap(queue->heap);
    FREE(queue);
}

int
pq_find_entry(queue, key, value)
register pq_t *queue;
register char *key;
char *value;
{
    pq_entry_t *entry;
    int i;
    
    SANITY_CHECK(queue, pq_find_entry);

    if((entry = heap_find(queue->heap, key)) != NIL(pq_entry_t))
	for(i = 0; i < (entry->num_records); ++i)
	    if(value == *daGet(char *, entry->records, i))
		return 1;

    return 0;
}

dary
pq_find_key(queue, key)
register pq_t *queue;
register char *key;
{
    pq_entry_t *entry;
    
    SANITY_CHECK(queue, pq_find_key);

    if((entry = heap_find(queue->heap, key)) != NIL(pq_entry_t))
	return entry->records;
    else return (dary)0;
}

static pq_entry_t *
new_pq_entry(value, key)
char *value, *key;
{
    pq_entry_t *result;

    result = ALLOC(pq_entry_t, 1);

    result->key = key;
    result->records = daAlloc(char *, 0);
    result->num_records = 1;
    result->cur_record = 0;
    *daSetLast(char *, result->records) = value;
    return result;
}
	

/* Insert an entry into the queue under key key.  This permits insertion of
   duplicate items */

int
pq_insert(queue, key, value)
register pq_t *queue;
register char *key;
char *value;
{
    pq_entry_t *entry;
    
    SANITY_CHECK(queue, pq_insert);

    if((entry = heap_find(queue->heap, key)) != NIL(pq_entry_t)) {
	++(queue->num_entries);
	++(entry->num_records);
	*daSetLast(char *, entry->records) = value;
	return 1;
    }
    else {
	entry = new_pq_entry(value, key);
	heap_insert(queue->heap, *entry);
	++(queue->num_entries);
	++(queue->num_keys);
	FREE(entry);
	return 0;
    }
}

/* Insert an entry into the queue under key key.  This permits insertion of
   duplicate items and keys */

void
pq_insert_fast(queue, key, value)
register pq_t *queue;
register char *key;
char *value;
{
    pq_entry_t *entry;
    
    SANITY_CHECK(queue, pq_insert);

    entry = new_pq_entry(value, key);
    heap_insert(queue->heap, *entry);
    ++(queue->num_entries);
    ++(queue->num_keys);
    FREE(entry);
 
}


/* Insert an entry into the queue under key key.  This does not permit
   insertion of duplicate items */

int
pq_insert_unique(queue, key, value)
register pq_t *queue;
register char *key;
char *value;
{
    pq_entry_t *entry;
    int i, found = 0;
    
    SANITY_CHECK(queue, pq_insert_unique);

    if((entry = heap_find(queue->heap, key)) != NIL(pq_entry_t)) {
	for(i = 0; i < (entry->num_records); ++i)
	    if(value == *daGet(char *, entry->records, i)) {
		found = 1;
		break;
	    }
	if(!found) {
	    ++(queue->num_entries);
	    ++(entry->num_records);
	    *daSetLast(char *, entry->records) = value;
	    return 0;
	}
	else return 1;
    }
    else {
	entry = new_pq_entry(value, key);
	heap_insert(queue->heap, *entry);
	++(queue->num_entries);
	++(queue->num_keys);
	FREE(entry);
	return 0;
    }
}

int
pq_delete(queue, key, value)
pq_t *queue;
char *key;
char *value;
{
    pq_entry_t *entry;
    char *record;
    int i, k;
    
    SANITY_CHECK(queue, pq_delete);

    if( (k = heap_get_index(queue->heap, key, 1)) ) {
	entry = &HEAP_GET(queue->heap->data, k);
	for(i = 0; i < (entry->num_records); ++i)
	    if(value == *daGet(char *, entry->records, i)) {
		--(queue->num_entries);
		if(--(entry->num_records)) {
		    while(i < entry->num_records) {
			record = *daGet(char *, entry->records, i+1);
			*daSet(char *, entry->records, i) = record;
			++i;
		    }
		}
		else {
		    --(queue->num_keys);
		    heap_remove(queue->heap, k);
		}
		return 1;
	    }
    }
    return 0;
}
    

int
pq_delete_key(queue, key)
pq_t *queue;
char *key;
{
    int k;
    pq_entry_t entry;

    SANITY_CHECK(queue, pq_delete_key);


    if( (k = heap_get_index(queue->heap, key, 1)) ) {
	entry = HEAP_GET(queue->heap->data, k);
	(queue->num_entries) -= (entry.num_records);
	heap_remove(queue->heap, k);
	--(queue->num_keys);
	return 1;
    } else return 0;
}

int
pq_count(queue)
pq_t *queue;
{
   SANITY_CHECK(queue, pq_count);
   return(queue->num_entries);
}


int
pq_count_under_key(queue, key)
pq_t *queue;
char *key;
{
    pq_entry_t *entry;

    SANITY_CHECK(queue, pq_count_under_key);
    
    entry = heap_find(queue->heap, key);

    if(entry != NIL(pq_entry_t)) return entry->num_records;
    else return 0;
}

int
pq_empty(queue)
pq_t *queue;
{
    SANITY_CHECK(queue, pq_empty);

    return(!(queue->num_entries));
}

    
int
pq_pop(queue, result)
register pq_t *queue;
char **result;
{
    pq_entry_t *entry;

    SANITY_CHECK(queue, pq_pop);

    entry = heap_top(queue->heap);

    if(entry == NIL(pq_entry_t)) return 0;

    *result = *daGet(char *, entry->records, entry->cur_record);
    
    if(++(entry->cur_record) ==  entry->num_records) {
	heap_remove(queue->heap, 1);
	--(queue->num_keys);
    }
    --(queue->num_entries);
    return 1;
}

    
int
pq_maximal_key(queue, key)
register pq_t *queue;
char **key;
{
    pq_entry_t *entry;

    SANITY_CHECK(queue, pq_maximal_key);

    entry = heap_top(queue->heap);

    if(entry == NIL(pq_entry_t)) return 0;

    *key = entry->key;

    return 1;

    
}
    
	

pq_t *pq_copy(old_queue)
pq_t *old_queue;
{
    pq_t *result;

    SANITY_CHECK(old_queue, pq_copy);
    
    result = ALLOC(pq_t, 1);
    *result = *old_queue;
    result->heap = heap_copy(old_queue->heap);

    if(result->heap == NIL(heap_t)) {
	FREE(result);
	return NIL(pq_t);
    }
    else return result;
}
	

/* Standard comparison functions */
int
pq_numcmp(x, y)
char *x;
char *y;
{
    return PQ_NUMCMP(x, y);
}

int
pq_ptrcmp(x, y)
char *x;
char *y;
{
    return PQ_NUMCMP(x, y);
}

int
pq_rev_numcmp(x, y)
char *x;
char *y;
{
    return -1 * PQ_NUMCMP(x, y);
}

int
pq_rev_ptrcmp(x, y)
char *x;
char *y;
{
    return -1 * PQ_NUMCMP(x, y);
}

int
pq_rev_strcmp(x, y)
char *x;
char *y;
{
    return -1 * strcmp(x, y);
}

/* Not really exported.  Supports the foreach macro */
pq_int_t *
pq_queue_to_array(queue)
pq_t *queue;
{
    int i, j, k;
    pq_entry_t entry;
    pq_int_t *recarray;

    SANITY_CHECK(queue, pq_foreach);

    recarray = ALLOC(pq_int_t, queue->num_entries);

    k = 0;

    for(i = 1; i <= queue->num_keys; ++i) {
	entry = HEAP_GET(queue->heap->data, i);
	for(j = 0; j < entry.num_records; ++j) {
	    recarray[k].key = entry.key;
	    recarray[k++].data = *daGet(char *, entry.records, j);
	}
    }

    return recarray;
}

int
pq_next1(gen, key, value)
pq_gen_t *gen;
char **key, **value;
{
    if( ((gen->continue_loop) = (++gen->recordnum < gen->keynum)) ) {
	*key = gen->heapcopy[gen->recordnum].key;
	*value = gen->heapcopy[gen->recordnum].data;
	return 1;
    } else {
	FREE(gen->heapcopy);
	return 0;
    }
}
	
void   
pq_initgen(queue, gen, key, data)
pq_t *queue;
pq_gen_t *gen;
char **key;
char **data;
{
    SANITY_CHECK(queue, pq_foreach);
    
    gen->continue_loop = 1;
    gen->keynum = 1;
    gen->recordnum = -1;

    if(queue->num_keys == 0) {
	gen->continue_loop = 0;
	return;
    }

    gen->entry = HEAP_GET(queue->heap->data, 1);
    pq_nextgen(queue, gen, key, data);
}

void
pq_nextgen(queue, gen, key, data)
pq_t *queue;
pq_gen_t *gen;
char **key;
char **data;
{
    if (gen->continue_loop == 0) {
	FREE(gen->heapcopy);
	return;
    }

    if((++(gen->recordnum)) >= gen->entry.num_records) {
	if((++(gen->keynum)) > queue->num_keys) {
	    gen->continue_loop = 0;
	    return;
	}
	else {
	    gen->entry = HEAP_GET(queue->heap->data, gen->keynum);
	    gen->recordnum = 0;
	}
    }
    *key = gen->entry.key;
    *data = *daGet(char *, gen->entry.records, gen->recordnum);
}
