/***** spin: main.c *****/

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
#include "spin.h"
#include "version.h"
#include <signal.h>
#include <malloc.h>
#include <time.h>
#ifdef PC
#include "y_tab.h"
#else
#include <unistd.h>
#include "y.tab.h"
#endif
#include "Error.h"

extern int	DstepStart, lineno;
extern FILE	*yyin, *yyout, *tl_out;
extern Symbol	*context;
extern char	*claimproc;

memList *spin_memList = (memList *)0;

Symbol	*Fname, *oFname;

int	Etimeouts=0;	/* nr timeouts in program */
int	Ntimeouts=0;	/* nr timeouts in never claim */
int	analyze=0, columns=0, dumptab=0, has_remote=0;
int	interactive=0, jumpsteps=0, m_loss=0, nr_errs=0;
int	s_trail=0, ntrail=0, verbose=0, xspin=0, no_print=0;
int	limited_vis=0;
int	proc_started=0;

/*
Element	*Same;
int	IsAsgn = 0, OrIsAsgn = 0;
*/
static char	Operator[] = "operator: ";
static char	Keyword[]  = "keyword: ";
static char	Function[] = "function-name: ";
char	**add_ltl  = (char **)0;
char	**ltl_file = (char **)0;
char	*PreProc = (char *)0;
char	PreArg[512] = "";
static void	explain(int);

#ifdef PC
	char out1[] = "_tmp1_";
#else
	char out1[255];
#endif

extern void spin_init(void);

void
alldone(int estatus)
{
	if (xspin && (analyze || s_trail))
	{	if (estatus)
			printf("spin: %d error(s) - aborting\n",
			estatus);
		else
			printf("Exit-Status 0\n");
	}
	exit(estatus);
}

#ifndef CPP
#ifdef PC
#define CPP	"cpp"		/*  or: "/gcc/bin/cpp"    */
				/* OS2: "icc /Pd+ /Q+"    */
#else
#ifdef NOCPP
#define CPP	"cat"

#else
#ifdef linux
#define CPP	"/usr/bin/cpp"

#else
#define CPP	"/usr/ccs/lib/cpp"
#endif
#endif
#endif
#endif

void
preprocess(char *a, char *b, int a_tmp)
{	char cmd[255];

	if (PreProc)
		sprintf(cmd, "%s%s %s > %s", PreProc, PreArg, a, b);
	else
{
		sprintf(cmd, "%s%s %s > %s", CPP, PreArg, a, b);
}
	if (system((const char *)cmd))
	{	printf("FATAL ERROR! SPIN FAILED IN PARSING!\n");
		(void) unlink((const char *) b);
		if (a_tmp) (void) unlink((const char *) a);
		alldone(1); /* failed; no return */
	}
	if (a_tmp) (void) unlink((const char *) a); 
}

void
cpyfile(char *src, char *tgt)
{	FILE *inp, *out;
	char buf[1024];

	inp = fopen(src, "r");
	out = fopen(tgt, "w");
	if (!inp || !out)
	{	printf("spin: cannot cp %s to %s\n", src, tgt);
		alldone(1);
	}
	while (fgets(buf, 1024, inp))
		fprintf(out, "%s\n", buf);
	fclose(inp);
	fclose(out);
}

