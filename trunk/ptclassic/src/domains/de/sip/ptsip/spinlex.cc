/***** spin: spinlex.c *****/

/* Copyright (c) 1991-1997 by Lucent Technologies - Bell Laboratories     */
/* All Rights Reserved.  This software is for educational purposes only.  */
/* Permission is given to distribute this code provided that this intro-  */
/* ductory message is not removed and no monies are exchanged.            */
/* No guarantee is expressed or implied by the distribution of this code. */
/* Software written by Gerard J. Holzmann as part of the book:            */
/* `Design and Validation of Computer Protocols,' ISBN 0-13-539925-4,     */
/* Prentice Hall, Englewood Cliffs, NJ, 07632.                            */
/* Send bug-reports and/or questions to: gerard@research.bell-labs.com    */

#include <stdlib.h>
#include "spin.h"
#ifdef PC
#include "y_tab.h"
#else
#include "y.tab.h"
#endif

extern Symbol	*Fname;
extern YYSTYPE	yylval;
extern short	has_last;

int	lineno=1;
char	yytext[512];
FILE	*yyin, *yyout;

unsigned char	in_comment=0;
static int	check_name(char *);

#define Token(y)	{ yylval = nn(ZN,0,ZN,ZN); \
			if (!in_comment) return y; else goto again; }

#define ValToken(x, y)	{ yylval = nn(ZN,0,ZN,ZN); yylval->val = x; \
			if (!in_comment) return y; else goto again; }

#define SymToken(x, y)	{ yylval = nn(ZN,0,ZN,ZN); yylval->sym = x; \
			if (!in_comment) return y; else goto again; }

#define Getchar()	getc(yyin)
#define Ungetch(c)	ungetc(c, yyin)

static int
notquote(int c)
{	return (c != '\"' && c != '\n');
}

int
isalnum_(int c)
{	return (isalnum(c) || c == '_');
}

static int
isalpha_(int c)
{	return isalpha(c);	/* could be macro */
}
       
static int
isdigit_(int c)
{	return isdigit(c);	/* could be macro */
}

static void
getword(int first, int (*tst)(int))
{	int i=0; char c;

	yytext[i++]= (char) first;
	while (tst(c = Getchar()))
		yytext[i++] = c;
	yytext[i] = '\0';
	Ungetch(c);
}

static int
follow(int tok, int ifyes, int ifno)
{	int c;

	if ((c = Getchar()) == tok)
		return ifyes;
	Ungetch(c);

	return ifno;
}

static int
lex(void)
{	int c;

again:
	c = Getchar();
	yytext[0] = (char) c;
	yytext[1] = '\0';
	switch (c) {
	case '\n':		/* newline */
		lineno++;
		goto again;

	case  ' ': case '\t': case '\f':	/* white space */
		goto again;

	case '#':		/* preprocessor directive */
		getword(c, isalpha_);
		if (Getchar() != ' ')
		  fatal("malformed preprocessor directive - # .", 0);
		if (!isdigit_(c = Getchar()))
		  fatal("malformed preprocessor directive - # .lineno", 0);
		getword(c, isdigit_);
		lineno = atoi(yytext);		/* removed -1 */
		c = Getchar();
		if (c == '\n') goto again;	/* no filename */

		if (c != ' ')
		  fatal("malformed preprocessor directive - .fname", 0);
		if ((c = Getchar()) != '\"')
		  fatal("malformed preprocessor directive - .fname", 0);
		getword(c, notquote);
		if (Getchar() != '\"')
		  fatal("malformed preprocessor directive - fname.", 0);
		strcat(yytext, "\"");
		Fname = lookup(yytext);
		while (Getchar() != '\n')
			;
		goto again;

	case '\"':
		getword(c, notquote);
		if (Getchar() != '\"')
			fatal("string not terminated", yytext);
		strcat(yytext, "\"");
		SymToken(lookup(yytext), SPIN_STRING)

	default:
		break;
	}

	if (isdigit_(c))
	{	getword(c, isdigit_);
		ValToken(atoi(yytext), CONST)
	}

	if (isalpha_(c) || c == '_')
	{	getword(c, isalnum_);
		if (!in_comment)
			return check_name(yytext);
		else goto again;
	}

	switch (c) {
	case '/': c = follow('*', 0, '/');
		  if (!c) { in_comment = 1; goto again; }
		  break;
	case '*': c = follow('/', 0, '*');
		  if (!c) { in_comment = 0; goto again; }
		  break;
	case ':': c = follow(':', SEP, ':'); break;
	case '-': c = follow('>', SEMI, follow('-', DECR, '-')); break;
	case '+': c = follow('+', INCR, '+'); break;
	case '<': c = follow('<', LSHIFT, follow('=', LE, LT)); break;
	case '>': c = follow('>', RSHIFT, follow('=', GE, GT)); break;
	case '=': c = follow('=', EQ, ASGN); break;
	case '!': c = follow('=', NE, follow('!', O_SND, SND)); break;
	case '?': c = follow('?', R_RCV, RCV); break;
	case '&': c = follow('&', AND, '&'); break;
	case '|': c = follow('|', OR, '|'); break;
	case ';': c = SEMI; break;
	default : break;
	}
	Token(c)
}

