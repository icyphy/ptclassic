/***** tl_spin: tl_cache.c *****/

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

typedef struct Cache {
	Node *before;
	Node *after;
	int same;
	struct Cache *nxt;
} Cache;

Cache	*stored = (Cache *) 0;
static unsigned long	Caches, CacheHits;

static int	ismatch(Node *, Node *);

Node *
in_cache(Node *n)
{	Cache *d;

	for (d = stored; d; d = d->nxt)
		if (isequal(d->before, n))
		{	CacheHits++;
			if (d->same) return n;
			return dupnode(d->after);
		}
	return ZN;
}

Node *
cached(Node *n)
{	Cache *d;
	Node *m;

	if (!n) return n;

	if ((m = in_cache(n)))
		return m;

	Caches++;
	d = (Cache *) tl_emalloc(sizeof(Cache));
	d->before = dupnode(n);
	d->after  = Canonical(n); /* n is released */

	if (ismatch(d->before, d->after))
	{	d->same = 1;
		releasenode(1, d->after);
		d->after = d->before;
	}
	d->nxt = stored;
	stored = d;
	return dupnode(d->after);
}

void
cache_stats(void)
{
	printf("cache stores     : %9ld\n", Caches);
	printf("cache hits       : %9ld\n", CacheHits);
}

void
releasenode(int all_levels, Node *n)
{
	if (!n) return;

	if (all_levels)
	{	releasenode(1, n->lft);
		n->lft = ZN;
		releasenode(1, n->rgt);
		n->rgt = ZN;
	}
	tfree((void *) n);
}

Node *
tl_nn(int t, Node *ll, Node *rl)
{	Node *n = (Node *) tl_emalloc(sizeof(Node));

	n->ntyp = (short) t;
	n->lft  = ll;
	n->rgt  = rl;

	return n;
}

Node *
getnode(Node *p)
{	Node *n;

	if (!p) return p;

	n =  (Node *) tl_emalloc(sizeof(Node));
	n->ntyp = p->ntyp;
	n->sym  = p->sym; /* same name */
	n->lft  = p->lft;
	n->rgt  = p->rgt;

	return n;
}

Node *
dupnode(Node *n)
{	Node *d;

	if (!n) return n;
	d = getnode(n);
	d->lft = dupnode(n->lft);
	d->rgt = dupnode(n->rgt);
	return d;
}

int
isequal(Node *a, Node *b)
{
	if (!a && !b) return 1;
	if (!a || !b) return 0;
	if (a->ntyp != b->ntyp) return 0;

	if (a->sym
	&&  b->sym
	&&  strcmp(a->sym->name, b->sym->name) != 0)
		return 0;

	if (isequal(a->lft, b->lft) && isequal(a->rgt, b->rgt)) return 1;
	if (isequal(a->rgt, b->lft) && isequal(a->lft, b->rgt)) return 1;

	return 0;
}

static int
ismatch(Node *a, Node *b)
{
	if (!a && !b) return 1;
	if (!a || !b) return 0;
	if (a->ntyp != b->ntyp) return 0;

	if (a->sym
	&&  b->sym
	&&  strcmp(a->sym->name, b->sym->name) != 0)
		return 0;

	if (isequal(a->lft, b->lft) && isequal(a->rgt, b->rgt)) return 1;

	return 0;
}

int
anywhere(int tok, Node *srch, Node *in)
{
	if (!in)
		return 0;

	if (tok == AND && srch->ntyp == AND)
		return	anywhere(tok, srch->lft, in) &&
			anywhere(tok, srch->rgt, in);

	if (isequal(in, srch))
		return 1;

	if (tok == 0)
	{	if (in->ntyp == OR)
			return 0;
	} else
	{	if (in->ntyp != tok)
			return 0;
	}

	return	anywhere(tok, srch, in->lft) ||
		anywhere(tok, srch, in->rgt);
}
