/* $Id$ */

# include "stdio.h"
# define U(x) x
# define NLSTATE flowyyprevious=YYNEWLINE
# define BEGIN flowyybgin = flowyysvec + 1 +
# define INITIAL 0
# define YYLERR flowyysvec
# define YYSTATE (flowyyestate-flowyysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,flowyyout)
# define input() (((flowyytchar=flowyysptr>flowyysbuf?U(*--flowyysptr):getc(flowyyin))==10?(flowyylineno++,flowyytchar):flowyytchar)==EOF?0:flowyytchar)
# define unput(c) {flowyytchar= (c);if(flowyytchar=='\n')flowyylineno--;*flowyysptr++=flowyytchar;}
# define flowyymore() (flowyymorfg=1)
# define ECHO fprintf(flowyyout, "%s",flowyytext)
# define REJECT { nstr = flowyyreject(); goto flowyyfussy;}
int flowyyleng; extern char flowyytext[];
int flowyymorfg;
extern char *flowyysptr, flowyysbuf[];
int flowyytchar;
FILE *flowyyin = {stdin}, *flowyyout = {stdout};
extern int flowyylineno;
struct flowyysvf { 
	struct flowyywork *flowyystoff;
	struct flowyysvf *flowyyother;
	int *flowyystops;};
struct flowyysvf *flowyyestate;
extern struct flowyysvf flowyysvec[], *flowyybgin;
# define COMMENT 2
# undef input()
# define input() (((flowyytchar=flowyysptr>flowyysbuf?U(*--flowyysptr):getc(fparser))==10?(flowyylineno++,flowyytchar):flowyytchar)==EOF?0:flowyytchar)
static st_table *key_table = NULL;
# define YYNEWLINE 10
flowyylex(){
int nstr; extern int flowyyprevious;
       extern char *Intern(), *StripQuotes();
while((nstr = flowyylook()) >= 0)
flowyyfussy: switch(nstr){
case 0:
if(flowyywrap()) return(0); break;
case 1:
  { BEGIN 0; CurrentChar += 2; }
break;
case 2:
    { CurrentChar = 0; CurrentLine++; }
break;
case 3:
       case 4:
   { CurrentChar += strlen(flowyytext); }
break;
case 5:
            { CurrentChar++; }
break;
case 6:
             { CurrentChar = (CurrentChar+8) & 0xfffffff8; }
break;
case 7:
             { CurrentChar = 0; CurrentLine++; }
break;
case 8:
           { BEGIN COMMENT; CurrentChar += 2; }
break;
case 9:
     { if (flowyytext[flowyyleng-2] == '\\')
                        flowyymore();
                  else {
                      flowyylval.string = Intern(StripQuotes(flowyytext));
                      CurrentChar += strlen(flowyytext);
                      return(STRING);
                  }
                }
break;
case 10:
{ int Key;
                   if (st_lookup(key_table, flowyytext, &Key) != 0) {
                       return (Key);
                   } 
                   flowyylval.string = Intern(flowyytext);
                   CurrentChar += strlen(flowyytext);
                   return(STRING);
                }
break;
case 11:
    { CurrentChar += strlen(flowyytext);
                  flowyylval.token = atoi(flowyytext);
                  return (INTEGER); }
break;
case 12:
		{ CurrentChar++; return(flowyytext[0]); }
break;
case 13:
		{ LexError("Illegal character."); 
                  CurrentChar++; }
break;
case -1:
break;
default:
fprintf(flowyyout,"bad switch flowyylook %d",nstr);
} return(0); }
/* end of flowyylex */

/*
 * Keyword Table and lookup routines
 */

static struct KEYSTRUCT { char *key; int code; } KeywordTable[] = {
      { "GRAPH", GRAPH},
      { "NODE", NODE},
      { "EDGE", EDGE},
      { "NODELIST", NODELIST},
      { "EDGELIST", EDGELIST},
      { "CONTROLLIST", CONTROLLIST},
      { "NAME", NAME},
      { "CLASS", CLASS},
      { "MASTER", MASTER},
      { "IN_NODES", IN_NODES},
      { "OUT_NODES", OUT_NODES},
      { "IN_EDGES", IN_EDGES},
      { "OUT_EDGES", OUT_EDGES},
      { "IN_CONTROL", IN_CONTROL},
      { "OUT_CONTROL", OUT_CONTROL},
      { "ARGUMENTS", ARGUMENTS},
      { "ATTRIBUTES", ATTRIBUTES},
      { "MODEL", MODEL},
      { "NIL", NILLNODE},
      { "graph", GRAPH},
      { "node", NODE},
      { "edge", EDGE},
      { "nodelist", NODELIST},
      { "edgelist", EDGELIST},
      { "controllist", CONTROLLIST},
      { "name", NAME},
      { "class", CLASS},
      { "master", MASTER},
      { "in_nodes", IN_NODES},
      { "out_nodes", OUT_NODES},
      { "in_edges", IN_EDGES},
      { "out_edges", OUT_EDGES},
      { "in_control", IN_CONTROL},
      { "out_control", OUT_CONTROL},
      { "arguments", ARGUMENTS},
      { "attributes", ATTRIBUTES},
      { "model", MODEL},
      { "nil", NILLNODE},
};

