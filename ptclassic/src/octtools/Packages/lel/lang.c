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
#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/***********************************************************************
 This software is a modified version of software originally developed by 
 the Electronics Research  Laboratory at the University of California at
 Berkeley.  The  following  copyright  notice  applies  to  all original 
 (non-MCC modified) software.

  +--------------------------------------------------------------------+
  | Copyright (c) 1988, 1989, Regents of the University of California. |
  | All rights reserved.                                               |
  +--------------------------------------------------------------------+

 To  designate  which  portions  of  the software are MCC modifications,
 compile-time conditionalizations are used

         #ifdef MCC
         #endif 
     and
         #ifndef MCC
         #endif

     or  in  a  few cases, the software is simply marked with a comment
     containing the name "MCC".

 The  following  copyright  notice  applies  to  all MCC  developed  or
 modified software:

  +-------------------------------------------------------------------+
  | Copyright (C) 1989 Microelectronics and Computer Technology Corp. |
  | MCC CAD Program                                                   |
  | All rights reserved.                                              |
  +-------------------------------------------------------------------+

  Note: Because  this  version  is a COMBINED MCC/UCB version, the code
        conditionalization   makes  the  source   more  fragmented  and
        difficult  to  follow.    Later  releases  will not  have  this 
        problem.   MCC  apologizes  for  any  inconveniences  this  may
        cause.

***********************************************************************
 * 
 * $Source$
 * $Author$
 * Original Author: David Harrison, UCB
 * $Revision$
 * Last Modified: $Date$
 * 
 * Modification History:
 *
 * 9/14/89 Kevin Gourley
 *         Added:
 *          instid_op
 *          labeled_op
 *          instname_op
 *          fmtcellname_op
 *          libname_op
 *          cellname_op
 *          viewname_op
 *          facetname_op
 *          portname_op
 *          instpropval_op
 *          propval_op
 *          if_op
 *          or_op
 *
 *          Added lbl arg to all *_op functions.
 * 9/28/89  Fixed bug in allocation of string in portname_op.
 * 9/28/89  Doubled the RING_SIZE.
 * 
 * $Log$
 * Revision 1.6  89/11/30  11:57:55  gourley
 * Fixed bugs in or_op and instname_op.  There was a memory "leak"
 * that was causing ring_alloc to fail.  It was a minor memory leak,
 * but the way ring_alloc and ring_free are designed, it appears that
 * if even ONE block in the ring is allocated but not freed, ring_alloc
 * will eventually fail due to the way freed data is reclaimed by ring_free.
 * 
 * 
***********************************************************************/

/*
 * Label Evaluation Language
 *
 * David Harrison
 * University of California
 * Berkeley 1989
 *
 * This is a small post-fix language used for evaulating expressions
 * in Oct label specifications.
 *
 *
 */

#include "port.h"
#include "st.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "errtrap.h"		/* For errRaise() */
#include "lel.h"



int ring_alloc_count = 0;
int ring_free_count = 0;
#define RING_SIZE	2048 /* MCC */
static long ring[RING_SIZE];
static long top = 0;
static long bottom = 0;

#define STD_PAT		0x5A000000
#define AL_BIT		0x04000000
#define OFF_PAT		0x00FFFFFF

static void err(msg)
char *msg;
/* Horribly nasty error occurred */
{
    errRaise("lel", 0, msg);
}

/*
 * Ring Allocation
 *
 * Performance tests showed that allocation is critical in this
 * small language.  To speed its response,  a ring based allocation
 * algorithm is used.  Since items are often allocated/deallocated
 * in a first-in/first-out pattern,  a ring was found to be an effective
 * allocation paradigm.
 */

static void ring_free(ptr)
char *ptr;
/*
 * Deallocates memory in ring.  If it is the last item in the ring,
 * it moves the last item pointer up to the next allocated block.
 */
{
    long offset = (long *) ptr - ring - 1;

    /* Mark as unallocated */
    if ((ring[offset] & STD_PAT) != STD_PAT) {
	err("ring_free: corrupt free list");
    }
    if (ring[offset] & AL_BIT) {
	ring[offset] &= ~(AL_BIT);
    } else {
	err("ring_free: pointer already free");
    }
    ring_free_count += ((ring[offset] & OFF_PAT)-1);
    if (offset == bottom) {
	/* Move bottom forward */
	while ((!(ring[bottom] & AL_BIT)) && (bottom != top)) {
	    if ((ring[bottom] & STD_PAT) != STD_PAT) {
		err("ring_free: corrupt free list");
	    }
	    bottom += (ring[bottom] & OFF_PAT);
	    if (bottom >= RING_SIZE) bottom -= RING_SIZE;
	}
    }
}

static char *ring_alloc(bytes)
unsigned bytes;
/*
 * Ring based allocator.  Moves the top pointer forward as far as
 * necessary and links the old top block to point at the new block.
 */
{
    char *result;
    char error_message[256];

    if (bytes & 0x3) {
	bytes = (bytes >> 2) + 1;
    } else {
	bytes = (bytes >> 2);
    }
    if ((bottom - top + ((top >= bottom) ? RING_SIZE : 0)) < bytes) {
	/* No space - should create new larger ring */
	(void) sprintf(error_message,"ring_alloc: no more space - bytes=%d bot=%ld top=%ld size=%d",
		bytes, bottom, top, RING_SIZE);
	err(error_message);
    }
    if (bytes <= RING_SIZE-top-1) {
	ring_alloc_count += bytes;
	ring[top] = (bytes+1) | STD_PAT | AL_BIT;
	result = (char *) &(ring[top+1]);
	top += bytes+1;
	if (top >= RING_SIZE) top -= RING_SIZE;
	return result;
    } else {
	/* Allocate to end and then reallocate */
	ring_free(ring_alloc((unsigned) (((RING_SIZE-top)-1)<<2)));
	return ring_alloc(bytes<<2);
    }
}



