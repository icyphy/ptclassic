/******************************************************************
Version identification:
$Id$

Copyright (c) 1997-%Q% The Regents of the University of California.
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

 Programmer:  Shang-Pin Chang
 Date of creation: 11/2/97 

 SPIN evaluator

*/
/***** spin: DESiPrun.cc *****/


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
#include "DESiPStar.h"

extern RunList *X, *spin_run;
extern Symbol *Fname;
extern Element *LastStep;
extern int Rvous, lineno, Tval, interactive, MadeChoice;
extern int TstOnly, verbose, s_trail, xspin, jumpsteps, depth;
extern int nproc, nstop, no_print;
extern double stopedInterval;

extern Symbol *has_lab(Element *);

long Seed = 1;
int E_Check = 0;

static int eval_sync(Element *);
static int interprint(Lextok *);
static int pc_enabled(Lextok * n);

void
Srand(unsigned int s)
{
  Seed = s;
}

long
Rand(void)
{				/* CACM 31(10), Oct 1988 */
  Seed = 16807 * (Seed % 127773) - 2836 * (Seed / 127773);
  if (Seed <= 0)
    Seed += 2147483647;
  return Seed;
}

Element *
eval_sub(Element * e)
{
  Element *f, *g;
  SeqList *z;
  int i, j, k;
  double ff;

  if (!e->n)
    return ZE;
#ifdef DEBUG
  printf("\n\teval_sub(%d %s) ",
	 e->Seqno, e->esc ? "+esc" : "");
  comment(stdout, e->n, 0);
  printf("\n");
#endif
  if (e->n->ntyp == GOTO) {
    if (Rvous)
      return ZE;
    f = get_lab(e->n, 1);
    cross_dsteps(e->n, f->n);
    return f;
  }
  if (e->n->ntyp == UNLESS) {	/* escapes were distributed into sequence */
    return eval_sub(e->sub->thisseq->frst);
  }
  else if (e->sub) {		/* true for IF, DO, and UNLESS */
    Element *has_else = ZE;

    if (interactive
	&& !MadeChoice && !E_Check
	&& depth >= jumpsteps) {
      printf("Select stmnt (");
      whoruns(0);
      printf(")\n");
      if (nproc - nstop > 1)
	printf("\tchoice 0: other process\n");
    }
    for (z = e->sub, j = 0; z; z = z->nxt) {
      j++;
      if (interactive
	  && !MadeChoice && !E_Check
	  && depth >= jumpsteps
	  && z->thisseq->frst
	  && (xspin || (verbose & 32) || Enabled0(z->thisseq->frst))) {
	printf("\tchoice %d: ", j);
	if (!Enabled0(z->thisseq->frst))
	  printf("unexecutable, ");
	comment(stdout, z->thisseq->frst->n, 0);
	printf("\n");
      }
    }
    if (interactive && depth >= jumpsteps && !E_Check) {
      if (!MadeChoice) {
	char buf[256];
	if (xspin)
	  printf("Make Selection %d\n\n", j);
	else
	  printf("Select [0-%d]: ", j);
	fflush(stdout);
	scanf("%s", buf);
	if (isdigit(buf[0]))
	  k = atoi(buf);
	else {
	  if (buf[0] == 'q')
	    alldone(0);
	  k = -1;
	}
      }
      else {
	k = MadeChoice;
	MadeChoice = 0;
      }
      if (k < 1 || k > j) {
	if (k != 0)
	  printf("\tchoice outside range\n");
	return ZE;
      }
      k--;
    }
    else {
      if (e->n && e->n->indstep >= 0)
	k = 0;			/* select 1st executable guard */
      else
	k = Rand() % j;		/* nondeterminism */
    }
    for (i = 0, z = e->sub; i < j + k; i++) {
      if (z->thisseq->frst
	  && z->thisseq->frst->n->ntyp == ELSE) {
	has_else = (Rvous) ? ZE : z->thisseq->frst->nxt;
	if (!interactive || depth < jumpsteps) {
	  z = (z->nxt) ? z->nxt : e->sub;
	  continue;
	}
      }
      if (i >= k) {
	if ((f = eval_sub(z->thisseq->frst)))
	  return f;
	else if (interactive && depth >= jumpsteps) {
	  if (!E_Check)
	    printf("\tunexecutable\n");
	  return ZE;
	}
      }
      z = (z->nxt) ? z->nxt : e->sub;
    }
    LastStep = z->thisseq->frst;
    return has_else;
  }
  else {
    if (e->n->ntyp == ATOMIC
	|| e->n->ntyp == D_STEP) {
      f = e->n->sl->thisseq->frst;
      g = e->n->sl->thisseq->last;
      g->nxt = e->nxt;
      if (!(g = eval_sub(f)))	/* atomic guard */
	return ZE;
      return g;
    }
    else if (e->n->ntyp == NON_ATOMIC) {
      f = e->n->sl->thisseq->frst;
      g = e->n->sl->thisseq->last;
      g->nxt = e->nxt;		/* close it */
      return eval_sub(f);
    }
    else if (!Rvous && e->n->ntyp == '.') {
      return e->nxt;
    }
    else {
      SeqList *x;
      if (!(e->status & D_ATOM) && e->esc && verbose & 32) {
	printf("Stmnt [");
	comment(stdout, e->n, 0);
	printf("] has escape(s): ");
	for (x = e->esc; x; x = x->nxt) {
	  printf("[");
	  g = x->thisseq->frst;
	  if (g->n->ntyp == ATOMIC
	      || g->n->ntyp == NON_ATOMIC)
	    g = g->n->sl->thisseq->frst;
	  comment(stdout, g->n, 0);
	  printf("] ");
	}
	printf("\n");
      }
      for (x = e->esc; x; x = x->nxt) {
	if ((g = eval_sub(x->thisseq->frst))) {
	  if (verbose & 4)
	    printf("\tEscape taken\n");
	  return g;
	}
      }
      switch (e->n->ntyp) {
      case DELAY:
      case RETURN:
      case TURNOFF:
      case TIMEOUT:
      case RUN:
      case EXTOPER:
      case PRINT:
      case ASGN:
      case ASSERT:
      case 's':
      case 'r':
      case 'c':
	/* toplevel statements only */
	LastStep = e;
      default:
	break;
      }
      if (Rvous) {
	return (eval_sync(e)) ? e->nxt : ZE;
      }
      return (eval(e->n, &ff)) ? e->nxt : ZE;
    }
  }
  return ZE;			/* not reached */
}