void
spin_main(char *fname)
{
	static char *spinargs="-v";
	int argc,argi=0;
	char argv[3][255];
	Symbol *s;
	FILE *bugf;
	int T, preprocessonly;

	if ((bugf=fopen("/tmp/SPIN_DEBUG","r"))==NULL) {
	 argc = 2;
	 strcpy(argv[1],fname);
        } else {
	 argc = 3;
	 strcpy(argv[1],spinargs);
	 strcpy(argv[2],fname);
 	}	
        if (spin_memList) {
         spin_init();
	}

	preprocessonly = 0;
	T = (int) time((time_t *)0);
	proc_started = 0;
#ifdef PC
        char out1[] = "_tmp1_";
#else
        char out1[255];
/*        extern char *tmpnam(char *); 
        (void) tmpnam(out1);  */
	sprintf(out1,"%s_lex",fname); 
#endif
	yyin  = stdin;
	yyout = stdout;
	tl_out = stdout;

	while (argc > 1 && argv[argi+1][0] == '-')
	{	switch (argv[1][1]) {
		case 'a': analyze  =  1; break;
		case 'b': no_print = 1; break;
		case 'M': columns = 2; break;
		case 'c': columns = 1; break;
		case 'd': dumptab =  1; break;
		case 'f': add_ltl = (char **) argv;
			  argc--; argi++; break;
		case 'F': ltl_file = (char **) argv;
			  argc--; argi++; break;
		case 'g': verbose +=  1; break;
		case 'i': interactive = 1; break;
		case 'j': jumpsteps = atoi(&argv[argi+1][2]); break;
		case 'l': verbose +=  2; break;
		case 'm': m_loss   =  1; break;
		case 'n': T = atoi(&argv[argi+1][2]); break;
		case 'E': strcat(PreArg, " ");
			  strcat(PreArg, (char *) &argv[argi+1][2]);
			  break;
		case 'P': PreProc = (char *) &argv[argi+1][2]; break;
		case 'p': verbose +=  4; break;
		case 'r': verbose +=  8; break;
		case 's': verbose += 16; break;
		case 't': s_trail  =  1;
			  if (isdigit(argv[argi+1][2]))
				ntrail = atoi(&argv[argi+1][2]);
			  break;
		case 'v': verbose += 32; break;
		case 'V': printf("%s\n", Version);
			  alldone(0);
			  break;
		case 'X': xspin = 1;
#ifndef PC
			  signal(SIGPIPE, alldone); /* not posix... */
#endif
			  break;
		case 'Z': preprocessonly = 1; break;	/* used by xspin */
		case 'Y': limited_vis = 1; break;	/* used by xspin */

		default : printf("use: spin [-option] ... [-option] file\n");
			  printf("\tnote: file must be the last argument\n");
			  printf("\t-a generate a verifier in pan.c\n");
			  printf("\t-b don't execute printfs in simulation\n");
			  printf("\t-c columnated -s -r simulation output\n");
			  printf("\t-d produce symbol-table information\n");
			  printf("\t-f \"..formula..\"  translate LTL ");
				printf("into never claim\n");
			  printf("\t-F file  like -f, but with the LTL ");
				printf("formula stored in a 1-line file\n");
			  printf("\t-g print all global variables\n");
			  printf("\t-i interactive (random simulation)\n");
			  printf("\t-jN skip the first N steps ");
				printf("in simulation trail\n");
			  printf("\t-l print all local variables\n");
			  printf("\t-M print msc-flow in Postscript\n");
			  printf("\t-m lose msgs sent to full queues\n");
			  printf("\t-nN seed for random nr generator\n");
			  printf("\t-Eyyy add yyy to args for preprocessing\n");
			  printf("\t-Pxxx use xxx as the preprocessor\n");
			  printf("\t-p print all statements\n");
			  printf("\t-r print receive events\n");
			  printf("\t-s print send events\n");
			  printf("\t-v verbose, more warnings\n");
			  printf("\t-t[N] follow [Nth] simulation trail\n");
			  printf("\t-[XY] reserved for use by xspin interface\n");
			  printf("\t-V print version number and exit\n");
			  alldone(1);
		}
		argc--, argi++;
	}
	if (ltl_file)
	{	char formula[512];
		add_ltl = ltl_file; add_ltl[1][1] = 'f';
		ltl_file++; ltl_file++; /* skip to file arg */
		if (!(tl_out = fopen(*ltl_file, "r")))
		{	printf("spin: cannot open %s\n", *ltl_file);
			alldone(1);
		}
		fgets(formula, 512, tl_out);
		fclose(tl_out);
		tl_out = stdout;
		*ltl_file = (char *) formula;
	}
	if (argc > 1)
	{	char cmd[255], out2[255];
#ifdef PC
		strcpy(out2, "_tmp2_");
#else
		(void) tmpnam(out2);
#endif
		if (add_ltl)
		{	
			cpyfile(argv[argi+1], out2);
			if (!(tl_out = fopen(out2, "a")))
			{	printf("spin: cannot append to %s\n", out2);
				alldone(1);
			}
			nr_errs = tl_main(2, add_ltl);	/* in tl_main.c */
			fclose(tl_out);
			preprocess(out2, out1, 1);
		} else
			preprocess(argv[argi+1], out1, 0);
		if (preprocessonly)
		{	
			if (rename((const char *) out1, "pan.pre"))
			{	printf("spin: rename %s failed\n", out1);
				alldone(1);
			}
			alldone(0);
		}
		if (!(yyin = fopen(out1, "r")))
		{	printf("cannot open %s\n", out1);
			alldone(1);
		}

		if (strncmp(argv[argi+1], "progress", 8) == 0
		||  strncmp(argv[argi+1], "accept", 6) == 0)
			sprintf(cmd, "_%s", argv[argi+1]);
		else
			strcpy(cmd, argv[argi+1]);
		oFname = Fname = lookup(cmd);
		if (oFname->name[0] == '\"')
		{	int i = strlen(oFname->name);
			oFname->name[i-1] = '\0';
			oFname = lookup(&oFname->name[1]);
		}
	} else
	{	oFname = Fname = lookup("<stdin>");
		if (add_ltl)
		{	if (argc > 0)
				exit(tl_main(2, add_ltl));
			printf("spin: missing argument to -f\n");
			exit(1);
		}
		printf("%s\n", Version);
		printf("reading input from stdin:\n");
		fflush(stdout);
	}

	if (columns == 2)
	{	extern void putprelude(void);
		if (xspin || verbose&(1|4|8|16|32))
		{	printf("spin: -C precludes all flags except -t\n");
			exit(0);
		}
		putprelude();
	}
	if (columns && !(verbose&8) && !(verbose&16))
		verbose += (8+16);
	if (columns == 2 && limited_vis)
		verbose += (1+4);
	Srand(T);	/* defined in run.c */
	s = lookup("_");	s->type = PREDEF; /* a write only global var */
	s = lookup("_p");	s->type = PREDEF;
	s = lookup("_pid");	s->type = PREDEF;
	s = lookup("_last");	s->type = PREDEF;
	yyparse();
	fclose(yyin);
	(void) unlink((const char *)out1); 

/*	REMOVE PROMELA FILE HERE */
/*	(void) unlink((const char *)argv[argi+1]); */
	sched(0);
}