#define ALLOC(type, num) \
(type *) ring_alloc((unsigned) (num * sizeof(type)))

#define FREE(ptr) \
(void) ring_free((char *) ptr)

typedef enum _stack_type { INT, REAL, STR, OP } stack_type;

typedef struct _int_val {
    stack_type type;		/* INT */
    long value;
} int_val;

typedef struct _real_val {
    stack_type type;		/* REAL */
    double value;
} real_val;

typedef struct _str_val {
    stack_type type;		/* STR or OP */
    char *value;
} str_val;

typedef union _stack_item {
    stack_type type;
    int_val integer;
    real_val real;
    str_val string;
} stack_item;

#define MAX_DEPTH	10
static stack_item 	stack[MAX_DEPTH];
static int 		stack_top = -1;
static int		stack_idx;

#define FREE_ITEM(item) \
if (((item).type == STR) || ((item).type == OP)) { \
   FREE((item).string.value);			   \
}

#define PUSH(item)	\
if (stack_top >= MAX_DEPTH) {  	       	       	       	       	    \
    FREE_ITEM(stack[0]);					    \
    for (stack_idx = 0;  stack_idx < MAX_DEPTH-1;  stack_idx++) {   \
	stack[stack_idx] = stack[stack_idx+1];			    \
    }								    \
    stack_top--;						    \
}								    \
stack[stack_top++] = *(item);

#define POP		\
(stack_top > 0 ? &stack[--stack_top] : (stack_item *) 0)

static st_table *op_table = (st_table *) 0;

/*
 * Token type tables.  Each is an array of characters.  Non-zero
 * entries in the array indicate the character is an appropriate
 * member of the indicated type.
 */

#define MAX_CHAR	128
static char 		int_tbl[MAX_CHAR];
static char		real_tbl[MAX_CHAR];


/*
 * Operator functions
 */

#define MIN_SIZE	4

static stack_item *null_item(item, type)
stack_item *item;
stack_type type;
/*
 * Fills in `item' with an appropriate null of type `type'.
 */
{
    item->type = type;
    switch (type) {
    case INT:
	item->integer.value = 0;
	break;
    case REAL:
	item->real.value = 0.0;
	break;
    case STR:
    case OP:
	item->string.value = ALLOC(char, MIN_SIZE);
	item->string.value[0] = '\0';
	break;
    }
    return item;
}

/* MCC */
#define ITEM_IS_NONNULL(item) \
    (((item)->type==INT  && (item)->integer.value!=0) ||  \
     ((item)->type==REAL && (item)->real.value!=0.0) ||   \
     ((item)->type==STR  && ((item)->string.value &&      \
			     (item)->string.value[0]!='\0')) || \
     ((item)->type==OP   && ((item)->string.value &&      \
			     (item)->string.value[0]!='\0')))

/* MCC */
#define COPY_ITEM(dst,src) \
switch ((dst).type = (src).type) { \
  case INT: \
    (dst).integer.value = (src).integer.value; \
    break;\
  case REAL:\
    (dst).real.value = (src).real.value;\
    break;\
  case STR:\
  case OP:\
    if ((src).string.value) {\
       (dst).string.value = ALLOC(char,strlen((src).string.value)+1); \
       strcpy((dst).string.value,(src).string.value);\
    } else { \
       (dst).string.value = NULL; \
    }\
    break;\
  default:\
    break;\
}


#define MAX_INT_LENGTH	50
#define MAX_REAL_LENGTH	100

static void convert(type, item)
stack_type type;
stack_item *item;
/*
 * Converts `item' into `type'.  The most straightforward approach is used.
 */
{
    stack_item old;

    if (type == item->type) return;
    old = *item;
    switch (item->type) {
    case INT:
	if (type == REAL) {
	    item->real.value = (double) old.integer.value;
	} else {
	    item->string.value = ALLOC(char, MAX_INT_LENGTH);
	    (void) sprintf(item->string.value, "%ld", old.integer.value);
	}
	break;
    case REAL:
	if (type == INT) {
	    if (old.real.value < 0.0) {
		item->integer.value = (int) (old.real.value - 0.5);
	    } else {
		item->integer.value = (int) (old.real.value + 0.5);
	    }
	} else {
	    item->string.value = ALLOC(char, MAX_REAL_LENGTH);
	    (void) sprintf(item->string.value, "%g", old.real.value);
	}
	break;
    case STR:
    case OP:
	if (type == INT) {
	    item->integer.value = 0;
	    (void) sscanf(old.string.value, "%ld", &item->integer.value);
	    FREE(old.string.value);
	} else if (type == REAL) {
	    item->real.value = 0.0;
	    (void) sscanf(old.string.value, "%lf", &item->real.value);
	    FREE(old.string.value);
	} else {
	    /* Nothing - types compatible */
	}
	break;
    }
    item->type = type;
}

static stack_type bin_args(one, two)
stack_item **one, **two;
/*
 * Automatically fills in two arguments suitable for a binary operator.
 * The types are automatically upgraded from INT to REAL to STR as
 * needed.  The resulting items are guaranteed to be both of the same
 * type.  That type is returned.
 */
{
    static stack_item null_one, null_two;
    stack_type result;

    *one = POP;
    if (!*one) *one = null_item(&null_one, STR);
    *two = POP;
    if (!*two) *two = null_item(&null_two, (*one)->type);
    if (((*one)->type == STR) || ((*two)->type == STR)) {
	result = STR;
    } else if (((*one)->type == REAL) || ((*two)->type == REAL)) {
	result = REAL;
    } else {
	result = INT;
    }
    convert(result, *one);
    convert(result, *two);
    return result;
}


