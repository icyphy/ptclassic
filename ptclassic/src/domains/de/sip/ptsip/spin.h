/***** spin: spin.h *****/

/* Copyright (c) 1991-1997 by Lucent Technologies - Bell Laboratories     */
/* All Rights Reserved.  This software is for educational purposes only.  */
/* Permission is given to distribute this code provided that this intro-  */
/* ductory message is not removed and no monies are exchanged.            */
/* No guarantee is expressed or implied by the distribution of this code. */
/* Software written by Gerard J. Holzmann as part of the book:            */
/* `Design and Validation of Computer Protocols,' ISBN 0-13-539925-4,     */
/* Prentice Hall, Englewood Cliffs, NJ, 07632.                            */
/* Send bug-reports and/or questions to: gerard@research.bell-labs.com    */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifndef PC
#include <memory.h>
#endif

class DESiPStar;

typedef struct Lextok {
	unsigned short	ntyp;	/* node type */
	short	ismtyp;		/* CONST derived from MTYP */
	int	val;		/* value attribute */
	double	fval;		/* value attribute */
	int	ln;		/* line number */
	int	indstep;	/* part of d_step sequence */
	struct Symbol	*fn;	/* file name */
	struct Symbol	*sym;	/* symbol reference */
        struct Sequence *sq;	/* sequence */
        struct SeqList	*sl;	/* sequence list */
	struct Lextok	*lft, *rgt; /* children in parse tree */
} Lextok;

typedef struct Symbol {
	char	*name;
	int	Nid;		/* unique number for the name */
	short	type;		/* bit,short,.., chan,struct  */
	unsigned char	timer;	/* flag indicates if a timer var */
	unsigned char	present;/* flag indicates if new data present */
	unsigned char	updated;/* flag indicates if data updated */
	double		*togo;  /* duration to expire */
	unsigned char	islocal;/* flag indicates if is a local var */
	unsigned char	hidden;	/* bit-flags: 1,2,4 hide,show,bit-equiv */
	unsigned char	colnr;	/* for use with xspin */
	int	nbits;		/* an optional width specifier */
	int	nel;		/* 1 if scalar, >1 if array   */
	int	setat;		/* last depth value changed   */
	int	*val;		/* runtime value(s), initl 0  */
	struct Lextok	**Sval;	/* values for structures */

	int	xu;		/* exclusive r or w by 1 pid  */
	struct Symbol	*xup[2];  /* xr or xs proctype  */

	struct Lextok	*ini;	/* initial value, or chan-def */
	struct Lextok	*Slst;	/* template for structure if struct */
	struct Symbol	*Snm;	/* name of the defining struct */
	struct Symbol	*owner;	/* set for names of subfields in typedefs */
	struct Symbol	*context; /* 0 if global, or procname */
	struct Symbol	*next;	/* linked list */
} Symbol;

typedef struct Ordered {	/* links all names in Symbol table */ 
	struct Symbol	*entry;
	struct Ordered	*next;
} Ordered;

typedef struct SpinQueue {
	short	qid;		/* runtime q index */
	short	qlen;		/* nr messages stored */
	short	nslots, nflds;	/* capacity, flds/slot */
	int	setat;		/* last depth value changed */
	int	*fld_width;	/* type of each field */
	int	*contents;	/* the values stored */
	int	*stepnr;	/* depth when each msg was sent */
	Symbol	*sym;		/* corresponding symbol in symtab */
	struct SpinQueue	*nxt;	/* linked list */
} SpinQueue;

typedef struct Element {
	Lextok	*n;		/* defines the type & contents */
	int	Seqno;		/* identifies this el within system */
	int	seqno;		/* identifies this el within a proc */
	unsigned char	status;	/* used by analyzer generator  */
	struct SeqList	*sub;	/* subsequences, for compounds */
	struct SeqList	*esc;	/* zero or more escape sequences */
	struct Element	*Nxt;	/* linked list - for global lookup */
	struct Element	*nxt;	/* linked list - program structure */
} Element;

