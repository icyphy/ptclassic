#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "oct_id.h"

static void do_free_table();

  /* 
   * This file implements the oct id table that maps between object 
   * ids and pointers to the objects themselves.  Object ids are 
   * sequential integers that are used to index into the leaves of a 
   * full ID_BLOCK_SIZE-ary tree. The choice of 1024 for ID_BLOCK_SIZE 
   * means the tree will be at most 3 deep (counting the root), so 
   * access is quite fast.
   */
  
#define ID_BLOCK_SIZE 1024
#define SHIFT_AMOUNT 10		/* = log2(ID_BLOCK_SIZE) */
#define BOTTOM_MASK (ID_BLOCK_SIZE - 1)	/* SHIFT_AMOUNT 1's */
#define MAX_LEVEL 2		/* the largest indirect_level allowed */
#define SIZEOF_ID_BLOCK ID_BLOCK_SIZE*sizeof(char *) /* compiler bug */

typedef char *id_block[ID_BLOCK_SIZE];

static int indirect_level = 0;
static int next_id;
static char **top_block;

static struct {
    char **next_slot;
    char **end;
} bounds_array[MAX_LEVEL+1];

/* 
 * return the number of bytes used by the id table. This routine is 
 * used only by internal statistics routines
 */

int
oct_id_table_size()
{
    int div = ID_BLOCK_SIZE;
    int i = indirect_level;
    int size = 0;
    
    while ( i-- >= 0) {
	size += (next_id+div-1)/div;
	div *= ID_BLOCK_SIZE;
    }
    return size*SIZEOF_ID_BLOCK;
}

	
int
oct_init_ids()
{
    top_block = ALLOC(char *, SIZEOF_ID_BLOCK);
    next_id = 1;
    indirect_level = 0;
    bounds_array[0].next_slot = &top_block[1];
    bounds_array[0].end = &top_block[ID_BLOCK_SIZE];
    top_block[0] = (char *) 0; /* make oct_null_id an invalid id */
    return 1;
}
    

generic *
oct_id_to_ptr(id)
register octId id;
{
    register char **base;
    
    /*
     * find the pointer corresponding to id, returning (generic *) 0 in case
     * of invalid id
     */
    if (id >= next_id || id < 0) {
	return NIL(generic);
    } else {
	base = top_block;
	
	switch (indirect_level) {
	case 2:
	    base = (char **) base[id >> (SHIFT_AMOUNT<<1)];
	    /*FALL THROUGH*/
	case 1:
	    base = (char **) base[(id >> SHIFT_AMOUNT)&BOTTOM_MASK];
	    /*FALL THROUGH*/
	case 0:
	    return (struct generic *) base[id&BOTTOM_MASK];
	default :
	    return NIL(generic);
	}
	
    }
}

octStatus
oct_set_id(id, ptr)
register octId id;
generic *ptr;
{
    char **base = top_block;
    
    /*
     * Mark `id' as invalid (with NIL(char))
     * so further references will fail.
     */
    if (id < 0 || id >= next_id)
	{
		return OCT_ERROR;
    }
	else
	{
		switch (indirect_level)
		{
		case 2:
			base = (char **) base[id >> (SHIFT_AMOUNT<<1)];
		case 1:
			base = (char **) base[(id >> SHIFT_AMOUNT)&BOTTOM_MASK];
		case 0:
			base[id&BOTTOM_MASK] = (char *)ptr;
		}
    }
    return OCT_OK;
}


