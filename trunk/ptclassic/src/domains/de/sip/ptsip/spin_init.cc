/******************************************************************
Version identification:
@(#)spin_init.c		1.00    07/26/98

Copyright (c) 1990-1998 The Regents of the University of California.
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

 Definition of spin_init (initialize variables belonged to spin.h scope).

*******************************************************************/

#include <malloc.h>
#include "spin.h"
#include "y.tab.h"

typedef struct SRC {
	short ln, st;
	struct SRC *nxt;
} SRC;

typedef struct UType {
	Symbol *nm;		/* name of the type */
	Lextok *cn;		/* contents */
	struct UType *nxt;	/* linked list */
} UType;


// extern int 	yy_yys[], *yys;
// extern YYSTYPE 	yy_yyv[], *yyv;

/* extern int	yymaxdepth; */
extern int	Level, GenCode, IsGuard, TestOnly;
extern int	Tj, Jt, LastGoto;
extern int	lineno;
extern unsigned char	in_comment;
extern Element *Al_El;
extern Label	*labtab;
extern int	Elcnt, DstepStart;
extern Lbreak	*breakstack;
extern SeqList	*cur_s;
extern int	Unique, break_id;
extern int	Stack_mx, Max_Red, Total;
extern int	Red_cnt;
extern int	Lab_cnt;
extern int	Base;
extern int	Stack_sz;
extern int	TstOnly;
extern int	Npars, u_sync, u_async;
extern Symbol	*LstSet;
extern int	acceptors, progressors, nBits;
extern int MH;		/* page height - can be scaled */
extern int oMH;		/* page height - not scaled */
extern int	pspno;	/* postscript page */
extern int	ldepth;
extern int	TotSteps; /* max nr of steps, about 40 pages */
extern float	Scaler;
extern long	Seed;
extern int	E_Check;
extern UType *Unames;
extern UType *Pnames;
extern int	tl_yychar;
extern int	maxcolnr;
extern memList *spin_memList;
extern int	Etimeouts;	/* nr timeouts in program */
extern int	Ntimeouts;	
extern int	analyze, columns, dumptab, has_remote;
extern int	interactive, jumpsteps, m_loss, nr_errs;
extern int	s_trail, ntrail, verbose, xspin, no_print;
extern int	limited_vis;
extern int	proc_started;
extern char	**add_ltl;
extern char	**ltl_file;
extern char	*PreProc;
extern char	PreArg[512];
extern int	OkBreak;
extern short	nocast;	/* to turn off casts in lvalues */
extern short	terse;	/* terse printing of varnames */
extern short	no_arrays;
extern short	has_last;	/* spec refers to _last */
extern short	has_badelse;	/* spec contains else combined with chan refs */
extern short	has_enabled;	/* spec contains enabled() */
extern short	has_pcvalue;	/* spec contains pc_value() */
extern short	has_np;	/* spec contains np_ */
extern short	has_sorted;	/* spec contains `!!' (sorted-send) operator */
extern short	has_random;	/* spec contains `??' (random-recv) operator */
extern short	has_xu;	/* spec contains xr or xs assertions */
extern short	has_unless;	/* spec contains unless statements */
extern short	has_provided;	/* spec contains PROVIDED clauses on procs */
extern int	mst;		/* max nr of state/process */
extern int	claimnr;	/* claim process, if any */
extern int	eventmapnr; /* event trace, if any */
extern int	uniq;
extern short	withprocname;	/* prefix local varnames with procname */
extern short	_isok;	/* checks usage of predefined varble _ */
extern RunList		*X;
extern RunList		*spin_run;
extern RunList		*LastX; /* previous executing proc */
extern ProcList		*rdy;
extern Element		*LastStep;
extern int		nproc, nstop, Tval;
extern int		Rvous, depth, nrRdy;
extern short		Have_claim, Skip_claim;
extern Ordered		*allglobal;
extern double		stopedInterval;
extern int	Priority_Sum;
extern Symbol	*context;
extern Ordered	*all_names;
extern int	Nid;
extern Ordered	*last_name;
extern Lextok   *runstmnts;
extern Lextok   *Xu_List;
extern Lextok   *Mtype;
extern int	newstates;	/* debugging only */
extern int	tl_errs;
extern int	tl_verbose;
extern int	tl_terse;
extern unsigned long	All_Mem;
extern int	hasuform, cnt;
extern int	Mpars;	/* max nr of message parameters  */
extern int	runsafe;	/* 1 if all run stmnts are in init */
extern int	Expand_Ok;
extern char	*claimproc;
extern char	*eventmap;
extern	int	Embedded, inEventMap, has_ini;
extern SpinQueue	*qtab;	/* linked list of queues */
extern int	nqs, firstrow;
extern Lextok	*n_rem;
extern SpinQueue	*q_rem;
extern SRC	*frst;
extern SRC	*skip;
extern Symbol *symtab_sym[];
extern Symbol *symtab_tl[];

