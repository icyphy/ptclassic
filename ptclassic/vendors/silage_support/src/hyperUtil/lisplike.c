/* $Id$ */

/* lisplike.c -- Read and provide parsing functions for lisp like files.
 *
 * Copyright (C) 1989 Regents of the University of California.
 * All rights reserved.
 *
 *  FUNCTIONS IN THIS FILE:
 *  read_lisp(): Expects a valid FILE pointer and returns a handle to an
 *      lnode union structure
 *
 *  free_lisp(): expects a handle to an lnode structure and frees the memory
 *      malloc'ed to it
 *
 */
#include "port.h"
#include "lisplike.h"

#define	FALSE	0
#define	TRUE	!FALSE

/* Parser Definitions */
#define	NO_TOKEN	0
#define	STRING		1
#define	SYMBOL		2
#define	OPEN_PAREN	3
#define	CLOSE_PAREN	4
#define	SINGLE_QUOTE	5
#define	NIL_TOKEN	6

char		*lagerAllocText();
char		*getenv();
static		skip_space(), skip_comment();
static lnode	*s_expr(), *s_expr_list();
int		get_token();

lnode		*NewLNode();
void		dump_s_expr();

char		tval[200];
int		line_number, char_number;

/*
main()
{
	lnode *ln_root;
	char	*s;

	lisplike_paren_tally = 0;
	line_number = 1;
	char_number = 0;

	while (!feof(stdin)) {
		ln_root = s_expr(stdin);

		dump_s_expr(ln_root);
		(void) fputc('\n', stdout);
		s = StringFromSExpr(ln_root);
		fputs(s, stdout);
		free(s);
		(void) fputc('\n', stdout);
	}
}
*/

/* car mimic: _p indicates that the list is not to be evaluated */
lnode *
car_p(list)
	lnode	*list;
{
	return (CAR_P(list));
}



/* cdr mimic: _p indicates that the list is not to be evaluated */
lnode *
cdr_p(list)
	lnode	*list;
{
	return (CDR_P(list));
}



lnode *
read_lisp( fp )
FILE *fp;
{
    lnode *ln_root;

    lisplike_paren_tally = 0;
    line_number = 1;
    char_number = 0;

    ln_root = s_expr_list( fp );

    return( ln_root );
} /* read_lisp() */



static lnode *
s_expr_list(fp)
	FILE	*fp;
{
	int	token;
	lnode	*ln;

	token = get_token(fp);

	if (token == CLOSE_PAREN) {
		return (NULL);
	}

	if (token == EOF) return (NULL);

	unget_token(token);
	ln = s_expr( fp );
	return (create_dotdot(ln, s_expr_list(fp), LN_DOTDOT));
} /* s_expr_list() */

static lnode *
s_expr( fp )
FILE *fp;
{
	switch(get_token(fp)) {
	case EOF:
		return (NULL);
	case CLOSE_PAREN:		/* Too many close parentheses! */
		return (NULL);
	case OPEN_PAREN:
		return (s_expr_list(fp));
	case SINGLE_QUOTE:
		return (create_dotdot(s_expr(fp), (lnode *) NULL, LN_QUOTE));
	case STRING:
		return (create_string(tval, LN_STRING));
	case SYMBOL:
		return (create_string(tval, LN_SYMBOL));
	case NIL_TOKEN:
		return (NULL);
	}

	return (NULL);
} /* s_expr() */



/*
 * Routines for storing s-expressions.
 */

lnode *
create_string(s, type)
	char	*s;
	int	type;
{
	lnode *ln;

	ln = NewLNode();
	ln->type = type;
	ln->value.string = lagerAllocText( s );
	ln->line_no = line_number;
	ln->char_no = char_number;

	return (ln);
} /* create_string() */

lnode *
create_integer(i, type)
	int	i;
	int	type;
{
	lnode *ln;

	ln = NewLNode();
	ln->type = type;
	ln->value.integer = i;
	ln->line_no = line_number;
	ln->char_no = char_number;

	return (ln);
} /* create_string() */

lnode *
create_dotdot(a, d, type)
	lnode	*a, *d;
	int	type;
{
	lnode	*ln;

	ln = NewLNode();
	ln->type = type;
	ln->value.cons.a = a;
	ln->value.cons.d = d;
	ln->line_no = line_number;
	ln->char_no = char_number;

	return (ln);
} /* create_dotdot() */

lnode *
ReverseCons(list)
	register lnode	*list;
{
	register lnode	*new;
	register lnode	*result;

	result = (lnode *) NULL;
	while (list) {
		new = list;
		list = list->value.cons.d;
		new->value.cons.d = result;
		result = new;
	}

	return (result);
}