octId 
oct_new_id(ptr)
generic *ptr;
{
    /*
     * Generate a new id and enter `ptr' as its' corresponding value
     * return oct_null_id as error indicator
     */
    if (bounds_array[indirect_level].next_slot >=
	bounds_array[indirect_level].end) {

	    /* the current block is full, allocate a new one */
	char **new;
	int i;

	for (i=indirect_level-1; i >= 0; i--) {
	    if (bounds_array[i].next_slot < bounds_array[i].end) {
		break;
	    }
	}

	if (i < 0) {
	        /*
		 * The current arrangement is totally filled,
		 * so the indirect_level must be increased.
		 * A new root node is created and the old root is made 
		 * its' first child
		 */
	    if (++indirect_level > MAX_LEVEL) {
		    /*
		     * The array has filled up.
		     * (> ID_BLOCK_SIZE ** (MAX_LEVEL+1) elements)
		     */
		indirect_level--;
		return 0;
	    }
	    
	    new = ALLOC(char *, SIZEOF_ID_BLOCK);

	        /*
		 * push another level of indirection on top of the current
		 * chain
		 */
	    
	    new[0] = (char *) top_block;
	    top_block = new;
	    bounds_array[0].next_slot = &new[1];
	    bounds_array[0].end = &new[ID_BLOCK_SIZE];
	    i = 0;
	}
	    /*
	     * A free slot has been found at level i.
	     * We must next allocate and link id_blocks until
	     * we reach indirect_level
	     */

	for(++i; i <= indirect_level; i++) {
	    new = ALLOC(char *, SIZEOF_ID_BLOCK);
	    *(bounds_array[i-1].next_slot++) = (char *) new;
	    bounds_array[i].next_slot = &new[0];
	    bounds_array[i].end = &new[ID_BLOCK_SIZE];
	}
    }
    
        /* Now there is room in the current block, so use it */
    *(bounds_array[indirect_level].next_slot++) = (char *) ptr;
    
    return next_id++;
}


void
oct_free_id(id)
octId id;
{
    char **base = top_block;
    
    /*
     * Mark `id' as invalid (with NIL(char))
     * so further references will fail.
     */
    if (id < 0 || id >= next_id) {
	return;
    } else {
	switch (indirect_level) {
	case 2:
	    base = (char **) base[id >> (SHIFT_AMOUNT<<1)];
	case 1:
	    base = (char **) base[(id >> SHIFT_AMOUNT)&BOTTOM_MASK];
	case 0:
	    base[id&BOTTOM_MASK] = NIL(char);
	}
    }
    return;
}


octId
oct_next_id()
{
    return next_id;
}


/* recursively free all the nodes in the id table */

void
oct_free_id_table()
{
    do_free_table(top_block, 0, 1);
    return;
}


static void
do_free_table(block, ilevel, partial)
char **block;
int ilevel;
int partial;
{
    char **ptr, **last_block;
    
    if (ilevel < indirect_level) {
	if (partial) {
	    last_block = bounds_array[ilevel].next_slot - 1;
	} else {
	    last_block = &block[ID_BLOCK_SIZE];
	}
	for(ptr=block; ptr < last_block; ptr++) {
	    do_free_table((char **) *ptr, ilevel+1, 0);
	}
	
	if (partial) {
	    do_free_table((char **) *ptr, ilevel+1, 1);
	}
    }

    FREE(block);
}


#ifdef BIGID

octId oct_null_id = {0, 0};

octIdsEqual(id1, id2)
octId id1, id2;
{
    return ((id1.lsb == id2.lsb) && (ld1.msb == id2.msb));
}

int
octIdIsNull(id)
octId id;
{
    return ((id.lsb == oct_null_id.lsb) && (id.msb == oct_null_id.msb));
}


int
octIdCmp(a, b)
CONST char *a, *b;
{
    return octIdsEqual(*(octId *) a, *(octId *) b);
}


int
octIdHash(a, mod)
char *a;
int mod;
{
    octId id = *(octId *) a;
    char *dummy = (char *) (id.lsb + ld.msb);
    return st_numhash(dummy, mod);
}

#else

int
octIdsEqual(id1, id2)
octId id1, id2;
{
    return id1 == id2;
}


int
octIdIsNull(id)
octId id;
{
    return id == oct_null_id;
}


/* for use with the `st' package */

int
octIdCmp(a, b)
CONST char *a, *b;
{
    return *((CONST octId *) a) - *((CONST octId *) b);
}


int
octIdHash(a, mod)
char *a;
int mod;
{
    char *dummy = (char *) *((octId *) a);
    return st_numhash(dummy, mod);
}
#endif

