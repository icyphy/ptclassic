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
/*
 * Print functions for various structures in VEM
 * used primarily for debugging.
 */

#ifdef PRT
#include "general.h"

typedef struct struct_table {
    char *struct_name;
    int (*struct_print)();
} StructTable;

void pp_string();

extern int prt_oct();
extern int prt_tl();
extern int prt_octid();
extern int prt_octxid();
extern int prt_down();
extern int prt_up();

StructTable all_structs[] = {
    { "octObject",	prt_oct			},
    { "octId",		prt_octid		},
    { "xid",		prt_octxid		},
    { "contents",	prt_down		},
    { "containers",	prt_up			},
    { "tapLayerList",	prt_tl			},
};

#define NUM_STRUCTS	sizeof(all_structs)/sizeof(StructTable)

#define MAX_BUF	16384
static char out_buf[MAX_BUF];

int prt(name, value)
char *name;
char *value;
/* 
 * Prints out structure named `name'.  `value' is assumed
 * to be a pointer to the structure.
 */
{
    int i;
    char *ptr = &out_buf[0];

    for (i = 0;  i < NUM_STRUCTS;  i++) {
	if (strcmp(name, all_structs[i].struct_name) == 0) {
	    (void) ((*all_structs[i].struct_print)(ptr, value));
	    pp_string(out_buf);
	    break;
	}
    }
    if (i >= NUM_STRUCTS) {
	char all_structs_buf[1024];

	printf("No print function for `%s'.  Print functions defined:\n",
	       name);
	strcpy(all_structs_buf, "[ ");
	strcat(all_structs_buf, all_structs[0].struct_name);
	for (i = 1;  i < NUM_STRUCTS;  i++) {
	    strcat(all_structs_buf, " ");
	    strcat(all_structs_buf, all_structs[i].struct_name);
	}
	strcat(all_structs_buf, "]");
	pp_string(all_structs_buf);
    }
    return 0;
}

/*
 * Some standard conforming string writers
 */

int prt_str(ptr, val)
char *ptr, *val;
/* Writes a string into `ptr' and returns the number of characters
 * written.
 */
{
    strcpy(ptr, val);
    return strlen(val);
}

int prt_int(ptr, val)
char *ptr;
int val;
{
    sprintf(ptr, "%d", val);
    return strlen(ptr);
}

int prt_dbl(ptr, val)
char *ptr;
double val;
{
    sprintf(ptr, "%g", val);
    return strlen(ptr);
}

/*
 * Pretty printer
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 */

static int cols = 80;

#define MAX_TOKEN	256
#define DELIM_BREAK

typedef enum t_types {
    IGNORE = 0, IDENTIFIER, SPACE, OPEN_DELIM, CLOSE_DELIM
} TClass;

typedef enum d_class {
    NOCLASS = 0, PAREN = 0x10, BRACKET = 0x20, BRACE = 0x40, QUOTE = 0x80
} DClass;

typedef struct token_def {
    TClass token_type;
    DClass token_class;
    int token_size;
    char token_str[MAX_TOKEN];
} Token;
    
#define MAX_CHAR	128
#define TYPE_MASK	0x0f
#define DELIM_MASK	0xf0

static int delim_init = 0;
static int delim_array[MAX_CHAR];

#define IV(enum_val)	((int) enum_val)

static void load_array()
/*
 * Loads delimiter array with data.  The low four bits are the base
 * delimiter type (see the token types above).  The high bits for
 * OPEN_DELIM and CLOSE_DELIM give the delimiter class.  Assumes
 * ASCII character set.
 */
{
    int i;

    /* Initialize */
    for (i = 0;  i < MAX_CHAR;  i++) delim_array[i] = IV(IDENTIFIER);

    /* Turn off various unprintables */
    for (i = 0x0;  i < 0x20;  i++) delim_array[i] = IV(IGNORE);
    delim_array[0x7f] = IV(IGNORE);

    /* Handle space */
    delim_array[' '] = IV(SPACE);
    delim_array['\n'] = IV(SPACE);
    delim_array['\t'] = IV(SPACE);

    /* Change basic delimiter types */
    delim_array['('] = IV(OPEN_DELIM) | IV(PAREN);
    delim_array[')'] = IV(CLOSE_DELIM) | IV(PAREN);
    delim_array['['] = IV(OPEN_DELIM) | IV(BRACKET);
    delim_array[']'] = IV(CLOSE_DELIM) | IV(BRACKET);
    delim_array['{'] = IV(OPEN_DELIM) | IV(BRACE);
    delim_array['}'] = IV(CLOSE_DELIM) | IV(BRACE);
    delim_array['`'] = IV(OPEN_DELIM) | IV(QUOTE);
    delim_array['\''] = IV(CLOSE_DELIM) | IV(QUOTE);

    delim_init = 1;
}


