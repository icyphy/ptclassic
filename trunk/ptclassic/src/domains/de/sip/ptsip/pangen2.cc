/***** spin: pangen2.c *****/

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
#include "version.h"
#ifdef PC
#include "y_tab.h"
#else
#include "y.tab.h"
#endif
#include "pangen2.h"
#include "pangen4.h"
#include "pangen5.h"

#define DELTA	500	/* sets an upperbound on nr of chan names */

#define blurb(fd, e)	fprintf(fd, "\n\t\t/* %s:%d */\n", \
				e->n->fn->name, e->n->ln)
#define tr_map(m, e)	fprintf(tt, "\t\ttr_2_src(%d, %s, %d);\n", \
				m, e->n->fn->name, e->n->ln);

extern ProcList	*rdy;
extern RunList	*spin_run;
extern Symbol	*Fname, *oFname, *owner, *context;
extern char	*claimproc, *eventmap;
extern int	lineno, verbose, Npars, Mpars;
extern int	m_loss, Nid, has_remote;
extern int	Ntimeouts, Etimeouts;
extern int	u_sync, u_async, nrRdy, nqs;
extern int	GenCode, IsGuard, Level, TestOnly;
extern char	*NextLab[];

FILE	*tc, *th, *tt, *tm, *tb;
int	OkBreak = -1;
short	nocast=0;	/* to turn off casts in lvalues */
short	terse=0;	/* terse printing of varnames */
short	no_arrays=0;
short	has_last=0;	/* spec refers to _last */
short	has_badelse=0;	/* spec contains else combined with chan refs */
short	has_enabled=0;	/* spec contains enabled() */
short	has_pcvalue=0;	/* spec contains pc_value() */
short	has_np=0;	/* spec contains np_ */
short	has_sorted=0;	/* spec contains `!!' (sorted-send) operator */
short	has_random=0;	/* spec contains `??' (random-recv) operator */
short	has_xu=0;	/* spec contains xr or xs assertions */
short	has_unless=0;	/* spec contains unless statements */
short	has_provided=0;	/* spec contains PROVIDED clauses on procs */
int	mst=0;		/* max nr of state/process */
int	claimnr = -1;	/* claim process, if any */
int	eventmapnr = -1; /* event trace, if any */
int	Pid;		/* proc currently processed */

static Lextok	*Nn[2];
static int	Det;	/* set if deterministic */
static int	T_sum, T_mus, t_cyc;
static int	TPE[2], EPT[2];
int	uniq=1;
short	withprocname=0;	/* prefix local varnames with procname */
short	_isok=0;	/* checks usage of predefined varble _ */

static int	has_global(Lextok *);
static int	getweight(Lextok *);
static int	scan_seq(Sequence *);
static void	genconditionals(void);
static void	mark_seq(Sequence *);
static void	patch_atomic(Sequence *);
static void	put_seq(Sequence *, int, int);
static void	putproc(ProcList *);
static void	Tpe(Lextok *);

static int
fproc(char *s)
{	ProcList *p;

	for (p = rdy; p; p = p->nxt)
		if (strcmp(p->n->name, s) == 0)
			return p->tn;

	fatal("proctype %s not found", s);
	return -1;
}

static void
reverse_procs(RunList *q)
{
	if (!q) return;
	reverse_procs(q->nxt);
	fprintf(tc, "	Addproc(%d);\n", q->tn);
}

static void
tm_predef_np(void)
{
	fprintf(th, "#define _T5	%d\n", uniq++);
	fprintf(th, "#define _T2	%d\n", uniq++);
	fprintf(tm, "\tcase  _T5:\t/* np_ */\n");
	fprintf(tm, "\t\tif (!((!(trpt->o_pm&4) && !(trpt->tau&128))))\n");
	fprintf(tm, "\t\t\tcontinue;\n");
	fprintf(tm, "\t\t/* else fall through */\n");
	fprintf(tm, "\tcase  _T2:\t/* true */\n");
	fprintf(tm, "\t\tm = 3; goto P999;\n");
}

static void
tt_predef_np(void)
{
	fprintf(tt, "\t/* np_ demon: */\n");
	fprintf(tt, "\ttrans[_NP_] = ");
	fprintf(tt, "(Trans **) emalloc(2*sizeof(Trans *));\n");
	fprintf(tt, "\tT = trans[_NP_][0] = ");
	fprintf(tt, "settr(9997,0,0,_T2,0,\"(1)\",   0,2,0);\n");
	fprintf(tt, "\t    T->nxt	  = ");
	fprintf(tt, "settr(9998,0,1,_T5,0,\"(np_)\", 1,2,0);\n");
	fprintf(tt, "\tT = trans[_NP_][1] = ");
	fprintf(tt, "settr(9999,0,1,_T5,0,\"(np_)\", 1,2,0);\n");
}

void
gensrc(void)
{	ProcList *p;

	if (!(tc = fopen("pan.c", "w"))		/* main routines */
	||  !(th = fopen("pan.h", "w"))		/* header file   */
	||  !(tt = fopen("pan.t", "w"))		/* transition matrix */
	||  !(tm = fopen("pan.m", "w"))		/* forward  moves */
	||  !(tb = fopen("pan.b", "w")))	/* backward moves */
	{	printf("spin: cannot create pan.[chtmfb]\n");
		alldone(1);
	}
	fprintf(th, "#define Version	\"%s\"\n", Version);
	fprintf(th, "#define Source	\"%s\"\n\n", oFname->name);
	fprintf(th, "#define uchar	unsigned char\n");
	fprintf(th, "#define DELTA	%d\n", DELTA);
	fprintf(th, "#ifdef MA\n");
	fprintf(th, "#if MA==1\n"); /* user typed -DMA without size */
	fprintf(th, "#undef MA\n#define MA	100\n");
	fprintf(th, "#endif\n#endif\n");
	fprintf(th, "#ifdef W_XPT\n");
	fprintf(th, "#if W_XPT==1\n"); /* user typed -DW_XPT without size */
	fprintf(th, "#undef W_XPT\n#define W_XPT 1000000\n");
	fprintf(th, "#endif\n#endif\n");
	fprintf(th, "#ifndef NFAIR\n");
	fprintf(th, "#define NFAIR	2	/* must be >= 2 */\n");
	fprintf(th, "#endif\n");
	if (Ntimeouts)
	fprintf(th, "#define NTIM	%d\n", Ntimeouts);
	if (Etimeouts)
	fprintf(th, "#define ETIM	%d\n", Etimeouts);
	if (has_remote)
	fprintf(th, "#define REM_REFS	%d\n", has_remote);
	if (has_last)
	fprintf(th, "#define HAS_LAST	%d\n", has_last);
	if (has_sorted)
	fprintf(th, "#define HAS_SORTED	%d\n", has_sorted);
	if (has_random)
	fprintf(th, "#define HAS_RANDOM	%d\n", has_random);
	if (!has_enabled && !eventmap)
		fprintf(th, "#define INLINE	1\n");
	if (has_enabled)
		fprintf(th, "#define HAS_ENABLED	1\n");
	if (has_unless)
	fprintf(th, "#define HAS_UNLESS	%d\n", has_unless);
	if (has_provided)
	fprintf(th, "#define HAS_PROVIDED	%d\n", has_provided);
	if (has_pcvalue)
	fprintf(th, "#define HAS_PCVALUE	%d\n", has_pcvalue);
	if (has_badelse)
	fprintf(th, "#define HAS_BADELSE	%d\n", has_badelse);
	if (has_np)
	fprintf(th, "#define HAS_NP	%d\n", has_np);

	fprintf(th, "#ifdef NP	/* includes np_ demon */\n");
	if (!has_np)
	fprintf(th, "#define HAS_NP	2\n");
	fprintf(th, "#define VERI	%d\n",	nrRdy);
	fprintf(th, "#define endclaim	3 /* none */\n");
	fprintf(th, "#endif\n");

	if (claimproc)
	{	claimnr = fproc(claimproc);
		/* NP overrides claimproc */
		fprintf(th, "#if !defined(NOCLAIM) && !defined NP\n");
		fprintf(th, "#define VERI	%d\n",	claimnr);
		fprintf(th, "#define endclaim	endstate%d\n",	claimnr);
		fprintf(th, "#endif\n");
	}
	if (eventmap)
	{	eventmapnr = fproc(eventmap);
		fprintf(th, "#define EVENT_TRACE	%d\n",	eventmapnr);
		fprintf(th, "#define endevent	endstate%d\n",	eventmapnr);
		if (eventmap[2] == 'o')	/* ":no_trace:" */
		fprintf(th, "#define NEGATED_TRACE	1\n");
	}

	fprintf(tc, "/*** Generated by %s ***/\n", Version);
	fprintf(tc, "/*** From source: %s ***/\n\n", oFname->name);

	ntimes(tc, 0, 1, Preamble);

	fprintf(tc, "#ifndef NOBOUNDCHECK\n");
	fprintf(tc, "#define Index(x, y)\tBoundcheck(x, y, II, tt, t)\n");
	fprintf(tc, "#else\n");
	fprintf(tc, "#define Index(x, y)\tx\n");
	fprintf(tc, "#endif\n");

	for (p = rdy; p; p = p->nxt)
		mst = max(p->s->maxel, mst);

	fprintf(tt, "#ifdef PEG\n");
	fprintf(tt, "struct T_SRC {\n");
	fprintf(tt, "	char *fl; int ln;\n");
	fprintf(tt, "} T_SRC[NTRANS];\n\n");
	fprintf(tt, "void\ntr_2_src(int m, char *file, int ln)\n");
	fprintf(tt, "{	T_SRC[m].fl = file;\n");
	fprintf(tt, "	T_SRC[m].ln = ln;\n");
	fprintf(tt, "}\n\n");
	fprintf(tt, "void\nputpeg(int n, int m)\n");
	fprintf(tt, "{	printf(\"%%5d\ttrans %%4d \", m, n);\n");
	fprintf(tt, "	printf(\"file %%s line %%3d\\n\",\n");
	fprintf(tt, "		T_SRC[n].fl, T_SRC[n].ln);\n");
	fprintf(tt, "}\n");
	fprintf(tt, "#else\n");
	fprintf(tt, "#define tr_2_src(m,f,l)\n");
	fprintf(tt, "#endif\n\n");

	fprintf(tt, "void\nsettable(void)\n{\tTrans *T;\n");
	fprintf(tt, "\tTrans *settr(int, int, int, int, int,");
	fprintf(tt, " char *, int, int, int);\n\n");
	fprintf(tt, "\ttrans = (Trans ***) ");
	fprintf(tt, "emalloc(%d*sizeof(Trans **));\n", nrRdy+1);
			/* +1 for np_ automaton */
	fprintf(tm, "	switch (t->forw) {\n");
	fprintf(tm, "	default: Uerror(\"bad forward move\");\n");

	fprintf(tb, "	switch (t->back) {\n");
	fprintf(tb, "	default: Uerror(\"bad return move\");\n");
	fprintf(tb, "	case  0: goto R999; /* nothing to undo */\n");

	for (p = rdy; p; p = p->nxt)
		putproc(p);

	tm_predef_np();
	tt_predef_np();

	fprintf(tt, "}\n\n");	/* end of settable() */
	fprintf(tm, "	}\n\n");
	fprintf(tb, "	}\n\n");
	ntimes(tt, 0, 1, Tail);
	genheader();
	genaddproc();
	genother();
	genaddqueue();
	genunio();
	genconditionals();
	gensvmap();

	if (!spin_run) fatal("no runable process", (char *)0);

	fprintf(tc, "void\n");
	fprintf(tc, "active_procs(void)\n{\n");
		reverse_procs(spin_run);
	fprintf(tc, "}\n");
	ntimes(tc, 0, 1, Dfa);
	ntimes(tc, 0, 1, Xpt);

	fprintf(th, "#define NTRANS	%d\n", uniq);
	fprintf(th, "#ifdef PEG\n");
	fprintf(th, "long peg[NTRANS];\n");
	fprintf(th, "#endif\n");
}