static int
eval_sync(Element * e)
{				/* allow only synchronous receives
				 * and related node types    */
  Lextok *now = (e) ? e->n : ZN;
  double ff;

  if (!now
      || now->ntyp != 'r'
      || now->val >= 2		/* no rv with a poll */
      || !q_is_sync(now)) {
    return 0;
  }

  LastStep = e;
  return eval(now, &ff);
}

static int
assign(Lextok * now)
{
  int t;
  Symbol *sp;
  double ff;

  if (TstOnly)
    return 1;

  switch (now->rgt->ntyp) {
  case FULL:
  case NFULL:
  case EMPTY:
  case NEMPTY:
  case RUN:
  case LEN:
    t = BYTE;
    break;
  default:
    t = Sym_typ(now->rgt);
    break;
  }
  typ_ck(Sym_typ(now->lft), t, "assignment");
  if (now->lft->sym->context && now->lft->sym->type) {
    sp = findloc(now->lft->sym);
    t = eval(now->rgt, &ff);
    if ((sp->type != SPIN_DOUBLE) && (sp->type != TIMER))
      return setval(now->lft, ((ff == N_A_N) ? t : (int) (ff + 0.5)), sp);
    else
      return setlocal_double(now->lft, ((ff != N_A_N) ? ff : (double) t), sp);
  }
  else
    return setval(now->lft, eval(now->rgt, &ff), NULL);
}

static int
nonprogress(void)
{				/* np_ */
  RunList *r;
  Symbol *z;
  for (r = spin_run; r; r = r->nxt) {
    z = has_lab(r->pc);
    if (z && strncmp(z->name, "progress", 8))
      return 0;
  }
  return 1;
}

