/* $Id$ */

extern char *malloc(), *realloc();

# line 6 "flowYacc.y"
# include "flowgraph.h"
# include "st.h"
static NodePointer CurrentNode = NULL;
static EdgePointer CurrentEdge = NULL;
static GraphPointer CurrentGraph = NULL;
static GraphPointer GraphList = NULL;

extern NodePointer new_node();
extern NodePointer NodeListAppend();
extern EdgePointer new_edge();
extern EdgePointer EdgeListAppend();
extern ListPointer new_list();
extern GraphPointer new_graph();
extern GraphPointer GraphListAppend();
extern ListPointer ListAppend();

# line 43 "flowYacc.y"
typedef union  {
    int token;
    char *string;
    GraphPointer graph;
    NodePointer node;
    EdgePointer edge;
    ListPointer list;
} YYSTYPE;
# define GRAPH 257
# define EDGELIST 258
# define NODELIST 259
# define CONTROLLIST 260
# define EDGE 261
# define NODE 262
# define NAME 263
# define TYPE 264
# define CLASS 265
# define FUNCTION 266
# define MASTER 267
# define IN_NODES 268
# define OUT_NODES 269
# define IN_EDGES 270
# define OUT_EDGES 271
# define ATTRIBUTES 272
# define ARGUMENTS 273
# define MODEL 274
# define IN_CONTROL 275
# define OUT_CONTROL 276
# define INTEGER 277
# define STRING 278
# define NILLNODE 279
#define flowyyclearin flowyychar = -1
#define flowyyerrok flowyyerrflag = 0
extern int flowyychar;
extern int flowyyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE flowyylval, flowyyval;
# define YYERRCODE 256

# line 365 "flowYacc.y"


static int CurrentLine, CurrentChar;
static char CurrentFileName[WORDLENGTH];

FILE *fparser;
#include "flowLex.c"

GraphPointer
Parser(FileName)
char *FileName;
{
    FILE *popen();
    char command[WORDLENGTH];

    GraphList = NULL;

    strcpy(CurrentFileName, FileName);
    CurrentLine = 1;
    CurrentChar = 0;

    if ((fparser = fopen(FileName, "r")) == NULL) {
	 printmsg("Parser", "Cannot open flowgraph file %s\n", FileName);
         return (NULL);
    }
    if (flowyyparse() || fatal == TRUE) {
	printmsg ("Parser",
		  "Fatal errors in FlowGraph Description (%s)\n", FileName);
	printmsg ("\t", "Please check.\n");
	GraphList = NULL;
    }
    pclose(fparser);
    return (GraphList);
}

/*
 * Simple Parser support functions
 */

InitializeEdge()
{
    if (CurrentEdge == NULL)
	CurrentEdge = new_edge();
}

InitializeNode()
{
    if (CurrentNode == NULL)
	CurrentNode = new_node();
}

InitializeGraph()
{
    if (CurrentGraph == NULL)
	CurrentGraph = new_graph();
}

ErrorHandler(string)
char *string;
{

    printmsg("Flowgraph Error", "Line %d in %s\n",CurrentLine, CurrentFileName);
    printmsg("\t%s\n", string);

    fatal = TRUE;
}

LexError(mssg)
char *mssg;
{
    printmsg("Flowgraph Error" , "File %s, line %d, character %d: %s\n",
                CurrentFileName, CurrentLine, CurrentChar, mssg);
    fatal = TRUE;
}
int flowyyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 69
# define YYLAST 246
int flowyyact[]={

    24,    26,    25,    49,   100,    27,    49,    28,    46,    45,
    45,    62,    29,    30,    33,    34,    35,    31,    32,    27,
    58,    28,     6,   108,    72,     9,    29,    30,    33,    34,
    10,    31,    32,    27,    69,    28,    11,    12,   104,   105,
    80,   114,    33,    34,    17,    16,    15,    89,    14,   113,
    68,    99,   106,   112,    48,    50,    51,    52,    53,    54,
    55,    78,    43,    47,    87,   102,    42,    60,    38,    59,
    38,    56,    21,    22,    77,    76,    75,    74,    73,    66,
    65,    64,    63,    41,    81,    49,    99,    87,    90,    82,
    42,    37,    38,    91,    21,    83,    86,     4,     8,    93,
    94,    36,    92,    67,    70,    81,    23,    98,    97,    96,
    82,    95,     3,    13,    90,     5,    83,    86,   101,    91,
   103,    85,    84,    88,    61,    93,    94,    39,    57,   109,
   110,    57,    44,    98,    97,    96,   107,    95,    40,    18,
    79,    20,    19,   111,     7,     2,     1,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    46,    45,    71,    46,    45,    71 };