static int
find_id(Symbol *s)
{	ProcList *p;

	if (s)
	for (p = rdy; p; p = p->nxt)
		if (s == p->n)
			return p->tn;
	return 0;
}

static void
dolen(Symbol *s, char *pre, int pid, int ai, int qln)
{
	if (ai > 0)
		fprintf(tc, "\n\t\t\t ||    ");
	fprintf(tc, "%s(", pre);
	if (!(s->hidden&1))
	{	if (s->context)
			fprintf(tc, "((P%d *)thisseq)->", pid);
		else
			fprintf(tc, "now.");
	}
	fprintf(tc, "%s", s->name);
	if (qln > 1) fprintf(tc, "[%d]", ai);
	fprintf(tc, ")");
}

static struct AA {
	char TT[9];	char CC[8];
} BB[4] = {
	{ "Q_FULL_F",	" q_full" },
	{ "Q_FULL_T",	"!q_full" },
	{ "Q_EMPT_F",	" !q_len" },
	{ "Q_EMPT_T",	"  q_len" }
	};

static void
Done_case(char *nm, Symbol *z)
{	int j, k;
	int nid = z->Nid;
	int qln = z->nel;

	fprintf(tc, "\t\tcase %d: if (", nid);
	for (j = 0; j < 4; j++)
	{	fprintf(tc, "\t(t->ty[i] == %s && (", BB[j].TT);
		for (k = 0; k < qln; k++)
		{	if (k > 0)
				fprintf(tc, "\n\t\t\t ||    ");
			fprintf(tc, "%s(%s%s", BB[j].CC, nm, z->name);
			if (qln > 1)
				fprintf(tc, "[%d]", k);
			fprintf(tc, ")");
		}
		fprintf(tc, "))\n\t\t\t ");
		if (j < 3)
			fprintf(tc, "|| ");
		else
			fprintf(tc, "   ");
	}
	fprintf(tc, ") return 0; break;\n");
}

static void
Docase(Symbol *s, int pid, int nid)
{	int i, j;

	fprintf(tc, "\t\tcase %d: if (", nid);
	for (j = 0; j < 4; j++)
	{	fprintf(tc, "\t(t->ty[i] == %s && (", BB[j].TT);
		for (i = 0; i < s->nel; i++)
			dolen(s, BB[j].CC, pid, i, s->nel);
		fprintf(tc, "))\n\t\t\t ");
		if (j < 3)
			fprintf(tc, "|| ");
		else
			fprintf(tc, "   ");
	}
	fprintf(tc, ") return 0; break;\n");
}

static void
genconditionals(void)
{	Symbol *s;
	int last=0, j;
	extern Ordered	*all_names;
	Ordered *walk;

	fprintf(th, "#define LOCAL	1\n");
	fprintf(th, "#define Q_FULL_F	2\n");
	fprintf(th, "#define Q_EMPT_F	3\n");
	fprintf(th, "#define Q_EMPT_T	4\n");
	fprintf(th, "#define Q_FULL_T	5\n");
	fprintf(th, "#define TIMEOUT_F	6\n");
	fprintf(th, "#define GLOBAL	7\n");
	fprintf(th, "#define BAD	8\n");
	fprintf(th, "#define ALPHA_F	9\n");

	fprintf(tc, "int\n");
	fprintf(tc, "q_cond(short II, Trans *t)\n");
	fprintf(tc, "{	int i = 0;\n");
	fprintf(tc, "	for (i = 0; i < 6; i++)\n");
	fprintf(tc, "	{	if (t->ty[i] == TIMEOUT_F) return %s;\n",
					(Etimeouts)?"(!(trpt->tau&1))":"1");
	fprintf(tc, "		if (t->ty[i] == ALPHA_F)\n");
	fprintf(tc, "#ifdef GLOB_ALPHA\n");
	fprintf(tc, "			return 0;\n");
	fprintf(tc, "#else\n\t\t\treturn ");
	fprintf(tc, "(II+1 == (short) now._nr_pr && II+1 < MAXPROC);\n");
	fprintf(tc, "#endif\n");

	/* we switch on the chan name from the spec (as identified by
	 * the corresponding Nid number) rather than the actual qid
	 * because we cannot predict at compile time which specific qid
	 * will be accessed by the statement at runtime.  that is:
	 * we do not know which qid to pass to q_cond at runtime
	 * but we do know which name is used.  if it's a chan array, we
	 * must check all elements of the array for compliance (bummer)
	 */
	fprintf(tc, "		switch (t->qu[i]) {\n");
	fprintf(tc, "		case 0: break;\n");

	for (walk = all_names; walk; walk = walk->next)
	{	s = walk->entry;
		if (s->owner) continue;
		j = find_id(s->context);
		if (s->type == CHAN)
		{	if (last == s->Nid) continue;	/* chan array */
			last = s->Nid;
			Docase(s, j, last);
		} else if (s->type == STRUCT)
		{	/* struct may contain a chan */
			char gather[128];
			char pregat[128]; Symbol *z = (Symbol *)0;
			int nc, ov=0;
			strcpy(pregat, "");
			if (!(s->hidden&1))
			{	if (s->context)
					sprintf(pregat, "((P%d *)thisseq)->",j);
				else
					sprintf(pregat, "now.");
			}
			strcat(pregat, s->name);
			do {	strcpy(gather, pregat);
				nc = fill_struct(s, 0, ov, gather, &z, 0);
				if (z && last != z->Nid)
				{	last = z->Nid;
					if (nc < 0)
					Done_case(gather, z);
				}
				ov = -nc;
			} while (nc < 0);
		}
	}
	fprintf(tc, "	\tdefault: Uerror(\"unknown qid - q_cond\");\n");
	fprintf(tc, "	\t\t\treturn 0;\n");
	fprintf(tc, "	\t}\n");
	fprintf(tc, "	}\n");
	fprintf(tc, "	return 1;\n");
	fprintf(tc, "}\n");
}

