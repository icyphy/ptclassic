/***** tl_spin: tl_parse.c *****/

/* Copyright (c) 1995-1997 by Lucent Technologies - Bell Laboratories     */
/* All Rights Reserved.  This software is for educational purposes only.  */
/* Permission is given to distribute this code provided that this intro-  */
/* ductory message is not removed and no monies are exchanged.            */
/* No guarantee is expressed or implied by the distribution of this code. */
/* Written by Gerard J. Holzmann, Bell Laboratories, U.S.A.               */
/* Based on the translation algorithm by Gerth, Peled, Vardi, and Wolper, */
/* presented at the PSTV Conference, held in 1995, Warsaw, Poland 1995.   */
/* Send bug-reports and/or questions to: gerard@research.bell-labs.com    */

#include <stdio.h>
#include <string.h>
#include "tl.h"

extern int   tl_yylex(void);

int	tl_yychar = 0;
YYSTYPE	tl_yylval;

static Node	*tl_formula(void);
static Node	*tl_factor(void);
static Node	*tl_level(int);

static int	prec[2][4] = {
	{ U_OPER,  V_OPER, 0, 0 },	/* left associative */
	{ OR, AND, IMPLIES, EQUIV, },	/* left associative */
};

static Node *
tl_factor(void)
{	Node *ptr = ZN;

	switch (tl_yychar) {
	case '(':
		ptr = tl_formula();
		if (tl_yychar != ')')
			tl_yyerror("expected ')'");
		tl_yychar = tl_yylex();
		break;
	case NOT:
		ptr = tl_yylval;
		tl_yychar = tl_yylex();
		ptr->lft = tl_factor();
		ptr = push_negation(ptr);
		break;
	case ALWAYS:
		tl_yychar = tl_yylex();
		ptr = tl_nn(V_OPER, False, tl_factor());
		break;
#ifdef NXT
	case NEXT:
		tl_yychar = tl_yylex();
		ptr = tl_nn(NEXT, tl_factor(), ZN);
		break;
#endif
	case EVENTUALLY:
		tl_yychar = tl_yylex();
		ptr = tl_nn(U_OPER, True, tl_factor());
		break;
	case PREDICATE:
		ptr = tl_yylval;
		tl_yychar = tl_yylex();
		break;
	case TRUE:
	case FALSE:
		ptr = tl_yylval;
		tl_yychar = tl_yylex();
		break;
	}
	if (!ptr) tl_yyerror("expected predicate");
#if 0
	printf("factor:	");
	tl_explain(ptr->ntyp);
	printf("\n");
#endif
	return ptr;
}

static Node *
tl_level(int nr)
{	int i; Node *ptr = ZN;

	if (nr < 0)
		return tl_factor();

	ptr = tl_level(nr-1);
again:
	for (i = 0; i < 4; i++)
		if (tl_yychar == prec[nr][i])
		{	tl_yychar = tl_yylex();
			ptr = tl_nn(prec[nr][i],
				ptr, tl_level(nr-1));
			if (prec[nr][i] == IMPLIES)
			{	ptr = tl_nn(OR,
					Not(ptr->lft), ptr->rgt);
				ptr = rewrite(ptr);
			} else if (prec[nr][i] == EQUIV)
			{	ptr = tl_nn(OR,
				rewrite(tl_nn(AND,
					dupnode(ptr->lft),
					dupnode(ptr->rgt))),
				rewrite(tl_nn(AND,
					Not(ptr->lft),
					Not(ptr->rgt))));
				ptr = rewrite(ptr);
			}
			goto again;
		}
	if (!ptr) tl_yyerror("syntax error");
#if 0
	printf("level %d:	", nr);
	tl_explain(ptr->ntyp);
	printf("\n");
#endif
	return ptr;
}

static Node *
tl_formula(void)
{	tl_yychar = tl_yylex();
	return tl_level(1);	/* 2 precedence levels, 1 and 0 */	
}

void
tl_parse(void)
{
	trans(tl_formula());
}