static int KeywordTableSize = (sizeof(KeywordTable)/sizeof(KeywordTable[0]));
extern int strcmp();

int
KeyInit(){
	struct KEYSTRUCT *p, *p_max;

	key_table = st_init_table(strcmp, st_strhash);
        p_max = KeywordTable + KeywordTableSize;
        for (p = KeywordTable; p != p_max; p++) 
	    st_insert(key_table, p->key, p->code);
}

flowyyerror(s)
char *s;
{
        LexError(s);
}

int flowyywrap()
{
        return(1);
}
int flowyyvstop[] = {
0,

3,
0,

3,
0,

3,
0,

3,
0,

13,
0,

6,
13,
0,

7,
0,

5,
13,
0,

13,
0,

12,
13,
0,

13,
0,

13,
0,

11,
13,
0,

10,
13,
0,

3,
13,
0,

3,
6,
13,
0,

2,
7,
0,

3,
5,
13,
0,

3,
13,
0,

3,
12,
13,
0,

4,
13,
0,

3,
13,
0,

3,
13,
0,

3,
11,
13,
0,

3,
10,
13,
0,

9,
0,

11,
0,

8,
0,

10,
0,

3,
0,

3,
0,

3,
9,
0,

1,
0,

3,
11,
0,

3,
10,
0,
0};
# define YYTYPE char
struct flowyywork { YYTYPE verify, advance; } flowyycrank[] = {
0,0,	0,0,	1,5,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,6,	1,7,	
0,0,	0,0,	2,6,	4,16,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	3,15,	
0,0,	0,0,	16,0,	18,0,	
0,0,	0,0,	0,0,	3,16,	
3,17,	1,8,	0,0,	1,9,	
1,5,	2,8,	4,18,	20,0,	
22,0,	1,10,	9,26,	1,5,	
1,11,	12,29,	1,11,	23,0,	
1,12,	1,13,	24,0,	9,26,	
2,12,	4,23,	3,18,	0,0,	
3,19,	3,15,	16,0,	18,0,	
0,0,	0,0,	3,20,	0,0,	
3,21,	3,22,	1,14,	3,22,	
31,0,	3,23,	3,24,	20,0,	
22,0,	33,0,	0,0,	9,27,	
9,26,	21,34,	22,35,	23,29,	
0,0,	9,26,	24,0,	9,26,	
9,26,	0,0,	9,26,	3,25,	
24,35,	9,26,	11,28,	11,28,	
11,28,	11,28,	11,28,	11,28,	
11,28,	11,28,	11,28,	11,28,	
31,0,	0,0,	0,0,	0,0,	
0,0,	33,0,	9,26,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,30,	14,0,	19,32,	
14,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	15,31,	14,30,	
14,0,	14,0,	14,30,	0,0,	
0,0,	0,0,	0,0,	15,0,	
0,0,	25,0,	0,0,	35,0,	
14,0,	14,0,	0,0,	14,0,	
0,0,	14,0,	14,0,	14,30,	
0,0,	0,0,	0,0,	0,0,	
19,33,	19,32,	0,0,	0,0,	
0,0,	0,0,	19,32,	15,31,	
15,31,	19,32,	25,36,	19,32,	
0,0,	15,31,	19,32,	15,0,	
15,31,	25,0,	15,31,	35,0,	
25,36,	15,31,	14,0,	25,36,	
14,0,	35,35,	14,0,	0,0,	
0,0,	0,0,	0,0,	19,32,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	15,31,	0,0,	
25,36,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	14,0,	14,0,	14,0,	
14,0,	14,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
32,32,	30,0,	0,0,	30,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	0,0,	0,0,	30,0,	
30,0,	36,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	30,0,	
30,0,	0,0,	30,0,	0,0,	
30,0,	30,0,	0,0,	0,0,	
0,0,	32,33,	32,32,	0,0,	
0,0,	0,0,	36,36,	32,32,	
0,0,	0,0,	32,32,	0,0,	
32,32,	36,0,	0,0,	32,32,	
36,36,	0,0,	0,0,	36,36,	
0,0,	0,0,	0,0,	0,0,	
0,0,	30,0,	0,0,	30,0,	
0,0,	30,0,	0,0,	0,0,	
32,32,	0,0,	0,0,	0,0,	
36,36,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
30,0,	30,0,	30,0,	30,0,	
30,0,	0,0,	0,0,	0,0,	
0,0};
struct flowyysvf flowyysvec[] = {
0,	0,	0,
flowyycrank+-1,	0,		flowyyvstop+1,
flowyycrank+-5,	flowyysvec+1,	flowyyvstop+3,
flowyycrank+-22,	0,		flowyyvstop+5,
flowyycrank+-6,	flowyysvec+3,	flowyyvstop+7,
flowyycrank+0,	0,		flowyyvstop+9,
flowyycrank+0,	0,		flowyyvstop+11,
flowyycrank+0,	0,		flowyyvstop+14,
flowyycrank+0,	0,		flowyyvstop+16,
flowyycrank+-41,	0,		flowyyvstop+19,
flowyycrank+0,	0,		flowyyvstop+21,
flowyycrank+42,	0,		flowyyvstop+24,
flowyycrank+3,	0,		flowyyvstop+26,
flowyycrank+0,	flowyysvec+11,	flowyyvstop+28,
flowyycrank+-106,	0,		flowyyvstop+31,
flowyycrank+-149,	0,		flowyyvstop+34,
flowyycrank+-16,	flowyysvec+15,	flowyyvstop+37,
flowyycrank+0,	0,		flowyyvstop+41,
flowyycrank+-17,	flowyysvec+15,	flowyyvstop+44,
flowyycrank+-142,	flowyysvec+9,	flowyyvstop+48,
flowyycrank+-29,	flowyysvec+15,	flowyyvstop+51,
flowyycrank+30,	0,		flowyyvstop+55,
flowyycrank+-30,	flowyysvec+15,	flowyyvstop+58,
flowyycrank+-37,	flowyysvec+15,	flowyyvstop+61,
flowyycrank+-40,	flowyysvec+15,	flowyyvstop+64,
flowyycrank+-151,	flowyysvec+15,	flowyyvstop+68,
flowyycrank+0,	flowyysvec+9,	0,	
flowyycrank+0,	0,		flowyyvstop+72,
flowyycrank+0,	flowyysvec+11,	flowyyvstop+74,
flowyycrank+0,	0,		flowyyvstop+76,
flowyycrank+-233,	flowyysvec+14,	flowyyvstop+78,
flowyycrank+-58,	flowyysvec+15,	flowyyvstop+80,
flowyycrank+-267,	flowyysvec+9,	flowyyvstop+82,
flowyycrank+-63,	flowyysvec+15,	flowyyvstop+84,
flowyycrank+0,	0,		flowyyvstop+87,
flowyycrank+-153,	flowyysvec+15,	flowyyvstop+89,
flowyycrank+-271,	flowyysvec+15,	flowyyvstop+92,
0,	0,	0};
struct flowyywork *flowyytop = flowyycrank+360;
struct flowyysvf *flowyybgin = flowyysvec+1;
char flowyymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,'"' ,'#' ,01  ,'#' ,01  ,01  ,
'(' ,'(' ,'*' ,'+' ,'#' ,'-' ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,'#' ,01  ,'#' ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'#' ,01  ,'#' ,01  ,'#' ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
0};
char flowyyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.6 88/02/08 SMI"; /* from S5R2 1.2 */
#endif