/*ARGSUSED*/
static void plus_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Adds two top most items pushes the result.  Upgrades types from
 * integers to reals to strings as necessary.
 */
{
    stack_item *one, *two;
    stack_item result;
    int tot_len;

    result.type = bin_args(&one, &two);
    switch (result.type) {
    case INT:
	result.integer.value = one->integer.value + two->integer.value;
	break;
    case REAL:
	result.real.value = one->real.value + two->real.value;
	break;
    case STR:
    case OP:
	tot_len = strlen(one->string.value) + strlen(two->string.value) + 1;
	result.string.value = ALLOC(char, tot_len);
	(void) strcpy(result.string.value, two->string.value);
	(void) strcat(result.string.value, one->string.value);
	break;
    }
    FREE_ITEM(*one);
    FREE_ITEM(*two);
    PUSH(&result);
}



/* MCC */
/*ARGSUSED*/
static void instid_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the OCT instance id on the stack.
 */
{
    stack_item result;
    static char generic_string[]="%instid%";
    int32 xid;

    if (inst) {
	result.type = INT;
	xid = 0;
	octExternalId(inst,&xid);
	result.integer.value = xid;
    } else {
	result.type = STR;
	result.string.value = ALLOC(char, strlen(generic_string)+1);
	(void) strcpy(result.string.value,generic_string);
    } 
    PUSH(&result);
}




/* MCC */
/*ARGSUSED*/
static void labeled_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the xid of the label object's first contents
 * (i.e., the first object attached to this lable)
 */
{
    stack_item result;
    octObject  labeled_obj;
    int32 xid;

    if (lbl &&
	octGenFirstContent(lbl,OCT_ALL_MASK,&labeled_obj)==OCT_OK) {
	result.type = INT;
	xid = 0;
	octExternalId(&labeled_obj,&xid);
	result.integer.value = xid;
    } else {
	null_item(&result,INT);
    } 
    PUSH(&result);
}


/* MCC */
/*ARGSUSED*/
static void if_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * %<false_val> <true_val> <test> if%
 */
{
    stack_item result,*test,*true_val,*false_val;

    if (!(test = POP)) {
	result.type=STR;
	result.string.value = ALLOC(char, 10);
	(void) strcpy(result.string.value, "?if:test?");
	PUSH(&result);
	return;
    }
    if (!(true_val = POP)) {
	result.type=STR;
	result.string.value = ALLOC(char, 10);
	(void) strcpy(result.string.value, "?if:Tval?");
	FREE_ITEM(*test);
	PUSH(&result);
	return;
    }
    if (!(false_val = POP)) {
	result.type=STR;
	result.string.value = ALLOC(char, 10);
	(void) strcpy(result.string.value, "?if:Fval?");
	FREE_ITEM(*true_val);
	FREE_ITEM(*test);
	PUSH(&result);
	return;
    } 
   if (ITEM_IS_NONNULL(test)) {
	COPY_ITEM(result,*true_val);
    } else {
	COPY_ITEM(result,*false_val);
    }
    FREE_ITEM(*false_val);
    FREE_ITEM(*true_val);
    FREE_ITEM(*test);    
    PUSH(&result);
}




/* MCC */
/*ARGSUSED*/
static void or_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * %... <val> <n> or%
 */
{
    stack_item result,*val,*nval;
    int i,n,found_nonnull;

    if (!(nval = POP)) {
	result.type = STR;
	result.string.value = ALLOC(char, 10);
	(void) strcpy(result.string.value, "?or:n?");
	PUSH(&result);
	return;
    }
    convert(INT,*nval);
    n = nval->integer.value;
    FREE_ITEM(*nval);

    if (n<=0) {
	null_item(&result,STR);
	PUSH(&result);
	return;
    }
    for (i=0,found_nonnull=0; i<n; i++) {
	val = POP;
	if (!val) {
	    result.type = STR;
	    result.string.value = ALLOC(char, 10);
	    (void) strcpy(result.string.value, "?or:val?");
	    PUSH(&result);
	    return;
	}
	if (!found_nonnull &&
	    ITEM_IS_NONNULL(val)) {
	    COPY_ITEM(result,*val);
	    found_nonnull = 1;
	} 
	FREE_ITEM(*val);
    }
    if (!found_nonnull) null_item(&result,STR);
    PUSH(&result);
}



/* MCC */
/*ARGSUSED*/
static void instname_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the OCT instance id name on the stack.
 */
{
    stack_item result;
    static char generic_string[]="%instname%";

    if (inst) {
	if (inst->contents.instance.name &&
	    inst->contents.instance.name[0]!='\000') {
	    result.type = STR;
	    result.string.value = ALLOC(char, strlen(inst->contents.instance.name)+1);
	    (void) strcpy(result.string.value, inst->contents.instance.name);
	    PUSH(&result);
	    return;
	}
    } else {
	result.type = STR;
	result.string.value = ALLOC(char, strlen(generic_string)+1);
	(void) strcpy(result.string.value,generic_string);
	PUSH(&result);
	return;
    } 
    null_item(&result,STR);
    PUSH(&result);
}




/* MCC */
/*ARGSUSED*/
static void portname_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the port name of the given port instance on the stack.
 */
{
    octObject actual,formal;
    static char generic_string[]="%portname%";
    stack_item result;

    if (inst) {
	if (octGenFirstContent(inst,OCT_TERM_MASK,&actual)==OCT_OK) {
	    result.type = STR;
	    if (octGenFirstContainer(&actual,OCT_TERM_MASK,&formal)==OCT_OK &&
		formal.contents.term.name) {
		result.string.value = ALLOC(char,strlen(formal.contents.term.name)+1);
		(void) strcpy(result.string.value,formal.contents.term.name);
	    } else {
		result.string.value = ALLOC(char,strlen(actual.contents.term.name)+1);
		(void) strcpy(result.string.value,actual.contents.term.name);
	    }
	} else {
	    null_item(&result,STR);
	}
    } else {
	result.type = STR;
	result.string.value = ALLOC(char, strlen(generic_string)+1);
	(void) strcpy(result.string.value,generic_string);
    }
    PUSH(&result);
}


