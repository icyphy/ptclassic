/***** spin: sym.c *****/

/* Copyright (c) 1991-1997 by Lucent Technologies - Bell Laboratories     */
/* All Rights Reserved.  This software is for educational purposes only.  */
/* Permission is given to distribute this code provided that this intro-  */
/* ductory message is not removed and no monies are exchanged.            */
/* No guarantee is expressed or implied by the distribution of this code. */
/* Software written by Gerard J. Holzmann as part of the book:            */
/* `Design and Validation of Computer Protocols,' ISBN 0-13-539925-4,     */
/* Prentice Hall, Englewood Cliffs, NJ, 07632.                            */
/* Send bug-reports and/or questions to: gerard@research.bell-labs.com    */

#include "spin.h"
#ifdef PC
#include "y_tab.h"
#else
#include "y.tab.h"
#endif

extern Symbol	*Fname, *owner;
extern int	lineno, depth;
extern short	has_xu;

Symbol	*context = ZS;
Ordered	*all_names = (Ordered *)0;
int	Nid = 0;

Ordered	*last_name = (Ordered *)0;
Symbol	*symtab_sym[Nhash+1];

static int
samename(Symbol *a, Symbol *b)
{
	if (!a && !b) return 1;
	if (!a || !b) return 0;
	return !strcmp(a->name, b->name);
}

int
hash(char *s)
{	int h=0;

	while (*s)
	{	h += *s++;
		h <<= 1;
		if (h&(Nhash+1))
			h |= 1;
	}
	return h&Nhash;
}

Symbol *
lookup(char *s)
{	Symbol *sp; Ordered *no;
	int h = hash(s);

	for (sp = symtab_sym[h]; sp; sp = sp->next)
		if (strcmp(sp->name, s) == 0
		&&  samename(sp->context, context)
		&&  samename(sp->owner, owner))
			return sp;		/* found */

	if (context)				/* in proctype */
	for (sp = symtab_sym[h]; sp; sp = sp->next)
		if (strcmp(sp->name, s) == 0
		&& !sp->context
		&&  samename(sp->owner, owner))
			return sp;		/* global */

	sp = (Symbol *) emalloc(sizeof(Symbol));
	sp->name = (char *) emalloc(strlen(s) + 1);
	strcpy(sp->name, s);
	sp->nel = 1;
	sp->setat = depth;
	sp->context = context;
	sp->owner = owner;			/* if fld in struct */
	sp->next = symtab_sym[h];
	symtab_sym[h] = sp;

	no = (Ordered *) emalloc(sizeof(Ordered));
	no->entry = sp;
	if (!last_name)
		last_name = all_names = no;
	else
	{	last_name->next = no;
		last_name = no;
	}

	return sp;
}

void
trackvar(Lextok *n, Lextok *m)
{	Symbol *sp = n->sym;

	if (!sp) return;	/* a structure list */
	switch (m->ntyp) {
	case NAME:
		if (m->sym->type != BIT)
		{	sp->hidden |= 4;
			if (m->sym->type != BYTE)
				sp->hidden |= 8;
		}
		break;
	case CONST:
		if (m->val != 0 && m->val != 1)
			sp->hidden |= 4;
		if (m->val < 0 || m->val > 256)
			sp->hidden |= 8; /* ditto byte-equiv */
		break;
	default:	/* unknown */
		sp->hidden |= (4|8); /* not known bit-equiv */
	}
}

Lextok *runstmnts = ZN;

void
trackrun(Lextok *n)
{
	runstmnts = nn(ZN, 0, n, runstmnts);
}