typedef struct Sequence {
	Element	*frst;
	Element	*last;		/* links onto continuations */
	Element *extent;	/* last element in original */
	int	maxel;		/* 1+largest id in sequence */
} Sequence;

typedef struct SeqList {
	Sequence	*thisseq;	/* one sequence */
	struct SeqList	*nxt;	/* linked list  */
} SeqList;

typedef struct Label {
	Symbol	*s;
	Symbol	*c;
	Element	*e;
	struct Label	*nxt;
} Label;

typedef struct Lbreak {
	Symbol	*l;
	struct Lbreak	*nxt;
} Lbreak;

typedef struct RunList {
	Symbol	*n;		/* name            */
	int	tn;		/* ordinal of type */
	int	pid;		/* process id      */
	int	priority;	/* for simulations only */
	unsigned char waitExtOp;
	double	sysTime;
	double	idlTime;
	unsigned char suspend;	/* if suspended now */
	Element	*pc;		/* current stmnt   */
	Sequence *ps;		/* used by analyzer generator */
	Lextok	*prov;		/* provided clause */
	Symbol	*symtab;	/* local variables */
	struct RunList	*nxt;	/* linked list */
	DESiPStar *host;	/* pointer to host star */
} RunList;

typedef struct ProcList {
	Symbol	*n;		/* name       */
	Lextok	*p;		/* parameters */
	Sequence *s;		/* body       */
	Lextok	*prov;		/* provided clause */
	short	tn;		/* ordinal number */
	short	det;		/* deterministic */
	struct ProcList	*nxt;	/* linked list */
} ProcList;

typedef	Lextok *Lexptr;

typedef struct memList {
	char *entry;
	struct memList *next;
} memList;

#define YYSTYPE	Lexptr

#define ZN	(Lextok *)0
#define ZS	(Symbol *)0
#define ZE	(Element *)0

#define DONE	  1     	/* status bits of elements */
#define ATOM	  2     	/* part of an atomic chain */
#define L_ATOM	  4     	/* last element in a chain */
#define I_GLOB    8		/* inherited global ref    */
#define DONE2	 16		/* used in putcode and main*/
#define D_ATOM	 32		/* deterministic atomic    */
#define CHECK1	 64
#define CHECK2	128

#define Nhash	255    		/* slots in symbol hash-table */

#define XR	  1		/* non-shared receive-only */
#define XS	  2		/* non-shared send-only    */
#define XX	  4		/* overrides XR or XS tag  */

#define PREDEF	  3		/* predefined names: _p, _last */
#define UNSIGNED  5		/* val defines width in bits */
#define BIT	  1		/* also equal to width in bits */
#define BYTE	  8		/* ditto */
#define SHORT	 16		/* ditto */
#define SPIN_INT 32		/* ditto */
#define TIMER	 	33	/* ditto */
#define SPIN_DOUBLE	34	/* ditto */
#define	CHAN	 64		/* not */
#define STRUCT	128		/* user defined structure name */

#define N_A_N	1e50

#ifndef max
#define max(a,b) (((a)<(b)) ? (b) : (a))
#endif

/***** prototype definitions *****/
Element	*eval_sub(Element *);
Element	*get_lab(Lextok *, int);
Element	*huntele(Element *, int);
Element	*huntstart(Element *);
Element	*target(Element *);

Lextok	*do_unless(Lextok *, Lextok *);
Lextok	*expand(Lextok *, int);
Lextok	*getuname(Symbol *);
Lextok	*mk_explicit(Lextok *, int, int);
Lextok	*nn(Lextok *, int, Lextok *, Lextok *);
Lextok	*rem_lab(Symbol *, Lextok *, Symbol *);
Lextok	*tail_add(Lextok *, Lextok *);

ProcList *ready(Symbol *, Lextok *, Sequence *, int, Lextok *);

SeqList	*seqlist(Sequence *, SeqList *);
Sequence *close_seq(int);

Symbol	*break_dest(void);
Symbol	*findloc(Symbol *);
Symbol	*lookup(char *);

char	*emalloc(int);
long	Rand(void);

