/***** tl_spin: tl_rewrite.c *****/

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

extern int	tl_verbose;

Node	*can = ZN;

Node *
right_linked(Node *n)
{
	if (!n || (n->ntyp != AND && n->ntyp != OR))
		return n;

	while (n->lft->ntyp == n->ntyp)
	{	Node *tmp = n->lft;
		n->lft = tmp->rgt;
		tmp->rgt = n;
		n = tmp;
	}
	if (n->rgt)
		n->rgt = right_linked(n->rgt);
	return n;
}

Node *
canonical(Node *n)
{	Node *m;	/* assumes input is right_linked */

	if (!n) return n;

	if ((m = in_cache(n)))
		return m;

	n->rgt = canonical(n->rgt);
	n->lft = canonical(n->lft);

	return cached(n);
}

Node *
push_negation(Node *n)
{	Node *m;
	Assert(n->ntyp == NOT, n->ntyp);

	switch (n->lft->ntyp) {
	case TRUE:
		Debug("!true => false\n");
		releasenode(0, n->lft);
		n->lft = ZN;
		n->ntyp = FALSE;
		break;
	case FALSE:
		Debug("!false => true\n");
		releasenode(0, n->lft);
		n->lft = ZN;
		n->ntyp = TRUE;
		break;
	case NOT:
		Debug("!!p => p\n");
		m = n->lft->lft;
		releasenode(0, n->lft);
		n->lft = ZN;
		releasenode(0, n);
		n = m;
		break;
	case V_OPER:
		Debug("!(p V q) => (!p U !q)\n");
		n->ntyp = U_OPER;
		goto same;
	case U_OPER:
		Debug("!(p U q) => (!p V !q)\n");
		n->ntyp = V_OPER;
		goto same;
#ifdef NXT
	case NEXT:
		Debug("!X -> X!\n");
		n->ntyp = NEXT;
		n->lft->ntyp = NOT;
		n->lft = push_negation(n->lft);
		break;
#endif
	case  AND:
		Debug("!(p && q) => !p || !q\n"); 
		n->ntyp = OR;
		goto same;
	case  OR:
		Debug("!(p || q) => !p && !q\n");
		n->ntyp = AND;

same:		m = n->lft->rgt;
		n->lft->rgt = ZN;

		n->rgt = Not(m);
		n->lft->ntyp = NOT;
		m = n->lft;
		n->lft = push_negation(m);
		break;
	}
	return rewrite(n);
}

static void
addcan(int tok, Node *n)
{	Node	*m, *prev = ZN;
	Node	**ptr;
	Node	*N;
	Symbol	*s, *t; int cmp;

	if (!n) return;

	if (n->ntyp == tok)
	{	addcan(tok, n->rgt);
		addcan(tok, n->lft);
		return;
	}
	if ((tok == AND && n->ntyp == TRUE)
	||  (tok == OR  && n->ntyp == FALSE))
		return;

	N = dupnode(n);
	if (!can)	
	{	can = N;
		return;
	}

	s = DoDump(N);
	if (can->ntyp != tok)	/* only one element in list so far */
	{	ptr = &can;
		goto insert;
	}

	/* there are at least 2 elements in list */
	prev = ZN;
	for (m = can; m->ntyp == tok && m->rgt; prev = m, m = m->rgt)
	{	t = DoDump(m->lft);
		cmp = strcmp(s->name, t->name);
		if (cmp == 0)	/* duplicate */
			return;
		if (cmp < 0)
		{	if (!prev)
			{	can = tl_nn(tok, N, can);
				return;
			} else
			{	ptr = &(prev->rgt);
				goto insert;
	}	}	}

	/* new entry goes at the end of the list */
	ptr = &(prev->rgt);
insert:
	t = DoDump(*ptr);
	cmp = strcmp(s->name, t->name);
	if (cmp == 0)	/* duplicate */
		return;
	if (cmp < 0)
		*ptr = tl_nn(tok, N, *ptr);
	else
		*ptr = tl_nn(tok, *ptr, N);
}

static void
marknode(int tok, Node *m)
{
	if (m->ntyp != tok)
	{	releasenode(0, m->rgt);
		m->rgt = ZN;
	}
	m->ntyp = -1;
}