static int	last_token = NO_TOKEN;

int
get_token( fp )
FILE *fp;
{
	int	c;

	if (last_token != NO_TOKEN) {
		c = last_token;
		last_token = NO_TOKEN;
		return (c);
	}

	skip_space( fp );

	c = getc(fp);
	++char_number;

	switch(c) {
	case '(':
		++lisplike_paren_tally;
		return (OPEN_PAREN);
	case ')':
		--lisplike_paren_tally;
		return (CLOSE_PAREN);
	case '"':
		scan_string(fp);
		return (STRING);
	case '\'':
		/* Ignore a single quote. */
		return (SINGLE_QUOTE);
	case EOF:
		return (EOF);
	default:
		--char_number;
		(void) ungetc(c, fp);
		scan_symbol(fp);
		if (!strcmp(tval, "nil"))
			return (NIL_TOKEN);
		else
			return (SYMBOL);
	}
} /* get_token() */

unget_token(token)
	int	token;
{
	if (last_token != NO_TOKEN) {
		(void) printmsg(NULL,
			"unget_token: Only one token can be released.\n");
		return;
	}

	last_token = token;
}

static
scan_symbol(fp)
	register FILE	*fp;
{
	register int	c;
	register char	*sym;
	register int	found_eos;

	sym = tval;
	found_eos = FALSE;
	while ((c = getc(fp)) != EOF) {
		++char_number;
		switch (c) {
		case ' ':
		case '\r':
		case '\t':
		case '\n':
		case '(':
		case ')':
		case EOF:
			found_eos = TRUE;
			break;
		case '\\':
			*sym++ = c;
			c = getc(fp);
			++char_number;
			switch (c) {
			      case '\n':
				++line_number;
				char_number = 0;
				break;
			      case EOF:
				found_eos = TRUE;
				--sym;	/* Don't '\' escape an EOS. */
				break;
			}
			break;
		}

		if (found_eos) break;
		*sym++ = c;
	}
	*sym = '\000';
	--char_number;
	(void) ungetc(c, fp);
}

static
scan_string(fp)
	register FILE	*fp;
{
	register int	c;
	register char	*sym;
	register int	string_len;
	register int	found_eos;

	sym = tval;
	string_len = 0;

	found_eos = FALSE;
	while ((c = getc(fp)) != EOF) {
		++char_number;
		switch (c) {
		case '"':
			found_eos = TRUE;
			break;
		case '\\':
			*sym++ = c;
			if (++string_len > 198) {
				(void) printmsg(NULL,
					"String on line %d was too long.\n",
					line_number);
			}
			c = getc(fp);
			++char_number;
			switch (c) {
			      case '\n':
				++line_number;
				char_number = 0;
				break;
			      case EOF:
				found_eos = TRUE;
				--sym;	/* Don't '\' escape an EOF. */
				--string_len;
				break;
			}
			break;
		case '\n':
			++line_number;
			char_number = 0;
			break;
		}

		if (found_eos) break;

		*sym++ = c;
		if (++string_len > 198) {
			(void) printmsg(NULL,
					"String on line %d was too long.\n",
					line_number);
		}
	}

	*sym = '\000';
}

static
skip_space(fp)
	FILE	*fp;
{
	int c;
	/* int getc(); */

	while ( (c = getc( fp )) != EOF) {
		++char_number;
		if (c == '\n') {
			++line_number;
			char_number = 0;
		}
		if (isgraph(c)) {
			if (c != ';') break;
			else skip_comment( fp );
		}
	}

	(void) ungetc( c, fp );
	--char_number;
} /* skip_space() */



static
skip_comment( fp )
FILE *fp;
{
	int	c;

	while ((c = getc( fp )) != EOF)
		if (c == '\n') break;
	++line_number;
	char_number = 0;
} /* skip_comment() */

/*
 * Routines for printing out an s-expression to stdout.
 */

void
dump_s_expr_list(ln)
	lnode		*ln;
{
	if (!IS_DOTDOT(ln)) {
		(void) fprintf(stdout, ". ");
		dump_s_expr(ln);
		return;
	}
	dump_s_expr(ln->value.cons.a);
	if (ln->value.cons.d) {
		(void) fputc(' ', stdout);
		dump_s_expr_list(ln->value.cons.d);
	}
}

