/* $Id$ */

#ifndef ANSI_H
#define ANSI_H
/*
 * ANSI Compiler Support
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * ANSI compatible compilers are supposed to define the preprocessor
 * directive __STDC__.  Based on this directive, this file defines
 * certain ANSI specific macros.
 *
 * ARGS:
 *   Used in function prototypes.  Example:
 *   extern int foo
 *     ARGS((char *blah, double threshold));
 */

/* Function prototypes */
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args)	args
#else
#define ARGS(args)	()
#endif

#if defined(__cplusplus)
#define NULLARGS	(void)
#else
#define NULLARGS	()
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

#if defined(__cplusplus) || defined(__STDC__) 
#define HAS_STDARG
#endif

#endif
#ifndef UTIL_H
#define UTIL_H

#if defined(_IBMR2)
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE           /* Argh!  IBM strikes again */
#endif
#ifndef _ALL_SOURCE
#define _ALL_SOURCE             /* Argh!  IBM strikes again */
#endif
#ifndef _ANSI_C_SOURCE
#define _ANSI_C_SOURCE          /* Argh!  IBM strikes again */
#endif
#endif

#if defined(_IBMR2) && !defined(__STDC__)
#define _BSD
#endif

#if !defined(__STDC__) && !defined(__cplusplus)
/* these are too entrenched to get away with changing the name */
#define strsav		util_strsav
#define getopt		util_getopt
#define getopt_reset	util_getopt_reset
#define optarg		util_optarg
#define optind		util_optind
#endif

#define NIL(type)		((type *) 0)

#define ALLOC(type, num)	\
    ((type *) malloc(sizeof(type) * (num)))
#define REALLOC(type, obj, num)	\
    (obj) ? ((type *) realloc((char *) obj, sizeof(type) * (num))) : \
	    ((type *) malloc(sizeof(type) * (num)))
#define FREE(obj)		\
    ((obj) ? (free((char *) (obj)), (obj) = 0) : 0)

/* Ultrix (and SABER) have 'fixed' certain functions which used to be int */
#if defined(ultrix) || defined(SABER) || defined(aiws) || defined(hpux) || defined(__STDC__) || defined(apollo)
#define VOID_HACK void
#else
#define VOID_HACK int
#endif


/* No machines seem to have much of a problem with these */
#include <stdio.h>
#include <ctype.h>


/* Some machines fail to define some functions in stdio.h */
#if !defined(__STDC__) && !defined(sprite) && !defined(_IBMR2)
extern FILE *popen(), *tmpfile();
extern int pclose();
#ifndef clearerr		/* is a macro on many machines, but not all */
extern VOID_HACK clearerr();
#endif
#ifndef rewind
extern VOID_HACK rewind();
#endif
#endif

#ifndef PORT_H
#include <sys/types.h>
#include <signal.h>
#if defined(ultrix)
#if defined(_SIZE_T_)
#define ultrix4
#else
#if defined(SIGLOST)
#define ultrix3
#else
#define ultrix2
#endif
#endif
#endif
#endif

/* most machines don't give us a header file for these */
/*
#if defined(__STDC__) || defined(sprite) || defined(_IBMR2)
#include <stdlib.h>
#else
#if defined(hpux)
extern int abort();
extern VOID_HACK free(), exit(), perror();
extern char *getenv(), *malloc(), *realloc(), *calloc();
#else
extern VOID_HACK abort(), free(), exit(), perror();
extern char *getenv();
#ifdef ultrix4
extern void *malloc(), *realloc(), *calloc();
#else
extern char *malloc(), *realloc(), *calloc();
#endif
#endif
#if defined(aiws) || defined(hpux)
extern int sprintf();
#else
#ifndef _IBMR2
extern char *sprintf();
#endif
#endif
extern int system();
extern double atof();
#endif
*/

#ifndef PORT_H
#if defined(ultrix3) || defined(sunos4) || defined(_IBMR2)
#define SIGNAL_FN       void
#else
/* sequent, ultrix2, 4.3BSD (vax, hp), sunos3 */
#define SIGNAL_FN       int
#endif
#endif