extern void spin_init2(void);

/* Initialize all global variables for Ptolemy simulation */

void spin_init(void)
{
int i;
memList *ptr;
if (spin_memList)
{
while(spin_memList)
{
 if (spin_memList->entry) free(spin_memList->entry);
 ptr=spin_memList;
 spin_memList=spin_memList->next;
 if (ptr) free(ptr);
}
spin_memList=NULL;
for(i=0;i<=Nhash;i++) symtab_sym[i]=NULL;
for(i=0;i<=Nhash;i++) symtab_tl[i]=NULL;
spin_init2();
// yys = yy_yys;
// yyv = yy_yyv;
/* yymaxdepth = 150; */
X   = (RunList  *) 0;
spin_run = (RunList  *) 0;
LastX  = (RunList  *) 0; /* previous executing proc */
rdy = (ProcList *) 0;
LastStep = ZE;
nproc=0; nstop=0; Tval=0;
Rvous=0; depth=0; nrRdy=0;
Have_claim=0; Skip_claim=0;
allglobal=(Ordered *)0;
stopedInterval = 0.0;
Priority_Sum = 0;
context = ZS;
all_names = (Ordered *)0;
Nid = 0;
last_name = (Ordered *)0;
runstmnts = ZN;
Xu_List = (Lextok *) 0;
Mtype = (Lextok *) 0;
newstates  = 0;	/* debugging only */
tl_errs    = 0;
tl_verbose = 0;
tl_terse   = 0;
All_Mem = 0;
add_ltl  = (char **)0;
ltl_file = (char **)0;
PreProc = (char *)0;
PreArg[0]='\0';
hasuform=cnt=0;
Mpars = 0;	/* max nr of message parameters  */
runsafe = 1;	/* 1 if all run stmnts are in init */
Expand_Ok = 0;
claimproc = (char *) 0;
eventmap = (char *) 0;
Embedded = inEventMap =  has_ini = 0;
Etimeouts=0;	/* nr timeouts in program */
Ntimeouts=0;	/* nr timeouts in never claim */
analyze=0; columns=0; dumptab=0; has_remote=0;
interactive=0; jumpsteps=0; m_loss=0; nr_errs=0;
s_trail=0; ntrail=0; verbose=0; xspin=0; no_print=0;
limited_vis=0;
proc_started=0;
OkBreak = -1;
nocast=0;	/* to turn off casts in lvalues */
terse=0;	/* terse printing of varnames */
no_arrays=0;
has_last=0;	/* spec refers to _last */
has_badelse=0;	/* spec contains else combined with chan refs */
has_enabled=0;	/* spec contains enabled() */
has_pcvalue=0;	/* spec contains pc_value() */
has_np=0;	/* spec contains np_ */
has_sorted=0;	/* spec contains `!!' (sorted-send) operator */
has_random=0;	/* spec contains `??' (random-recv) operator */
has_xu=0;	/* spec contains xr or xs assertions */
has_unless=0;	/* spec contains unless statements */
has_provided=0;	/* spec contains PROVIDED clauses on procs */
mst=0;		/* max nr of state/process */
claimnr = -1;	/* claim process, if any */
eventmapnr = -1; /* event trace, if any */
uniq=1;
withprocname=0;	/* prefix local varnames with procname */
_isok=0;	/* checks usage of predefined varble _ */
MH  = 600;		/* page height - can be scaled */
oMH = 600;		/* page height - not scaled */
pspno = 0;	/* postscript page */
ldepth = 1;
TotSteps = 4096; /* max nr of steps, about 40 pages */
Scaler = (float) 1.0;
lineno=1;
in_comment=0;
Level=0; GenCode=0;
IsGuard=0; TestOnly=0;
Tj=0; Jt=0; LastGoto=0;
Stack_mx=0; Max_Red=0; Total=0;
Red_cnt  = 0;
Lab_cnt  = 0;
Base     = 0;
Stack_sz = 0;
Al_El = ZE;
labtab = (Label *) 0;
Elcnt=0;
DstepStart = -1;
breakstack = (Lbreak *) 0;
cur_s = (SeqList *) 0;
Unique=break_id=0;
qtab = (SpinQueue *) 0;	/* linked list of queues */
nqs = 0;
firstrow = 1;
n_rem = (Lextok *) 0;
q_rem = (SpinQueue  *) 0;
frst = (SRC *) 0;
skip = (SRC *) 0;
Seed = 1;
E_Check = 0;
Unames = 0;
Pnames = 0;
tl_yychar = 0;
maxcolnr = 1;
TstOnly = 0;
Npars=0; u_sync=0; u_async=0;
LstSet=ZS;
acceptors=0; progressors=0; nBits=0;
}
}
