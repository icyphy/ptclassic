#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
/*LINTLIBRARY*/
/*
 * K-d tree geometric data structure
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This is an implementation of k-d trees as described by Rosenberg
 * in "Geographical Data Structures", IEEE Transactions on CAD, Vol. CAD-4,
 * No. 1, January 1988.  His work is based on that of Jon Bentley in
 * "Multidimensional Binary Search Trees used for Associative Searching",
 * CACM, Vol. 18, No. 9, pp. 509-517, Sept. 1985.
 */

#include "copyright.h"
#include "port.h"
#include "kd.h"
#include "errtrap.h"
#include "utility.h"

#ifndef MAXINT
/* Needed for linux */
#define MAXINT	2147483647
#endif

#ifndef MININT
/* Needed for linux */
#define MININT	-2147483647
#endif

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)		((a) < 0 ? -(a) : (a))
#endif

char *kd_pkg_name = "kd";

static char *mem_ret;		/* Memory allocation */

/* Note: these are NOT multi-callable */
#define KDALLOC(type) \
((mem_ret = malloc(sizeof(type))) ? (type *) mem_ret : (type *) kd_fault(KDF_M))

#ifdef NEVER
#define ALLOC(type) \
((mem_ret = malloc(sizeof(type))) ? (type *) mem_ret : (type *) kd_fault(KDF_M))
#endif

#define MULTALLOC(type, num) \
((mem_ret = malloc((unsigned) (sizeof(type) * (num)))) ? \
 (type *) mem_ret : (type *) kd_fault(KDF_M))

#define KDREALLOC(type, ptr, newsize) \
((mem_ret = (char *) (REALLOC(type, (ptr), (newsize)))) ? \
 (type *) mem_ret : (type *) kd_fault(KDF_M))

#ifdef NEVER
/* Old krufty definitions.  Use stuff from utility.h instead */
#define REALLOC(type, ptr, newsize) \
((mem_ret = realloc((char *) ptr, (unsigned) (sizeof(type) * (newsize)))) ? \
 (type *) mem_ret : (type *) kd_fault(KDF_M))

#define FREE(ptr)		free((char *) ptr)
#endif
#define BOXINTERSECT(b1, b2) \
  (((b1)[KD_RIGHT] >= (b2)[KD_LEFT]) && \
   ((b2)[KD_RIGHT] >= (b1)[KD_LEFT]) && \
   ((b1)[KD_TOP] >= (b2)[KD_BOTTOM]) && \
   ((b2)[KD_TOP] >= (b1)[KD_BOTTOM]))

static char kd_err_buf[1024];

#define Sprintf		(void) sprintf
#define Printf		(void) printf

/* Forward declarations */
static void sel_k();
static void resolve();
static int get_min_max();
static void del_elem();
static kd_status del_element();


/*
 * The kd_tree type is actually (KDTree *).
 */

#define KD_LOSON	0
#define KD_HISON	1

typedef struct KDElem_defn {
    kd_generic item;		/* Actual item at this node */
    kd_box size;		/* Size of item             */
    int lo_min_bound;		/* Lower minimum boundary   */
    int hi_max_bound;		/* High maximum boundary    */
    int other_bound;		/* Discriminator dependent  */
    struct KDElem_defn *sons[2];/* Children                 */
} KDElem;

typedef struct KDTree_defn {
    KDElem *tree;		/* K-d tree itself      */
    int item_count;		/* Number of nodes in tree */
    int dead_count;		/* Number of dead nodes */
} KDTree;

/*
 * K-d tree  generators are actually a pointer to (KDState).
 * Since these generators must trace the heirarchy
 * of the tree,  the generator contains a stack representing
 * the activation records of a depth first recursive tracing
 * of the tree.
 */

#define KD_INIT_STACK	15	/* Initial size of stack                */
#define KD_GROWSIZE(s)	10	/* Linear expansion                     */
#define	KD_THIS_ONE	-1	/* Indicates going through this element */
#define KD_DONE		2	/* Entirely done searching this element */

typedef struct kd_save {
    short disc;			/* Discriminator             */
    short state;		/* Current state (see above) */
    KDElem *item;		/* Element saved             */
} KDSave;

typedef struct kd_state {
    kd_box extent;		/* Search area 		     */
    short stack_size;		/* Allocated size of stack   */
    short top_index;		/* Top of the stack          */
    KDSave *stk;		/* Stack of active states    */
} KDState;