static void
putproc(ProcList *p)
{	Pid = p->tn;
	Det = p->det;

	fprintf(th, "\nshort nstates%d=%d;\t/* %s */\n",
		p->tn, p->s->maxel, p->n->name);
	if (p->tn == claimnr)
	fprintf(th, "#define nstates_claim	nstates%d\n", p->tn);
	if (p->tn == eventmapnr)
	fprintf(th, "#define nstates_event	nstates%d\n", p->tn);

	fprintf(th, "#define endstate%d	%d\n",
		p->tn, p->s->last->seqno);
	fprintf(tm, "\n		 /* PROC %s */\n", p->n->name);
	fprintf(tb, "\n		 /* PROC %s */\n", p->n->name);
	fprintf(tt, "\n	/* proctype %d: %s */\n", p->tn, p->n->name);
	fprintf(tt, "\n	trans[%d] = (Trans **)", p->tn);
	fprintf(tt, " emalloc(%d*sizeof(Trans *));\n\n", p->s->maxel);

	if (Pid == eventmapnr)
	{	fprintf(th, "\n#define in_s_scope(x)	0");
		fprintf(tc, "\n#define in_r_scope(x)	0");
	}
	put_seq(p->s, 2, 0);
	if (Pid == eventmapnr)
	{	fprintf(th, "\n\n");
		fprintf(tc, "\n\n");
	}
	dumpsrc(p->s->maxel, p->tn);
}

static void
addTpe(int x)
{	int i;

	if (x <= 2) return;

	for (i = 0; i < T_sum; i++)
		if (TPE[i] == x)
			return;
	TPE[(T_sum++)%2] = x;
}

static void
cnt_seq(Sequence *s)
{	Element *f;
	SeqList *h;

	if (s)
	for (f = s->frst; ; f = f->nxt)
	{	Tpe(f->n);	/* sets EPT */
		addTpe(EPT[0]);
		addTpe(EPT[1]);
		for (h = f->sub; h; h = h->nxt)
			cnt_seq(h->thisseq);
		if (f == s->last)
			break;
	}
}

static void
typ_seq(Sequence *s)
{
	T_sum = 0;
	TPE[0] = 2; TPE[1] = 0;
	cnt_seq(s);
	if (T_sum > 2)		/* more than one type */
	{	TPE[0] = 5*DELTA;	/* non-mixing */
		TPE[1] = 0;
	}
}

static int
hidden(Lextok *n)
{
	if (n)
	switch (n->ntyp) {
	case  FULL: case  EMPTY:
	case NFULL: case NEMPTY: case TIMEOUT:
		Nn[(T_mus++)%2] = n;
		break;
	case '!': case UMIN: case '~': case ASSERT: case 'c':
		(void) hidden(n->lft);
		break;
	case '/': case '*': case '-': case '+':
	case '%': case LT:  case GT: case '&': case '^':
	case '|': case LE:  case GE:  case NE: case '?':
	case EQ:  case OR:  case AND: case LSHIFT: case RSHIFT:
		(void) hidden(n->lft);
		(void) hidden(n->rgt);
		break;
	}
	return T_mus;
}

static int
getNid(Lextok *n)
{
	if (n->sym->type == STRUCT
	&&  n->rgt && n->rgt->lft)
		return getNid(n->rgt->lft);

	if (n->sym->Nid == 0)
	{	fatal("bad channel name '%s'",
		(char *) ((n->sym) ? n->sym->name : "no name"));
	}
	return n->sym->Nid;
}

static int
valTpe(Lextok *n)
{	int res = 2;
	/*
	2 = local
	2+1	    .. 2+1*DELTA = nfull,  's'	- require q_full==false
	2+1+1*DELTA .. 2+2*DELTA = nempty, 'r'	- require q_len!=0
	2+1+2*DELTA .. 2+3*DELTA = empty	- require q_len==0
	2+1+3*DELTA .. 2+4*DELTA = full		- require q_full==true
	5*DELTA = non-mixing (i.e., always makes the selection global)
	6*DELTA = timeout (conditionally safe)
	7*DELTA = @, process deletion (conditionally safe)
	 */
	switch (n->ntyp) { /* a series of fall-thru cases: */
	case   FULL:	res += DELTA;		/* add 3*DELTA + chan nr */
	case  EMPTY:	res += DELTA;		/* add 2*DELTA + chan nr */
	case    'r':
	case NEMPTY:	res += DELTA;		/* add 1*DELTA + chan nr */
	case    's':
	case  NFULL:	res += getNid(n->lft);	/* add channel nr */
			break;

	case TIMEOUT:	res = 6*DELTA; break;
	case '@':	res = 7*DELTA; break;
	default:	break;
	}
	return res;
}

static void
Tpe(Lextok *n)	/* mixing in selections */
{
	EPT[0] = 2; EPT[1] = 0;

	if (!n) return;

	T_mus = 0;
	Nn[0] = Nn[1] = ZN;

	if (n->ntyp == 'c')
	{	if (hidden(n->lft) > 2)
		{	EPT[0] = 5*DELTA; /* non-mixing */
			EPT[1] = 0;
			return;
		}
	} else
		Nn[0] = n;

	if (Nn[0]) EPT[0] = valTpe(Nn[0]);
	if (Nn[1]) EPT[1] = valTpe(Nn[1]);
}

static void
put_escp(Element *e)
{	int n;
	SeqList *x;

	if (e->esc && e->n->ntyp != GOTO && e->n->ntyp != '.')
	{	for (x = e->esc, n = 0; x; x = x->nxt, n++)
		{	int i = huntele(x->thisseq->frst, e->status)->seqno;
			fprintf(tt, "\ttrans[%d][%d]->escp[%d] = %d;\n",
				Pid, e->seqno, n, i);
			fprintf(tt, "\treached%d[%d] = 1;\n",
				Pid, i);
		}
		for (x = e->esc, n=0; x; x = x->nxt, n++)
		{	fprintf(tt, "	/* escape #%d: %d */\n", n,
				huntele(x->thisseq->frst, e->status)->seqno);
			put_seq(x->thisseq, 2, 0);	/* args?? */
		}
		fprintf(tt, "	/* end-escapes */\n");
	}
}