/* some call it strings.h, some call it string.h; others, also have memory.h */
#if defined(__STDC__) || defined(sprite)
#include <string.h>
#else
#if defined(ultrix4) || defined(hpux)
#include <strings.h>
#else
#if defined(_IBMR2)
#include<string.h>
#include<strings.h>
#else
/* ANSI C string.h -- 1/11/88 Draft Standard */
/* ugly, awful hack */
#ifndef PORT_H
/*
extern char *strcpy(), *strncpy(), *strcat(), *strncat(), *strerror();
extern char *strpbrk(), *strtok(), *strchr(), *strrchr(), *strstr();
extern int strcoll(), strxfrm(), strncmp(), strlen(), strspn(), strcspn();
extern char *memmove(), *memccpy(), *memchr(), *memcpy(), *memset();
extern int memcmp(), strcmp();
*/
#endif
#endif
#endif
#endif

/* a few extras */
#if defined(hpux)
extern VOID_HACK srand();
extern int rand();
#define random() rand()
#define srandom(a) srand(a)
#define bzero(a,b) memset(a, 0, b)
#else
extern VOID_HACK srandom();
extern long random();
#endif

#if defined(__STDC__) || defined(sprite)
#include <assert.h>
#else
#ifndef NDEBUG
#define assert(ex) {\
    if (! (ex)) {\
	(void) fprintf(stderr,\
	    "Assertion failed: file %s, line %d\n\"%s\"\n",\
	    __FILE__, __LINE__, "ex");\
	(void) fflush(stdout);\
	abort();\
    }\
}
#else
#define assert(ex) ;
#endif
#endif


#define fail(why) {\
    (void) fprintf(stderr, "Fatal error: file %s, line %d\n%s\n",\
	__FILE__, __LINE__, why);\
    (void) fflush(stdout);\
    abort();\
}


#define MAXPATHLEN 1024

/* These arguably do NOT belong in util.h */
#ifndef ABS
#define ABS(a)			((a) < 0 ? -(a) : (a))
#endif
#ifndef MAX
#define MAX(a,b)		((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)		((a) < (b) ? (a) : (b))
#endif


EXTERN void util_print_cpu_stats ARGS((FILE *));
EXTERN long util_cpu_time ARGS((void));
EXTERN void util_getopt_reset ARGS((void));
EXTERN int util_getopt ARGS((int, char **, char *));
EXTERN char *util_path_search ARGS((char *));
EXTERN char *util_file_search ARGS((char *, char *, char *));
EXTERN int util_pipefork ARGS((char **, FILE **, FILE **));
EXTERN char *util_print_time ARGS((long));
EXTERN int util_save_image ARGS((char *, char *));
EXTERN char *util_strsav ARGS((char *));
EXTERN int util_do_nothing ARGS((void));
EXTERN char *util_tilde_expand ARGS((char *));
EXTERN FILE *util_tmpfile ARGS((void));

#define ptime()         util_cpu_time()
#define print_time(t)   util_print_time(t)

/* util_getopt() global variables (ack !) */
extern int util_optind;
extern char *util_optarg;

#include <math.h>
#ifndef HUGE_VAL
#ifndef HUGE
#define HUGE  8.9884656743115790e+307
#endif
#define HUGE_VAL HUGE
#endif

#include <varargs.h>
#endif


/*
 * List Management Package Header File
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains public type definitions for the List Managment
 * package implemented in list.c.  This is stand alone package.
 */

#ifndef LS_DEFINED
#define LS_DEFINED

/* This can be typedef'ed to void if supported */
typedef struct ls_dummy_defn {
    int dummy;			/* Not used */
} ls_dummy;

typedef ls_dummy *lsList;	/* List handle           */
typedef ls_dummy *lsGen;	/* List generator handle */
typedef ls_dummy *lsHandle;	/* Handle to an item     */
typedef int lsStatus;		/* Return codes          */
typedef char *lsGeneric;	/* Generic pointer       */

#define	LS_NIL		0	/* Nil for lsList       */

#define LS_BADSTATE	-3	/* Bad generator state   */
#define LS_BADPARAM	-2	/* Bad parameter value   */
#define LS_NOMORE	-1	/* No more items         */