static char *kd_fault(t)
/* Fatal faults - raises an error using the error handling package */
int t;
{
    switch(t) {
    case KDF_M:
	errRaise(kd_pkg_name, KDF_M, "out of memory");
	/* NOTREACHED */
	break;
    case KDF_ZEROID:
	errRaise(kd_pkg_name, KDF_ZEROID, "attempt to insert null data");
	/* NOTREACHED */
	break;
    case KDF_MD:
	errRaise(kd_pkg_name, KDF_MD, "bad median");
	/* NOTREACHED */
	break;
    case KDF_F:
	errRaise(kd_pkg_name, KDF_F, "bad father node");
	/* NOTREACHED */
	break;
    case KDF_DUPL:
	errRaise(kd_pkg_name, KDF_DUPL, "attempt to insert duplicate item");
	/* NOTREACHED */
    default:
	errRaise(kd_pkg_name, KDF_UNKNOWN, "unknown fault: %d", t);
	/* NOTREACHED */
	break;
    }
    return (char *) 0;
}

static int kd_set_error(err)
int err;			/* Error number */
/* Sets an error message in an area for later retrieval */
{
    switch (err) {
    case KD_NOTFOUND:
	Sprintf(kd_err_buf, "k-d error: data not found");
	break;
    default:
	Sprintf(kd_err_buf, "k-d error: unknown error %d", err);
	break;
    }
    return err;
}

char *kd_err_string()
/* Returns a textual description of an error */
{
    return kd_err_buf;
}


static KDElem *kd_new_node(item, size, lomin, himax, other, loson, hison)
kd_generic item;		/* New node value */
kd_box size;			/* Size of item   */
int lomin, himax, other;	/* Bounds info    */
KDElem *loson, *hison;		/* Sons           */
/* Allocates and initializes a new node element */
{
    KDElem *newElem;

    newElem = KDALLOC(KDElem);
    newElem->item = item;
    newElem->size[0] = size[0];
    newElem->size[1] = size[1];
    newElem->size[2] = size[2];
    newElem->size[3] = size[3];
    newElem->lo_min_bound = lomin;
    newElem->hi_max_bound = himax;
    newElem->other_bound = other;
    newElem->sons[0] = loson;
    newElem->sons[1] = hison;
    return newElem;
}


/*
 * K-d tree creation and deletion.
 */


kd_tree kd_create()
/*
 * Creates a new k-d tree and returns its handle.  This handle is
 * used by all other k-d tree operations.  It can be freed using
 * kd_destroy().
 */
{
    KDTree *newTree;

    newTree = KDALLOC(KDTree);
    newTree->tree = (KDElem *) 0;
    newTree->item_count = newTree->dead_count = 0;
    return (kd_tree) newTree;
}



/*
 * kd_build() requires a simple linked list.  This page implements
 * that list.
 */

/* The following is what is stored in the data field of the list */
typedef struct kd_info_defn {
    kd_generic item;		/* Item itself  */
    kd_box bb;			/* Size of item */
} kd_info;

typedef struct kd_list_defn {
    kd_generic data;
    struct kd_list_defn *next;
} kd_list;

static kd_list *kd_tmp_ptr;

#define KDNIL			(kd_list *) 0
#define CAR(list)		(list)->data
#define CDR(list)		(list ? (list)->next : KDNIL)
#define CONS(item, list) \
  (kd_tmp_ptr = KDALLOC(kd_list), \
   kd_tmp_ptr->data = (kd_generic) (item),   \
   kd_tmp_ptr->next = (list),   \
   kd_tmp_ptr)
/*
 * The following moves the front of `list1' to the front of `list2' and
 * returns `list1'.  This is a destructive operation: both `list1' and
 * `list2' are changed.
 */
#define CMV(list1, list2) \
  (kd_tmp_ptr = CDR(list1),                 \
   (list1 ? (list1)->next = (list2) : KDNIL), \
   (list2) = (list1),                       \
   (list1) = kd_tmp_ptr)
/*
 * Destructively replaces the next item of list1 with list2.
 * Returns modified list1.
 */
#define RCDR(list1, list2) \
  ((list1) ? (((list1)->next = (list2)), (list1)) : (list2))



/* Forward declarations */
static kd_list *load_items();
static KDElem *build_node();