static void
put_sub(Element *e, int Tt0, int Tt1)
{	Sequence *s = e->n->sl->thisseq;
	Element *g = ZE;
	int a;

	patch_atomic(s);
	putskip(s->frst->seqno);
	g = huntstart(s->frst);
	a = g->seqno;

	if ((e->n->ntyp == ATOMIC
	||  e->n->ntyp == D_STEP)
	&&  scan_seq(s))
		mark_seq(s);
	s->last->nxt = e->nxt;

	typ_seq(s);	/* sets TPE */

	if (e->n->ntyp == D_STEP)
	{	int inherit = (e->status&(ATOM|L_ATOM));
		fprintf(tm, "\tcase  %d: ", uniq++);
		fprintf(tm, "/* STATE %d - line %d %s - [",
			e->seqno, e->n->ln, e->n->fn->name);
		comment(tm, e->n, 0);
		fprintf(tm, "] */\n\t\t");

		if (s->last->n->ntyp == BREAK)
			OkBreak = target(huntele(s->last->nxt,
				s->last->status))->Seqno;
		else
			OkBreak = -1;

		if (!putcode(tm, s, e->nxt, 0, e->n->ln))
			fprintf(tm, "\t\tm = %d; goto P999;\n\n",
			getweight(s->frst->n));
	
		fprintf(tb, "\tcase  %d: ", uniq-1);
		fprintf(tb, "/* STATE %d */\n", e->seqno);
		fprintf(tb, "#if defined(FULLSTACK) && defined(NOCOMP)");
		fprintf(tb, " && !defined(BITSTATE) && !defined(MA)\n");
		fprintf(tb, "\t\tsv_restor(!(t->atom&2));\n");
		fprintf(tb, "#else\n");
		fprintf(tb, "\t\tsv_restor(0);\n");
		fprintf(tb, "#endif\n");
		fprintf(tb, "\t\tgoto R999;\n");
		if (e->nxt)
			a = huntele(e->nxt, e->status)->seqno;
		else
			a = 0;
		tr_map(uniq-1, e);
		fprintf(tt, "/*->*/\ttrans[%d][%d]\t= ",
			Pid, e->seqno);
		fprintf(tt, "settr(%d,%d,%d,%d,%d,\"",
			e->Seqno, D_ATOM|inherit, a, uniq-1, uniq-1);
		comment(tt, e->n, e->seqno);
		fprintf(tt, "\", %d, ", (s->frst->status&I_GLOB)?1:0);
		fprintf(tt, "%d, %d);\n", TPE[0], TPE[1]);
		put_escp(e);
	} else
	{	/* ATOMIC or NON_ATOMIC */
		fprintf(tt, "\tT = trans[ %d][%d] = ", Pid, e->seqno);
		fprintf(tt, "settr(%d,%d,0,0,0,\"",
		e->Seqno, (e->n->ntyp == ATOMIC)?ATOM:0);
		comment(tt, e->n, e->seqno);
		if ((e->status&CHECK2)
		||  (g->status&CHECK2))
			s->frst->status |= I_GLOB;
		fprintf(tt, "\", %d, %d, %d);",
			(s->frst->status&I_GLOB)?1:0, Tt0, Tt1);
		blurb(tt, e);
		fprintf(tt, "\tT->nxt\t= ");
		fprintf(tt, "settr(%d,%d,%d,0,0,\"",
			e->Seqno, (e->n->ntyp == ATOMIC)?ATOM:0, a);
		comment(tt, e->n, e->seqno);
		fprintf(tt, "\", %d, ", (s->frst->status&I_GLOB)?1:0);
		if (e->n->ntyp == NON_ATOMIC)
		{	fprintf(tt, "%d, %d);", Tt0, Tt1);
			blurb(tt, e);
			put_seq(s, Tt0, Tt1);
		} else
		{	fprintf(tt, "%d, %d);", TPE[0], TPE[1]);
			blurb(tt, e);
			put_seq(s, TPE[0], TPE[1]);
		}
	}
}

static void
put_el(Element *e, int Tt0, int Tt1)
{	int n, a, Global_ref;
	Element *g = ZE;

#ifdef DEBUG
	SeqList *x;
	printf("put_el %d (%d) (->%d (%d)) (", e->Seqno, e->seqno,
		(e->nxt)?e->nxt->Seqno:-1, (e->nxt)?e->nxt->seqno:-1);
	comment(stdout, e->n, 0);
	printf(")");
	if (e->esc) printf(" - escaped by ");
	for (x = e->esc; x; x = x->nxt)
	{	printf("%d (", x->thisseq->frst->Seqno);
		comment(stdout, x->thisseq->frst->n, 0);
		printf(") ");
	}
	printf("\n");
#endif
	if (e->n->ntyp == GOTO)
	{	g = get_lab(e->n, 1);
		g = huntele(g, e->status);
		cross_dsteps(e->n, g->n);
		a = g->seqno;
	} else if (e->nxt)
	{	g = huntele(e->nxt, e->status);
		a = g->seqno;
	} else
		a = 0;
	if (g
	&&  (g->status&CHECK2	/* entering remotely ref'd state */
	||   e->status&CHECK2))	/* leaving  remotely ref'd state */
		e->status |= I_GLOB;

	tr_map(uniq, e);
	fprintf(tt, "\ttrans[%d][%d]\t= ", Pid, e->seqno);

	fprintf(tm, "\tcase  %d: /* STATE %d - line %d %s - [",
		uniq++, e->seqno, e->n->ln, e->n->fn->name);
	comment(tm, e->n, 0);
	fprintf(tm, "] */\n\t\t");
	if (e->n->ntyp != 'r' && Pid != claimnr)
		fprintf(tm, "IfNotBlocked\n\t\t");

	Global_ref = (e->status&I_GLOB)?1:has_global(e->n);

	putstmnt(tm, e->n, e->seqno);
	if (e->n->ntyp != ELSE && Det)
	{	fprintf(tm, ";\n\t\tif (trpt->o_pm&1)\n\t\t");
		fprintf(tm, "\tuerror(\"non-determinism in D_proctype\")");
	}
	n = getweight(e->n);
	fprintf(tm, ";\n\t\tm = %d; goto P999;\n", n);
	if (any_undo(e->n))
	{	fprintf(tb, "\tcase  %d: ", uniq-1);
		fprintf(tb, "/* STATE %d */\n\t\t", e->seqno);
		if (any_undo(e->n))
		{
			_isok++;
			undostmnt(e->n, e->seqno);
			_isok--;
		}
		fprintf(tb, ";\n\t\tgoto R999;\n");
		fprintf(tt, "settr(%d,%d,%d,%d,%d,\"",
			e->Seqno, e->status&2, a, uniq-1, uniq-1);
	} else
		fprintf(tt, "settr(%d,%d,%d,%d,0,\"",
			e->Seqno, e->status&2, a, uniq-1);
	comment(tt, e->n, e->seqno);
	fprintf(tt, "\", %d, ", Global_ref);
	if (Tt0 != 2)
	{	fprintf(tt, "%d, %d);\n", Tt0, Tt1);
	}
	else
	{	Tpe(e->n);	/* sets EPT */
		fprintf(tt, "%d, %d);\n", EPT[0], EPT[1]);
	}
	put_escp(e);
}

static void
nested_unless(Element *e, Element *g)
{	struct SeqList *y = e->esc, *z = g->esc;

	for ( ; y && z; y = y->nxt, z = z->nxt)
		if (z->thisseq != y->thisseq)
			break;
	if (!y && !z)
		return;

	if (g->n->ntyp != GOTO
	&&  g->n->ntyp != '.'
	&&  e->sub->nxt)
	{	printf("error: (%s:%d) saw 'unless' on a guard:\n",
			(e->n)?e->n->fn->name:"-",
			(e->n)?e->n->ln:0);
		printf("=====>instead of\n");
		printf("	do (or if)\n");
		printf("	:: ...\n");
		printf("	:: stmnt1 unless stmnt2\n");
		printf("	od (of fi)\n");
		printf("=====>use\n");
		printf("	do (or if)\n");
		printf("	:: ...\n");
		printf("	:: stmnt1\n");
		printf("	od (or fi) unless stmnt2\n");
		printf("=====>or rewrite\n");
	}
}

static void
put_seq(Sequence *s, int Tt0, int Tt1)
{	SeqList *h;
	Element *e, *g;
	int a;
#ifdef DEBUG
	printf("put_seq %d - %d [%d] (%d,%d)\n",
	s->frst->Seqno,
	s->last->Seqno,
	s->extent->Seqno,
	Tt0, Tt1);
#endif
	for (e = s->frst; e; e = e->nxt)
	{
#ifdef DEBUG
		printf("put_seq %d (%d) (; %d (%d)) ",
			e->Seqno, e->seqno,
			e->nxt?e->nxt->Seqno:-1,
			e->nxt?e->nxt->seqno:-1);
		comment(stdout, e->n, 0);
		printf(" (%s)\n", (e->status & DONE)?"done":"new");
#endif
		if (e->status & DONE) goto checklast;	/* was continue */

		e->status |= DONE;

		if (e->n->ln)
			putsrc(e->n->ln, e->seqno);

		if (e->n->ntyp == UNLESS)
		{
#ifdef DEBUG
			printf("\t(unless) %d\n",
				e->sub->thisseq->frst->seqno);
#endif
			put_seq(e->sub->thisseq, Tt0, Tt1);
		} else if (e->sub)
		{	fprintf(tt, "\tT = trans[%d][%d] = ",
				Pid, e->seqno);
			fprintf(tt, "settr(%d,%d,0,0,0,\"",
				e->Seqno, e->status&2);
			comment(tt, e->n, e->seqno);
			if (e->status&CHECK2)
				e->status |= I_GLOB;
			fprintf(tt, "\", %d, %d, %d);",
				(e->status&I_GLOB)?1:0, Tt0, Tt1);
			blurb(tt, e);
			for (h = e->sub; h; h = h->nxt)
			{	putskip(h->thisseq->frst->seqno);
				g = huntstart(h->thisseq->frst);
				if (g->esc)
					nested_unless(e, g);
				a = g->seqno;
				if (h->nxt)
					fprintf(tt, "\tT = T->nxt\t= ");
				else
					fprintf(tt, "\t    T->nxt\t= ");
				fprintf(tt, "settr(%d,%d,%d,0,0,\"",
					e->Seqno, e->status&2, a);
				comment(tt, e->n, e->seqno);
				if (g->status&CHECK2)
					h->thisseq->frst->status |= I_GLOB;
				fprintf(tt, "\", %d, %d, %d);",
					(h->thisseq->frst->status&I_GLOB)?1:0,
					Tt0, Tt1);
				blurb(tt, e);
			}
#ifdef DEBUG
printf("	subsequence\n");
#endif
			for (h = e->sub; h; h = h->nxt)
				put_seq(h->thisseq, Tt0, Tt1);
		} else
		{	if (e->n->ntyp == ATOMIC
			||  e->n->ntyp == D_STEP
			||  e->n->ntyp == NON_ATOMIC)
{
#ifdef DEBUG
printf("	putsub\n");
#endif
				put_sub(e, Tt0, Tt1);
}
			else 
{
#ifdef DEBUG
printf("	putel\n");
#endif
				put_el(e, Tt0, Tt1);
}
		}
checklast:
		if (e == s->last)
{
#ifdef DEBUG
printf("	islast\n");
#endif
			break;
}
	}
#ifdef DEBUG
printf("	putseq returns\n");
#endif
}

