/***** tl_spin: tl_buchi.c *****/

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

extern int tl_verbose, Total, Max_Red;

FILE	*tl_out;	/* if standalone: = stdout; */

typedef struct Transition {
	Symbol *name;
	Node *cond;
	int redundant;
	struct Transition *nxt;
} Transition;

typedef struct State {
	Symbol	*name;
	Transition *trans;
	Graph	*colors;
	unsigned char redundant;
	unsigned char accepting;
	unsigned char reachable;
	struct State *nxt;
} State;

State *never = (State *) 0;

static int
sametrans(Transition *s, Transition *t)
{
	if (strcmp(s->name->name, t->name->name) != 0)
		return 0;
	return isequal(s->cond, t->cond);
}

static Node *
Prune(Node *p)
{
	if (p)
	switch (p->ntyp) {
	case PREDICATE:
	case NOT:
	case FALSE:
	case TRUE:
#ifdef NXT
	case NEXT:
#endif
		return p;
	case OR:
		p->lft = Prune(p->lft);
		if (!p->lft)
		{	releasenode(1, p->rgt);
			return ZN;
		}
		p->rgt = Prune(p->rgt);
		if (!p->rgt)
		{	releasenode(1, p->lft);
			return ZN;
		}
		return p;
	case AND:
		p->lft = Prune(p->lft);
		if (!p->lft)
			return Prune(p->rgt);
		p->rgt = Prune(p->rgt);
		if (!p->rgt)
			return p->lft;
		return p;
	case V_OPER:
		releasenode(1, p->lft);
		return Prune(p->rgt);
	}
	releasenode(1, p);
	return ZN;
}

static State *
findstate(char *nm)
{	State *b;
	for (b = never; b; b = b->nxt)
		if (!strcmp(b->name->name, nm))
			return b;
	if (strcmp(nm, "accept_all"))
	{	if (strncmp(nm, "accept", 6))
		{	int i; char altnm[64];
			for (i = 0; i < 64; i++)
				if (nm[i] == '_')
					break;
			if (i >= 64)
				Fatal("name too long %s", nm);
			sprintf(altnm, "accept%s", &nm[i]);
			return findstate(altnm);
		}
	/*	Fatal("buchi: no state %s", nm); */
	}
	return (State *) 0;
}

static void
Dfs(State *b)
{	Transition *t;
	if (!b || b->reachable) return;
	b->reachable = 1;

	if (b->redundant)
		printf("Dfs hits redundant state %s\n", b->name->name);
	for (t = b->trans; t; t = t->nxt)
		Dfs(findstate(t->name->name));
}

static void
retarget(char *from, char *to)
{	State *b;
	Transition *t;
	Symbol *To = tl_lookup(to);

	for (b = never; b; b = b->nxt)
	{	if (!strcmp(b->name->name, from))
		{	b->redundant = 1;
			for (t = b->trans; t; t = t->nxt)
			{	releasenode(1, t->cond);
				t->cond = ZN;
			}
		} else
		for (t = b->trans; t; t = t->nxt)
		{	if (!strcmp(t->name->name, from))
				t->name = To;
	}	}
}

#ifdef NXT
static Node *
nonxt(Node *n)
{
	switch (n->ntyp) {
	case AND:
		n->lft = nonxt(n->lft);
		n->rgt = nonxt(n->rgt);
		if (!n->lft)
		{	if (!n->rgt)
				n = ZN;
			else
				n = n->rgt;
		} else if (!n->rgt)
			n = n->lft;
		return n;
		break;
	case OR:
	case U_OPER:
	case V_OPER:
	case NEXT:
		return ZN;
	}
	return n;
}
#endif

static Node *
combination(Node *s, Node *t)
{	Node *nc;
#ifdef NXT
	Node *a = nonxt(s);
	Node *b = nonxt(t);
	if (!a)
	{	releasenode(1, s);
		if (!b)
		{	releasenode(1, t);
			nc = False;
		} else
		{	nc = b;
		}
	} else if (!b)
	{	releasenode(1, t);
		nc = a;
	} else
	{	nc = tl_nn(OR, a, b);
	}
#else
	nc = tl_nn(OR, s, t);
#endif
	return nc;
}