kd_tree kd_build(itemfunc, arg)
int (*itemfunc)();		/* Returns new items       */
kd_generic arg;			/* Data to itemfunc        */
/*
 * This routine builds a new, reasonably balanced k-d tree
 * from a list of items.  This list of items is collected
 * by repeatedly calling `itemfunc' using the following
 * arguments:
 *   int itemfunc(arg, val, size)
 *   kd_generic arg;
 *   kd_generic *val;
 *   kd_box size;
 * Each time the itemfunc is called,  it should return the
 * next item to be placed in the tree in `val',  and the size
 * of the item in `size'.  When there are no more items,  the
 * routine should return zero.  `itemfunc' is guaranteed to be
 * called for all items. `arg' is passed as a convenience (usually
 * for state information).
 */
{
    KDTree *newTree = (KDTree *) kd_create();
    kd_list *items;
    kd_box extent;

    /* First build up list of items and their overall extent */
    items = load_items(itemfunc, arg, extent, &(newTree->item_count));
    if (!items) {
	(void) kd_fault(KDF_ZEROID);
	/* NOTREACHED */
    }

    /* Then recursively fill the tree */
    newTree->tree = build_node(items, newTree->item_count, extent, 0);

    return (kd_tree) newTree;
}


static kd_list *load_items(itemfunc, arg, extent, length)
int (*itemfunc)();		/* Generate next item       */
kd_generic arg;			/* State passed to itemfunc */
kd_box extent;			/* Overall extent           */
int *length;			/* List length (returned)   */
/*
 * This routine uses `itemfunc' to generate all of the items
 * that are to be loaded into the new k-d tree and places them
 * in a simple linked list.  This list will be used to build
 * a perfectly balanced initial tree.  The routine also
 * stores the size of all items and the overall size of
 * all items.  The routine will return zero if any of the
 * items returned have no size.
 */
{
    kd_list *new_list = (kd_list *) 0;
    kd_info *new_item;
    int add_flag = 1;

    *length = 0;
    extent[KD_LEFT] = extent[KD_BOTTOM] = MAXINT;
    extent[KD_RIGHT] = extent[KD_TOP] = MININT;
    for (;;) {
	new_item = KDALLOC(kd_info);
	if ((*itemfunc)(arg, &new_item->item, new_item->bb)) {
	    if (!new_item->item) add_flag = 0;
	    if (add_flag) {
		/* Add to list */
		if (new_item->bb[KD_LEFT] < extent[KD_LEFT])
		  extent[KD_LEFT] = new_item->bb[KD_LEFT];
		if (new_item->bb[KD_BOTTOM] < extent[KD_BOTTOM])
		  extent[KD_BOTTOM] = new_item->bb[KD_BOTTOM];
		if (new_item->bb[KD_RIGHT] > extent[KD_RIGHT])
		  extent[KD_RIGHT] = new_item->bb[KD_RIGHT];
		if (new_item->bb[KD_TOP] > extent[KD_TOP])
		  extent[KD_TOP] = new_item->bb[KD_TOP];
		new_list = CONS(new_item, new_list);
		(*length)++;
	    }
	} else {
	    break;
	}
    }
    if (!add_flag) {
	kd_list *ptr;

	/* Destroy list and return */
	while (new_list) {
	    ptr = new_list;
	    new_list = CDR(new_list);
	    FREE(CAR(ptr));
	    FREE(ptr);
	    (*length)--;
	}
    }
    return new_list;
}


#define NEXTDISC(val)	(((val)+1)%4)

static KDElem *build_node(items, num, extent, disc)
kd_list *items;			/* Items to insert          */
int num;			/* Number of items          */
kd_box extent;			/* Extent of items          */
int disc;			/* Discriminator            */
/*
 * This routine builds a new node by finding an approximate median of
 * the items according to the edge given by `disc' and
 * making that the node.  Items less than the median are
 * recursively placed in the lower son,  items greater
 * than the median are recursively placed in the upper son.
 * Bounds information is also updated.  The node is deleted
 * from the list once placed.
 */
{
    KDElem *loson, *hison, *ret_elem;
    kd_list *lo, *eq, *hi;
    int lo_min_bound, lo_max_bound, hi_min_bound, hi_max_bound;
    int num_lo, num_hi;
    int hort, tmp, m;

    if (num == 0) return (KDElem *) 0;

    /* Find (disc)-median of items */
    hort = disc & 0x01;
    m = (extent[hort] + extent[hort+2]) >> 1;
    sel_k(items, m, disc, &lo, &eq, &hi);

    /* If more than one median -- try to distinguish them */
    if (CDR(eq)) resolve(&lo, &eq, &hi, disc);

    /* Find min-max boundaries based on discriminator */
    RCDR(eq, lo);
    num_lo = get_min_max(eq, disc, &lo_min_bound, &lo_max_bound) - 1;

    RCDR(eq, hi);
    num_hi = get_min_max(eq, disc, &hi_min_bound, &hi_max_bound) - 1;

    tmp = extent[hort+2];  extent[hort+2] = m;
    loson = build_node(lo, num_lo, extent, NEXTDISC(disc));
    extent[hort+2] = tmp;

    tmp = extent[hort];    extent[hort] = m;
    hison = build_node(hi, num_hi, extent, NEXTDISC(disc));
    extent[hort] = tmp;

    /* Make new node with appropriate values */
    ret_elem = kd_new_node(((kd_info *) CAR(eq))->item,
			   ((kd_info *) CAR(eq))->bb,
			   lo_min_bound, hi_max_bound,
			   ((disc & 0x2) ? hi_min_bound : lo_max_bound),
			   loson, hison);
    FREE(CAR(eq));
    FREE(eq);
    return ret_elem;
}