static void
patch_atomic(Sequence *s)	/* catch goto's that break the chain */
{	Element *f, *g;
	SeqList *h;

	for (f = s->frst; ; f = f->nxt)
	{	if (f->n && f->n->ntyp == GOTO)
		{	g = get_lab(f->n,1);
			cross_dsteps(f->n, g->n);
			if ((f->status & (ATOM|L_ATOM))
			&& !(g->status & (ATOM|L_ATOM)))
			{	f->status &= ~ATOM;
				f->status |= L_ATOM;
			}
		} else
		for (h = f->sub; h; h = h->nxt)
			patch_atomic(h->thisseq);
		if (f == s->extent)
			break;
	}
}

static void
mark_seq(Sequence *s)
{	Element *f;
	SeqList *h;

	for (f = s->frst; ; f = f->nxt)
	{	f->status |= I_GLOB;
		for (h = f->sub; h; h = h->nxt)
			mark_seq(h->thisseq);
		if (f == s->last)
			return;
	}
}

static Element *
find_target(Element *e)
{	Element *f;

	if (!e) return e;

	if (t_cyc++ > 32)
	{	fatal("cycle of goto jumps", (char *) 0);
	}
	switch (e->n->ntyp) {
	case  GOTO:
		f = get_lab(e->n,1);
		cross_dsteps(e->n, f->n);
		f = find_target(f);
		break;
	case BREAK:
		if (e->nxt)
		f = find_target(huntele(e->nxt, e->status));
		/* else fall through */
	default:
		f = e;
		break;
	}
	return f;
}

Element *
target(Element *e)
{
	if (!e) return e;
	lineno = e->n->ln;
	Fname  = e->n->fn;
	t_cyc = 0;
	return find_target(e);
}

static int
scan_seq(Sequence *s)
{	Element *f, *g;
	SeqList *h;

	for (f = s->frst; ; f = f->nxt)
	{	if ((f->status&CHECK2)
		||  has_global(f->n))
			return 1;
		if (f->n->ntyp == GOTO)	/* may reach other atomic */
		{	g = target(f);
			if (g
			&& !(f->status & L_ATOM)
			&& !(g->status & (ATOM|L_ATOM)))
			{	fprintf(tt, "	/* goto mark-down, ");
				fprintf(tt, "line %d - %d */\n",
					f->n->ln, (g->n)?g->n->ln:0);
				return 1; /* assume worst case */
		}	}
		for (h = f->sub; h; h = h->nxt)
			if (scan_seq(h->thisseq))
				return 1;
		if (f == s->last)
			break;
	}
	return 0;
}

static int
glob_args(Lextok *n)
{	int result = 0;
	Lextok *v;

	for (v = n->rgt; v; v = v->rgt)
	{	if (v->lft->ntyp == CONST)
			continue;
		if (v->lft->ntyp == EVAL)
			result += has_global(v->lft->lft);
		else
			result += has_global(v->lft);
	}
	return result;
}

static int
has_global(Lextok *n)
{	Lextok *v; extern int runsafe;

	if (!n) return 0;
	switch (n->ntyp) {
	case '.': case BREAK: case GOTO: case CONST:
		return 0;

	case   ELSE: return n->val; /* true if combined with chan refs */

	case   NAME: return (n->sym->context)?0:1;

	case    's': return glob_args(n)!=0 || ((n->sym->xu&(XS|XX)) != XS);
	case    'r': return glob_args(n)!=0 || ((n->sym->xu&(XR|XX)) != XR);
	case    'R': return glob_args(n)!=0 || (((n->sym->xu)&(XR|XS|XX)) != (XR|XS));
	case NEMPTY: return ((n->sym->xu&(XR|XX)) != XR);
	case  NFULL: return ((n->sym->xu&(XS|XX)) != XS);
	case   FULL: return ((n->sym->xu&(XR|XX)) != XR);
	case  EMPTY: return ((n->sym->xu&(XS|XX)) != XS);
	case  LEN:   return (((n->sym->xu)&(XR|XS|XX)) != (XR|XS));

	case RUN: return 1-runsafe;

	case ENABLED: case PC_VAL: case NONPROGRESS:
	case 'p': case 'q':
	case TIMEOUT:
		return 1;

	/* 	@ was 1 (global) since 2.8.5
		in 3.0 it is considered local and
		conditionally safe, provided:
			II is the youngest process
			and nrprocs < MAXPROCS
	*/
	case '@': return 0;

	case '!': case UMIN: case '~': case ASSERT:
		return has_global(n->lft);

	case '/': case '*': case '-': case '+':
	case '%': case LT:  case GT: case '&': case '^':
	case '|': case LE:  case GE:  case NE: case '?':
	case EQ:  case OR:  case AND: case LSHIFT:
	case RSHIFT: case 'c': case ASGN:
		return has_global(n->lft) || has_global(n->rgt);

	case PRINT:
		for (v = n->lft; v; v = v->rgt)
			if (has_global(v->lft)) return 1;
		return 0;
	}
	return 0;
}

static void
Bailout(FILE *fd, char *str)
{
	if (!GenCode)
		fprintf(fd, "continue%s", str);
	else if (IsGuard)
		fprintf(fd, "%s%s", NextLab[Level], str);
	else
		fprintf(fd, "Uerror(\"block in step seq\")%s", str);
}

#define cat0(x)   	putstmnt(fd,now->lft,m); fprintf(fd, x); \
			putstmnt(fd,now->rgt,m)
#define cat1(x)		fprintf(fd,"("); cat0(x); fprintf(fd,")")
#define cat2(x,y)  	fprintf(fd,x); putstmnt(fd,y,m)
#define cat3(x,y,z)	fprintf(fd,x); putstmnt(fd,y,m); fprintf(fd,z)