/* MCC */
/*ARGSUSED*/
static void libname_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the OCT library name on the stack.  
 *
 */
{
    stack_item result;
    char *cptr,lib_name[MAX_CHAR];

    if (fct->contents.facet.cell) {
	result.type = STR;
	strcpy(lib_name,fct->contents.facet.cell);
	if ( (cptr=strrchr(lib_name,'/')) ) {
	    *cptr='\0';
	} else {
	    lib_name[0]='\0';
	}
	result.string.value = ALLOC(char,strlen(lib_name)+1);
	strcpy(result.string.value, lib_name);
    } else {
	null_item(&result,STR);
    }
    PUSH(&result);
}



/* MCC */
/*ARGSUSED*/
static void cellname_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the OCT cell name on the stack.  
 *
 */
{
    stack_item result;
    char *cptr;

    if (fct->contents.facet.cell) {
	result.type = STR;
	if ( (cptr=strrchr(fct->contents.facet.cell,'/')) ) {
	    ++cptr;
	    result.string.value = ALLOC(char,strlen(cptr)+1);
	    strcpy(result.string.value, cptr);
	} else {
	    result.string.value = ALLOC(char,strlen(fct->contents.facet.cell)+1);
	    strcpy(result.string.value, fct->contents.facet.cell);
	}
    } else {
	null_item(&result,STR);
    }
    PUSH(&result);
}



/* MCC */
/*ARGSUSED*/
static void viewname_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the OCT view name on the stack.  
 *
 */
{
    stack_item result;

    if (fct->contents.facet.view) {
	result.type = STR;
	result.string.value = ALLOC(char,strlen(fct->contents.facet.view)+1);
	strcpy(result.string.value, fct->contents.facet.view);
    } else {
	null_item(&result,STR);
    }
    PUSH(&result);
}



/* MCC */
/*ARGSUSED*/
static void facetname_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the OCT facet name on the stack.  
 *
 */
{
    stack_item result;

    if (fct->contents.facet.facet) {
	result.type = STR;
	result.string.value = ALLOC(char,strlen(fct->contents.facet.facet)+1);
	strcpy(result.string.value, fct->contents.facet.facet);
    } else {
	null_item(&result,STR);
    }
    PUSH(&result);
}



/* MCC */
/*ARGSUSED*/
static void fmtcellname_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Pushes the formatted OCT cell name on the stack.  Arg 1 is
 * used as the format.
 * $l = library name
 * $c = cell name
 * $v = view name
 * $f = facet name
 */
{
#define FMT_ESC_CHAR '$'
    stack_item result,*format;
    char *cptr,
         *format_string,
          lib_name[MAX_CHAR],cell_name[MAX_CHAR],temp_string[MAX_CHAR];
    int i,j,escape;

    temp_string[0]='\0';
    format = POP;
    result.type = STR;
    if (format==NULL ||
	format->type != STR) {
	result.string.value = ALLOC(char, 15);
	(void) strcpy(result.string.value, "?fmtcellname?");
	PUSH(&result);
	return;
    }
    format_string = format->string.value;
    strcpy(lib_name,fct->contents.facet.cell);
    if ( (cptr=strrchr(lib_name,'/')) ) {
	strcpy(cell_name,++cptr);
	*--cptr='\0';
    } else {
	strcpy(cell_name,fct->contents.facet.cell);
	lib_name[0]='\0';
    }
    for (i=0,j=0,escape=0; i<strlen(format_string), j<MAX_CHAR; i++) {
	if (escape) {
	    temp_string[j]='\0';
	    switch (format_string[i]) {
	      case 'l':
		strcat(temp_string,lib_name);
		j += strlen(lib_name);
		break;
	      case 'c':
		strcat(temp_string,cell_name);
		j += strlen(cell_name);
		break;
	      case 'v':
		strcat(temp_string,fct->contents.facet.view);
		j += strlen(fct->contents.facet.view);
		break;
	      case 'f':
		strcat(temp_string,fct->contents.facet.facet);
		j += strlen(fct->contents.facet.facet);
		break;
	      default:
		break;
	    }
	    escape=0;
	} else {
	    if (!(escape=(format_string[i]==FMT_ESC_CHAR)))
		temp_string[j++] = format_string[i];
	}
    }
    result.string.value = ALLOC(char,
				strlen(temp_string)+1);
    strcpy(result.string.value, temp_string);
    FREE_ITEM(*format);
    PUSH(&result);
}



/*ARGSUSED*/
static void minus_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Subtracts the top most item from the next top most and pushes the
 * result.  Subtraction of integer and real numbers is defined
 * in a customary fashion.  Subtraction of strings is not defined
 * and an error message is pushed onto the stack.
 */
{
    stack_item *one, *two;
    stack_item result;

    result.type = bin_args(&one, &two);
    switch (result.type) {
    case INT:
	result.integer.value = two->integer.value - one->integer.value;
	break;
    case REAL:
	result.real.value = two->real.value - one->real.value;
	break;
    case STR:
    case OP:
	/* Subtraction of strings not defined */
	result.string.value = ALLOC(char, 15);
	(void) strcpy(result.string.value, "?str -?");
	break;
    }
    FREE_ITEM(*one);
    FREE_ITEM(*two);
    PUSH(&result);
}



/*ARGSUSED*/
static void times_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Multiplies the top most item and the next top most and pushes the
 * result.  Multiplication of integer and real numbers is defined
 * in a customary fashion.  Multiplication of strings is not defined
 * and an error message is pushed onto the stack.
 */
{
    stack_item *one, *two;
    stack_item result;

    result.type = bin_args(&one, &two);
    switch (result.type) {
    case INT:
	result.integer.value = two->integer.value * one->integer.value;
	break;
    case REAL:
	result.real.value = two->real.value * one->real.value;
	break;
    case STR:
    case OP:
	/* Multiplication of strings not defined */
	result.string.value = ALLOC(char, 15);
	(void) strcpy(result.string.value, "?str *?");
	break;
    }
    FREE_ITEM(*one);
    FREE_ITEM(*two);
    PUSH(&result);
}


