/***** spin: guided.c *****/

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
#include <sys/types.h>
#include <sys/stat.h>
#ifdef PC
#include "y_tab.h"
#else
#include "y.tab.h"
#endif

extern RunList	*spin_run, *X;
extern Element	*Al_El;
extern Symbol	*Fname, *oFname;
extern int	verbose, lineno, xspin, jumpsteps, depth;
extern int	nproc, nstop, Tval, Rvous, m_loss, ntrail, columns;
extern short	Skip_claim;

int	TstOnly = 0, pno;

static FILE	*fd;
static void	lost_trail(void);

static void
whichproc(int p)
{	RunList *oX;

	for (oX = spin_run; oX; oX = oX->nxt)
		if (oX->pid == p)
		{	printf("(%s) ", oX->n->name);
			break;
		}
}

static int
newer(char *f1, char *f2)
{	struct stat x, y;

	if (stat(f1, (struct stat *)&x) < 0) return 0;
	if (stat(f2, (struct stat *)&y) < 0) return 1;
	if (x.st_mtime < y.st_mtime) return 0;
	
	return 1;
}

void
match_trail(void)
{	int i, nst;
	Element *dothis;
	RunList *oX;
	char snap[256];
	double ff;

	if (ntrail)
		sprintf(snap, "%s%d.trail", oFname->name, ntrail);
	else
		sprintf(snap, "%s.trail", oFname->name);
	if (!(fd = fopen(snap, "r")))
	{	printf("spin: cannot find file %s\n", snap);
		alldone(1);
	}
			
	if (xspin == 0 && newer(oFname->name, snap))
	printf("spin: warning, \"%s\" is newer than %s\n",
		oFname->name, snap);

	Tval = m_loss = 1; /* timeouts and losses may be part of trail */

	while (fscanf(fd, "%d:%d:%d\n", &depth, &pno, &nst) == 3)
	{	if (depth == -2) { start_claim(pno); continue; }
		if (depth == -1)
		{	if (verbose)
			{	if (columns == 2)
				dotag(" CYCLE>\n");
				else
				dotag("<<<<<START OF CYCLE>>>>>\n");
			}
			continue;
		}
		if (Skip_claim && pno == 0) continue;

		for (dothis = Al_El; dothis; dothis = dothis->Nxt)
		{	if (dothis->Seqno == nst)
				break;
		}
		if (!dothis)
		{	printf("%3d: proc %d, no matching stmnt %d\n",
				depth, pno, nst);
			lost_trail();
		}
		i = nproc - nstop + Skip_claim;
		if (dothis->n->ntyp == '@')
		{	if (pno == i-1)
			{	spin_run = spin_run->nxt;
				nstop++;
				if (verbose&4)
				{	if (columns == 2)
					{	dotag("<end>");
						continue;
					}
					printf("%3d: proc %d terminates\n",
						depth, pno);
				}
				continue;
			}
			if (pno <= 1) continue;	/* init dies before never */
			printf("%3d: stop error, ", depth);
			printf("proc %d (i=%d) trans %d, %c\n",
				pno, i, nst, dothis->n->ntyp);
			lost_trail();
		}
		for (X = spin_run; X; X = X->nxt)
		{	if (--i == pno)
				break;
		}
		if (!X)
		{	printf("%3d: no process %d ", depth, pno);
			printf("(state %d)\n", nst);
			lost_trail();
		}
		X->pc  = dothis;
		lineno = dothis->n->ln;
		Fname  = dothis->n->fn;

		oX = X;	/* a rendezvous could change it */
		if (dothis->n->ntyp == GOTO)
			X->pc = get_lab(dothis->n,1);
		else
		{	if (dothis->n->ntyp == D_STEP)
			{	Element *g = dothis;
				do {
					g = eval_sub(g);
				} while (g && g != dothis->nxt);
				if (!g)
				{	printf("%3d: d_step ", depth);
					printf("failed %d->%d\n",
					dothis->Seqno, dothis->nxt->Seqno);
					wrapup(1);
				}
				X->pc = g;
			} else
			{	(void) eval(dothis->n,&ff);
				X->pc = dothis->nxt;
			}
		}

		if (depth >= jumpsteps
		&& (verbose&32 || verbose&4))
		{	if (X == oX && columns != 2)
			{	p_talk(dothis, 1);
				printf("	[");

				if (dothis->n->ntyp == D_STEP)
				dothis = dothis->n->sl->thisseq->frst;

				comment(stdout, dothis->n, 0);
				printf("]\n");
			}
			if (verbose&1) dumpglobals();
			if (verbose&2) dumplocal(X);
			if (xspin) printf("\n"); /* xspin looks for these */
		}

		if (X->pc) X->pc = huntele(X->pc, 0);
	}
	printf("spin: trail ends after %d steps\n", depth);
	wrapup(0);
}

static void
lost_trail(void)
{	int d, p, n, l;

	while (fscanf(fd, "%d:%d:%d:%d\n", &d, &p, &n, &l) == 4)
	{	printf("step %d: proc  %d ", d, p); whichproc(p);
		printf("(state %d) - d %d\n", n, l);
	}
	wrapup(1);	/* no return */
}

int
pc_value(Lextok *n)
{	int i = nproc - nstop;
	int pid;
	double ff;

	pid = eval(n,&ff);
	RunList *Y;

	for (Y = spin_run; Y; Y = Y->nxt)
	{	if (--i == pid)
			return Y->pc->seqno;
	}
	return 0;
}