void
checkrun(Symbol *parnm, int posno)
{	Lextok *n, *now, *v; int i, m;
	int res = 0; char buf[16], buf2[16];

	for (n = runstmnts; n; n = n->rgt)
	{	now = n->lft;
		if (now->sym != parnm->context)
			continue;
		for (v = now->lft, i = 0; v; v = v->rgt, i++)
			if (i == posno)
			{	m = v->lft->ntyp;
				if (m == CONST)
				{	m = v->lft->val;
					if (m != 0 && m != 1)
						res |= 4;
					if (m < 0 || m > 256)
						res |= 8;
				} else if (m == NAME)
				{	m = v->lft->sym->type;
					if (m != BIT)
					{	res |= 4;
						if (m != BYTE)
							res |= 8;
					}
				} else
					res |= (4|8); /* unknown */
				break;
	}		}
	if (!(res&4) || !(res&8))
	{	strcpy(buf2, (!(res&4))?"bit":"byte");
		sputtype(buf, parnm->type);
		i = strlen(buf);
		while (buf[--i] == ' ') buf[i] = '\0';
		if (strcmp(buf, buf2) == 0) return;
		prehint(parnm);
		printf("proctype %s, '%s %s' could be declared",
			parnm->context->name, buf, parnm->name);
		printf(" '%s %s'\n", buf2, parnm->name);
	}
}

void
setptype(Lextok *n, int t, Lextok *vis)	/* predefined types */
{	int oln = lineno; extern int Expand_Ok;
	while (n)
	{	if (n->sym->type)
		{ lineno = n->ln; Fname = n->fn;
		  non_fatal("redeclaration of '%s'", n->sym->name);
		  lineno = oln;
		}
		n->sym->type = (short) t;

		if (Expand_Ok) n->sym->hidden |= (4|8|16); /* formal par */

		if (t == UNSIGNED)
		{	if (n->sym->nbits < 0)
			fatal("(%s) has invalid width-field", n->sym->name);
			if (n->sym->nbits == 0)
			{	n->sym->nbits = 16;
				non_fatal("unsigned without width-field", 0);
			}
		} else if (n->sym->nbits > 0)
		{	non_fatal("(%s) only an unsigned can have width-field",
				n->sym->name);
		}
		if (vis && strncmp(vis->sym->name, ":hide:", 6) == 0)
		{	n->sym->hidden |= 1;
			if (t == BIT)
			fatal("a bit variable (%s) cannot be hidden.",
				n->sym->name);
		}
		if (vis && strncmp(vis->sym->name, ":show:", 6) == 0)
		{	n->sym->hidden |= 2;
		}
		if (t == CHAN)
			n->sym->Nid = ++Nid;
		else
		{	n->sym->Nid = 0;
			if (n->sym->ini
			&&  n->sym->ini->ntyp == CHAN)
			{	Fname = n->fn;
				lineno = n->ln;
				fatal("chan initializer for non-channel %s",
				n->sym->name);
			}
		}
		if (n->sym->nel <= 0)
		{ lineno = n->ln; Fname = n->fn;
		  non_fatal("bad array size for '%s'", n->sym->name);
		  lineno = oln;
		}
		n = n->rgt;
	}
}

static void
setonexu(Symbol *sp, int t)
{
	sp->xu |= t;
	if (t == XR || t == XS)
	{	if (sp->xup[t-1]
		&&  strcmp(sp->xup[t-1]->name, context->name))
		{	printf("error: x[rs] claims from %s and %s\n",
				sp->xup[t-1]->name, context->name);
			non_fatal("conflicting claims on chan '%s'",
				sp->name);
		}
		sp->xup[t-1] = context;
	}
}

static void
setallxu(Lextok *n, int t)
{	Lextok *fp, *tl;

	for (fp = n; fp; fp = fp->rgt)
	for (tl = fp->lft; tl; tl = tl->rgt)
	{	if (tl->sym->type == STRUCT)
			setallxu(tl->sym->Slst, t);
		else if (tl->sym->type == CHAN)
			setonexu(tl->sym, t);
	}
}

Lextok *Xu_List = (Lextok *) 0;

void
setxus(Lextok *p, int t)
{	Lextok *m, *n;

	has_xu = 1; 
	if (!context)
	{	lineno = p->ln;
		Fname = p->fn;
		non_fatal("non-local x[rs] assertion", (char *)0);
	}
	for (m = p; m; m = m->rgt)
	{	Lextok *Xu_new = (Lextok *) emalloc(sizeof(Lextok));
		Xu_new->val = t;
		Xu_new->lft = m->lft;
		Xu_new->sym = context;
		Xu_new->rgt = Xu_List;
		Xu_List = Xu_new;

		n = m->lft;
		if (n->sym->type == STRUCT)
			setallxu(n->sym->Slst, t);
		else if (n->sym->type == CHAN)
			setonexu(n->sym, t);
		else
		{	int oln = lineno;
			lineno = n->ln; Fname = n->fn;
			non_fatal("xr or xs of non-chan '%s'",
				n->sym->name);
			lineno = oln;
		}
	}
}