#define KD_SIZE(val)	((kd_info *) (val))->bb

#ifdef OLD_SELECT
static void sel_k(items, k, disc, lo, eq, hi)
kd_list *items;			/* Items to examine                 */
int k;				/* Look for item close to `k'       */
int disc;			/* Discriminator                    */
kd_list **lo;			/* Returned items less than `k'th   */
kd_list **eq;			/* Returned items equal to `k'th    */
kd_list **hi;			/* Returned items larger than `k'th */
/*
 * This routine uses a heuristic to attempt to find a rough median
 * using an inline comparison function.  The routine takes a `target'
 * number `k' and places all items that are `disc'-less than `k' in
 * lo, equal in `eq', and greater in `hi'.
 */
{
    register kd_list *idx;
    register int cmp_val;
    register kd_list *median;
    int lo_val;

    idx = items;
    lo_val = MAXINT;
    /* First find closest to median value */
    while (idx) {
	cmp_val = KD_SIZE(CAR(idx))[disc] - k;
	cmp_val = ABS(cmp_val);
	if (cmp_val < lo_val) {
	    median = idx;
	    lo_val = cmp_val;
	}
	idx = CDR(idx);
    }
    /* Now divide based on median */
    *lo = *eq = *hi = KDNIL;
    idx = items;
    while (idx) {
	cmp_val = KD_SIZE(CAR(idx))[disc] - KD_SIZE(CAR(median))[disc];
	if (cmp_val < 0) {
	    CMV(idx, *lo);
	} else if (cmp_val > 0) {
	    CMV(idx, *hi);
	} else {
	    CMV(idx, *eq);
	}
    }
}
#endif

static void sel_k(items, k, disc, lo, eq, hi)
kd_list *items;			/* Items to examine                 */
int k;				/* Look for item close to `k'       */
int disc;			/* Discriminator                    */
kd_list **lo;			/* Returned items less than `k'th   */
kd_list **eq;			/* Returned items equal to `k'th    */
kd_list **hi;			/* Returned items larger than `k'th */
/*
 * This routine uses a heuristic to attempt to find a rough median
 * using an inline comparison function.  The routine takes a `target'
 * number `k' and places all items that are `disc'-less than `k' in
 * lo, equal in `eq', and greater in `hi'.
 */
{
    register kd_list *idx, *median;
    register int cmp_val;
    int lo_val;

    idx = items;
    *lo = *eq = *hi = KDNIL;
    lo_val = MAXINT;
    median = KDNIL;
    while (idx) {
	/* Check to see if new median */
	cmp_val = KD_SIZE(CAR(idx))[disc] - k;
	if (ABS(cmp_val) < lo_val) {
	    lo_val = ABS(cmp_val);
	    median = idx;
	    while (*eq) {
		cmp_val = KD_SIZE(CAR(*eq))[disc] - KD_SIZE(CAR(median))[disc];
		if (cmp_val < 0) {
		    CMV(*eq, *lo);
		} else if (cmp_val > 0) {
		    CMV(*eq, *hi);
		} else {
		    (void) kd_fault(KDF_MD);
		}
	    }
	}
	/* Place element in list */
	if (median) {
	    cmp_val = KD_SIZE(CAR(idx))[disc] - KD_SIZE(CAR(median))[disc];
	}
	if (cmp_val < 0) {
	    CMV(idx, *lo);
	} else if (cmp_val > 0) {
	    CMV(idx, *hi);
	} else {
	    CMV(idx, *eq);
	}
    }
}


#define KD_BB(val)	((kd_info *) CAR(val))->bb