Node *
Canonical(Node *n)
{	Node *m, *p=(Node *)0, *k1, *k2, *prev;
	int tok;

	if (!n) return n;

	tok = n->ntyp;
	if (tok != AND && tok != OR)
		return n;

	can = ZN;
	addcan(tok, n);
	releasenode(1, n);

#if 0
	Debug("\nA1: "); Dump(n); Debug("\n");
#endif
	/* mark redundant nodes */
	if (tok == AND)
	{	for (m = can; m; m = (m->ntyp == AND) ? m->rgt : ZN)
		{	k1 = (m->ntyp == AND) ? m->lft : m;
			if (k1->ntyp == TRUE)
			{	marknode(AND, m);
				continue;
			}
			if (k1->ntyp == FALSE)
			{	releasenode(1, can);
				can = False;
				goto out;
		}	}
		for (m = can; m; m = (m->ntyp == AND) ? m->rgt : ZN)
		for (p = can; p; p = (p->ntyp == AND) ? p->rgt : ZN)
		{	if (p == m
			||  p->ntyp == -1
			||  m->ntyp == -1)
				continue;
			k1 = (m->ntyp == AND) ? m->lft : m;
			k2 = (p->ntyp == AND) ? p->lft : p;

			if (isequal(k1, k2))
			{	marknode(AND, p);
				continue;
			}
			if (anywhere(OR, k1, k2))
			{	marknode(AND, p);
				continue;
			}
			if (k2->ntyp == U_OPER
			&&  anywhere(AND, k1, k2->rgt))
			{	marknode(AND, p);
				continue;
			}	/* q && (p U q) = q */
			if (k2->ntyp == V_OPER
			&&  k2->lft->ntyp == FALSE
			&&  anywhere(AND, k1, k2->rgt))
			{	marknode(AND, m);
				continue;
			}	/* p && (F V p) = (F V p) */
	}	}
	if (tok == OR)
	{	for (m = can; m; m = (m->ntyp == OR) ? m->rgt : ZN)
		{	k1 = (m->ntyp == OR) ? m->lft : m;
			if (k1->ntyp == FALSE)
			{	marknode(OR, p);
				continue;
			}
			if (k1->ntyp == TRUE)
			{	releasenode(1, can);
				can = True;
				goto out;
		}	}
		for (m = can; m; m = (m->ntyp == OR) ? m->rgt : ZN)
		for (p = can; p; p = (p->ntyp == OR) ? p->rgt : ZN)
		{	if (p == m
			||  p->ntyp == -1
			||  m->ntyp == -1)
				continue;
			k1 = (m->ntyp == OR) ? m->lft : m;
			k2 = (p->ntyp == OR) ? p->lft : p;

			if (isequal(k1, k2))
			{	marknode(OR, p);
				continue;
			}
			if (anywhere(AND, k1, k2))
			{	marknode(OR, p);
				continue;
			}
			if (k2->ntyp == U_OPER
			&&  anywhere(AND, k1, k2->rgt))
			{	marknode(OR, m);
				continue;
			}	/* q || (p U q) = (p U q) */
			if (k2->ntyp == V_OPER
			&&  k2->lft->ntyp == FALSE
			&&  anywhere(AND, k1, k2->rgt))
			{	marknode(OR, p);
				continue;
			}	/* p || (F V p) = p */
	}	}
	for (m = can, prev = ZN; m; )	/* remove marked nodes */
	{	if (m->ntyp == -1)
		{	k2 = m->rgt;
			releasenode(0, m);
			if (!prev)
			{	m = can = can->rgt;
			} else
			{	m = prev->rgt = k2;
				/* if deleted the last node in a chain */
				if (!prev->rgt && prev->lft
				&&  (prev->ntyp == AND || prev->ntyp == OR))
				{	k1 = prev->lft;
					prev->ntyp = prev->lft->ntyp;
					prev->sym = prev->lft->sym;
					prev->rgt = prev->lft->rgt;
					prev->lft = prev->lft->lft;
					releasenode(0, k1);
				}
			}
			continue;
		}
		prev = m;
		m = m->rgt;
	}
out:
#if 0
	Debug("A2: "); Dump(can); Debug("\n");
#endif
	if (!can) return True;

	return can;
}