int	any_oper(Lextok *, int);
int	any_undo(Lextok *);
int	cast_val(int, int);
int	checkvar(Symbol *, int);
int	Cnt_flds(Lextok *);
int	cnt_mpars(Lextok *);
int	complete_rendez(void);
int	enable(Lextok *);
int	Enabled0(Element *);
int	eval(Lextok *, double *);
int	fill_struct(Symbol *, int, int, char *, Symbol **, int);
int	find_lab(Symbol *, Symbol *);
int	find_maxel(Symbol *);
int	full_name(FILE *, Lextok *, Symbol *, int);
int	getlocal(Lextok *, Symbol *);
double	getlocal_double(Lextok *, Symbol *);
int	getval(Lextok *, Symbol *);
int	has_typ(Lextok *, int);
int	ismtype(char *);
int	isproctype(char *);
int	isutype(char *);
int	Lval_struct(Lextok *, Symbol *, int, int);
int	pc_value(Lextok *);
int	proper_enabler(Lextok *);
int	putcode(FILE *, Sequence *, Element *, int, int);
int	q_is_sync(Lextok *);
int	qlen(Lextok *);
int	qfull(Lextok *);
int	qmake(Symbol *);
int	qrecv(Lextok *, int);
int	qsend(Lextok *);
int	remotelab(Lextok *);
int	remotevar(Lextok *);
int	Rval_struct(Lextok *, Symbol *, int);
int	setlocal(Lextok *, int, Symbol *);
int	setlocal_double(Lextok *, double, Symbol *);
int	setval(Lextok *, int, Symbol *);
int	sputtype(char *, int);
int	Sym_typ(Lextok *);
int	tl_main(int, char *[]);
int	Width_set(int *, int, Lextok *);
int	yyparse(void);
int	yywrap(void);
int	yylex(void);

void	add_seq(Lextok *);
void	alldone(int);
void	announce(char *);
void	checkrun(Symbol *, int);
void	comment(FILE *, Lextok *, int);
void	cross_dsteps(Lextok *, Lextok *);
void	doq(Symbol *, int, RunList *);
void	dotag(char *);
void	dump_struct(Symbol *, char *, RunList *);
void	dumpclaims(FILE *, int, char *);
void	dumpglobals(void);
void	dumplabels(void);
void	dumplocal(RunList *);
void	dumpsrc(int, int);
/* void	exit(int); */
void	fatal(char *, char *);
void	fix_dest(Symbol *, Symbol *);
void	genaddproc(void);
void	genaddqueue(void);
void	genheader(void);
void	genother(void);
void	gensrc(void);
void	gensvmap(void);
void	genunio(void);
void	ini_struct(Symbol *);
void	make_atomic(Sequence *, int);
void	match_trail(void);
void	nochan_manip(Lextok *, int);
void	non_fatal(char *, char *);
void	ntimes(FILE *, int, int, char *c[]);
void	open_seq(int);
void	p_talk(Element *, int);
void	prehint(Symbol *);
void	pstext(int, char *);
void	pushbreak(void);
void	putname(FILE *, char *, Lextok *, int, char *);
void	putremote(FILE *, Lextok *, int);
void	putskip(int);
void	putsrc(int, int);
void	putstmnt(FILE *, Lextok *, int);
void	putunames(FILE *);
void	rem_Seq(void);
void	runnable(ProcList *, int, int);
double	sched(int);
void	setmtype(Lextok *);
void	setpname(Lextok *);
void	setptype(Lextok *, int, Lextok *);
void	setuname(Lextok *);
void	setutype(Lextok *, Symbol *, Lextok *);
void	setxus(Lextok *, int);
void	Srand(unsigned);
void	start_claim(int);
void	struct_name(Lextok *, Symbol *, int, char *);
void	symdump(void);
void	trackvar(Lextok *, Lextok *);
void	trackrun(Lextok *);
void	typ2c(Symbol *);
void	typ_ck(int, int, char *);
void	undostmnt(Lextok *, int);
void	unrem_Seq(void);
void	unskip(int);
void	varcheck(Element *, Element *);
void	whoruns(int);
void	wrapup(int);
void	yyerror(char *, ...);