static void resolve(lo, eq, hi, disc)
register kd_list **lo, **eq, **hi; /* Lists for examination */
int disc;
/*
 * This routine is called if more than one possible median
 * was found.  The first is chosen as the actual median.
 * The rest are reclassified using cyclical comparison.
 */
{
    kd_list *others;
    int cur_disc, val = 0;

    others = CDR(*eq);
    RCDR(*eq, KDNIL);
    while (others) {
	cur_disc = NEXTDISC(disc);
	while (cur_disc != disc) {
	    val = KD_BB(others)[cur_disc] - KD_BB(*eq)[cur_disc];
	    if (val != 0) break;
	    cur_disc = NEXTDISC(cur_disc);
	}
	if (val < 0) {
	    CMV(others, *lo);
	} else {
	    CMV(others, *hi);
	}
    }
}



static int get_min_max(list, disc, b_min, b_max)
kd_list *list;			/* List to examine */
int disc;			/* Discriminator   */
int *b_min;			/* Lower bound     */
int *b_max;			/* Upper bound     */
/*
 * This routine examines all of the items in `list' and
 * finds the lowest and highest edges based on the discriminator
 * `disc'.  If the discriminator is 0 or 2,  the left and
 * right edges of the boxes are examined.  Otherwise, the
 * top and bottom edges are examined.  Returns the number
 * of items in the list.
 */
{
    kd_info *item;
    int count;

    *b_min = MAXINT;
    *b_max = MININT;

    disc = disc & 0x01;		/* zero: horizontal, one: vertical */
    count = 0;
    while (list) {
	item = (kd_info *) CAR(list);
	if (item->bb[disc] < *b_min) *b_min = item->bb[disc];
	if (item->bb[disc+2] > *b_max) *b_max = item->bb[disc+2];
	list = CDR(list);
	count++;
    }
    return count;
}



static void del_elem(elem, delfunc)
KDElem *elem;			/* Element to release */
void (*delfunc)();		/* Free function      */
/*
 * Recursively releases resources associated with `elem'.
 * User data items are freed using `delfunc'.
 */
{
    int i;

    /* If the tree does not exist,  return normally */
    if (!elem) return;

    /* If there are children,  recursively destroy them first */
    for (i = 0;  i < 2;  i++) {
	del_elem(elem->sons[i], delfunc);
    }

    /* Now get rid of the rest of it */
    if (delfunc) (*delfunc)(elem->item);
    FREE(elem);
}

void kd_destroy(this_one, delfunc)
kd_tree this_one;		/* k-d tree to destroy */
void (*delfunc)();		/* Free function called on user data */
/*
 * This routine frees all resources associated with the
 * specified quad-tree.
 */
{
    del_elem(((KDTree *) this_one)->tree, delfunc);
}




/*
 * Insertion
 */

/* Forward declaration */
static KDElem *find_item();

void kd_insert(theTree, data, size)
kd_tree theTree;		/* k-d tree for insertion */
kd_generic data;		/* User supplied data     */
kd_box size;			/* Size of item           */
/*
 * Inserts a new data item into the specified k-d tree.  The `data'
 * item cannot be zero.  This value is used internally by the package.
 * Fatal errors:
 *   KDF_ZEROID: attempt to insert an item with a null generic pointer.
 *   KDF_DUPL:   an exact duplicate is already in the tree.
 */
{
    KDTree *realTree = (KDTree *) theTree;

    if (!data) (void) kd_fault(KDF_ZEROID);
    if (realTree->tree) {
	if (find_item(realTree->tree, 0, data, size, 0)) {
	    realTree->item_count += 1;
	} else {
	    (void) kd_fault(KDF_DUPL);
	}
    } else {
	realTree->tree = kd_new_node(data, size, size[0], size[2], size[0],
				     (KDElem *) 0, (KDElem *) 0);
	realTree->item_count += 1;
    }
}


/*
 * The find_item() routine optionally produces a path down to the
 * item in the following global dynamic array if search_p is true.
 * find_item() uses NEW_PATH to record the decent down the tree
 * until it finds the object.  It uses LAST_PATH to mark the
 * last item.
 */

static int path_length = 0;
static int path_alloc = 0;
static int path_reset = 1;
static KDElem **path_to_item = (KDElem **) 0;

#define PATH_INIT	20
#define PATH_INCR	10

#define NEW_PATH(elem)	\
if (path_reset) { path_length = 0;  path_reset = 0; } \
if (path_length >= path_alloc) { \
   if (path_alloc == 0) path_alloc = PATH_INIT; \
   else path_alloc += PATH_INCR; \
   path_to_item = KDREALLOC(KDElem *, path_to_item, path_alloc); \
 } \