static int get_token(ptr, token)
char **ptr;			/* String to examine */
Token *token;			/* Token (returned)  */
/*
 * Moves `ptr' forward looking for the next token.  If one is
 * found,  it is returned in `token'.  If no more tokens remain,
 * the routine returns zero.
 */
{
    int val;

    /* First,  see if this is the end */
    if (**ptr == '\0') return 0;

    if (!delim_init) load_array();
    val = delim_array[**ptr];
    token->token_size = 0;
    switch (val & TYPE_MASK) {
    case IGNORE:
	*ptr += 1;
	return get_token(ptr, token);
    case IDENTIFIER:
	token->token_str[(token->token_size)++] = **ptr;
	while ((delim_array[*(++(*ptr))] & TYPE_MASK) == IV(IDENTIFIER)) {
	    token->token_str[(token->token_size)++] = **ptr;
	}
	token->token_str[token->token_size] = '\0';
	token->token_class = NOCLASS;
	token->token_type = IDENTIFIER;
	return 1;
    case SPACE:
	while ((delim_array[*(++(*ptr))] & TYPE_MASK) == IV(SPACE)) {
	    /* Empty body */
	}
	token->token_type = SPACE;
	token->token_class = NOCLASS;
	token->token_str[0] = ' ';
	token->token_str[1] = '\0';
	token->token_size = 1;
	return 1;
    case OPEN_DELIM:
    case CLOSE_DELIM:
	token->token_type = (TClass) (val & TYPE_MASK);
	token->token_size = 1;
	token->token_class = (DClass) (val & DELIM_MASK);
	token->token_str[0] = **ptr;
	token->token_str[1] = '\0';
	*ptr += 1;
	return 1;
    }
    return 0;
}

typedef struct col_item {
    int col_pos;
    DClass delim_class;
} ColItem;

#define INIT_DEPTH	10

static ColItem *col_stack = (ColItem *) 0;
static int col_top_item = 0;
static int col_alloc = 0;

extern char *malloc();
extern char *realloc();

static void init_col()
/* Initializes column stack */
{
    if (!col_stack) {
	col_stack = (ColItem *) malloc(sizeof(ColItem) * INIT_DEPTH);
	col_alloc = INIT_DEPTH;
    }
    col_top_item = 0;
}

static ColItem *top_col()
/* Returns item at top of the stack (0 if none) */
{
    if (col_top_item > 0) {
	return &(col_stack[col_top_item-1]);
    } else {
	return (ColItem *) 0;
    }
}

static void push_col(pos, class)
int pos;			/* What column to freeze */
DClass class;			/* Delimiter class       */
/* Pushes a new item onto the stack */
{
    if (col_top_item >= col_alloc) {
	/* Make the stack bigger */
	col_alloc *= 2;
	col_stack = (ColItem *)
	  realloc((char *) col_stack,
		  (unsigned) (col_alloc * sizeof(ColItem)));
    }
    col_stack[col_top_item].col_pos = pos;
    col_stack[col_top_item].delim_class = class;
    col_top_item += 1;
}

static void pop_col(class)
DClass class;			/* What class to pop to */
/* Pops down to the highest frame having `class' as its class */
{
    while (col_top_item > 0) {
	col_top_item--;
	if (col_stack[col_top_item].delim_class == class) break;
    }
}

static void line_break(col, frame)
int *col;			/* Current column (modified) */
ColItem *frame;			/* Current top stack frame   */
/*
 * Outputs carriage return and enough spaces to come up to
 * the column in `frame'.  `col' is set to that column.
 */
{
    int i;

    if (*col > 0) {
	putchar('\n');
	if (frame) {
	    for (i = 0;  i < frame->col_pos;  i++) {
		putchar(' ');
	    }
	    *col = frame->col_pos;
	} else {
	    *col = 0;
	}
    }
}

void pp_set_cols(new_cols)
int new_cols;
/* Sets new number of columns */
{
    cols = new_cols;
}


/*
 * Initial parsing measurements
 */

#define INIT_M_STACK	10
static int m_stk_top = -1;
static int m_stk_alloc = 0;
static int *m_stack = (int *) 0;