void
putstmnt(FILE *fd, Lextok *now, int m)
{	Lextok *v;
	int i, j;

	if (!now) { fprintf(fd, "0"); return; }
	lineno = now->ln;
	Fname  = now->fn;

	switch (now->ntyp) {
	case CONST:	if (now->fval==N_A_N) fprintf(fd, "%d", now->val);
			else fprintf(fd, "%f", now->fval);
			break;
	case '!':	cat3(" !(", now->lft, ")"); break;
	case UMIN:	cat3(" -(", now->lft, ")"); break;
	case '~':	cat3(" ~(", now->lft, ")"); break;

	case '/':	cat1("/");  break;
	case '*':	cat1("*");  break;
	case '-':	cat1("-");  break;
	case '+':	cat1("+");  break;
	case '%':	cat1("%%"); break;
	case '&':	cat1("&");  break;
	case '^':	cat1("^");  break;
	case '|':	cat1("|");  break;
	case LT:	cat1("<");  break;
	case GT:	cat1(">");  break;
	case LE:	cat1("<="); break;
	case GE:	cat1(">="); break;
	case NE:	cat1("!="); break;
	case EQ:	cat1("=="); break;
	case OR:	cat1("||"); break;
	case AND:	cat1("&&"); break;
	case LSHIFT:	cat1("<<"); break;
	case RSHIFT:	cat1(">>"); break;
        case FUNC:      fprintf(fd,"func(parameters)"); break;
        case EXTOPER:   fprintf(fd,"extoper(%s,%s)",now->lft->sym->name,now->rgt->sym->name);
			break;
        case EXPIRE:    fprintf(fd,"expire(%s",now->lft->sym->name);
			cat3(",", now->rgt, ")");
                        break;
        case PACKF:     fprintf(fd,"pack a double-type number");
                        break;
        case PRESENT:   fprintf(fd,"present(%s)",now->lft->sym->name);
                        break;
        case ADMIT:     fprintf(fd,"admit(%s)",now->lft->sym->name);
                        break;
        case TURNOFF:   fprintf(fd,"turnoff(%s)",now->lft->sym->name);
                        break;
        case DELAY:     cat3("delay(", now->lft, ")");
                        break;
        case RETURN:    cat3("return(", now->lft, ")");
                        break;

	case TIMEOUT:
		fprintf(fd, "((trpt->tau)&1)");
		if (GenCode)
		  fatal("'timeout' inside d_step sequence", (char *)0);
		break;

	case RUN:
		if (claimproc
		&&  strcmp(now->sym->name, claimproc) == 0)
			fatal("claim %s, (not runnable)", claimproc);
		if (eventmap
		&&  strcmp(now->sym->name, eventmap) == 0)
			fatal("eventmap %s, (not runnable)", eventmap);

		if (GenCode)
		  fatal("'run' in d_step sequence (use atomic)",
			(char *)0);

		fprintf(fd,"addproc(%d", fproc(now->sym->name));
		for (v = now->lft, i = 0; v; v = v->rgt, i++)
		{	cat2(", ", v->lft);
		}

		if (i > Npars)
			fatal("too many parameters in run %s(...)",
			now->sym->name);

		for ( ; i < Npars; i++)
			fprintf(fd, ", 0");
		fprintf(fd, ")");
		break;

	case ENABLED:
		cat3("enabled(II, ", now->lft, ")");
		break;

	case NONPROGRESS:
		/* o_pm&4=progress, tau&128=claim stutter */
		fprintf(fd, "(!(trpt->o_pm&4) && !(trpt->tau&128))");
		break;

	case PC_VAL:
		cat3("((P0 *) Pptr(", now->lft, "+BASE))->_p");
		break;

	case LEN:
		if (!terse && !TestOnly && has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&1) || ");
			putname(fd, "q_R_check(", now->lft, m, "");
			fprintf(fd, ", II)) &&\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&2) || ");
			putname(fd, "q_S_check(", now->lft, m, ", II)) &&");
			fprintf(fd, "\n#endif\n\t\t");
		}
		putname(fd, "q_len(", now->lft, m, ")");
		break;

	case FULL:
		if (!terse && !TestOnly && has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&1) || ");
			putname(fd, "q_R_check(", now->lft, m, "");
			fprintf(fd, ", II)) &&\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&2) || ");
			putname(fd, "q_S_check(", now->lft, m, ", II)) &&");
			fprintf(fd, "\n#endif\n\t\t");
		}
		putname(fd, "q_full(", now->lft, m, ")");
		break;

	case EMPTY:
		if (!terse && !TestOnly && has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&1) || ");
			putname(fd, "q_R_check(", now->lft, m, "");
			fprintf(fd, ", II)) &&\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&2) || ");
			putname(fd, "q_S_check(", now->lft, m, ", II)) &&");
			fprintf(fd, "\n#endif\n\t\t");
		}
		putname(fd, "(q_len(", now->lft, m, ")==0)");
		break;

	case NFULL:
		if (!terse && !TestOnly && has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&2) || ");
			putname(fd, "q_S_check(", now->lft, m, ", II)) &&");
			fprintf(fd, "\n#endif\n\t\t");
		}
		putname(fd, "(!q_full(", now->lft, m, "))");
		break;

	case NEMPTY:
		if (!terse && !TestOnly && has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&1) || ");
			putname(fd, "q_R_check(", now->lft, m, ", II)) &&");
			fprintf(fd, "\n#endif\n\t\t");
		}
		putname(fd, "(q_len(", now->lft, m, ")>0)");
		break;

	case 's':
		if (Pid == eventmapnr)
		{	fprintf(fd, "if (_tp != 's' ");
			putname(fd, "|| _qid+1 != ", now->lft, m, "");
			for (v = now->rgt, i=0; v; v = v->rgt, i++)
			{	if (v->lft->ntyp != CONST
				&&  v->lft->ntyp != EVAL)
					continue;
				fprintf(fd, " \\\n\t\t|| qrecv(");
				putname(fd, "", now->lft, m, ", ");
				putname(fd, "q_len(", now->lft, m, ")-1, ");
				fprintf(fd, "%d, 0) != ", i);
				if (v->lft->ntyp == CONST)
					putstmnt(fd, v->lft, m);
				else /* EVAL */
					putstmnt(fd, v->lft->lft, m);
			}
			fprintf(fd, ")\n");
			fprintf(fd, "\t\t	continue");
putname(th, " || (x == ", now->lft, m, ")");
			break;
		}
		if (TestOnly)
		{	if (m_loss)
				fprintf(fd, "1");
			else
				putname(fd, "!q_full(", now->lft, m, ")");
			break;
		}
		if (has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "if (q_claim[", now->lft, m, "]&2) ");
			putname(fd, "q_S_check(", now->lft, m, ", II);");
			fprintf(fd, "\n#endif\n\t\t");
		}
		fprintf(fd, "if (q_%s",
			(u_sync > 0 && u_async == 0)?"len":"full");
		putname(fd, "(", now->lft, m, "))\n");

		if (m_loss)
		{	if (!GenCode)
			  fprintf(fd, "\t\t{ nlost++; m=3; goto P999; }\n");
			else
			  fprintf(fd, "\t\t\tnlost++;\n\t\telse {");
		} else
		{	fprintf(fd, "\t\t\t");
			Bailout(fd, ";");
		}

		if (has_enabled)
			fprintf(fd, "\n\t\tif (TstOnly) return 1;");

		putname(fd, "\n\t\tqsend(", now->lft, m, "");
		fprintf(fd, ", %d", now->val);
		for (v = now->rgt, i = 0; v; v = v->rgt, i++)
		{	cat2(", ", v->lft);
		}
		if (i > Mpars)
		{	putname(stdout, "channel name: ", now->lft, m, "\n");
			printf("	%d msg parameters\n", i);
			fatal("too many pars in send", "");
		}
		for ( ; i < Mpars; i++)
			fprintf(fd, ", 0");
		fprintf(fd, ")");
		if (u_sync)
		{	fprintf(fd, ";\n\t\t");
			if (u_async)
			  putname(fd, "if (q_zero(", now->lft, m, ")) ");
			putname(fd, "{ boq = ", now->lft, m, "");
			if (GenCode)
			  fprintf(fd, "; Uerror(\"rv-attempt in d_step\")");
			fprintf(fd, "; }");
		}
		if (m_loss && GenCode)
			fprintf(fd, ";\n\t\t}\n\t\t");
		break;

	case 'r':
		if (Pid == eventmapnr)
		{	fprintf(fd, "if (_tp != 'r' ");
			putname(fd, "|| _qid+1 != ", now->lft, m, "");
			for (v = now->rgt, i=0; v; v = v->rgt, i++)
			{	if (v->lft->ntyp != CONST
				&&  v->lft->ntyp != EVAL)
					continue;
				fprintf(fd, " \\\n\t\t|| qrecv(");
				putname(fd, "", now->lft, m, ", ");
				fprintf(fd, "0, %d, 0) != ", i);
				if (v->lft->ntyp == CONST)
					putstmnt(fd, v->lft, m);
				else /* EVAL */
					putstmnt(fd, v->lft->lft, m);
			}
			fprintf(fd, ")\n");
			fprintf(fd, "\t\t	continue");
putname(tc, " || (x == ", now->lft, m, ")");
			break;
		}
		if (TestOnly)
		{	fprintf(fd, "((");
			if (u_sync)
				fprintf(fd, "(boq == -1 && ");
			putname(fd, "q_len(", now->lft, m, ")");
			if (u_sync && now->val <= 1)
			{ putname(fd, ") || (boq == ",  now->lft,m," && ");
			  putname(fd, "q_zero(", now->lft,m,"))");
			}
			fprintf(fd, ")");
			if (now->val == 0 || now->val == 2)
			{	for (v = now->rgt, i=j=0; v; v = v->rgt, i++)
				{ if (v->lft->ntyp == CONST)
				  { cat3("\n\t\t&& (", v->lft, " == ");
				    putname(fd, "qrecv(", now->lft, m, ", ");
				    fprintf(fd, "0, %d, 0))", i);
				  } else if (v->lft->ntyp == EVAL)
				  { cat3("\n\t\t&& (", v->lft->lft, " == ");
				    putname(fd, "qrecv(", now->lft, m, ", ");
				    fprintf(fd, "0, %d, 0))", i);
				  } else
				  {	j++; continue;
				  }
				}
				fprintf(fd, ")");
			} else
			{	fprintf(fd, "\n\t\t&& Q_has(");
				putname(fd, "", now->lft, m, "");
				for (v = now->rgt, i=0; v; v = v->rgt, i++)
				{	if (v->lft->ntyp == CONST)
					{	fprintf(fd, ", 1, ");
						putstmnt(fd, v->lft, m);
					} else if (v->lft->ntyp == EVAL)
					{	fprintf(fd, ", 1, ");
						putstmnt(fd, v->lft->lft, m);
					} else
					{	fprintf(fd, ", 0, 0");
				}	}
				for ( ; i < Mpars; i++)
					fprintf(fd, ", 0, 0");
				fprintf(fd, "))");
			}
			break;
		}
		if (has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "if (q_claim[", now->lft, m, "]&1) ");
			putname(fd, "q_R_check(", now->lft, m, ", II);");
			fprintf(fd, "\n#endif\n\t\t");
		}
		if (u_sync)
		{	if (now->val >= 2)
			{	if (u_async)
				{ fprintf(fd, "if (");
				  putname(fd, "q_zero(", now->lft,m,"))");
				  fprintf(fd, "\n\t\t{\t");
				}
				fprintf(fd, "uerror(\"polling ");
				fprintf(fd, "rv chan\");\n\t\t");
				if (u_async)
				  fprintf(fd, "	continue;\n\t\t}\n\t\t");
				fprintf(fd, "IfNotBlocked\n\t\t");
			} else
			{	fprintf(fd, "if (");
				if (u_async == 0)
				  putname(fd, "boq != ", now->lft,m,") ");
				else
				{ putname(fd, "q_zero(", now->lft,m,"))");
				  fprintf(fd, "\n\t\t{\tif (boq != ");
				  putname(fd, "",  now->lft,m,") ");
				  Bailout(fd, ";\n\t\t} else\n\t\t");
				  fprintf(fd, "{\tif (boq != -1) ");
				}
				Bailout(fd, ";\n\t\t");
				if (u_async)
					fprintf(fd, "}\n\t\t");
		}	}
		putname(fd, "if (q_len(", now->lft, m, ") == 0) ");
		Bailout(fd, "");

		for (v = now->rgt, j=0; v; v = v->rgt)
		{	if (v->lft->ntyp != CONST
			&&  v->lft->ntyp != EVAL)
				j++;	/* count settables */
		}
		fprintf(fd, ";\n\t{\n\t\tint XX=1");