#define	LS_OK		0

#define LS_BEFORE	1	/* Set spot before object */
#define LS_AFTER	2	/* Set spot after object  */
#define LS_STOP		3	/* Stop generating items  */
#define LS_DELETE	4	/* Delete generated item  */

/*
 * For all those routines that take a handle,  this macro can be
 * used when no handle is required.
 */

#define LS_NH		(lsHandle *) 0

typedef lsGeneric (*LS_PFLSG)();

#define lsForeachItem(list, gen, data)                          \
    for(gen = lsStart(list);                                    \
	    (lsNext(gen, (lsGeneric *) &data, LS_NH) == LS_OK)      \
			|| ((void) lsFinish(gen), 0); )

EXTERN lsList lsCreate ARGS((void));
  /* Create a new list */
EXTERN lsStatus lsDestroy ARGS((lsList list, void (*delFunc)()));
  /* Delete a previously created list */
EXTERN lsList lsCopy ARGS((lsList list, LS_PFLSG copyFunc));
   /* Copies the contents of a list    */

EXTERN lsStatus lsFirstItem ARGS((lsList list, lsGeneric *data, lsHandle *itemHandle));
  /* Gets the first item of a list */
EXTERN lsStatus lsLastItem ARGS((lsList list, lsGeneric *data, lsHandle *itemHandle));
  /* Gets the last item of a list */

EXTERN lsStatus lsNewBegin ARGS((lsList list, lsGeneric data, lsHandle *itemHandle));
  /* Add item to start of list */
EXTERN lsStatus lsNewEnd ARGS((lsList list, lsGeneric data, lsHandle *itemHandle));
  /* Add item to end of list */

EXTERN lsStatus lsDelBegin ARGS((lsList list, lsGeneric *data));
  /* Delete first item of a list */
EXTERN lsStatus lsDelEnd ARGS((lsList list, lsGeneric *data));
  /* Delete last item of a list */

EXTERN int lsLength ARGS((lsList list));
  /* Returns the length of the list */

EXTERN lsGen lsStart ARGS((lsList list));
  /* Begin generation of items in a list */
EXTERN lsGen lsEnd ARGS((lsList list));
  /* Begin generation at end of list */
EXTERN lsGen lsGenHandle ARGS((lsHandle itemHandle, lsGeneric *data, int option));
  /* Produces a generator given a handle */
EXTERN lsStatus lsNext ARGS((lsGen generator, lsGeneric *data, lsHandle *itemhandle));
  /* Generate next item in sequence */
EXTERN lsStatus lsPrev ARGS((lsGen generator, lsGeneric *data, lsHandle *itemHandle));
  /* Generate previous item in sequence */
EXTERN lsStatus lsInBefore ARGS((lsGen generator, lsGeneric data, 
   lsHandle *itemhandle));
  /* Insert an item before the most recently generated by lsNext */
EXTERN lsStatus lsInAfter ARGS((lsGen generator, lsGeneric data, 
   lsHandle *itemhandle));
  /* Insert an item after the most recently generated by lsNext  */
EXTERN lsStatus lsDelBefore ARGS((lsGen generator, lsGeneric *data));
  /* Delete the item before the current spot */
EXTERN lsStatus lsDelAfter ARGS((lsGen generator, lsGeneric *data));
  /* Delete the item after the current spot */
EXTERN lsStatus lsFinish ARGS((lsGen generator));
  /* End generation of items in a list */

EXTERN lsList lsQueryHandle ARGS((lsHandle itemHandle));
  /* Returns the list of a handle */
EXTERN lsGeneric lsFetchHandle ARGS((lsHandle itemHandle));
  /* Returns data associated with handle */
EXTERN lsStatus lsRemoveItem ARGS((lsHandle itemHandle, lsGeneric *userData));
  /* Removes item associated with handle from list */

EXTERN lsStatus lsSort ARGS((lsList list, int (*compare)()));

  /* Sorts a list */
EXTERN lsStatus lsUniq ARGS((lsList list, int (*compare)(), void (*delFunc)() ));
  /* Removes duplicates from a sorted list */

#endif