static void
mergetrans(void)
{	State *b;
	Transition *s, *t;
	Node *nc;

	for (b = never; b; b = b->nxt)
	{	if (!b->reachable) continue;
		for (s = b->trans; s; s = s->nxt)
		for (t = s->nxt; t; t = t->nxt)
			if (!strcmp(s->name->name, t->name->name))
			{	if (!s->cond)
				{	releasenode(1, t->cond);
					nc = True;
				} else if (!t->cond)
				{	releasenode(1, s->cond);
					nc = True;
				} else
				{	nc = combination(s->cond, t->cond);
				}
				if (tl_verbose)
				printf("state %s, trans to %s redundant\n",
					b->name->name, s->name->name);
				t->cond = rewrite(nc);
				s->redundant = 1;
				break;
			}
	}
}

static int
all_trans_match(State *a, State *b)
{	Transition *s, *t;
	int found;

	if (a->accepting != b->accepting)
		return 0;

	for (s = a->trans; s; s = s->nxt)
	{	found = 0;
		for (t = b->trans; t; t = t->nxt)
			if (sametrans(s, t))
			{	found = 1;
				break;
			}
		if (!found)
			return 0;
	}
	for (s = b->trans; s; s = s->nxt)
	{	found = 0;
		for (t = a->trans; t; t = t->nxt)
			if (sametrans(s, t))
			{	found = 1;
				break;
			}
		if (!found)
			return 0;
	}
	return 1;
}

static void
mergestates(void)
{	State *a, *b;

	for (a = never; a; a = a->nxt)
	for (b = a->nxt; b; b = b->nxt)
		if (all_trans_match(a, b))
		{	if (tl_verbose)
				printf("state %s equals state %s\n",
				a->name->name, b->name->name);
			retarget(a->name->name, b->name->name);
			if (!strncmp(a->name->name, "accept", 6)
			&&  Max_Red == 0)
			{	char buf[64];
				sprintf(buf, "T0%s", &(a->name->name[6]));
				retarget(buf, b->name->name);
			}
			break;
		}
}

static void
printstate(State *b)
{	Transition *t;

	if (!b || (!tl_verbose && !b->reachable)) return;

	b->reachable = 0;	/* print only once */
	fprintf(tl_out, "%s:\n", b->name->name);

	if (strncmp(b->name->name, "accept", 6) == 0
	&&  Max_Red == 0)
		fprintf(tl_out, "T0%s:\n", &(b->name->name[6]));

	fprintf(tl_out, "\tif\n");
	for (t = b->trans; t; t = t->nxt)
	{	if (t->redundant) continue;
		fprintf(tl_out, "\t:: (");
		if (dump_cond(t->cond, t->cond, ZN, 0, 1))
			fprintf(tl_out, "1");
		fprintf(tl_out, ") -> goto %s\n", t->name->name);
	}
	fprintf(tl_out, "\tfi;\n");
	Total++;
}

void
addtrans(Graph *col, char *from, Node *op, char *to)
{	State *b;
	Transition *t;

	t = (Transition *) tl_emalloc(sizeof(Transition));
	t->name = tl_lookup(to);
	t->cond = Prune(dupnode(op));
	if (t->cond) t->cond = rewrite(t->cond);

	for (b = never; b; b = b->nxt)
		if (!strcmp(b->name->name, from))
		{	t->nxt = b->trans;
			b->trans = t;
			return;
		}
	b = (State *) tl_emalloc(sizeof(State));
	b->name   = tl_lookup(from);
	b->colors = col;
	b->trans  = t;
	if (!strncmp(from, "accept", 6))
		b->accepting = 1;
	b->nxt = never;
	never  = b;
}

void
fsm_print(void)
{	State *b;
	extern void put_uform(void);

	mergestates();
	b = findstate("T0_init");

	fprintf(tl_out, "never {    /* ");
		put_uform();
	fprintf(tl_out, " */\n");

	Dfs(b);
	mergetrans();

	if (Max_Red == 0)
		fprintf(tl_out, "accept_init:\n");
	if (!b && !never)
	{	fprintf(tl_out, "	0 /* false */;\n");
	} else
	{	printstate(b);	/* init state must be first */
		for (b = never; b; b = b->nxt)
			printstate(b);
	}
	fprintf(tl_out, "accept_all:\n	skip\n");
	fprintf(tl_out, "}\n");
}