static void init_stk()
/* Initializes the measure stack */
{
    if (!m_stack) {
	m_stk_alloc = INIT_M_STACK;
	m_stack = (int *) malloc((unsigned) (m_stk_alloc * sizeof(int)));
    }
    m_stk_top = -1;  /* m_stk_top always points to last allocated */
}

static void push_stk(val)
int *val;
{
    m_stk_top++;
    if (m_stk_top >= m_stk_alloc) {
	/* Make bigger */
	m_stk_alloc *= 2;
	m_stack = (int *) realloc((char *) m_stack,
				  (unsigned) (m_stk_alloc * sizeof(int)));
    }
    m_stack[m_stk_top] = *val;
    (*val)++;
}

static void pop_stk()
{
    m_stk_top--;
}

#define INIT_M_LEN	10
static int m_len_top = 0;
static int m_len_alloc = 0;
static int *m_len = (int *) 0;


static void m_init()
/* Initializes the length data structure */
{
    init_stk();
    if (!m_len) {
	m_len_alloc = INIT_M_LEN;
	m_len = (int *) malloc((unsigned) (m_len_alloc * sizeof(int)));
    }
    m_len_top = 0;
}

static void m_push()
/* Pushes a reference to m_len_top onto the stack */
{
    if (m_len_top >= m_len_alloc) {
	m_len_alloc *= 2;
	m_len = (int *) realloc((char *) m_len, (unsigned)
				(m_len_alloc * sizeof(int)));
    }
    m_len[m_len_top] = 0;
    push_stk(&m_len_top);
}

static void m_add(val)
int val;
/* Adds length count to top of stack */
{
    m_len[m_stack[m_stk_top]] += val;
}

static void m_pop()
/* Pops off value from stack adds it to next one down */
{
    pop_stk();
    if (m_stk_top >= 0) {
	m_len[m_stack[m_stk_top]] += m_len[m_stack[m_stk_top+1]];
    }
}

static int *measure(str)
char *str;
/*
 * Parses string and returns an array of lengths corresponding
 * to each opening delimiter.  Array is locally allocated
 * and should not be freed.
 */
{
    Token token;

    m_init();
    while (get_token(&str, &token)) {
	switch (token.token_type) {
	case IDENTIFIER:
	case SPACE:
	    /* Add length to item at top of stack */
	    m_add(token.token_size);
	    break;
	case OPEN_DELIM:
	    m_push();
	    m_add(token.token_size);
	    break;
	case CLOSE_DELIM:
	    m_add(token.token_size);
	    m_pop();
	    break;
	}
    }
    return m_len;
}


void pp_string(str)
char *str;
/* 
 * Pretty prints string to standard output.  Word wraps based on spaces,
 * automatically indents parentheses, braces, brackets, and quotes.
 */
{
    char *p = str;
    Token token;
    int cur_col = 0;
    int open_count = 0;
    int *lengths;

    lengths = measure(str);
    init_col();
    while (get_token(&p, &token)) {
	switch (token.token_type) {
	case IDENTIFIER:
	case SPACE:
	    if ((cur_col + token.token_size) >= cols) {
		/* Line break */
		line_break(&cur_col, top_col());
		if (token.token_type == IDENTIFIER) {
		    (void) fputs(token.token_str, stdout);
		    cur_col += token.token_size;
		}
	    } else {
		/* Output token */
		(void) fputs(token.token_str, stdout);
		cur_col += token.token_size;
	    }
	    break;
	case OPEN_DELIM:
	    if ((cur_col + lengths[open_count]) >= cols) {
		line_break(&cur_col, top_col());
	    }
	    open_count += 1;
	    /* Fall through */
	case CLOSE_DELIM:
	    if ((cur_col + token.token_size) >= cols) {
		/* Line break */
		line_break(&cur_col, top_col());
	    }
	    /* Output token */
	    (void) fputs(token.token_str, stdout);
	    cur_col += token.token_size;
	    if (token.token_type == OPEN_DELIM) {
		push_col(cur_col, token.token_class);
	    } else {
		pop_col(token.token_class);
	    }
	    break;
	}
    }
    (void) fputs("\n", stdout);
}

#define PRT_AVAIL	1
#else
#define PRT_AVAIL	0
#endif  /* PRT */

/*
 * Some compilers have problems with empty .o files.  This
 * solves the problem.
 */

int vemPrtAvailable = PRT_AVAIL;