/*ARGSUSED*/
static void divide_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/* 
 * Divides the top most item into the next top most and pushes the
 * result.  Division of integer and real numbers is defined
 * in a customary fashion.  Division of strings is not defined
 * and an error message is pushed onto the stack.  Dividing by
 * zero is also an error.
 */
{
    stack_item *one, *two;
    stack_item result;

    result.type = bin_args(&one, &two);
    switch (result.type) {
    case INT:
	if (one->integer.value == 0) {
	    result.type = STR;
	    result.string.value = ALLOC(char, 15);
	    (void) strcpy(result.string.value, "?0 /?");
	} else {
	    result.integer.value = two->integer.value / one->integer.value;
	}
	break;
    case REAL:
	if (one->real.value == 0.0) {
	    result.type = STR;
	    result.string.value = ALLOC(char, 15);
	    (void) strcpy(result.string.value, "?0 /?");
	} else {
	    result.real.value = two->real.value / one->real.value;
	}
	break;
    case STR:
    case OP:
	/* Division of strings not defined */
	result.string.value = ALLOC(char, 15);
	(void) strcpy(result.string.value, "?str /?");
	break;
    }
    FREE_ITEM(*one);
    FREE_ITEM(*two);
    PUSH(&result);
}


/*ARGSUSED*/
static void name_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pops off the top most item converts it into an integer and looks
 * it up using octGetByExternalId.  The name field of the object
 * is pushed onto the stack as a string.  If the object can't be
 * found or the object has no name,  an error message is pushed.
 */
{
    stack_item *one;
    stack_item result;
    octObject obj;

    one = POP;
    if (!one) {
	result.type = STR;
	result.string.value = ALLOC(char, 10);
	(void) strcpy(result.string.value, "?id?");
	PUSH(&result);
	return;
    } else {
	convert(INT, one);
    }
    result.type = STR;
    if (octGetByExternalId(fct, one->integer.value, &obj) >= OCT_OK) {
	switch (obj.type) {
	case OCT_FACET:
	    result.string.value = ALLOC(char,
					strlen(obj.contents.facet.cell) +
					strlen(obj.contents.facet.view) +
					strlen(obj.contents.facet.facet) + 3);
	    (void) strcpy(result.string.value, obj.contents.facet.cell);
	    (void) strcat(result.string.value, ":");
	    (void) strcat(result.string.value, obj.contents.facet.view);
	    (void) strcat(result.string.value, ":");
	    (void) strcat(result.string.value, obj.contents.facet.facet);
	    break;
	case OCT_TERM:
	    result.string.value = ALLOC(char, strlen(obj.contents.term.name)+1);
	    (void) strcpy(result.string.value, obj.contents.term.name);
	    break;
	case OCT_NET:
	    result.string.value = ALLOC(char, strlen(obj.contents.net.name)+1);
	    (void) strcpy(result.string.value, obj.contents.net.name);
	    break;
	case OCT_INSTANCE:
	    result.string.value = ALLOC(char, strlen(obj.contents.instance.name)+1);
	    (void) strcpy(result.string.value, obj.contents.instance.name);
	    break;
	case OCT_PROP:
	    result.string.value = ALLOC(char, strlen(obj.contents.prop.name)+1);
	    (void) strcpy(result.string.value, obj.contents.prop.name);
	    break;
	case OCT_BAG:
	    result.string.value = ALLOC(char, strlen(obj.contents.bag.name)+1);
	    (void) strcpy(result.string.value, obj.contents.bag.name);
	    break;
	case OCT_LAYER:
	    result.string.value = ALLOC(char, strlen(obj.contents.layer.name)+1);
	    (void) strcpy(result.string.value, obj.contents.layer.name);
	    break;
	default:
	    result.string.value = ALLOC(char, 15);
	    (void) strcpy(result.string.value, "?no name?");
	    break;
	}
    } else {
	char errbuf[1024];

	(void) sprintf(errbuf, "?%ld?", one->integer.value);
	result.string.value = ALLOC(char, strlen(errbuf)+1);
	(void) strcpy(result.string.value, errbuf);
    }
    FREE_ITEM(*one);
    PUSH(&result);
}



static void value_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pops off the top most item converts it into an integer and looks
 * it up using octGetByExternalId.  The value field of the object
 * is pushed onto the stack as an appropriate type.  If the object
 * is a property,  lelFindProp will be used to resolve the property.
 * If the object isn't found or the object has no name,  an error 
 * message is pushed on the stack as result.
 */
{
    stack_item *one;
    stack_item result;
    octObject obj, container;

    one = POP;
    if (!one) {
	result.type = STR;
	result.string.value = ALLOC(char, 10);
	(void) strcpy(result.string.value, "?id?");
	PUSH(&result);
	return;
    } else {
	convert(INT, one);
    }
    if (octGetByExternalId(fct, one->integer.value, &obj) >= OCT_OK) {
	if ((obj.type == OCT_PROP) &&
	    (octGenFirstContainer(&obj, OCT_FACET_MASK|
				  OCT_TERM_MASK|
				  OCT_INSTANCE_MASK,
				  &container) == OCT_OK)) {
	    (void) lelFindProp(inst, &container, &obj,
			      fct_spec, obj.contents.prop.name);
	}
	switch (obj.type) {
	case OCT_INSTANCE:
	    result.type = STR;
	    result.string.value = ALLOC(char,
					strlen(obj.contents.instance.master) +
					strlen(obj.contents.instance.view) +
					strlen(obj.contents.instance.facet) + 3);
	    (void) strcpy(result.string.value, obj.contents.instance.master);
	    (void) strcat(result.string.value, ":");
	    (void) strcat(result.string.value, obj.contents.instance.view);
	    (void) strcat(result.string.value, ":");
	    (void) strcat(result.string.value, obj.contents.instance.facet);
	    break;
	case OCT_PROP:
	    switch (obj.contents.prop.type) {
	    case OCT_INTEGER:
		result.type = INT;
		result.integer.value = obj.contents.prop.value.integer;
		break;
	    case OCT_REAL:
		result.type = REAL;
		result.real.value = obj.contents.prop.value.real;
		break;
	    case OCT_STRING:
		result.type = STR;
		result.string.value = ALLOC(char,
					    strlen(obj.contents.prop.value.string)+1);
		(void) strcpy(result.string.value,
			      obj.contents.prop.value.string);
		break;
	    default:
		result.type = STR;
		result.string.value = ALLOC(char, 15);
		(void) strcpy(result.string.value, "?type?");
		break;
	    }
	    break;
	case OCT_LABEL:
	    result.type = STR;
	    result.string.value = ALLOC(char, strlen(obj.contents.label.label)+1);
	    (void) strcpy(result.string.value, obj.contents.label.label);
	    break;
	default:
	    result.string.value = ALLOC(char, 15);
	    (void) strcpy(result.string.value, "?no value?");
	    break;
	}
    } else {
	char errbuf[1024];

	result.type = STR;
	(void) sprintf(errbuf, "?%d?", (int)one->integer.value);
	result.string.value = ALLOC(char, strlen(errbuf)+1);
	(void) strcpy(result.string.value, errbuf);
    }
    FREE_ITEM(*one);
    PUSH(&result);
}