void
spin_done(int argc)
{
	if (argc > 1) (void) unlink((const char *)out1);
	alldone(nr_errs);
}

int
yywrap(void)	/* dummy routine */
{
	return 1;
}

void
non_fatal(char *s1, char *s2)
{	extern int yychar; extern char yytext[];

	printf("spin: line %3d %s, Error: ",
		lineno, Fname->name);
	if (s2)
		printf(s1, s2);
	else
		printf(s1);
	if (yychar != -1 && yychar != 0)
	{	printf("	saw '");
		explain(yychar);
		printf("'");
	}
	if (yytext && strlen(yytext)>1)
		printf(" near '%s'", yytext);
	printf("\n");
	nr_errs++;
	Error::abortRun("Syntax error of .ppl file!");
}

void
fatal(char *s1, char *s2)
{
	non_fatal(s1, s2);
	alldone(1);
}

char *
emalloc(int n)
{	char *tmp;
	memList *mlst;

	if (!(tmp = (char *) malloc(n)))
		fatal("not enough memory", (char *)0);
	memset(tmp, 0, n);

	// attach allocated memory to list spin_memList
	mlst = (memList *)malloc(sizeof(memList));
	mlst->entry = tmp;
	mlst->next = spin_memList;
	spin_memList = mlst;

	return tmp;
}

Lextok *
pt_nn(Lextok *s, int t, Lextok *ll, Lextok *rl)
{	char *tmp;
	Lextok *n;
	tmp=(char *)malloc(sizeof(Lextok));
	memset(tmp, 0, sizeof(Lextok));
	n = (Lextok *)tmp;

	n->ntyp = (short) t;
	n->fval = N_A_N;
	if (s && s->fn)
	{	n->ln = s->ln;
		n->fn = s->fn;
	} else if (rl && rl->fn)
	{	n->ln = rl->ln;
		n->fn = rl->fn;
	} else if (ll && ll->fn)
	{	n->ln = ll->ln;
		n->fn = ll->fn;
	} else
	{	n->ln = lineno;
		n->fn = Fname;
	}
	if (s) n->sym  = s->sym;
	n->lft  = ll;
	n->rgt  = rl;
	n->indstep = DstepStart;

	if (t == TIMEOUT) Etimeouts++;

	if (!context)
		return n;
	if (context->name == claimproc)
	{	switch (t) {
		case ASGN:
			printf("spin: Warning, never claim has side-effect\n");
			break;
		case 'r': case 's':
			non_fatal("never claim contains i/o stmnts",(char *)0);
			break;
		case TIMEOUT:
			/* never claim polls timeout */
			Ntimeouts++; Etimeouts--;
			break;
		case LEN: case EMPTY: case FULL:
		case 'R': case NFULL: case NEMPTY:
			/* status bumped to non-exclusive */
			if (n->sym) n->sym->xu |= XX;
			break;
		default:
			break;
		}
	} else if (t == ENABLED || t == PC_VAL)
	{	printf("spin: Warning, using %s outside never-claim\n",
			(t == ENABLED)?"enabled()":"pc_value()");
	} else if (t == NONPROGRESS)
	{	fatal("spin: Error, using np_ outside never-claim\n",
		(char *)0);
	}
	return n;
}