/* test */	if (now->val == 0 || now->val == 2)
		{	for (v = now->rgt, i=0; v; v = v->rgt, i++)
			{	if (v->lft->ntyp == CONST)
				{ fprintf(fd, ";\n\t\t");
				  cat3("if (", v->lft, " != ");
				  putname(fd, "qrecv(", now->lft, m, ", ");
				  fprintf(fd, "0, %d, 0)) ", i);
				  Bailout(fd, "");
				} else if (v->lft->ntyp == EVAL)
				{ fprintf(fd, ";\n\t\t");
				  cat3("if (", v->lft->lft, " != ");
				  putname(fd, "qrecv(", now->lft, m, ", ");
				  fprintf(fd, "0, %d, 0)) ", i);
				  Bailout(fd, "");
			}	}
		} else	/* random receives */
		{	fprintf(fd, ";\n\t\tif (!(XX = Q_has(");
			putname(fd, "", now->lft, m, "");
			for (v = now->rgt, i=0; v; v = v->rgt, i++)
			{	if (v->lft->ntyp == CONST)
				{	fprintf(fd, ", 1, ");
					putstmnt(fd, v->lft, m);
				} else if (v->lft->ntyp == EVAL)
				{	fprintf(fd, ", 1, ");
					putstmnt(fd, v->lft->lft, m);
				} else
				{	fprintf(fd, ", 0, 0");
			}	}
			for ( ; i < Mpars; i++)
				fprintf(fd, ", 0, 0");
			fprintf(fd, "))) ");
			Bailout(fd, "");
		}

		if (has_enabled)
			fprintf(fd, ";\n\t\tif (TstOnly) return 1");

		if (j == 0 && now->val >= 2)
		{	fprintf(fd, ";\n\t}\n\t\t");
			break;	/* poll without side-effect */
		}

		if (!GenCode)
		{	fprintf(fd, ";\n");
			if (j == 0 && now->val == 0)
			{	fprintf(fd, "\t\tif (q_flds[((Q0 *)qptr(");
				putname(fd, "", now->lft, m, "-1))->_t]");
				fprintf(fd, " != %d)\n", i);
			}
			fprintf(fd, "#if defined(FULLSTACK)");
			fprintf(fd, " && defined(NOCOMP)");
			fprintf(fd, " && !defined(BITSTATE)");
			fprintf(fd, " && !defined(MA)\n");
			fprintf(fd, "\t\tif (t->atom&2)\n");
			fprintf(fd, "#endif\n");
			fprintf(fd, "#if VECTORSZ<=1024\n");
			fprintf(fd, "\t\t\tsv_save((char *)&now);\n");
			fprintf(fd, "#else\n");
			fprintf(fd, "\t\t\tp_q_save(II, ");
			putname(fd, "", now->lft, m, ");\n");
			fprintf(fd, "#endif\n\t\t");
		}
/* set */	for (v = now->rgt, i = 0; v; v = v->rgt, i++)
		{	if ((v->lft->ntyp == CONST
			||   v->lft->ntyp == EVAL) && v->rgt)
				continue;
			fprintf(fd, ";\n\t\t");
			if (v->lft->ntyp == NAME
			&&  v->lft->sym->type == CHAN)
			{
			fprintf(fd, ";\n#ifndef XUSAFE\n\t\t");
			fprintf(fd, "if (");
			putname(fd, "(q_claim[", v->lft, m, "]&3)");
			fprintf(fd, ")\n\t\t\tuerror(\"assigns to x[rs] chan\");");
			fprintf(fd, "\n#endif\n\t\t");
			}
			if (v->lft->ntyp != CONST
			&&  v->lft->ntyp != EVAL)
			{	nocast=1;
				_isok++;
				putstmnt(fd, v->lft, m);
				_isok--;
				nocast=0;
				fprintf(fd, " = ");
			}
			putname(fd, "qrecv(", now->lft, m, ", ");
			fprintf(fd, "XX-1, %d, ", i);
			fprintf(fd, "%d)", (v->rgt || now->val >= 2)?0:1);

			if (v->lft->ntyp != CONST
			&&  v->lft->ntyp != EVAL
			&&  (v->lft->ntyp != NAME
			||   v->lft->sym->type != CHAN
			||   verbose > 0))
			{	fprintf(fd, ";\n#ifdef VAR_RANGES");
				fprintf(fd, "\n\t\tlogval(\"");
				withprocname = terse = nocast = 1;
				_isok++;
				putstmnt(fd,v->lft,m);
				withprocname = terse = nocast = 0;
				fprintf(fd, "\", ");
				putstmnt(fd,v->lft,m);
				_isok--;
				fprintf(fd, ");\n#endif\n\t\t");
			}
		}
		fprintf(fd, ";\n\t}\n\t\t");
		if (u_sync)
		{	putname(fd, "if (q_zero(", now->lft, m, "");
			fprintf(fd, ")) boq = -1");
		}
		break;

	case 'R':
		if (!terse && !TestOnly && has_xu)
		{	fprintf(fd, "\n#ifndef XUSAFE\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&1) || ");
			fprintf(fd, "q_R_check(");
			putname(fd, "", now->lft, m, ", II)) &&\n\t\t");
			putname(fd, "(!(q_claim[", now->lft, m, "]&2) || ");
			putname(fd, "q_S_check(", now->lft, m, ", II)) &&");
			fprintf(fd, "\n#endif\n\t\t");
		}
		if (u_sync>0)
			putname(fd, "not_RV(", now->lft, m, ") &&\n\t\t");

		for (v = now->rgt, i=j=0; v; v = v->rgt, i++)
			if (v->lft->ntyp != CONST
			&&  v->lft->ntyp != EVAL)
			{	j++; continue;
			}
		if (now->val == 0 || i == j)
		{	putname(fd, "(q_len(", now->lft, m, ") > 0");
			for (v = now->rgt, i=0; v; v = v->rgt, i++)
			{	if (v->lft->ntyp != CONST
				&&  v->lft->ntyp != EVAL)
					continue;
				fprintf(fd, " \\\n\t\t&& qrecv(");
				putname(fd, "", now->lft, m, ", ");
				fprintf(fd, "0, %d, 0) == ", i);
				if (v->lft->ntyp == CONST)
					putstmnt(fd, v->lft, m);
				else /* EVAL */
					putstmnt(fd, v->lft->lft, m);
			}
			fprintf(fd, ")");
		} else
		{	putname(fd, "Q_has(", now->lft, m, "");
			for (v = now->rgt, i=0; v; v = v->rgt, i++)
			{	if (v->lft->ntyp == CONST)
				{	fprintf(fd, ", 1, ");
					putstmnt(fd, v->lft, m);
				} else if (v->lft->ntyp == EVAL)
				{	fprintf(fd, ", 1, ");
					putstmnt(fd, v->lft->lft, m);
				} else
					fprintf(fd, ", 0, 0");
			}	
			for ( ; i < Mpars; i++)
				fprintf(fd, ", 0, 0");
			fprintf(fd, ")");
		}
		break;

	case 'c':
		cat3("if (!(", now->lft, "))\n\t\t\t");
		Bailout(fd, "");
		break;

	case  ELSE:
		if (!GenCode)
		{	fprintf(fd, "if (trpt->o_pm&1)\n\t\t\t");
			Bailout(fd, "");
		} else
		{	fprintf(fd, "/* else */");
		}
		break;

	case '?':
		cat3("( (", now->lft, ") ? ");
		cat3("(", now->rgt->lft, ") : ");
		cat3("(", now->rgt->rgt, ") )");
		break;

	case ASGN:
		if (has_enabled)
		fprintf(fd, "if (TstOnly) return 1;\n\t\t");