int flowyypact[]={

    57, -1000,    57, -1000,  -235, -1000,    54,    32, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000,  -258, -1000, -1000,    52,    52,    50,  -269,  -268,    45,
    45,    45,    45,    45,    45,    45,    30, -1000,  -241,    28,
    26, -1000,  -251,    41,    40, -1000, -1000,    39,    38,   -34,
    37,    36,    35,    34,    33,    20, -1000, -1000,    47, -1000,
 -1000, -1000,    46, -1000, -1000, -1000, -1000,   -37, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,    24,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -230,    11, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -244,
 -1000, -1000, -1000, -1000,    45,    45, -1000, -1000,  -268,    12,
     8,     0, -1000, -1000, -1000 };
int flowyypgo[]={

     0,   146,   145,   112,   144,    98,   142,   141,   101,    91,
   140,    40,   139,   138,    83,   123,    47,   122,   121,    48,
    46,    45,    44,    37,    36,   113,    24,   103,    50,    30,
    25,   102,    34,   104 };
int flowyyr1[]={

     0,     1,     2,     2,     3,     4,     4,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     6,
     8,     8,     7,    12,    13,    13,    14,    15,    15,    16,
    16,    16,    16,    16,    16,    16,    16,    16,     9,    10,
    10,    11,    11,    11,    11,    11,    11,    30,    30,    29,
    31,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    27,    28,    28,    28,    28,    32,    33 };
int flowyyr2[]={

     0,     3,     2,     5,     9,     2,     4,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     9,
     2,     5,     9,     9,     2,     5,     9,     2,     4,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     9,     2,
     4,     3,     3,     3,     3,     3,     3,     9,     9,     9,
     9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
     7,     2,     5,     3,     3,     3,     3,     3,     3 };
int flowyychk[]={

 -1000,    -1,    -2,    -3,    40,    -3,   257,    -4,    -5,   -30,
   -29,   -24,   -23,   -25,   -19,   -20,   -21,   -22,   -12,    -6,
    -7,    40,    41,    -5,   258,   260,   259,   263,   265,   270,
   271,   275,   276,   272,   273,   274,    -8,    -9,    40,    -8,
   -13,   -14,    40,   -32,   -33,   278,   277,   -32,   -26,    40,
   -26,   -26,   -26,   -26,   -26,   -26,    41,    -9,   261,    41,
    41,   -14,   262,    41,    41,    41,    41,   -27,   -28,   -32,
   -33,   279,   -26,    41,    41,    41,    41,    41,    41,   -10,
   -11,   -30,   -29,   -24,   -17,   -18,   -23,    40,   -15,   -16,
   -30,   -29,   -31,   -24,   -23,   -19,   -20,   -21,   -22,    40,
    41,   -28,    41,   -11,   268,   269,    41,   -16,   267,   -26,
   -26,   -32,    41,    41,    41 };
int flowyydef[]={

     0,    -2,     1,     2,     0,     3,     0,     0,     5,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,     0,     4,     6,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    20,     0,     0,
     0,    24,     0,     0,     0,    67,    68,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    19,    21,     0,    22,
    23,    25,     0,    47,    48,    49,    53,     0,    61,    63,
    64,    65,    66,    54,    55,    56,    57,    58,    59,     0,
    39,    41,    42,    43,    44,    45,    46,     0,     0,    27,
    29,    30,    31,    32,    33,    34,    35,    36,    37,     0,
    60,    62,    38,    40,     0,     0,    26,    28,     0,     0,
     0,     0,    51,    52,    50 };
typedef struct { char *t_name; int t_val; } flowyytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