Lextok *
nn(Lextok *s, int t, Lextok *ll, Lextok *rl)
{	Lextok *n = (Lextok *) emalloc(sizeof(Lextok));

	n->ntyp = (short) t;
	if (s && s->fn)
	{	n->ln = s->ln;
		n->fn = s->fn;
	} else if (rl && rl->fn)
	{	n->ln = rl->ln;
		n->fn = rl->fn;
	} else if (ll && ll->fn)
	{	n->ln = ll->ln;
		n->fn = ll->fn;
	} else
	{	n->ln = lineno;
		n->fn = Fname;
	}
	if (s) n->sym  = s->sym;
	n->lft  = ll;
	n->rgt  = rl;
	n->indstep = DstepStart;

	if (t == TIMEOUT) Etimeouts++;

	if (!context)
		return n;
	if (context->name == claimproc)
	{	switch (t) {
		case ASGN:
			printf("spin: Warning, never claim has side-effect\n");
			break;
		case 'r': case 's':
			non_fatal("never claim contains i/o stmnts",(char *)0);
			break;
		case TIMEOUT:
			/* never claim polls timeout */
			Ntimeouts++; Etimeouts--;
			break;
		case LEN: case EMPTY: case FULL:
		case 'R': case NFULL: case NEMPTY:
			/* status bumped to non-exclusive */
			if (n->sym) n->sym->xu |= XX;
			break;
		default:
			break;
		}
	} else if (t == ENABLED || t == PC_VAL)
	{	printf("spin: Warning, using %s outside never-claim\n",
			(t == ENABLED)?"enabled()":"pc_value()");
	} else if (t == NONPROGRESS)
	{	fatal("spin: Error, using np_ outside never-claim\n",
		(char *)0);
	}
	return n;
}

Lextok *
rem_lab(Symbol *a, Lextok *b, Symbol *c)
{	Lextok *tmp1, *tmp2, *tmp3;

	has_remote++;
	fix_dest(c, a);	/* in case target is jump */
	tmp1 = nn(ZN, '?',   b, ZN); tmp1->sym = a;
	tmp1 = nn(ZN, 'p', tmp1, ZN);
	tmp1->sym = lookup("_p");
	tmp2 = nn(ZN, NAME,  ZN, ZN); tmp2->sym = a;
	tmp3 = nn(ZN, 'q', tmp2, ZN); tmp3->sym = c;
	return nn(ZN, EQ, tmp1, tmp3);
}