int
eval(Lextok * now, double *fp)
{
  int i, val1, val2;
  double ff, ff1, ff2;
  Symbol *sp;
  Lextok *tp;

  if (now) {
    lineno = now->ln;
    Fname = now->fn;
#ifdef DEBUG
    printf("eval ");
    comment(stdout, now, 0);
    printf(" (type is %d)\n", now->ntyp);
#endif
    switch (now->ntyp) {
    case CONST:
      if (now->fval == N_A_N) {
	*fp = N_A_N;
	return now->val;
      }
      else {
	*fp = now->fval;
	return 1;
      }
    case '!':
      *fp = N_A_N;
      return !eval(now->lft, &ff);
    case UMIN:
      val1 = eval(now->lft, &ff);
      if (ff == N_A_N)
	return (-val1);
      else {
	*fp = -ff;
	return 1;
      }
    case '~':
      *fp = N_A_N;
      return ~eval(now->lft, &ff);
    case '/':
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      if ((ff1 == N_A_N) && (ff2 == N_A_N)) {
	*fp = N_A_N;
	return (val1 / val2);
      }
      else {
	ff1 = (double) ((ff1 == N_A_N) ? val1 : ff1);
	ff2 = (double) ((ff2 == N_A_N) ? val2 : ff2);
	*fp = ff1 / ff2;
	return 1;
      }
    case '*':
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      if ((ff1 == N_A_N) && (ff2 == N_A_N)) {
	*fp = N_A_N;
	return (val1 * val2);
      }
      else {
	ff1 = (double) ((ff1 == N_A_N) ? val1 : ff1);
	ff2 = (double) ((ff2 == N_A_N) ? val2 : ff2);
	*fp = ff1 * ff2;
	return 1;
      }
      *fp = ff1 * ff2;
      return (val1 * val2);
    case '-':
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      if ((ff1 == N_A_N) && (ff2 == N_A_N)) {
	*fp = N_A_N;
	return (val1 - val2);
      }
      else {
	ff1 = (double) ((ff1 == N_A_N) ? val1 : ff1);
	ff2 = (double) ((ff2 == N_A_N) ? val2 : ff2);
	*fp = ff1 - ff2;
	return 1;
      }
    case '+':
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      if ((ff1 == N_A_N) && (ff2 == N_A_N)) {
	*fp = N_A_N;
	return (val1 + val2);
      }
      else {
	ff1 = (double) ((ff1 == N_A_N) ? val1 : ff1);
	ff2 = (double) ((ff2 == N_A_N) ? val2 : ff2);
	*fp = ff1 + ff2;
	return 1;
      }
    case '%':
      *fp = N_A_N;
      return (eval(now->lft, &ff) % eval(now->rgt, &ff));
    case LT:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      return (((ff1 == N_A_N) ? val1 : ff1) < ((ff2 == N_A_N) ? val2 : ff2));
    case GT:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      return (((ff1 == N_A_N) ? val1 : ff1) > ((ff2 == N_A_N) ? val2 : ff2));
    case '&':
      *fp = N_A_N;
      return (eval(now->lft, &ff) & eval(now->rgt, &ff));
    case '^':
      *fp = N_A_N;
      return (eval(now->lft, &ff) ^ eval(now->rgt, &ff));
    case '|':
      *fp = N_A_N;
      return (eval(now->lft, &ff) | eval(now->rgt, &ff));
    case LE:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      return (((ff1 == N_A_N) ? val1 : ff1) <= ((ff2 == N_A_N) ? val2 : ff2));
    case GE:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      return (((ff1 == N_A_N) ? val1 : ff1) >= ((ff2 == N_A_N) ? val2 : ff2));
    case NE:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      return (((ff1 == N_A_N) ? val1 : ff1) != ((ff2 == N_A_N) ? val2 : ff2));
    case EQ:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff1);
      val2 = eval(now->rgt, &ff2);
      return (((ff1 == N_A_N) ? val1 : ff1) == ((ff2 == N_A_N) ? val2 : ff2));
    case OR:
      *fp = N_A_N;
      return (eval(now->lft, &ff) || eval(now->rgt, &ff));
    case AND:
      *fp = N_A_N;
      return (eval(now->lft, &ff) && eval(now->rgt, &ff));
    case LSHIFT:
      *fp = N_A_N;
      return (eval(now->lft, &ff) << eval(now->rgt, &ff));
    case RSHIFT:
      *fp = N_A_N;
      return (eval(now->lft, &ff) >> eval(now->rgt, &ff));
    case '?':
      *fp = N_A_N;
      i = eval(now->lft, &ff);
      val1 = eval(now->rgt->lft, &ff1);
      val2 = eval(now->rgt->rgt, &ff2);
      if (i) {
	if (ff1 == N_A_N)
	  return val1;
	else
	  *fp = ff1;
	return 1;
      }
      else {
	if (ff2 == N_A_N)
	  return val2;
	else
	  *fp = ff2;
	return 1;
      }
    case 'p':
      *fp = N_A_N;
      return remotevar(now);	/* only _p allowed */
    case 'q':
      *fp = N_A_N;
      return remotelab(now);
    case 'R':
      *fp = N_A_N;
      return qrecv(now, 0);	/* test only    */
    case LEN:
      *fp = N_A_N;
      return qlen(now);
    case FULL:
      *fp = N_A_N;
      return (qfull(now));
    case EMPTY:
      *fp = N_A_N;
      return (qlen(now) == 0);
    case NFULL:
      *fp = N_A_N;
      return (!qfull(now));
    case NEMPTY:
      *fp = N_A_N;
      return (qlen(now) > 0);
    case ENABLED:
      if (s_trail)
	return 1;
      *fp = N_A_N;
      return pc_enabled(now->lft);
    case EVAL:
      val1 = eval(now->lft, &ff);
      if (ff == N_A_N)
	return val1;
      else {
	*fp = ff;
	return 1;
      }
    case PC_VAL:
      *fp = N_A_N;
      return pc_value(now->lft);
    case NONPROGRESS:
      *fp = N_A_N;
      return nonprogress();
    case NAME:
      if (now->sym->context && now->sym->type) {
	sp = findloc(now->sym);
	if ((sp->type != SPIN_DOUBLE) && (sp->type != TIMER)) {
	  *fp = N_A_N;
	  return (getval(now, sp));
	}
	else {
	  *fp = getlocal_double(now, sp);
	  return 1;
	}
      }
      else {
	*fp = N_A_N;
	return getval(now, NULL);
      }
    case FUNC:
      *fp = N_A_N;
      for (tp = now->rgt, i = 0; tp != ZN; tp = tp->rgt)
	X->host->arglist[i++] = eval(tp->lft, &ff);
      return (X->host->func(now->lft->val / 1000, now->lft->val % 1000));
    case EXPIRE:
      *fp = N_A_N;
      i = eval(now->lft->lft, &ff);
      sp = findloc(now->lft->sym);
      ff1 = getlocal_double(now->lft, sp);
      val2 = eval(now->rgt, &ff2);
      ff = ((ff2 == N_A_N) ? (double) val2 : ff2) - ff1;
      if (ff > 0) {
	if (sp->togo[i] != ff) {
	  sp->togo[i] = ff;
	  stopedInterval = -(sp->togo[i]);
	}
	return 0;
      }
      else
	return 1;
    case PRESENT:
      *fp = N_A_N;
      return findloc(now->lft->sym)->present;
    case ADMIT:
      *fp = N_A_N;
      sp = findloc(now->lft->sym);
      val1 = sp->present;
      sp->present = 0;
      return val1;
    case TURNOFF:
      *fp = N_A_N;
      findloc(now->lft->sym)->present = 0;
      return 1;
    case EXTOPER:
      *fp = N_A_N;
      if (X->waitExtOp) {
	if (findloc(now->rgt->sym)->present) {
	  X->waitExtOp = 0;
	  return 1;
	}
	else
	  return 0;
      }
      else {
	findloc(now->rgt->sym)->present = 0;
	X->waitExtOp = 1;
	findloc(now->lft->sym)->updated = 1;
	return 0;
      }
    case DELAY:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff);
      ff = (double) ((ff == N_A_N) ? val1 : ff);
      if (ff > 0) {
	stopedInterval = ff;
	X->idlTime = X->sysTime + ff;
	X->suspend = 1;
      }
      return 1;
    case RETURN:
      *fp = N_A_N;
      val1 = eval(now->lft, &ff);
      ff = (double) ((ff == N_A_N) ? val1 : ff);
      if (ff >= 0) {
	stopedInterval = -ff;
	X->suspend = 1;
      }
      return 1;
    case TIMEOUT:
      *fp = N_A_N;
      return Tval;
    case RUN:
      *fp = N_A_N;
      return TstOnly ? 1 : enable(now);
    case 's':
      *fp = N_A_N;
      return qsend(now);	/* send         */
    case 'r':
      *fp = N_A_N;
      return qrecv(now, 1);	/* receive or poll */
    case 'c':
      *fp = N_A_N;
      return eval(now->lft, &ff);	/* condition    */
    case PRINT:
      *fp = N_A_N;
      return TstOnly ? 1 : interprint(now);
    case ASGN:
      *fp = N_A_N;
      return assign(now);
    case ASSERT:
      *fp = N_A_N;
      if (TstOnly || eval(now->lft, &ff))
	return 1;
      non_fatal("assertion violated", (char *) 0);
      if (s_trail)
	return 1;
      wrapup(1);		/* doesn't return */
    case IF:
    case DO:
    case BREAK:
    case UNLESS:		/* compound */
    case '.':
      *fp = N_A_N;
      return 1;			/* return label for compound */
    case '@':
      *fp = N_A_N;
      return 0;			/* stop state */
    case ELSE:
      *fp = N_A_N;
      return 1;			/* only hit here in guided trails */
    default:
      *fp = N_A_N;
      printf("spin: bad node type %d (run)\n", now->ntyp);
      fatal("aborting", 0);
    }
  }
  return 0;
}