/* MCC */
static void instpropval_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pops off the top most item as a property name and pushes the
 * value of the named property onto the stack.  If inst is NULL,
 * the value of the name property is used.  If inst is not NULL,
 * the value of the property on the instance is used (if it exists),
 * otherwise, the instance's master's property is used (if it exists).
 */
{
    stack_item *one;
    stack_item result;
    octObject obj;

    one = POP;
    if (!one) {
	result.type = STR;
	result.string.value = ALLOC(char, 14);
	(void) strcpy(result.string.value, "?instpropval?");
	PUSH(&result);
	return;
    } else if (one->type != STR ||
	       one->string.value == NULL ||
	       one->string.value[0]=='\0') {
	null_item(&result,STR);
	FREE_ITEM(*one);
	PUSH(&result);
	return;
    }
    if (lelFindProp(inst,fct,&obj,fct_spec,one->string.value)==OCT_OK) {
	switch (obj.contents.prop.type) {
	  case OCT_INTEGER:
	    result.type = INT;
	    result.integer.value = obj.contents.prop.value.integer;
	    break;
	  case OCT_REAL:
	    result.type = REAL;
	    result.real.value = obj.contents.prop.value.real;
	    break;
	  case OCT_STRING:
	    result.type = STR;
	    result.string.value = ALLOC(char,
					strlen(obj.contents.prop.value.string)+1);
	    (void) strcpy(result.string.value,
			  obj.contents.prop.value.string);
	    break;
	  default:
	    result.type = STR;
	    result.string.value = ALLOC(char, 19);
	    (void) strcpy(result.string.value, "?instpropval:type?");
	    break;
	}
    } else {
	null_item(&result,STR);
    }
    FREE_ITEM(*one);
    PUSH(&result);
}




/* MCC */
static void propval_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pops off the top most item as a property name and pushes the
 * value of the named property onto the stack.  If inst is NULL,
 * the value of the named property is used.  If inst is not NULL,
 * the value of the property on the instance's master's property
 * is used (if it exists).
 */
{
    stack_item *one;
    stack_item result;
    octObject obj;

    one = POP;
    if (!one) {
	result.type = STR;
	result.string.value = ALLOC(char, 10);
	(void) strcpy(result.string.value, "?propval?");
	PUSH(&result);
	return;
    } else if (one->type != STR ||
	       one->string.value == NULL ||
	       one->string.value[0]=='\0') {
	null_item(&result,STR);
	FREE_ITEM(*one);
	PUSH(&result);
	return;
    }
    obj.type = OCT_PROP;
    obj.contents.prop.name = one->string.value;
    if (octGetByName(fct,&obj)==OCT_OK) {
	switch (obj.contents.prop.type) {
	  case OCT_INTEGER:
	    result.type = INT;
	    result.integer.value = obj.contents.prop.value.integer;
	    break;
	  case OCT_REAL:
	    result.type = REAL;
	    result.real.value = obj.contents.prop.value.real;
	    break;
	  case OCT_STRING:
	    result.type = STR;
	    result.string.value = ALLOC(char,
					strlen(obj.contents.prop.value.string)+1);
	    (void) strcpy(result.string.value,
			  obj.contents.prop.value.string);
	    break;
	  default:
	    result.type = STR;
	    result.string.value = ALLOC(char, 19);
	    (void) strcpy(result.string.value, "?propval:type?");
	    break;
	}
    } else {
	null_item(&result,STR);
    }
    FREE_ITEM(*one);
    PUSH(&result);
}



/*ARGSUSED*/
static void sci_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pops off three items off of the stack.  The two top most are
 * converted to integers and are used as the field width and
 * precision for the conversion of the last number into
 * scientific notation.  The routine pushes the result back
 * on the stack as a string.  This is implemented using
 * sprintf and the %e option.  The operator is very resilent
 * to bad arguments.
 */
{
    stack_item one, two, three, result;
    stack_item *fw, *pre, *num;
    char format[100];

    if (!(pre = POP)) pre = null_item(&two, INT);
    convert(INT, pre);
    if (!(fw = POP)) fw = null_item(&one, INT);
    convert(INT, fw);
    if (!(num = POP)) num = null_item(&three, REAL);
    convert(REAL, num);

    result.type = STR;
    result.string.value = ALLOC(char, (fw->integer.value > MAX_REAL_LENGTH) ?
				fw->integer.value+1 : MAX_REAL_LENGTH);
    if (fw->integer.value != 0) {
	if (pre->integer.value > 0) {
	    (void) sprintf(format, "%%%d.%de", (int)fw->integer.value,
			   (int)pre->integer.value);
	} else {
	    (void) sprintf(format, "%%%de", (int)fw->integer.value);
	}
    } else if (pre->integer.value > 0) {
	(void) sprintf(format, "%%.%de", (int)pre->integer.value);
    } else {
	(void) strcpy(format, "%e");
    }
    (void) sprintf(result.string.value, format, num->real.value);
    FREE_ITEM(*fw);
    FREE_ITEM(*pre);
    FREE_ITEM(*num);
    PUSH(&result);
}