flowyytoktype flowyytoks[] =
{
	"GRAPH",	257,
	"EDGELIST",	258,
	"NODELIST",	259,
	"CONTROLLIST",	260,
	"EDGE",	261,
	"NODE",	262,
	"NAME",	263,
	"TYPE",	264,
	"CLASS",	265,
	"FUNCTION",	266,
	"MASTER",	267,
	"IN_NODES",	268,
	"OUT_NODES",	269,
	"IN_EDGES",	270,
	"OUT_EDGES",	271,
	"ATTRIBUTES",	272,
	"ARGUMENTS",	273,
	"MODEL",	274,
	"IN_CONTROL",	275,
	"OUT_CONTROL",	276,
	"INTEGER",	277,
	"STRING",	278,
	"NILLNODE",	279,
	"-unknown-",	-1	/* ends search */
};

char * flowyyreds[] =
{
	"-no such reduction-",
	"flowgraph : GraphDefinitions",
	"GraphDefinitions : GraphDefinition",
	"GraphDefinitions : GraphDefinitions GraphDefinition",
	"GraphDefinition : '(' GRAPH GraphArguments ')'",
	"GraphArguments : GraphArgument",
	"GraphArguments : GraphArguments GraphArgument",
	"GraphArgument : NameDefinition",
	"GraphArgument : ClassDefinition",
	"GraphArgument : ArgumentList",
	"GraphArgument : AttributeList",
	"GraphArgument : ModelList",
	"GraphArgument : InEdgeList",
	"GraphArgument : OutEdgeList",
	"GraphArgument : InControl",
	"GraphArgument : OutControl",
	"GraphArgument : NodeList",
	"GraphArgument : EdgeList",
	"GraphArgument : ControlList",
	"EdgeList : '(' EDGELIST ListOfEdges ')'",
	"ListOfEdges : Edge",
	"ListOfEdges : ListOfEdges Edge",
	"ControlList : '(' CONTROLLIST ListOfEdges ')'",
	"NodeList : '(' NODELIST ListOfNodes ')'",
	"ListOfNodes : Node",
	"ListOfNodes : ListOfNodes Node",
	"Node : '(' NODE NodeArguments ')'",
	"NodeArguments : NodeArgument",
	"NodeArguments : NodeArguments NodeArgument",
	"NodeArgument : NameDefinition",
	"NodeArgument : ClassDefinition",
	"NodeArgument : MasterDefinition",
	"NodeArgument : ArgumentList",
	"NodeArgument : AttributeList",
	"NodeArgument : InEdgeList",
	"NodeArgument : OutEdgeList",
	"NodeArgument : InControl",
	"NodeArgument : OutControl",
	"Edge : '(' EDGE EdgeArguments ')'",
	"EdgeArguments : EdgeArgument",
	"EdgeArguments : EdgeArguments EdgeArgument",
	"EdgeArgument : NameDefinition",
	"EdgeArgument : ClassDefinition",
	"EdgeArgument : ArgumentList",
	"EdgeArgument : InNodeList",
	"EdgeArgument : OutNodeList",
	"EdgeArgument : AttributeList",
	"NameDefinition : '(' NAME string ')'",
	"NameDefinition : '(' NAME integer ')'",
	"ClassDefinition : '(' CLASS string ')'",
	"MasterDefinition : '(' MASTER string ')'",
	"InNodeList : '(' IN_NODES ElementList ')'",
	"OutNodeList : '(' OUT_NODES ElementList ')'",
	"InEdgeList : '(' IN_EDGES ElementList ')'",
	"OutEdgeList : '(' OUT_EDGES ElementList ')'",
	"InControl : '(' IN_CONTROL ElementList ')'",
	"OutControl : '(' OUT_CONTROL ElementList ')'",
	"AttributeList : '(' ATTRIBUTES ElementList ')'",
	"ArgumentList : '(' ARGUMENTS ElementList ')'",
	"ModelList : '(' MODEL ElementList ')'",
	"ElementList : '(' ListOfElements ')'",
	"ListOfElements : Element",
	"ListOfElements : ListOfElements Element",
	"Element : string",
	"Element : integer",
	"Element : NILLNODE",
	"Element : ElementList",
	"string : STRING",
	"integer : INTEGER",
};
#endif /* YYDEBUG */
#line 1 "/usr/lib/yaccpar"
/*	@(#)yaccpar 1.10 89/04/04 SMI; from S5R3 1.10	*/

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto flowyyerrlab
#define YYACCEPT	{ free(flowyys); free(flowyyv); return(0); }
#define YYABORT		{ free(flowyys); free(flowyyv); return(1); }
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( flowyychar >= 0 || ( flowyyr2[ flowyytmp ] >> 1 ) != 1 )\
	{\
		flowyyerror( "syntax error - cannot backup" );\
		goto flowyyerrlab;\
	}\
	flowyychar = newtoken;\
	flowyystate = *flowyyps;\
	flowyylval = newvalue;\
	goto flowyynewstate;\
}
#define YYRECOVERING()	(!!flowyyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int flowyydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** static variables used by the parser
*/
static YYSTYPE *flowyyv;			/* value stack */
static int *flowyys;			/* state stack */