#ifndef ST_INCLUDED
#define ST_INCLUDED

typedef struct st_table_entry st_table_entry;
struct st_table_entry {
    char *key;
    char *record;
    st_table_entry *next;
};

typedef struct st_table st_table;
struct st_table {
    int (*compare)();
    int (*hash)();
    int num_bins;
    int num_entries;
    int max_density;
    int reorder_flag;
    double grow_factor;
    st_table_entry **bins;
};

typedef struct st_generator st_generator;
struct st_generator {
    st_table *table;
    st_table_entry *entry;
    int index;
};

#define st_is_member(table,key) st_lookup(table,key,(char **) 0)
#define st_count(table) ((table)->num_entries)

enum st_retval {ST_CONTINUE, ST_STOP, ST_DELETE};

typedef enum st_retval (*ST_PFSR)();
typedef int (*ST_PFI)();

EXTERN int st_delete ARGS((st_table *table, char **key_ptr, char **value_ptr));
EXTERN int st_insert ARGS((st_table* table, char *key, char *value));
EXTERN int st_foreach ARGS((st_table *table, ST_PFSR func, char *arg));
EXTERN int st_gen ARGS((st_generator *gen, char **key_p, char **value_p));
EXTERN int st_lookup ARGS((st_table *table, char *key, char **value_ptr));
EXTERN int st_find_or_add ARGS((st_table *table, char *key, char ***slot_ptr));
EXTERN int st_find ARGS((st_table *table, char *key, char ***slot_ptr));
EXTERN int st_add_direct ARGS((st_table *table, char *key, char *value));
EXTERN int st_strhash ARGS((char *string, int modulus));
EXTERN int st_numhash ARGS((char *x, int size));
EXTERN int st_ptrhash ARGS((char *x, int size));
EXTERN int st_numcmp ARGS((char *x, char *y));
EXTERN int st_ptrcmp ARGS((char* x, char *y));
EXTERN st_table *st_init_table ARGS((ST_PFI compare_fn, ST_PFI hash_fn)), 
	*st_init_table_with_params ARGS((ST_PFI compare, ST_PFI hash, 
	int size, int density, double grow_factor, int reorder_flag));
EXTERN st_table *st_copy ARGS((st_table *old_table));
EXTERN st_generator *st_init_gen ARGS((st_table *table));
EXTERN void st_free_table ARGS((st_table* table));
EXTERN void st_free_gen ARGS((st_generator *gen));


#define ST_DEFAULT_MAX_DENSITY 5
#define ST_DEFAULT_INIT_TABLE_SIZE 11
#define ST_DEFAULT_GROW_FACTOR 2.0
#define ST_DEFAULT_REORDER_FLAG 0

#define st_foreach_item(table, gen, key, value) \
    for(gen=st_init_gen(table); st_gen(gen,key,value) || (st_free_gen(gen),0);)


#endif /* ST_INCLUDED */


#ifndef SPARSE_H
#define SPARSE_H

/* hack to fix conflict with libX11.a */
#define sm_alloc sm_allocate
#define sm_free sm_free_space

/*
 *  sparse.h -- sparse matrix package header file
 */

typedef struct sm_element_struct sm_element;
typedef struct sm_row_struct sm_row;
typedef struct sm_col_struct sm_col;
typedef struct sm_matrix_struct sm_matrix;


/*
 *  sparse matrix element
 */
struct sm_element_struct {
    int row_num;		/* row number of this element */
    int col_num;		/* column number of this element */
    sm_element *next_row;	/* next row in this column */
    sm_element *prev_row;	/* previous row in this column */
    sm_element *next_col;	/* next column in this row */
    sm_element *prev_col;	/* previous column in this row */
    char *user_word;		/* user-defined word */
};


/*
 *  row header
 */
struct sm_row_struct {
    int row_num;		/* the row number */
    int length;			/* number of elements in this row */
    int flag;			/* user-defined word */
    sm_element *first_col;	/* first element in this row */
    sm_element *last_col;	/* last element in this row */
    sm_row *next_row;		/* next row (in sm_matrix linked list) */
    sm_row *prev_row;		/* previous row (in sm_matrix linked list) */
    char *user_word;		/* user-defined word */
};