/*
 * Engineering notation
 * 
 * Most of this is stolen from octspice (thanks, Tom)
 */

struct engrScale {
    double low;
    double factor;
    char *prefix;
};
static struct engrScale fullScale[] = {
    {	1e+16,	1,	""	},
    {	1e+12,	1e+12,	"T"	},
    {	1e+9,	1e+9,	"G"	},
    {	1e+6,	1e+6,	"M"	},
    {	1e+3,	1e+3,	"k"	},
    {	1e-1,	1,	""	},
    {	1e-3,	1e-3,	"m"	},
    {	1e-6,	1e-6,	"u"	},
    {	1e-9,	1e-9,	"n"	},
    {	1e-12,	1e-12,	"p"	},
    {	1e-15,	1e-15,	"f"	},
    {	1e-18,	1e-18,	"a"	},
    {	0,	1,	""	},
};
#ifdef NEVER
/* These are apparently unused */
static struct engrScale capScale[] = {
    {	1e-1,	1,	""	},
    {	1e-8,	1e-6,	"u"	},
    {	1e-12,	1e-12,	"p"	},
    {	1e-15,	1e-15,	"f"	},
    {	1e-18,	1e-18,	"a"	},
    {	0,	1,	""	},
};
static struct engrScale noScale[] = {
    {	0,	1,	""	},
};
#endif



/*ARGSUSED*/
static void eng_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pops off three items off of the stack.  The two top most are
 * converted to integers and are used as the field width and
 * precision for the conversion of the last number into
 * engineering notation.  The routine pushes the result back
 * on the stack as a string.  This is implemented using
 * scaling arrays and sprintf with the %g operator.
 */
{
    stack_item one, two, three, result;
    stack_item *fw, *pre, *num;
    struct engrScale *idx;
    int neg_flag;
    char format[20];

    if (!(pre = POP)) pre = null_item(&two, INT);
    convert(INT, pre);
    if (!(fw = POP)) fw = null_item(&one, INT);
    convert(INT, fw);
    if (!(num = POP)) num = null_item(&three, REAL);
    convert(REAL, num);

    result.type = STR;
    result.string.value = ALLOC(char, (fw->integer.value > MAX_REAL_LENGTH) ?
				fw->integer.value+1 : MAX_REAL_LENGTH);
    if (num->real.value < 0.0) {
	num->real.value = -(num->real.value);
	neg_flag = 1;
    } else {
	neg_flag = 0;
    }
    for (idx = fullScale;  idx->low > num->real.value;  idx++) {
	/* Empty body */
    }
    if (neg_flag) num->real.value = -(num->real.value);
    if (pre->integer.value > 0) {
	(void) sprintf(format, "%%.%dg%s", (int)pre->integer.value,
		       idx->prefix); 
    } else {
	(void) sprintf(format, "%%g%s", idx->prefix);
    }
    if (fw->integer.value != 0) {
	char scratch[MAX_REAL_LENGTH];

	(void) sprintf(scratch, format, num->real.value/idx->factor);
	(void) sprintf(format, "%%%ds", (int)fw->integer.value);
	(void) sprintf(result.string.value, format, scratch);
    } else {
	(void) sprintf(result.string.value, format, num->real.value/idx->factor);
    }
    FREE_ITEM(*fw);
    FREE_ITEM(*pre);
    FREE_ITEM(*num);
    PUSH(&result);
}


/*ARGSUSED*/
static void lambda_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pushes the value of TAP_LAMBDA on the stack as an integer.
 */
{
    stack_item result;

    result.type = INT;
    if (tapGetProp(fct, TAP_LAMBDA)) {
	result.integer.value = TAP_LAMBDA->value.integer;
    } else {
	result.integer.value = -1;
    }
    PUSH(&result);
}

/*ARGSUSED*/
static void coord_op(fct, inst, lbl, fct_spec)
octObject *fct, *inst, *lbl;
char *fct_spec;
/*
 * Pushes the value of TAP_COORD_SIZE on the stack as a real number.
 */
{
    stack_item result;

    result.type = REAL;
    if (tapGetProp(fct, TAP_COORD_SIZE)) {
	result.real.value = TAP_COORD_SIZE->value.real;
    } else {
	result.real.value = -1.0;
    }
    PUSH(&result);
}