static YYSTYPE *flowyypv;			/* top of value stack */
static int *flowyyps;			/* top of state stack */

static int flowyystate;			/* current state */
static int flowyytmp;			/* extra var (lasts between blocks) */

int flowyynerrs;			/* number of errors */

int flowyyerrflag;			/* error recovery flag */
int flowyychar;			/* current input token number */


/*
** flowyyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
flowyyparse()
{
	register YYSTYPE *flowyypvt;	/* top of value stack for $vars */
	unsigned flowyymaxdepth = YYMAXDEPTH;

	/*
	** Initialize externals - flowyyparse may be called more than once
	*/
	flowyyv = (YYSTYPE*)malloc(flowyymaxdepth*sizeof(YYSTYPE));
	flowyys = (int*)malloc(flowyymaxdepth*sizeof(int));
	if (!flowyyv || !flowyys)
	{
		flowyyerror( "out of memory" );
		return(1);
	}
	flowyypv = &flowyyv[-1];
	flowyyps = &flowyys[-1];
	flowyystate = 0;
	flowyytmp = 0;
	flowyynerrs = 0;
	flowyyerrflag = 0;
	flowyychar = -1;

	goto flowyystack;
	{
		register YYSTYPE *flowyy_pv;	/* top of value stack */
		register int *flowyy_ps;		/* top of state stack */
		register int flowyy_state;		/* current state */
		register int  flowyy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	flowyynewstate:
		flowyy_pv = flowyypv;
		flowyy_ps = flowyyps;
		flowyy_state = flowyystate;
		goto flowyy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	flowyystack:
		flowyy_pv = flowyypv;
		flowyy_ps = flowyyps;
		flowyy_state = flowyystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	flowyy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( flowyydebug )
		{
			register int flowyy_i;

			(void)printf( "State %d, token ", flowyy_state );
			if ( flowyychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( flowyychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( flowyy_i = 0; flowyytoks[flowyy_i].t_val >= 0;
					flowyy_i++ )
				{
					if ( flowyytoks[flowyy_i].t_val == flowyychar )
						break;
				}
				(void)printf( "%s\n", flowyytoks[flowyy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++flowyy_ps >= &flowyys[ flowyymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int flowyyps_index = (flowyy_ps - flowyys);
			int flowyypv_index = (flowyy_pv - flowyyv);
			int flowyypvt_index = (flowyypvt - flowyyv);
			flowyymaxdepth += YYMAXDEPTH;
			flowyyv = (YYSTYPE*)realloc((char*)flowyyv,
				flowyymaxdepth * sizeof(YYSTYPE));
			flowyys = (int*)realloc((char*)flowyys,
				flowyymaxdepth * sizeof(int));
			if (!flowyyv || !flowyys)
			{
				flowyyerror( "yacc stack overflow" );
				return(1);
			}
			flowyy_ps = flowyys + flowyyps_index;
			flowyy_pv = flowyyv + flowyypv_index;
			flowyypvt = flowyyv + flowyypvt_index;
		}
		*flowyy_ps = flowyy_state;
		*++flowyy_pv = flowyyval;

		/*
		** we have a new state - find out what to do
		*/
	flowyy_newstate:
		if ( ( flowyy_n = flowyypact[ flowyy_state ] ) <= YYFLAG )
			goto flowyydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		flowyytmp = flowyychar < 0;
#endif
		if ( ( flowyychar < 0 ) && ( ( flowyychar = flowyylex() ) < 0 ) )
			flowyychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( flowyydebug && flowyytmp )
		{
			register int flowyy_i;

			(void)printf( "Received token " );
			if ( flowyychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( flowyychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( flowyy_i = 0; flowyytoks[flowyy_i].t_val >= 0;
					flowyy_i++ )
				{
					if ( flowyytoks[flowyy_i].t_val == flowyychar )
						break;
				}
				(void)printf( "%s\n", flowyytoks[flowyy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( flowyy_n += flowyychar ) < 0 ) || ( flowyy_n >= YYLAST ) )
			goto flowyydefault;
		if ( flowyychk[ flowyy_n = flowyyact[ flowyy_n ] ] == flowyychar )	/*valid shift*/
		{
			flowyychar = -1;
			flowyyval = flowyylval;
			flowyy_state = flowyy_n;
			if ( flowyyerrflag > 0 )
				flowyyerrflag--;
			goto flowyy_stack;
		}

	flowyydefault:
		if ( ( flowyy_n = flowyydef[ flowyy_state ] ) == -2 )
		{
#if YYDEBUG
			flowyytmp = flowyychar < 0;
#endif
			if ( ( flowyychar < 0 ) && ( ( flowyychar = flowyylex() ) < 0 ) )
				flowyychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( flowyydebug && flowyytmp )
			{
				register int flowyy_i;

				(void)printf( "Received token " );
				if ( flowyychar == 0 )
					(void)printf( "end-of-file\n" );
				else if ( flowyychar < 0 )
					(void)printf( "-none-\n" );
				else
				{
					for ( flowyy_i = 0;
						flowyytoks[flowyy_i].t_val >= 0;
						flowyy_i++ )
					{
						if ( flowyytoks[flowyy_i].t_val
							== flowyychar )
						{
							break;
						}
					}
					(void)printf( "%s\n", flowyytoks[flowyy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *flowyyxi = flowyyexca;

				while ( ( *flowyyxi != -1 ) ||
					( flowyyxi[1] != flowyy_state ) )
				{
					flowyyxi += 2;
				}
				while ( ( *(flowyyxi += 2) >= 0 ) &&
					( *flowyyxi != flowyychar ) )
					;
				if ( ( flowyy_n = flowyyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( flowyy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( flowyyerrflag )
			{
			case 0:		/* new error */
				flowyyerror( "syntax error" );
				goto skip_init;
			flowyyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				flowyy_pv = flowyypv;
				flowyy_ps = flowyyps;
				flowyy_state = flowyystate;
				flowyynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				flowyyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( flowyy_ps >= flowyys )
				{
					flowyy_n = flowyypact[ *flowyy_ps ] + YYERRCODE;
					if ( flowyy_n >= 0 && flowyy_n < YYLAST &&
						flowyychk[flowyyact[flowyy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						flowyy_state = flowyyact[ flowyy_n ];
						goto flowyy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( flowyydebug )
						(void)printf( _POP_, *flowyy_ps,
							flowyy_ps[-1] );
#	undef _POP_
#endif
					flowyy_ps--;
					flowyy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( flowyydebug )
				{
					register int flowyy_i;

					(void)printf( "Error recovery discards " );
					if ( flowyychar == 0 )
						(void)printf( "token end-of-file\n" );
					else if ( flowyychar < 0 )
						(void)printf( "token -none-\n" );
					else
					{
						for ( flowyy_i = 0;
							flowyytoks[flowyy_i].t_val >= 0;
							flowyy_i++ )
						{
							if ( flowyytoks[flowyy_i].t_val
								== flowyychar )
							{
								break;
							}
						}
						(void)printf( "token %s\n",
							flowyytoks[flowyy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( flowyychar == 0 )	/* reached EOF. quit */
					YYABORT;
				flowyychar = -1;
				goto flowyy_newstate;
			}
		}/* end if ( flowyy_n == 0 ) */
		/*
		** reduction by production flowyy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( flowyydebug )
			(void)printf( "Reduce by (%d) \"%s\"\n",
				flowyy_n, flowyyreds[ flowyy_n ] );
#endif
		flowyytmp = flowyy_n;			/* value to switch over */
		flowyypvt = flowyy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using flowyy_state here as temporary
		** register variable, but why not, if it works...
		** If flowyyr2[ flowyy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto flowyy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int flowyy_len = flowyyr2[ flowyy_n ];

			if ( !( flowyy_len & 01 ) )
			{
				flowyy_len >>= 1;
				flowyyval = ( flowyy_pv -= flowyy_len )[1];	/* $$ = $1 */
				flowyy_state = flowyypgo[ flowyy_n = flowyyr1[ flowyy_n ] ] +
					*( flowyy_ps -= flowyy_len ) + 1;
				if ( flowyy_state >= YYLAST ||
					flowyychk[ flowyy_state =
					flowyyact[ flowyy_state ] ] != -flowyy_n )
				{
					flowyy_state = flowyyact[ flowyypgo[ flowyy_n ] ];
				}
				goto flowyy_stack;
			}
			flowyy_len >>= 1;
			flowyyval = ( flowyy_pv -= flowyy_len )[1];	/* $$ = $1 */
			flowyy_state = flowyypgo[ flowyy_n = flowyyr1[ flowyy_n ] ] +
				*( flowyy_ps -= flowyy_len ) + 1;
			if ( flowyy_state >= YYLAST ||
				flowyychk[ flowyy_state = flowyyact[ flowyy_state ] ] != -flowyy_n )
			{
				flowyy_state = flowyyact[ flowyypgo[ flowyy_n ] ];
			}
		}
					/* save until reenter driver code */
		flowyystate = flowyy_state;
		flowyyps = flowyy_ps;
		flowyypv = flowyy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( flowyytmp )
	{
		
case 1:
# line 56 "flowYacc.y"
{ GraphList = flowyypvt[-0].graph; } break;
case 3:
# line 62 "flowYacc.y"
{ flowyyval.graph = GraphListAppend(flowyypvt[-0].graph, flowyypvt[-1].graph); } break;
case 4:
# line 67 "flowYacc.y"
{ flowyyval.graph = flowyypvt[-1].graph;
	      CurrentGraph = NULL;
	    } break;
case 7:
# line 79 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->Name = flowyypvt[-0].string;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 8:
# line 84 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->Class = flowyypvt[-0].string;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 9:
# line 89 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->Arguments = flowyypvt[-0].list;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 10:
# line 94 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->Attributes = flowyypvt[-0].list;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 11:
# line 99 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->Model = flowyypvt[-0].list;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 12:
# line 104 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->InEdges = flowyypvt[-0].list;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 13:
# line 109 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->OutEdges = flowyypvt[-0].list;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 14:
# line 114 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->InControl = flowyypvt[-0].list;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 15:
# line 119 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->OutControl = flowyypvt[-0].list;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 16:
# line 124 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->NodeList = flowyypvt[-0].node;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 17:
# line 129 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->EdgeList = flowyypvt[-0].edge;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 18:
# line 134 "flowYacc.y"
{ InitializeGraph();
	      CurrentGraph->ControlList = flowyypvt[-0].edge;
	      flowyyval.graph = CurrentGraph;
	    } break;
case 19:
# line 142 "flowYacc.y"
{ flowyyval.edge = flowyypvt[-1].edge; } break;
case 21:
# line 148 "flowYacc.y"
{ flowyyval.edge = EdgeListAppend(flowyypvt[-0].edge, flowyypvt[-1].edge); } break;
case 22:
# line 153 "flowYacc.y"
{ flowyyval.edge = flowyypvt[-1].edge; } break;
case 23:
# line 158 "flowYacc.y"
{ flowyyval.node = flowyypvt[-1].node; } break;
case 25:
# line 164 "flowYacc.y"
{ flowyyval.node = NodeListAppend(flowyypvt[-0].node, flowyypvt[-1].node); } break;
case 26:
# line 169 "flowYacc.y"
{ flowyyval.node = flowyypvt[-1].node;
	      CurrentNode = NULL;
	    } break;
case 29:
# line 181 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->Name = flowyypvt[-0].string;
	      flowyyval.node = CurrentNode;
	    } break;
case 30:
# line 186 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->Class = flowyypvt[-0].string;
	      flowyyval.node = CurrentNode;
	    } break;
case 31:
# line 191 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->Master = (GraphPointer) flowyypvt[-0].string;
	      flowyyval.node = CurrentNode;
	    } break;
case 32:
# line 196 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->Arguments = flowyypvt[-0].list;
	      flowyyval.node = CurrentNode;
	    } break;
case 33:
# line 201 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->Attributes = flowyypvt[-0].list;
	      flowyyval.node = CurrentNode;
	    } break;
case 34:
# line 206 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->InEdges = flowyypvt[-0].list;
	      flowyyval.node = CurrentNode;
	    } break;
case 35:
# line 211 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->OutEdges = flowyypvt[-0].list;
	      flowyyval.node = CurrentNode;
	    } break;
case 36:
# line 216 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->InControl = flowyypvt[-0].list;
	      flowyyval.node = CurrentNode;
	    } break;
case 37:
# line 221 "flowYacc.y"
{ InitializeNode();
	      CurrentNode->OutControl = flowyypvt[-0].list;
	      flowyyval.node = CurrentNode;
	    } break;
case 38:
# line 229 "flowYacc.y"
{ flowyyval.edge = flowyypvt[-1].edge;
	      CurrentEdge = NULL;
	    } break;
case 41:
# line 241 "flowYacc.y"
{ InitializeEdge();
	      CurrentEdge->Name = flowyypvt[-0].string;
	      flowyyval.edge = CurrentEdge;
	    } break;
case 42:
# line 246 "flowYacc.y"
{ InitializeEdge();
	      CurrentEdge->Class = flowyypvt[-0].string;
	      flowyyval.edge = CurrentEdge;
	    } break;
case 43:
# line 251 "flowYacc.y"
{ InitializeEdge();
	      CurrentEdge->Arguments = flowyypvt[-0].list;
	      flowyyval.edge = CurrentEdge;
	    } break;
case 44:
# line 256 "flowYacc.y"
{ InitializeEdge();
	      CurrentEdge->InNodes = flowyypvt[-0].list;
	      flowyyval.edge = CurrentEdge;
	    } break;
case 45:
# line 261 "flowYacc.y"
{ InitializeEdge();
	      CurrentEdge->OutNodes = flowyypvt[-0].list;
	      flowyyval.edge = CurrentEdge;
	    } break;
case 46:
# line 266 "flowYacc.y"
{ InitializeEdge();
	      CurrentEdge->Attributes = flowyypvt[-0].list;
	      flowyyval.edge = CurrentEdge;
	    } break;
case 47:
# line 274 "flowYacc.y"
{ flowyyval.string = flowyypvt[-1].string; } break;
case 48:
# line 276 "flowYacc.y"
{ char Text[WORDLENGTH];
	      sprintf(Text, "%d", flowyypvt[-1].token);
	      flowyyval.string = Intern(Text);
	    } break;
case 49:
# line 284 "flowYacc.y"
{ flowyyval.string = flowyypvt[-1].string; } break;
case 50:
# line 288 "flowYacc.y"
{ flowyyval.string = flowyypvt[-1].string; } break;
case 51:
# line 292 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 52:
# line 297 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 53:
# line 302 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 54:
# line 307 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 55:
# line 312 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 56:
# line 317 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 57:
# line 322 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 58:
# line 327 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 59:
# line 332 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 60:
# line 337 "flowYacc.y"
{ flowyyval.list = flowyypvt[-1].list; } break;
case 62:
# line 342 "flowYacc.y"
{ flowyyval.list = ListAppend(flowyypvt[-0].list, flowyypvt[-1].list); } break;
case 63:
# line 347 "flowYacc.y"
{ flowyyval.list = new_list(CharNode, (pointer) flowyypvt[-0].string); } break;
case 64:
# line 349 "flowYacc.y"
{ flowyyval.list = new_list(IntNode, (pointer) flowyypvt[-0].token); } break;
case 65:
# line 351 "flowYacc.y"
{ flowyyval.list = new_list(NullNode, (pointer) NULL); } break;
case 66:
# line 353 "flowYacc.y"
{ flowyyval.list = new_list(ListNode, (pointer) flowyypvt[-0].list); } break;
case 67:
# line 358 "flowYacc.y"
{ flowyyval.string = Intern(flowyylval.string); } break;
case 68:
# line 363 "flowYacc.y"
{ flowyyval.token = flowyylval.token; } break;
	}
	goto flowyystack;		/* reset registers in driver code */
}