static struct {
	char *s;	int tok;	int val;	char *sym;
} Names[] = {
	{"active",	ACTIVE,		0,		0},
	{"admit",	ADMIT,  	0,		0},
	{"assert",	ASSERT,		0,		0},
	{"atomic",	ATOMIC,		0,		0},
	{"bit",		TYPE,		BIT,		0},
	{"bool",	TYPE,		BIT,		0},
	{"break",	BREAK,		0,		0},
	{"byte",	TYPE,		BYTE,		0},
	{"chan",	TYPE,		CHAN,		0},
	{"D_proctype",	D_PROCTYPE,	0,		0},
	{"delay",	DELAY,		0,		0},
	{"do",		DO,		0,		0},
	{"double",	DOUBLETYPE,	SPIN_DOUBLE,	0},
	{"else", 	ELSE,		0,		0},
	{"empty",	EMPTY,		0,		0},
	{"enabled",	ENABLED,	0,		0},
	{"eval",	EVAL,		0,		0},
	{"expire",	EXPIRE,  	0,		0},
	{"extoper",	EXTOPER,  	0,		0},
	{"false",	CONST,		0,		0},
	{"fi",		FI,		0,		0},
	{"full",	FULL,		0,		0},
	{"_func",	FUNC,   	0,		0},
	{"goto",	GOTO,		0,		0},
	{"hidden",	HIDDEN,		0,		":hide:"},
	{"if",		IF,		0,		0},
	{"init",	INIT,		0,		":init:"},
	{"int",		TYPE,		SPIN_INT,	0},
	{"len",		LEN,		0,		0},
	{"mtype",	TYPE,		MTYPE,		0},
	{"nempty",	NEMPTY,		0,		0},
	{"never",	CLAIM,		0,		":never:"},
	{"nfull",	NFULL,		0,		0},
	{"notrace",	TRACE,		0,		":notrace:"},
	{"np_",		NONPROGRESS,	0,		0},
	{"od",		OD,		0,		0},
	{"of",		OF,		0,		0},
	{"_packf",	PACKF,		0,		0},
	{"pc_value",	PC_VAL,		0,		0},
	{"present",	PRESENT,	0,		0},
	{"printf",	PRINT,		0,		0},
	{"priority",	PRIORITY,	0,		0},
	{"proctype",	PROCTYPE,	0,		0},
	{"provided",	PROVIDED,	0,		0},
	{"return",	RETURN,		0,		0},
	{"run",		RUN,		0,		0},
	{"d_step",	D_STEP,		0,		0},
	{"short",	TYPE,		SHORT,		0},
	{"skip",	CONST,		1,		0},
	{"timeout",	TIMEOUT,	0,		0},
	{"timer",	TIMETYPE,	TIMER,		0},
	{"trace",	TRACE,		0,		":trace:"},
	{"true",	CONST,		1,		0},
	{"turnoff",	TURNOFF,	0,		0},
	{"show",	SHOW,		0,		":show:"},
	{"typedef",	TYPEDEF,	0,		0},
	{"unless",	UNLESS,		0,		0},
	{"unsigned",	TYPE,		UNSIGNED,	0},
	{"xr",		XU,		XR,		0},
	{"xs",		XU,		XS,		0},
	{0, 		0,		0,		0},
};

static int
check_name(char *s)
{	register int i;

	yylval = nn(ZN, 0, ZN, ZN);
	for (i = 0; Names[i].s; i++)
		if (strcmp(s, Names[i].s) == 0)
		{	yylval->val = Names[i].val;
			if (Names[i].sym)
				yylval->sym = lookup(Names[i].sym);
			return Names[i].tok;
		}

	if (((yylval->val) = ismtype(s)))
	{	yylval->ismtyp = 1;
		return CONST;
	}

	if (strcmp(s, "_last") == 0)
		has_last++;

	yylval->sym = lookup(s);	/* symbol table */

	if (isutype(s))
		return UNAME;
	if (isproctype(s))
		return PNAME;

	return NAME;
}

int
yylex(void)
{	static int last = 0;
	static int hold = 0;
	int c;
	/*
	 * repair two common syntax mistakes with
	 * semi-colons before or after a '}'
	 */
	if (hold)
	{	c = hold;
		hold = 0;
	} else
	{	c = lex();
		if (last == ELSE
		&&  c != SEMI
		&&  c != FI)
		{	hold = c;
			last = 0;
			return SEMI;
		}
		if (last == '}'
		&&  c != PROCTYPE
		&&  c != INIT
		&&  c != CLAIM
		&&  c != SEP
		&&  c != FI
		&&  c != OD
		&&  c != '}'
		&&  c != UNLESS
		&&  c != SEMI
		&&  c != EOF)
		{	hold = c;
			last = 0;
			return SEMI;	/* insert ';' */
		}
		if (c == SEMI)
		{	extern Symbol *context, *owner;
			/* if context, we're not in a typedef
			 * because they're global.
			 * if owner, we're at the end of a ref
			 * to a struct field -- prevent that the
			 * lookahead is interpreted as a field of
			 * the same struct...
			 */
			if (context) owner = ZS;
			hold = lex();	/* look ahead */
			if (hold == '}'
			||  hold == SEMI)
			{	c = hold; /* omit ';' */
				hold = 0;
			}
		}
	}
	last = c;
	return c;
}