static void
explain(int n)
{	FILE *fd = stdout;
	switch (n) {
	default:	if (n > 0 && n < 256)
				fprintf(fd, "%c' = '", n);
			fprintf(fd, "%d", n);
			break;
	case '\b':	fprintf(fd, "\\b"); break;
	case '\t':	fprintf(fd, "\\t"); break;
	case '\f':	fprintf(fd, "\\f"); break;
	case '\n':	fprintf(fd, "\\n"); break;
	case '\r':	fprintf(fd, "\\r"); break;
	case 'c':	fprintf(fd, "condition"); break;
	case 's':	fprintf(fd, "send"); break;
	case 'r':	fprintf(fd, "recv"); break;
	case '@':	fprintf(fd, "delproc"); break;
	case '?':	fprintf(fd, "(x->y:z)"); break;
	case ACTIVE:	fprintf(fd, "%sactive",	Keyword); break;
	case AND:	fprintf(fd, "%s&&",	Operator); break;
	case ASGN:	fprintf(fd, "%s=",	Operator); break;
	case ASSERT:	fprintf(fd, "%sassert",	Function); break;
	case ATOMIC:	fprintf(fd, "%satomic",	Keyword); break;
	case BREAK:	fprintf(fd, "%sbreak",	Keyword); break;
	case CLAIM:	fprintf(fd, "%snever",	Keyword); break;
	case CONST:	fprintf(fd, "a constant"); break;
	case DECR:	fprintf(fd, "%s--",	Operator); break;
	case D_STEP:	fprintf(fd, "%sd_step",	Keyword); break;
	case DO:	fprintf(fd, "%sdo",	Keyword); break;
	case DOT:	fprintf(fd, "."); break;
	case ELSE:	fprintf(fd, "%selse",	Keyword); break;
	case EMPTY:	fprintf(fd, "%sempty",	Function); break;
	case ENABLED:	fprintf(fd, "%senabled",Function); break;
	case EQ:	fprintf(fd, "%s==",	Operator); break;
	case EVAL:	fprintf(fd, "%seval",	Function); break;
	case FI:	fprintf(fd, "%sfi",	Keyword); break;
	case FULL:	fprintf(fd, "%sfull",	Function); break;
	case GE:	fprintf(fd, "%s>=",	Operator); break;
	case GOTO:	fprintf(fd, "%sgoto",	Keyword); break;
	case GT:	fprintf(fd, "%s>",	Operator); break;
	case IF:	fprintf(fd, "%sif",	Keyword); break;
	case INCR:	fprintf(fd, "%s++",	Operator); break;
	case INIT:	fprintf(fd, "%sinit",	Keyword); break;
	case LABEL:	fprintf(fd, "a label-name"); break;
	case LE:	fprintf(fd, "%s<=",	Operator); break;
	case LEN:	fprintf(fd, "%slen",	Function); break;
	case LSHIFT:	fprintf(fd, "%s<<",	Operator); break;
	case LT:	fprintf(fd, "%s<",	Operator); break;
	case MTYPE:	fprintf(fd, "%smtype",	Keyword); break;
	case TIMETYPE:	fprintf(fd, "%stimetype",Keyword); break;
	case NAME:	fprintf(fd, "an identifier"); break;
	case NE:	fprintf(fd, "%s!=",	Operator); break;
	case NEG:	fprintf(fd, "%s! (not)",Operator); break;
	case NEMPTY:	fprintf(fd, "%snempty",	Function); break;
	case NFULL:	fprintf(fd, "%snfull",	Function); break;
	case NON_ATOMIC: fprintf(fd, "sub-sequence"); break;
	case NONPROGRESS: fprintf(fd, "%snp_",	Function); break;
	case OD:	fprintf(fd, "%sod",	Keyword); break;
	case OF:	fprintf(fd, "%sof",	Keyword); break;
	case OR:	fprintf(fd, "%s||",	Operator); break;
	case O_SND:	fprintf(fd, "%s!!",	Operator); break;
	case PC_VAL:	fprintf(fd, "%spc_value",Function); break;
	case PNAME:	fprintf(fd, "process name"); break;
	case PRINT:	fprintf(fd, "%sprintf",	Function); break;
	case DELAY:	fprintf(fd, "delay here"); break;
	case RETURN:	fprintf(fd, "return control"); break;
	case PACKF:	fprintf(fd, "pack a double-type number"); break;
	case PRESENT:	fprintf(fd, "present new data"); break;
	case EXPIRE:	fprintf(fd, "check if timeout"); break;
	case EXTOPER:	fprintf(fd, "trigger external block"); break;
	case FUNC:	fprintf(fd, "execute external C function"); break;
  	case ADMIT:     fprintf(fd, "check and turnoff new signal"); break;
	case TURNOFF:	fprintf(fd, "turnoff new signal"); break;
	case PROCTYPE:	fprintf(fd, "%sproctype",Keyword); break;
	case RCV:	fprintf(fd, "%s?",	Operator); break;
	case R_RCV:	fprintf(fd, "%s??",	Operator); break;
	case RSHIFT:	fprintf(fd, "%s>>",	Operator); break;
	case RUN:	fprintf(fd, "%srun",	Operator); break;
	case SEP:	fprintf(fd, "token: ::"); break;
	case SEMI:	fprintf(fd, ";"); break;
	case SND:	fprintf(fd, "%s!",	Operator); break;
	case SPIN_STRING:	fprintf(fd, "a string"); break;
	case TIMEOUT:	fprintf(fd, "%stimeout",Keyword); break;
	case TYPE:	fprintf(fd, "data typename"); break;
	case TYPEDEF:	fprintf(fd, "%stypedef",Keyword); break;
	case XU:	fprintf(fd, "%sx[rs]",	Keyword); break;
	case UMIN:	fprintf(fd, "%s- (unary minus)", Operator); break;
	case UNAME:	fprintf(fd, "a typename"); break;
	case UNLESS:	fprintf(fd, "%sunless",	Keyword); break;
	}
}