int flowyylineno =1;
# define YYU(x) x
# define NLSTATE flowyyprevious=YYNEWLINE
char flowyytext[YYLMAX];
struct flowyysvf *flowyylstate [YYLMAX], **flowyylsp, **flowyyolsp;
char flowyysbuf[YYLMAX];
char *flowyysptr = flowyysbuf;
int *flowyyfnd;
extern struct flowyysvf *flowyyestate;
int flowyyprevious = YYNEWLINE;
flowyylook(){
	register struct flowyysvf *flowyystate, **lsp;
	register struct flowyywork *flowyyt;
	struct flowyysvf *flowyyz;
	int flowyych, flowyyfirst;
	struct flowyywork *flowyyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *flowyylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	flowyyfirst=1;
	if (!flowyymorfg)
		flowyylastch = flowyytext;
	else {
		flowyymorfg=0;
		flowyylastch = flowyytext+flowyyleng;
		}
	for(;;){
		lsp = flowyylstate;
		flowyyestate = flowyystate = flowyybgin;
		if (flowyyprevious==YYNEWLINE) flowyystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(flowyyout,"state %d\n",flowyystate-flowyysvec-1);
# endif
			flowyyt = flowyystate->flowyystoff;
			if(flowyyt == flowyycrank && !flowyyfirst){  /* may not be any transitions */
				flowyyz = flowyystate->flowyyother;
				if(flowyyz == 0)break;
				if(flowyyz->flowyystoff == flowyycrank)break;
				}
			*flowyylastch++ = flowyych = input();
			flowyyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(flowyyout,"char ");
				allprint(flowyych);
				putchar('\n');
				}
# endif
			flowyyr = flowyyt;
			if ( (int)flowyyt > (int)flowyycrank){
				flowyyt = flowyyr + flowyych;
				if (flowyyt <= flowyytop && flowyyt->verify+flowyysvec == flowyystate){
					if(flowyyt->advance+flowyysvec == YYLERR)	/* error transitions */
						{unput(*--flowyylastch);break;}
					*lsp++ = flowyystate = flowyyt->advance+flowyysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)flowyyt < (int)flowyycrank) {		/* r < flowyycrank */
				flowyyt = flowyyr = flowyycrank+(flowyycrank-flowyyt);
# ifdef LEXDEBUG
				if(debug)fprintf(flowyyout,"compressed state\n");
# endif
				flowyyt = flowyyt + flowyych;
				if(flowyyt <= flowyytop && flowyyt->verify+flowyysvec == flowyystate){
					if(flowyyt->advance+flowyysvec == YYLERR)	/* error transitions */
						{unput(*--flowyylastch);break;}
					*lsp++ = flowyystate = flowyyt->advance+flowyysvec;
					goto contin;
					}
				flowyyt = flowyyr + YYU(flowyymatch[flowyych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(flowyyout,"try fall back character ");
					allprint(YYU(flowyymatch[flowyych]));
					putchar('\n');
					}
# endif
				if(flowyyt <= flowyytop && flowyyt->verify+flowyysvec == flowyystate){
					if(flowyyt->advance+flowyysvec == YYLERR)	/* error transition */
						{unput(*--flowyylastch);break;}
					*lsp++ = flowyystate = flowyyt->advance+flowyysvec;
					goto contin;
					}
				}
			if ((flowyystate = flowyystate->flowyyother) && (flowyyt= flowyystate->flowyystoff) != flowyycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(flowyyout,"fall back to state %d\n",flowyystate-flowyysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--flowyylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(flowyyout,"state %d char ",flowyystate-flowyysvec-1);
				allprint(flowyych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(flowyyout,"stopped at %d with ",*(lsp-1)-flowyysvec-1);
			allprint(flowyych);
			putchar('\n');
			}
# endif
		while (lsp-- > flowyylstate){
			*flowyylastch-- = 0;
			if (*lsp != 0 && (flowyyfnd= (*lsp)->flowyystops) && *flowyyfnd > 0){
				flowyyolsp = lsp;
				if(flowyyextra[*flowyyfnd]){		/* must backup */
					while(flowyyback((*lsp)->flowyystops,-*flowyyfnd) != 1 && lsp > flowyylstate){
						lsp--;
						unput(*flowyylastch--);
						}
					}
				flowyyprevious = YYU(*flowyylastch);
				flowyylsp = lsp;
				flowyyleng = flowyylastch-flowyytext+1;
				flowyytext[flowyyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(flowyyout,"\nmatch ");
					sprint(flowyytext);
					fprintf(flowyyout," action %d\n",*flowyyfnd);
					}
# endif
				return(*flowyyfnd++);
				}
			unput(*flowyylastch);
			}
		if (flowyytext[0] == 0  /* && feof(flowyyin) */)
			{
			flowyysptr=flowyysbuf;
			return(0);
			}
		flowyyprevious = flowyytext[0] = input();
		if (flowyyprevious>0)
			output(flowyyprevious);
		flowyylastch=flowyytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
flowyyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
flowyyinput(){
	return(input());
	}
flowyyoutput(c)
  int c; {
	output(c);
	}
flowyyunput(c)
   int c; {
	unput(c);
	}