path_to_item[path_length++] = (elem)

#define LAST_PATH 	path_reset = 1



/* Forward declaration */
static void bounds_update();

static KDElem *find_item(elem, disc, item, size, search_p)
KDElem *elem;			/* Search location */
int disc;			/* Discriminator   */
kd_generic item;		/* Item to insert  */
kd_box size;			/* Size of item    */
int search_p;			/* Search or insert */
/*
 * This routine either searches for or inserts `item'
 * into the node `elem'.  The size of `item' is passed
 * in as `size'.  The function for returning the size
 * of an item is `s_func'.  If `search_p' is non-zero,
 * the routine expects to find the item rather than
 * insert it.  The routine returns either the newly
 * created element or the element found (zero if
 * it couldn't be found).
 */
{
    KDElem *result;
    int val, new_disc, vert;

    /* Compare current element against the one we are looking for */
    if (item == elem->item) {
	if (search_p) {
	    LAST_PATH;
	    if (elem->item) return elem;
	    else return (KDElem *) 0;
	} else return (KDElem *) 0;
    } else {
	/* Determine successor */
	val = size[disc] - elem->size[disc];
	if (val == 0) {
	    /* Cyclical comparison required */
	    new_disc = NEXTDISC(disc);
	    while (new_disc != disc) {
		val = size[new_disc] - elem->size[new_disc];
		if (val != 0) break;
		new_disc = NEXTDISC(new_disc);
	    }
	    if (val == 0) val = 1; /* Force upward if equal */
	}
	val = (val >= 0);
	if (elem->sons[val]) {
	    if (search_p) NEW_PATH(elem);
	    result = find_item(elem->sons[val], NEXTDISC(disc), item,
			      size, search_p);
	    /* Bounds update if insert */
	    if (!search_p) bounds_update(elem, disc, size);
	    return result;
	} else if (search_p) {
	    LAST_PATH;
	    return (KDElem *) 0;
	} else {
	    /* Insert here */
	    vert = NEXTDISC(disc) & 0x01;
	    elem->sons[val] =
	      kd_new_node(item, size, size[vert], size[vert+2],
			  ((NEXTDISC(disc)&0x2) ? size[vert] : size[vert+2]),
			  (KDElem *) 0, (KDElem *) 0);
	    /* Bounds update */
	    bounds_update(elem, disc, size);
	    return elem->sons[val];
	}
    }
}



static void bounds_update(elem, disc, size)
KDElem *elem;			/* Element to update */
int disc;			/* Discriminator     */
kd_box size;			/* Size of new item  */
/*
 * This routine updates the bounds information of `elem'
 * using `disc' and `size'.
 */
{
    int vert;

    vert = disc & 0x01;
    elem->lo_min_bound = MIN(elem->lo_min_bound, size[vert]);
    elem->hi_max_bound = MAX(elem->hi_max_bound, size[vert+2]);
    if (disc & 0x2) {
	/* hi_min_bound */
	elem->other_bound = MIN(elem->other_bound, size[vert]);
    } else {
	/* lo_max_bound */
	elem->other_bound = MAX(elem->other_bound, size[vert+2]);
    }
}



kd_status kd_is_member(theTree, data, size)
kd_tree theTree;		/* Tree to examine  */
kd_generic data;		/* Item to look for */
kd_box size;			/* Original size    */
/*
 * Returns KD_OK if `data' is stored in tree `theTree' with
 * the location `size' and KD_NOTFOUND if not.
 */
{
    KDTree *real_tree = (KDTree *) theTree;
    
    if (find_item(real_tree->tree, 0, data, size, 1)) {
	return KD_OK;
    } else {
	return KD_NOTFOUND;
    }
}


/*
 * Deletion
 *
 * Since the preconditions of a k-d tree are moderately complex,  the
 * deletion routine simply marks a node as deleted if it has sons.
 * If it doesn't have sons,  it can be fully deleted.  The routine
 * updates count information in the tree for eventual rebuild once
 * the `badness' of the tree exceeds a threshold.  This is not
 * implemented initially.
 *
 * Items are marked for deletion by setting the item pointer
 * to zero.  This means zero data items are not allowed.
 */