static int
interprint(Lextok * n)
{
  Lextok *tmp = n->lft;
  char c, *s = n->sym->name;
  int i, j;
  char lbuf[16];
  double ff;
  extern char Buf[];

  Buf[0] = '\0';
  if (!no_print)
    if (!s_trail || depth >= jumpsteps) {
      for (i = 0; i < (int) strlen(s); i++)
	switch (s[i]) {
	case '\"':
	  break;		/* ignore */
	case '\\':
	  switch (s[++i]) {
	  case 't':
	    strcat(Buf, "\t");
	    break;
	  case 'n':
	    strcat(Buf, "\n");
	    break;
	  default:
	    goto onechar;
	  }
	  break;
	case '%':
	  if ((c = s[++i]) == '%') {
	    strcat(Buf, "%");	/* literal */
	    break;
	  }
	  if (!tmp) {
	    non_fatal("too few print args %s", s);
	    break;
	  }
	  j = eval(tmp->lft, &ff);
	  tmp = tmp->rgt;
	  switch (c) {
	  case 'c':
	    sprintf(lbuf, "%c", j);
	    break;
	  case 'd':
	    sprintf(lbuf, "%d", j);
	    break;
	  case 'o':
	    sprintf(lbuf, "%o", j);
	    break;
	  case 'u':
	    sprintf(lbuf, "%u", (unsigned) j);
	    break;
	  case 'x':
	    sprintf(lbuf, "%x", j);
	    break;
	  case 'f':
	    sprintf(lbuf, "%f", ff);
	    break;
	  default:
	    non_fatal("bad print cmd: '%s'", &s[i - 1]);
	    lbuf[0] = '\0';
	    break;
	  }
	  goto append;
	default:
	onechar:lbuf[0] = s[i];
	  lbuf[1] = '\0';
	append:strcat(Buf, lbuf);
	  break;
	}
      dotag(Buf);
    }
  return 1;
}