/*
 *  column header
 */
struct sm_col_struct {
    int col_num;		/* the column number */
    int length;			/* number of elements in this column */
    int flag;			/* user-defined word */
    sm_element *first_row;	/* first element in this column */
    sm_element *last_row;	/* last element in this column */
    sm_col *next_col;		/* next column (in sm_matrix linked list) */
    sm_col *prev_col;		/* prev column (in sm_matrix linked list) */
    char *user_word;		/* user-defined word */
};


/*
 *  A sparse matrix
 */
struct sm_matrix_struct {
    sm_row **rows;		/* pointer to row headers (by row #) */
    int rows_size;		/* alloc'ed size of above array */
    sm_col **cols;		/* pointer to column headers (by col #) */
    int cols_size;		/* alloc'ed size of above array */
    sm_row *first_row;		/* first row (linked list of all rows) */
    sm_row *last_row;		/* last row (linked list of all rows) */
    int nrows;			/* number of rows */
    sm_col *first_col;		/* first column (linked list of columns) */
    sm_col *last_col;		/* last column (linked list of columns) */
    int ncols;			/* number of columns */
    char *user_word;		/* user-defined word */
};


#define sm_get_col(A, colnum)	\
    (((colnum) >= 0 && (colnum) < (A)->cols_size) ? \
	(A)->cols[colnum] : (sm_col *) 0)

#define sm_get_row(A, rownum)	\
    (((rownum) >= 0 && (rownum) < (A)->rows_size) ? \
	(A)->rows[rownum] : (sm_row *) 0)

#define sm_foreach_row(A, prow)	\
	for(prow = A->first_row; prow != 0; prow = prow->next_row)

#define sm_foreach_col(A, pcol)	\
	for(pcol = A->first_col; pcol != 0; pcol = pcol->next_col)

#define sm_foreach_row_element(prow, p)	\
	for(p = (prow == 0) ? 0 : prow->first_col; p != 0; p = p->next_col)

#define sm_foreach_col_element(pcol, p) \
	for(p = (pcol == 0) ? 0 : pcol->first_row; p != 0; p = p->next_row)

#define sm_put(x, val) \
	(x->user_word = (char *) val)

#define sm_get(type, x) \
	((type) (x->user_word))

EXTERN sm_matrix *sm_allocate ARGS((void));
EXTERN sm_matrix *sm_alloc_size ARGS((int, int));
EXTERN void sm_free_space ARGS((sm_matrix *));
EXTERN sm_matrix *sm_dup ARGS((sm_matrix *));
EXTERN void sm_resize ARGS((sm_matrix *, int, int));
EXTERN sm_element *sm_insert ARGS((sm_matrix *, int, int));
EXTERN sm_element *sm_find ARGS((sm_matrix *, int, int));
EXTERN void sm_remove ARGS((sm_matrix *, int, int));
EXTERN void sm_remove_element ARGS((sm_matrix *, sm_element *));
EXTERN void sm_delrow ARGS((sm_matrix *, int));
EXTERN void sm_delcol ARGS((sm_matrix *, int));
EXTERN void sm_copy_row ARGS((sm_matrix *, int, sm_row *));
EXTERN void sm_copy_col ARGS((sm_matrix *, int, sm_col *));
EXTERN sm_row *sm_longest_row ARGS((sm_matrix *));
EXTERN sm_col *sm_longest_col ARGS((sm_matrix *));
EXTERN int sm_num_elements ARGS((sm_matrix *));
EXTERN int sm_read ARGS((FILE *, sm_matrix **));
EXTERN int sm_read_compressed ARGS((FILE *, sm_matrix **));
EXTERN void sm_write ARGS((FILE *, sm_matrix *));
EXTERN void sm_print ARGS((FILE *, sm_matrix *));
EXTERN void sm_dump ARGS((sm_matrix *, char *, int));
EXTERN void sm_cleanup ARGS((void));