kd_status kd_delete(theTree, data, old_size)
kd_tree theTree;		/* Tree to delete from  */
kd_generic data;		/* Item to delete       */
kd_box old_size;		/* Original size        */
/*
 * Deletes the specified item from the specified k-d tree.  `old_size'
 * must be provided to efficiently locate the item.  May return
 * KD_NOTFOUND if the item is not in the tree.
 */
{
    KDTree *real_tree = (KDTree *) theTree;
    KDElem *elem;

    elem = find_item(real_tree->tree, 0, data, old_size, 1);
    if (elem) {
	/* Delete element */
	elem->item = (kd_generic) 0;
	(real_tree->dead_count)++;
	return del_element(real_tree, elem, path_length);
    } else {
	return kd_set_error(KD_NOTFOUND);
    }
}


static kd_status del_element(tree, elem, spot)
KDTree *tree;			/* Tree              */
KDElem *elem;			/* Item to delete    */
int spot;			/* Last item in path */
/*
 * This routine deletes `elem' from its tree.  It assumes that
 * the path information down to the element is stored in
 * path_to_item (see find_item() for details).  If the node
 * has no children,  it is deleted and the counts are modified
 * appropriately.  The routine is called recursively on its
 * parent.  If it has children,  it is left and the recursion
 * stops.
 */
{
    if (elem->item == (kd_generic) 0) {
	if (!elem->sons[KD_LOSON] && !elem->sons[KD_HISON]) {
	    if (spot > 0) {
		if (path_to_item[spot-1]->sons[KD_LOSON] == elem) {
		    path_to_item[--spot]->sons[KD_LOSON] = (KDElem *) 0;
		} else if (path_to_item[spot-1]->sons[KD_HISON] == elem) {
		    path_to_item[--spot]->sons[KD_HISON] = (KDElem *) 0;
		} else {
		    (void) kd_fault(KDF_F);
		}
		FREE(elem);
		(tree->dead_count)--;
		(tree->item_count)--;
		return del_element(tree, path_to_item[spot], spot);
	    } else {
		tree->tree = (KDElem *) 0;
		FREE(elem);
		(tree->dead_count)--;
		(tree->item_count)--;
		return KD_OK;
	    }
	}
    }
    /* No work required */
    return KD_OK;
}


/*
 * Generation of items
 */

#ifdef KD_PUSH_FUNC

#define KD_PUSH	kd_push

static void kd_push(gen, elem, disc)
KDState *gen;			/* Generator       */
KDElem *elem;			/* Element to push */
short disc;			/* Discriminator   */
/*
 * This routine pushes a new quad-tree element onto the generation
 * stack.  It also initializes the generation of items in
 * this element.
 */
{
    /* Allocate more space if necessary */
    if (gen->top_index >= gen->stack_size) {
	gen->stack_size += KD_GROWSIZE(gen->stack_size);
	gen->stk = KDREALLOC(KDSave, gen->stk, gen->stack_size);
    }
    gen->stk[gen->top_index].disc = disc;
    gen->stk[gen->top_index].state = KD_THIS_ONE;
    gen->stk[gen->top_index].item = elem;
    gen->top_index += 1;
}

#else

/* Inline version */

#define KD_PUSH(gen, elem, dk) \
    if ((gen)->top_index >= (gen)->stack_size) {                     \
	(gen)->stack_size += KD_GROWSIZE((gen)->stack_size);	     \
	(gen)->stk = KDREALLOC(KDSave, (gen)->stk, (gen)->stack_size); \
    }								     \
    (gen)->stk[(gen)->top_index].disc = (dk);		     	     \
    (gen)->stk[(gen)->top_index].state = KD_THIS_ONE;		     \
    (gen)->stk[(gen)->top_index].item = (elem);			     \
    (gen)->top_index += 1;

#endif

kd_gen kd_start(theTree, area)
kd_tree theTree;		/* Tree to generate from */
kd_box area;			/* Area to search 	 */
/*
 * This routine allocates a generator which can be used
 * to generate all items intersecting `area'.
 * The items are actually returned by kd_next.  Once the
 * sequence is finished,  kd_end should be called.
 */
{
    KDElem *realTree = ((KDTree *) theTree)->tree;
    KDState *newState;
    int i;

    newState = KDALLOC(KDState);

    for (i = 0;  i < KD_BOX_MAX;  i++) newState->extent[i] = area[i];

    newState->stack_size = KD_INIT_STACK;
    newState->top_index = 0;
    newState->stk = MULTALLOC(KDSave, KD_INIT_STACK);

    /* Initialize search state */
    if (realTree) {
	KD_PUSH(newState, realTree, 0);
    } else {
	newState->top_index = -1;
    }
    return (kd_gen) newState;
}