/* new 2.9 */
		if (now->sym->type == CHAN)
		{
			fprintf(fd, ";\n#ifndef XUSAFE\n\t\t");
			fprintf(fd, "if (");
			putname(fd, "(q_claim[", now->lft, m, "]&3)");
			fprintf(fd, ")\n\t\t\tuerror(\"assigns to x[rs] chan\");");
			fprintf(fd, "\n#endif\n\t\t");
		}
/* wen */
		_isok++;
		if (!GenCode)
		{
			cat3("(trpt+1)->oval = ", now->lft, ";\n\t\t");
		}
		nocast = 1; putstmnt(fd,now->lft,m); nocast = 0;
		fprintf(fd," = ");
		_isok--;
		putstmnt(fd,now->rgt,m);
		if (now->sym->type != CHAN
		||  verbose > 0)
		{	fprintf(fd, ";\n#ifdef VAR_RANGES");
			fprintf(fd, "\n\t\tlogval(\"");
			withprocname = terse = nocast = 1;
			_isok++;
			putstmnt(fd,now->lft,m);
			withprocname = terse = nocast = 0;
			fprintf(fd, "\", ");
			putstmnt(fd,now->lft,m);
			_isok--;
			fprintf(fd, ");\n#endif\n\t\t");
		}
		break;

	case PRINT:
		if (has_enabled)
		fprintf(fd, "if (TstOnly) return 1;\n\t\t");
#ifdef PRINTF
		fprintf(fd, "printf(%s", now->sym->name);
#else
		fprintf(fd, "Printf(%s", now->sym->name);
#endif
		for (v = now->lft; v; v = v->rgt)
		{	cat2(", ", v->lft);
		}
		fprintf(fd, ")");
		break;

	case NAME:
		if (!nocast && now->sym && Sym_typ(now) < SHORT)
			putname(fd, "((int)", now, m, ")");
		else
			putname(fd, "", now, m, "");
		break;

	case   'p':
		putremote(fd, now, m);
		break;

	case   'q':
		if (terse)
			fprintf(fd, "%s", now->sym->name);
		else
			fprintf(fd, "%d", remotelab(now));
		break;

	case ASSERT:
		if (has_enabled)
			fprintf(fd, "if (TstOnly) return 1;\n\t\t");

		cat3("assert(", now->lft, ", ");
		terse = nocast = 1;
		cat3("\"", now->lft, "\", II, tt, t)");
		terse = nocast = 0;
		break;

	case '.':
	case BREAK:
	case GOTO:
		if (Pid == eventmapnr)
			fprintf(fd, "Uerror(\"cannot get here\")");
		putskip(m);
		break;

	case '@':
		if (Pid == eventmapnr)
		{	fprintf(fd, "return 0");
			break;
		}

		if (has_enabled)
		{	fprintf(fd, "if (TstOnly)\n\t\t\t");
			fprintf(fd, "return (II+1 == now._nr_pr);\n\t\t");
		}
		fprintf(fd, "if (!delproc(1, II)) ");
		Bailout(fd, "");
		break;

	default:
		printf("spin: bad node type %d (.m)\n", now->ntyp);
		alldone(1);
	}
}

void
putname(FILE *fd, char *pre, Lextok *n, int m, char *suff) /* varref */
{	Symbol *s = n->sym;
	lineno = n->ln; Fname = n->fn;

	if (!s)
		fatal("no name - putname", "");
	if (s->context && context && s->type)
		s = findloc(s);		/* it's a local var */

	if (!s)
	{	fprintf(fd, "%s%s%s", pre, n->sym->name, suff);
		return;
	}

	if (!s->type)	/* not a local name */
		s = lookup(s->name);	/* must be a global */

	if (!s->type)
	{	if (strcmp(pre, ".") != 0)
		non_fatal("undeclared variable '%s'", s->name);
		s->type = SPIN_INT;
	}

	if (s->type == PROCTYPE)
		fatal("proctype-name '%s' used as array-name", s->name);

	fprintf(fd, pre);
	if (!terse && !s->owner)
	{	if (s->context
		||  strcmp(s->name, "_p") == 0
		||  strcmp(s->name, "_pid") == 0)
		{	fprintf(fd, "((P%d *)thisseq)->", Pid);
		} else
		{	int x = strcmp(s->name, "_");
			if (!(s->hidden&1) && x != 0)
				fprintf(fd, "now.");
			if (x == 0 && _isok == 0)
				fatal("attempt to read value of '_'", 0);
	}	}

	if (withprocname
	&&  s->context
	&&  strcmp(pre, "."))
		fprintf(fd, "%s:", s->context->name);

	fprintf(fd, "%s", s->name);

	if (s->nel != 1)
	{	if (no_arrays)
		{
		non_fatal("ref to array element invalid in this context",
			(char *)0);
		printf("\thint: instead of, e.g., x[rs] qu[3], use\n");
		printf("\tchan nm_3 = qu[3]; x[rs] nm_3;\n");
		printf("\tand use nm_3 in sends/recvs instead of qu[3]\n");
		}
		/* an xr or xs reference to an array element
		 * becomes an exclusion tag on the array itself -
		 * which could result in invalidly labeling
		 * operations on other elements of this array to
		 * be also safe under the partial order reduction
		 * (see procedure has_global())
		 */
		if (terse
		|| (n->lft
		&&  n->lft->ntyp == CONST
		&&  n->lft->val < s->nel)
		|| (!n->lft && s->nel > 0))
		{	cat3("[", n->lft, "]");
		} else
		{	cat3("[ Index(", n->lft, ", ");
			fprintf(fd, "%d) ]", s->nel);
		}
	}
	if (s->type == STRUCT && n->rgt && n->rgt->lft)
	{	putname(fd, ".", n->rgt->lft, m, "");
	}
	fprintf(fd, suff);
}

void
putremote(FILE *fd, Lextok *n, int m)	/* remote reference */
{	int promoted = 0;

	if (terse)
	{	fprintf(fd, "%s[", n->lft->sym->name);
		putstmnt(fd, n->lft->lft, m);
		fprintf(fd, "].%s", n->sym->name);
	} else
	{	if (Sym_typ(n) < SHORT)
		{	promoted = 1;
			fprintf(fd, "((int)");
		}
		fprintf(fd, "((P%d *)Pptr(",
			fproc(n->lft->sym->name));
		fprintf(fd, "BASE+");
		putstmnt(fd, n->lft->lft, m);
		fprintf(fd, "))->%s", n->sym->name);
	}
	if (n->rgt)
	{	fprintf(fd, "[");
		putstmnt(fd, n->rgt, m);
		fprintf(fd, "]");
	}
	if (promoted) fprintf(fd, ")");
}

static int
getweight(Lextok *n)
{
	switch (n->ntyp) {
	case 'r':     return 4;
	case 's':     return 2;
	case TIMEOUT: return 1;	/* lowest priority */
	case 'c':     if (has_typ(n->lft, TIMEOUT)) return 1;
	}
	return 3;
}

int
has_typ(Lextok *n, int m)
{
	if (!n) return 0;
	if (n->ntyp == m) return 1;
	return (has_typ(n->lft, m) || has_typ(n->rgt, m));
}