void
dump_s_expr(ln)
	lnode		*ln;
{
	if (!ln) {
		fputs("nil", stdout);
		return;
	}

	switch (ln->type) {
	case LN_DOTDOT:
		(void) fputc('(', stdout);
		dump_s_expr_list(ln);
		(void) fputc(')', stdout);
		break;
	case LN_QUOTE:
		(void) fputc('\'', stdout);
		dump_s_expr(ln->value.cons.a);
		break;
	case LN_SYMBOL:
		fputs(ln->value.string, stdout);
		break;
	case LN_STRING:
		(void) fputc('"', stdout);
		fputs(ln->value.string, stdout);
		(void) fputc('"', stdout);
		break;
	case LN_INT:
	case LN_TOKEN:
		(void) fprintf(stdout, "%d", ln->value.integer);
	}

} /* dump_s_expr() */

char *
StringFromSExpr(ln)
	lnode	*ln;
{
	char	work[200];
	char	*result;
	int	len;

	len = s_expr_to_cstring(ln, work, 199);

	result = malloc((unsigned)len + 1);
	if (len < 199) {
		work[len] = '\000'; /* Terminate the counted string. */
		return (strcpy(result, work));
	} else {
		(void) s_expr_to_cstring(ln, result, len);
		result[len] = '\000';	/* Terminate the counted string. */
	}

	return (result);
}

#define	STUFF_CHAR(ch) \
	if (++new_len < len) *result++ = (ch)

int
s_expr_list_to_cstring(ln, result, len)
	lnode		*ln;
	char		*result;
	int		len;
{
	int		new_len, ret_len;

	new_len = ret_len = s_expr_to_cstring(ln->value.cons.a, result, len);
	result += ret_len;

	if (ln->value.cons.d) {
		STUFF_CHAR(' ');
		ret_len = s_expr_list_to_cstring(ln->value.cons.d, result,
				len - new_len);
		new_len += ret_len;
	}

	return (new_len);
}

int
s_expr_to_cstring(ln, result, len)
	lnode		*ln;
	char		*result;
	int		len;
{
	int		ret_len, new_len;

	new_len = 0;

	if (!ln) {
		STUFF_CHAR('n');
		STUFF_CHAR('i');
		STUFF_CHAR('l');
		return (new_len);
	}

	switch (ln->type) {
	case LN_DOTDOT:
		STUFF_CHAR('(');
		ret_len = s_expr_list_to_cstring(ln, result, len - new_len);
		new_len += ret_len; result += ret_len;
		STUFF_CHAR(')');
		break;
	case LN_QUOTE:
		STUFF_CHAR('\'');
		ret_len = s_expr_to_cstring(ln->value.cons.a, result,
				len - new_len);
		new_len += ret_len; result += ret_len;
		break;
	case LN_SYMBOL:
		ret_len = strncpy_strlen(result, ln->value.string, len);
		new_len += ret_len; result += ret_len;
		break;
	case LN_STRING:
		STUFF_CHAR('"');
		ret_len = strncpy_strlen(result, ln->value.string, len - new_len);
		new_len += ret_len; result += ret_len;
		STUFF_CHAR('"');
		break;
	}

	return (new_len);
} /* s_expr_to_cstring() */

free_s_expr(ln)
lnode *ln;
{
	if (!ln) return;

	switch (ln->type) {
	case LN_DOTDOT:
	case LN_QUOTE:
		free_s_expr(ln->value.cons.a);
		free_s_expr(ln->value.cons.d);
		break;
	case LN_STRING:
	case LN_SYMBOL:
		free(ln->value.string);
		break;
	}

	FreeLNode(ln);
} /* free_s_expr() */

/*
 * Routines for allocating and freeing lnodes and strings.
 */

#define	LNBATCH	64

static lnode	*free_lnodes = NULL;

lnode *
NewLNode()
{
	lnode	*ln;
	int	i;

	if (!free_lnodes) {
		/* Create new lnodes. */
		ln = free_lnodes =
			(lnode *) malloc((unsigned) sizeof(*ln) * LNBATCH);
		for (i = 1; i < LNBATCH; ++i, ++ln) {
			ln->value.cons.d = &ln[1];
		}
		ln->value.cons.d = NULL;
	}

	ln = free_lnodes;
	free_lnodes = ln->value.cons.d;
	ln->value.cons.d = NULL;

	return (ln);
}

FreeLNode(ln)
	lnode	*ln;
{
	if (!ln) return;
	ln->value.cons.d = free_lnodes;
	free_lnodes = ln;
}

int
strncpy_strlen(dst, src, max_char)
	register char	*dst;
	register char	*src;
	register int	max_char;
{
	register char	c;
	register int	dst_len;

	dst_len = 0;	
	while (c = *src++) {
		if (++dst_len < max_char) *dst++ = c;
	}

	*dst = '\000';

	return (dst_len);
}