kd_status kd_next(theGen, data, size)
kd_gen theGen;			/* Current generator */
kd_generic *data;		/* Returned data     */
kd_box size;			/* Optional size     */
/*
 * Returns the next item in the generator sequence.  If
 * `size' is non-zero,  it will be filled with the item's
 * size.  If there are no more items,  it returns KD_NOMORE.
 */
{
    register KDState *realGen = (KDState *) theGen;
    register KDSave *top_elem;
    register KDElem *top_item;
    short hort;

    while (realGen->top_index > 0) {
	top_elem = &(realGen->stk[realGen->top_index-1]);
	top_item = top_elem->item;
	hort = top_elem->disc & 0x01;/* zero: vertical, one: horizontal */
	switch (top_elem->state) {
	case KD_THIS_ONE:
	    /* Check this one */
	    if (top_item->item && BOXINTERSECT(realGen->extent, top_item->size)) {
		*data = top_item->item;
		if (size) {
		    size[0] = top_item->size[0];  size[1] = top_item->size[1];
		    size[2] = top_item->size[2];  size[3] = top_item->size[3];
		}
		top_elem->state += 1;
		return KD_OK;
	    } else {
		top_elem->state += 1;
	    }
	    break;
	case KD_LOSON:
	    /* See if we push on the loson */
	    if (top_item->sons[KD_LOSON] &&
		((top_elem->disc & 0x02) ?
		 ((realGen->extent[hort] <= top_item->size[top_elem->disc]) &&
		  (realGen->extent[hort+2] >= top_item->lo_min_bound))
		 :
		 ((realGen->extent[hort] <= top_item->other_bound) &&
		  (realGen->extent[hort+2] >= top_item->lo_min_bound)))) {
		top_elem->state += 1;
		KD_PUSH(realGen, top_item->sons[KD_LOSON],
			NEXTDISC(top_elem->disc));
	    } else {
		top_elem->state += 1;
	    }
	    break;
	case KD_HISON:
	    /* See if we push on the hison */
	    if (top_item->sons[KD_HISON] &&
		((top_elem->disc & 0x02) ?
		 ((realGen->extent[hort] <= top_item->hi_max_bound) &&
		  (realGen->extent[hort+2] >= top_item->other_bound))
		 :
		 ((realGen->extent[hort] <= top_item->hi_max_bound) &&
		  (realGen->extent[hort+2] >= top_item->size[top_elem->disc])))) {
		top_elem->state += 1;
		KD_PUSH(realGen, top_item->sons[KD_HISON],
			NEXTDISC(top_elem->disc));
	    } else {
		top_elem->state += 1;
	    }
	    break;
	default:
	    /* We have exhausted this node -- pop off the next one */
	    realGen->top_index -= 1;
	    break;
	}
    }
    return KD_NOMORE;
}


void kd_finish(theGen)
kd_gen theGen;			/* Generator to destroy */
/*
 * Frees resources consumed by the specified generator.
 * This routine is NOT automatically called at the end
 * of a sequence.  Thus,  the user should ALWAYS calls
 * kd_finish to end a generation sequence.
 */
{
    KDState *realGen = (KDState *) theGen;

    FREE(realGen->stk);
    FREE(realGen);
}


#define KDR(t)	((KDTree *) (t))

int kd_count(tree)
kd_tree tree;
/* Returns the number of items in the specified tree */
{
    return KDR(tree)->item_count - KDR(tree)->dead_count;
}



#ifdef BENCH
char *package_name = "K-D Tree";
char *package_type = "k-d";
#endif
#define KD_PRINT
#ifdef KD_PRINT

static void pr_tree(elem, disc, depth)
KDElem *elem;
int disc;
int depth;
{
    int i;

    for (i = 0;  i < depth;  i++) putchar(' ');
    Printf("%d: %d %d %d (", (int) elem->item, elem->lo_min_bound,
		  elem->other_bound, elem->hi_max_bound);
    for (i = 0;  i < KD_BOX_MAX;  i++) {
	if (i == disc) putchar('*');
	Printf("%d ", elem->size[i]);
    }
    Printf(")\n");
    for (i = 0;  i < 2;  i++) {
	if (elem->sons[i]) {
	    pr_tree(elem->sons[i], NEXTDISC(disc), depth+3);
	}
    }
}

void kd_print(tree)
kd_tree tree;
/* Attempts to print out the tree assuming 160 characters across */
{
    KDTree *realTree = (KDTree *) tree;

    pr_tree(realTree->tree, 0, 0);
}

#endif