Lextok *Mtype = (Lextok *) 0;

void
setmtype(Lextok *m)
{	Lextok *n = m;
	int oln = lineno;
	int cnt = 1;

	if (n) { lineno = n->ln; Fname = n->fn; }

	if (Mtype)
		non_fatal("mtype redeclared", (char *)0);
	Mtype = n;

	while (n)	/* syntax check */
	{	if (!n->lft || !n->lft->sym
		||  (n->lft->ntyp != NAME)
		||   n->lft->lft)	/* indexed variable */
			fatal("bad mtype definition", (char *)0);

		/* label the name */
		n->lft->sym->type = MTYPE;
		n->lft->sym->ini = nn(ZN,CONST,ZN,ZN);
		n->lft->sym->ini->val = cnt;
		n = n->rgt;
		cnt++;
	}
	lineno = oln;
	if (cnt > 256)
	fatal("too many mtype elements (>255)", (char *)0);
}

int
ismtype(char *str)	/* name to number */
{	Lextok *n;
	int cnt = 1;

	for (n = Mtype; n; n = n->rgt)
	{	if (strcmp(str, n->lft->sym->name) == 0)
			return cnt;
		cnt++;
	}
	return 0;
}

int
sputtype(char *foo, int m)
{
	switch (m) {
	case UNSIGNED:	strcpy(foo, "unsigned "); break;
	case BIT:	strcpy(foo, "bit   "); break;
	case BYTE:	strcpy(foo, "byte  "); break;
	case CHAN:	strcpy(foo, "chan  "); break;
	case SHORT:	strcpy(foo, "short "); break;
	case SPIN_INT:	strcpy(foo, "int   "); break;
/*	case TIMER:	strcpy(foo, "timer "); break; */
	case MTYPE:	strcpy(foo, "mtype "); break;
	case STRUCT:	strcpy(foo, "struct"); break;
	case PROCTYPE:	strcpy(foo, "proctype"); break;
	case LABEL:	strcpy(foo, "label "); return 0;
	default:	strcpy(foo, "value "); return 0;
	}
	return 1;
}


static int
puttype(int m)
{	char buf[32];

	if (sputtype(buf, m))
	{	printf("%s", buf);
		return 1;
	}
	return 0;
}

static void
symvar(Symbol *sp)
{	Lextok *m;
	int val=0;
	double ff;

	if (!puttype(sp->type))
		return;

	printf("\t");
	if (sp->owner) printf("%s.", sp->owner->name);
	printf("%s", sp->name);
	if (sp->nel > 1) printf("[%d]", sp->nel);

	if (sp->type == CHAN)
		printf("\t%d", (sp->ini)?sp->ini->val:0);
	else if (sp->type == STRUCT) /* Frank Weil, 2.9.8 */
		printf("\t%s", sp->Snm->name);
	else {
		if ((sp->type!=SPIN_DOUBLE)&&(sp->type!=TIMER)) val = eval(sp->ini,&ff);
		else (void) eval(sp->ini,&ff);
		if (ff==N_A_N) printf("\t%d", val); else printf("\t%f", ff);
	     }

	if (sp->owner)
		printf("\t<struct-field>");
	else
	if (!sp->context)
		printf("\t<global>");
	else
		printf("\t<%s>", sp->context->name);

	if (sp->Nid < 0)	/* formal parameter */
		printf("\t<parameter %d>", -(sp->Nid));
	else
		printf("\t<variable>");
	if (sp->type == CHAN && sp->ini)
	{	int i;
		for (m = sp->ini->rgt, i = 0; m; m = m->rgt)
			i++;
		printf("\t%d\t", i);
		for (m = sp->ini->rgt; m; m = m->rgt)
		{	(void) puttype(m->ntyp);
			if (m->rgt) printf("\t");
		}
	}
	printf("\n");
}

void
symdump(void)
{	Ordered *walk;

	for (walk = all_names; walk; walk = walk->next)
		symvar(walk->entry);
}