EXTERN sm_col *sm_col_alloc ARGS((void));
EXTERN void sm_col_free ARGS((sm_col *));
EXTERN sm_col *sm_col_dup ARGS((sm_col *));
EXTERN sm_element *sm_col_insert ARGS((sm_col *, int));
EXTERN void sm_col_remove ARGS((sm_col *, int));
EXTERN sm_element *sm_col_find ARGS((sm_col *, int));
EXTERN int sm_col_contains ARGS((sm_col *, sm_col *));
EXTERN int sm_col_intersects ARGS((sm_col *, sm_col *));
EXTERN int sm_col_compare ARGS((sm_col *, sm_col *));
EXTERN sm_col *sm_col_and ARGS((sm_col *, sm_col *));
EXTERN int sm_col_hash ARGS((sm_col *, int));
EXTERN void sm_col_remove_element ARGS((sm_col *, sm_element *));
EXTERN void sm_col_print ARGS((FILE *, sm_col *));

EXTERN sm_row *sm_row_alloc ARGS((void));
EXTERN void sm_row_free ARGS((sm_row *));
EXTERN sm_row *sm_row_dup ARGS((sm_row *));
EXTERN sm_element *sm_row_insert ARGS((sm_row *, int));
EXTERN void sm_row_remove ARGS((sm_row *, int));
EXTERN sm_element *sm_row_find ARGS((sm_row *, int));
EXTERN int sm_row_contains ARGS((sm_row *, sm_row *));
EXTERN int sm_row_intersects ARGS((sm_row *, sm_row *));
EXTERN int sm_row_compare ARGS((sm_row *, sm_row *));
EXTERN sm_row *sm_row_and ARGS((sm_row *, sm_row *));
EXTERN int sm_row_hash ARGS((sm_row *, int));
EXTERN void sm_row_remove_element ARGS((sm_row *, sm_element *));
EXTERN void sm_row_print ARGS((FILE *, sm_row *));

#endif


#ifndef ARRAY_H
#define ARRAY_H


typedef struct array_struct array_t;
struct array_struct {
    char *space;
    int num;		/* number of array elements */
    int n_size;		/* size of 'data' array (in objects) */
    int obj_size;	/* size of each array object */
};


EXTERN array_t *array_do_alloc ARGS((int size, int number));
EXTERN array_t *array_dup ARGS((array_t* old));
EXTERN array_t *array_join ARGS((array_t* array1, array_t* array2));
EXTERN void array_free ARGS((array_t* array));
EXTERN void array_append ARGS((array_t* array1, array_t* array2));
EXTERN void array_sort ARGS((array_t* array, int (*compare)()));
EXTERN void array_uniq ARGS((array_t* array, int (*compare)(), void (*free_func)()));
EXTERN int array_abort ARGS((int i));
EXTERN int array_resize ARGS((array_t* array, int new_size));
EXTERN char *array_do_data ARGS((array_t* array));

extern int array_i;


#define array_alloc(type, number)		\
    array_do_alloc(sizeof(type), number)

#define array_insert(type, a, i, datum)		\
    (array_i = (i),				\
      array_i < 0 ? array_abort(0) : 0,		\
      sizeof(type) != (a)->obj_size ? array_abort(1) : 0,\
      array_i >= (a)->n_size ? array_resize(a, array_i + 1) : 0,\
      array_i >= (a)->num ? (a)->num = array_i + 1 : 0,\
      *((type *) ((a)->space + array_i * (a)->obj_size)) = datum)

#define array_fetch(type, a, i)			\
    (array_i = (i),				\
      (array_i < 0 || array_i >= (a)->num) ? array_abort(0) : 0,\
      *((type *) ((a)->space + array_i * (a)->obj_size)))

#define array_fetch_p(type, a, i)                       \
    (array_i = (i),                             \
      (array_i < 0 || array_i >= (a)->num) ? array_abort(0) : 0,\
      ((type *) ((a)->space + array_i * (a)->obj_size)))

#define array_insert_last(type, array, datum)	\
    array_insert(type, array, (array)->num, datum)

#define array_fetch_last(type, array)		\
    array_fetch(type, array, ((array)->num)-1)

#define array_n(array)				\
    (array)->num

#define array_data(type, array)			\
    (type *) array_do_data(array)

#endif