static int
Enabled1(Lextok * n)
{
  int i;
  int v = verbose;
  double ff;

  if (n)
    switch (n->ntyp) {
    case 'c':
      if (has_typ(n->lft, RUN))
	return 1;		/* conservative */
      /* else fall through */
    default:			/* side-effect free */
      verbose = 0;
      E_Check++;
      i = eval(n, &ff);
      E_Check--;
      verbose = v;
      return i;

    case PRINT:
    case ASGN:
    case ASSERT:
      return 1;

    case 's':
      if (q_is_sync(n)) {
	if (Rvous)
	  return 0;
	TstOnly = 1;
	verbose = 0;
	E_Check++;
	i = eval(n, &ff);
	E_Check--;
	TstOnly = 0;
	verbose = v;
	return i;
      }
      return (!qfull(n));
    case 'r':
      if (q_is_sync(n))
	return 0;		/* it's never a user-choice */
      n->ntyp = 'R';
      verbose = 0;
      E_Check++;
      i = eval(n, &ff);
      E_Check--;
      n->ntyp = 'r';
      verbose = v;
      return i;
    }
  return 0;
}

int
Enabled0(Element * e)
{
  SeqList *z;

  if (!e || !e->n)
    return 0;

  switch (e->n->ntyp) {
  case '@':
    return X->pid == (nproc - nstop - 1);
  case '.':
    return 1;
  case GOTO:
    if (Rvous)
      return 0;
    return 1;
  case UNLESS:
    return Enabled0(e->sub->thisseq->frst);
  case ATOMIC:
  case D_STEP:
  case NON_ATOMIC:
    return Enabled0(e->n->sl->thisseq->frst);
  }
  if (e->sub) {			/* true for IF, DO, and UNLESS */
    for (z = e->sub; z; z = z->nxt)
      if (Enabled0(z->thisseq->frst))
	return 1;
    return 0;
  }
  for (z = e->esc; z; z = z->nxt) {
    if (Enabled0(z->thisseq->frst))
      return 1;
  }
#if 0
  printf("enabled1 ");
  comment(stdout, e->n, 0);
  printf(" ==> %s\n", Enabled1(e->n) ? "yes" : "nope");
#endif
  return Enabled1(e->n);
}

int
pc_enabled(Lextok * n)
{
  int i = nproc - nstop;
  int pid;
  int result = 0;
  RunList *Y, *oX;
  double ff;

  pid = eval(n, &ff);
  for (Y = spin_run; Y; Y = Y->nxt)
    if (--i == pid) {
      oX = X;
      X = Y;
      result = Enabled0(Y->pc);
      X = oX;
      break;
    }
  return result;
}