static void init()
/*
 * Initializes the package.  Tables for determining token types are
 * constructed and the basic operator table is filled.
 */
{
    int i;
    char c;

    for (i = 0;  i < MAX_CHAR;  i++) {
	int_tbl[i] = real_tbl[i] = '\0';
    }
    for (c = '0';  c <= '9';  c++) {
	int_tbl[(int)c] = c;
	real_tbl[(int)c] = c;
    }
    int_tbl['+'] = real_tbl['+'] = '+';
    int_tbl['-'] = real_tbl['-'] = '-';
    real_tbl['e'] = 'e';
    real_tbl['E'] = 'E';
    real_tbl['.'] = '.';

    op_table = st_init_table(strcmp, st_strhash);
    (void) st_insert(op_table, "+", (char *) plus_op);
    (void) st_insert(op_table, "-", (char *) minus_op);
    (void) st_insert(op_table, "*", (char *) times_op);
    (void) st_insert(op_table, "/", (char *) divide_op);
    (void) st_insert(op_table, "name", (char *) name_op);
    (void) st_insert(op_table, "value", (char *) value_op);
    (void) st_insert(op_table, "sci", (char *) sci_op);
    (void) st_insert(op_table, "eng", (char *) eng_op);
    (void) st_insert(op_table, "lambda", (char *) lambda_op);
    (void) st_insert(op_table, "coord", (char *) coord_op);
  /* MCC added operators: */
    (void) st_insert(op_table, "instid", (char *) instid_op);
    (void) st_insert(op_table, "labeled", (char *) labeled_op);
    (void) st_insert(op_table, "instname", (char *) instname_op);
    (void) st_insert(op_table, "fmtcellname", (char *) fmtcellname_op);
    (void) st_insert(op_table, "libname", (char *) libname_op);
    (void) st_insert(op_table, "cellname", (char *) cellname_op);
    (void) st_insert(op_table, "viewname", (char *) viewname_op);
    (void) st_insert(op_table, "facetname", (char *) facetname_op);
    (void) st_insert(op_table, "portname", (char *) portname_op);
    (void) st_insert(op_table, "instpropval", (char *) instpropval_op);
    (void) st_insert(op_table, "propval", (char *) propval_op);
    (void) st_insert(op_table, "if", (char *) if_op);
    (void) st_insert(op_table, "or", (char *) or_op);
  /* End of MCC added operators */
    stack_top = 0;
}

#define INIT_TOK_SIZE	128

static char *get_token(spot)
char **spot;			/* Current spot in string */
/*
 * Gets the next space delimited token.  Also recognizes
 * string constants.  Moves `spot' to after the returned
 * token.
 */
{
    static char *tok_buf = (char *) 0;
    static unsigned int tok_buf_len = 0;
    char *idx = *spot;
    char *result = (char *) 0;
    int len;

    if (!tok_buf) {
	tok_buf_len = INIT_TOK_SIZE;
	tok_buf = malloc(tok_buf_len);
    }
    if (idx && *idx) {
	while (isspace(*idx)) idx++;
	if (*idx == '"') {
	    /* String constant */
	    result = idx;
	    idx++;
	    while (*idx && *idx != '"') idx++;
	    if (*idx == '"') idx++;
	} else {
	    /* Token until space */
	    result = idx;
	    while (*idx && !isspace(*idx)) idx++;
	}
	len = idx - result;
	if (len >= tok_buf_len) {
	    tok_buf_len = len*2;
	    tok_buf = realloc(tok_buf, tok_buf_len);
	}
	result = strncpy(tok_buf, result, len);
	result[len] = '\0';
	*spot = idx;
    }
    return result;
}


static char *build_result()
/*
 * Turns all remaining stack items into strings,  concatenates them
 * and returns the result.
 */
{
    int idx;
    int totlen = 0;
    char *result;

    for (idx = 0;  idx < stack_top;  idx++) {
	convert(STR, &stack[idx]);
	totlen += strlen(stack[idx].string.value);
    }
    result = ALLOC(char, totlen+1);
    result[0] = '\0';
    for (idx = 0;  idx < stack_top;  idx++) {
	(void) strcat(result, stack[idx].string.value);
	FREE_ITEM(stack[idx]);
    }
    stack_top = 0;
    return result;
}

char *pl_eval(fct, inst, lbl, fct_spec, expr)
octObject *fct, *inst, *lbl;
char *fct_spec;
char *expr;
/*
 * Evaluates `expr' as a post-fix expression.  Integer, real, and
 * string constants are pushed onto a stack.  Non-constants are
 * considered an operator and the associated operator is called.
 * The operator table is fixed.  The tokens must all be space
 * separated.  Types will be upgraded as necessary.  Bad arguments
 * to an operator will be eaten and an error string will be placed
 * on the stack.  `expr' will be touched but restored to
 * its initial state afterward.  After `expr' has been evaluated,
 * all items on the stack will be turned into strings, concatenated,
 * and returned as a single string.
 */
{
    static char *result = (char *) 0;
    char *idx = expr;
    char *token, *c;
    stack_item item;
    int last;
    void (*func)();

    if (result) FREE(result);
    if (stack_top < 0) init();
    while ( (token = get_token(&idx)) ) {
	if (token[0] == '"') {
	    item.type = STR;
	} else {
	    item.type = INT;
	    c = token;
	    /* Attempt to read through as if it were a number */
	    if ((*c == '+') || (*c == '-')) c++;
	    if (isdigit(*c)) {
		while (isdigit(*c)) c++;
		if (*c) {
		    item.type = REAL;
		    if (*c == '.') {
			c++;
			while (isdigit(*c)) c++;
		    }
		    if (*c) {
			if ((*c == 'e') || (*c == 'E')) c++;
			if ((*c == '+') || (*c == '-')) c++;
			if (isdigit(*c)) {
			    while (isdigit(*c)) c++;
			    if (*c) item.type = OP;
			} else {
			    item.type = OP;
			}
		    }
		}
	    } else {
		item.type = OP;
	    }
	}
	if (item.type == OP) {
	    /* Operator */
	    if (st_lookup(op_table, token, (char **) &func)) {
		(*func)(fct, inst, lbl, fct_spec);
	    } else {
		/* Error condition */
		item.type = STR;
		item.string.value = ALLOC(char, strlen(token)+10);
		(void) sprintf(item.string.value, "?%s?", token);
		PUSH(&item);
	    }
	} else {
	    switch (item.type) {
	    case INT:
		item.integer.value = atoi(token);
		break;
	    case REAL:
		item.real.value = atof(token);
		break;
	    case STR:
		item.string.value = ALLOC(char, strlen(token));
		last = strlen(token);
		(void) strncpy(item.string.value, token+1, last-1);
		if (item.string.value[last-2] == '"') {
		    item.string.value[last-2] = '\0';
		} else {
		    item.string.value[last-1] = '\0';
		}
		break;
	      case OP:		/* OP handled above. */
		break;
	    }
	    PUSH(&item);
	}
    }
    return result = build_result();
}

