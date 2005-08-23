
/*  A Bison parser, made from ../../src/ptlang/ptlang.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	DEFSTAR	257
#define	DEFCORONA	258
#define	DEFCORE	259
#define	GALAXY	260
#define	NAME	261
#define	DESC	262
#define	DEFSTATE	263
#define	CORONA	264
#define	CORECATEGORY	265
#define	DOMAIN	266
#define	NUMPORTS	267
#define	NUM	268
#define	VIRTUAL	269
#define	DERIVED	270
#define	ALSODERIVED	271
#define	CONSTRUCTOR	272
#define	DESTRUCTOR	273
#define	STAR	274
#define	ALIAS	275
#define	INPUT	276
#define	OUTPUT	277
#define	INOUT	278
#define	ACCESS	279
#define	INMULTI	280
#define	OUTMULTI	281
#define	INOUTMULTI	282
#define	TYPE	283
#define	DEFAULT	284
#define	CLASS	285
#define	BEGIN	286
#define	SETUP	287
#define	GO	288
#define	WRAPUP	289
#define	TICK	290
#define	CONNECT	291
#define	ID	292
#define	CCINCLUDE	293
#define	HINCLUDE	294
#define	PROTECTED	295
#define	PUBLIC	296
#define	PRIVATE	297
#define	METHOD	298
#define	ARGLIST	299
#define	CODE	300
#define	BODY	301
#define	IDENTIFIER	302
#define	STRING	303
#define	CONSCALLS	304
#define	ATTRIB	305
#define	LINE	306
#define	HTMLDOC	307
#define	VERSION	308
#define	AUTHOR	309
#define	ACKNOWLEDGE	310
#define	COPYRIGHT	311
#define	EXPLANATION	312
#define	SEEALSO	313
#define	LOCATION	314
#define	CODEBLOCK	315
#define	EXECTIME	316
#define	PURE	317
#define	INLINE	318
#define	STATIC	319
#define	HEADER	320
#define	INITCODE	321
#define	START	322
#define	URL	323

#line 1 "../../src/ptlang/ptlang.y"

/************************************************************************
 Version: @(#)ptlang.y	2.103	10/31/99

Copyright (c)1990-1999  The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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

-----------------------------------------------------------------------

Ptolemy "star language" preprocessor.  This version does not support
compiled-in galaxies yet and the language may still change slightly.
Caveat hacker.

Programmer: J. T. Buck and E. A. Lee

6/9/95 tgl: handle default arguments in method arglists correctly.

************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <malloc.h>
#include <stdlib.h>
#ifndef PT_NT4VC
#include <unistd.h> /* for getcwd() */
#endif

/* Symbols for special characters*/
#define LPAR '('
#define RPAR ')'
#define QUOTE '"'
#define ESC '\\'
#define NEWLINE '\n'

/* buffer sizes */
#define BIGBUFSIZE 200000
#define MEDBUFSIZE 50000
#define SMALLBUFSIZE 512

/* number of code blocks allowed */
#define NUMCODEBLOCKS 100

#define FLEN 256
/* number of include files */
#define NINC 30
/* Number of non-star classes we can derive from */
#define NALSODERIVED 10
/* number of see alsos */
#define NSEE 30
#define NSTR 20

/* chars allowed in "identifier" */
#define IDENTCHAR(c) (isalnum(c) || c == '.' || c == '_')


int versionMode = 0;		/* flag if we are processing a version field */

/* chars allowed in "url" */
int urlchar(c)
int c;
{
	/* This is an evil hack that allows RCS version keywords in
	 * the version field.
	 * RCS version keywords use $, so when we are parsing the inside
	 * of a version field, urlchar() does not match $ as a url
	 * keyword.  If we are not parsing the inside of a version field,
	 * then $ is matched as a url keyword.
	 * For more information, see the ptolemy-hackers messages 
	 * starting on 9/2/97. -cxh
	 */
	if (versionMode)
		return (c == ':' || c == '/');
	else
		return (c == ':' || c == '/' || c == '$');
}

char yytext[BIGBUFSIZE];	/* lexical analysis buffer */
int yyline = 1;			/* current input line */
int bodyMode = 0;		/* special lexan mode flag to read bodies  */
int docMode = 0;		/* flag document bodies  */
int descMode = 0;		/* flag descriptor bodies  */
int codeMode = 0;		/* flag code block bodies  */
int htmlOnly = 0;		/* If 1, then generate only .htm files */

FILE* yyin;			/* stream to read from */

char* progName = "ptlang";	/* program name */
int nerrs = 0;			/* # syntax errors detected */

char* blockID;			/* identifier for code blocks */
char* blockArg;
char* codeBlocks[NUMCODEBLOCKS];
char* codeBlockNames[NUMCODEBLOCKS];
char* codeBlockLines[NUMCODEBLOCKS];
char* codeBlockArgs[NUMCODEBLOCKS];
int numBlocks = 0;

/* scratch buffers */
char str1[SMALLBUFSIZE*2];
char str2[SMALLBUFSIZE*2];
char consStuff[BIGBUFSIZE];
char publicMembers[MEDBUFSIZE];
char protectedMembers[MEDBUFSIZE];
char privateMembers[MEDBUFSIZE];
char inputDescriptions[MEDBUFSIZE];
char outputDescriptions[MEDBUFSIZE];
char inoutDescriptions[MEDBUFSIZE];
char stateDescriptions[MEDBUFSIZE];
char inputDescHTML[MEDBUFSIZE];
char outputDescHTML[MEDBUFSIZE];
char inoutDescHTML[MEDBUFSIZE];
char stateDescHTML[MEDBUFSIZE];
char ccCode[BIGBUFSIZE];
char hCode[BIGBUFSIZE];
char miscCode[BIGBUFSIZE];
char consCalls[BIGBUFSIZE];


/* state classes */
#define T_INT 0
#define T_FLOAT 1
#define T_COMPLEX 2
#define T_STRING 3
#define T_INTARRAY 4
#define T_FLOATARRAY 5
#define T_COMPLEXARRAY 6
#define T_FIX 7
#define T_FIXARRAY 8
#define T_STRINGARRAY 9
#define T_PRECISION 10
#define NSTATECLASSES 11

#define M_PURE 1
#define M_VIRTUAL 2
#define M_INLINE 4
#define M_STATIC 8

/* note: names must match order of symbols above */
char* stateClasses[] = {
"IntState", "FloatState", "ComplexState", "StringState",
"IntArrayState", "FloatArrayState", "ComplexArrayState",
"FixState", "FixArrayState", "StringArrayState", "PrecisionState"
};

/* bookkeeping for state include files */
int stateMarks[NSTATECLASSES];

/* external functions */
char* save();			/* duplicate a string */
char* savelineref();
char* ctime();
time_t time();
void exit();

/* forward declarations for functions */

char* whichMembers();
char* checkArgs();
char* stripQuotes();
char* portDataType();
int   stateTypeClass(), lookup(), yyparse(), yylex(), unescape();
void clearDefs(), clearStateDefs(), addMembers(), genState(), describeState(),
     initPort(), genPort(), describePort(), clearMethodDefs(), wrapMethod(),
     genInstance(), genStdProto(), yyerror(), yyerr2(), cvtCodeBlockExpr(),
     cvtCodeBlock(), genCodeBlock(), cvtMethod(), genMethod(), genDef(),
     yywarn(), mismatch(), genAlias(), stripDefaultArgs(),
     checkIncludes(), checkSeeAlsos(), seeAlsoGenerate();

char* inputFile;		/* input file name */
char* idBlock;			/* ID block */
char* objName;			/* name of star or galaxy class being decld  */
char* objVer;			/* sccs version number */
char* objDate;			/* date of last update */
char* objDesc;			/* descriptor of star or galaxy */
char* objAuthor;		/* author of star or galaxy */
char* objAcknowledge;		/* acknowledgements (previous authors) */
char* objCopyright;		/* copyright */
char* objExpl;			/* long explanation for troff formatting */
char* objHTMLdoc;		/* long explanation for HTML formatting */
char* objLocation;		/* location string */
char* coreCategory;		/* core category (i.e. Fix) for this Core */
char* coronaName;		/* name of the Corona of this Core */
int   coreDef;			/* true if obj is a Core */
int   coronaDef;		/* true if obj is a Corona */
int   galDef;			/* true if obj is a galaxy */
char* domain;			/* domain of object (if star) */
char* portName;			/* name of porthole */
char* portType;			/* dataType of porthole */
char* portInherit;		/* porthole for inheritTypeFrom */
char* portNum;			/* expr giving # of tokens */
char* portDesc;			/* port descriptor */
int   portDir;			/* 0=input, 1=output, 2=inout */
int   portMulti;		/* true if porthole is multiporthole */
char* portAttrib;		/* attributes for porthole */
char* stateName;		/* name of state */
char* stateClass;		/* class of state */
char* stateDef;			/* default value of state */
char* stateDesc;		/* descriptor for state */
char* stateAttrib;		/* attributes for state */
char* instName;			/* star instance within galaxy */
char* instClass;		/* class of star instance */
char* methodName;		/* name of user method */
char* methodArgs;		/* arglist of user method */
char* methodAccess;		/* protection of user method */
char* methodType;		/* return type of user method */
char* methodCode;		/* body of user method */
int   methodMode;		/* modifier flags for a method */
int   pureFlag;			/* if true, class is abstract */
char* galPortName;		/* name of galaxy port */

/* Codes for "standard methods" (go, begin, etc.)
 * To add more,
 *
 * 1. Add a your method to the end of the list below
 *	e.g., #define C_MYMETHOD 10
 * 2. Modify #define N_FORMS to be the (new) number of standard methods
 *	e.g., #define N_FORMS 11 
 * 3. Add the return type to the end of the codeType array below
 *	e.g., char* codeType[] = { ..., "void "};
 * 4. Add the method name to the end of the codeFuncName array below
 *	e.g., char* codeFuncName[] = { ..., "myMethod" };
 * 5. Add the new token to the token list (one of the %token lines)
 *	e.g., %token MYMETHOD
 * 6. Add the token to the others in the stdkey2: rule
 *	 e.g., | MYMETHOD { methKey = C_MYMETHOD; }
 * 7. Add the token to the keyword: rule
 *	e.g., |MYMETHOD
 * 8. Add your token to struct tentry keyTable[] =
 *	e.g., {"myMethod", MYMETHOD},
 *
 * The C_CONS is processed specially.
 * C_SETUP and after are protected; rest are public.
 */
#define C_CONS 0
#define C_EXECTIME 1
#define C_WRAPUP 2
#define C_INITCODE 3
#define C_BEGIN 4
#define C_DEST 5
#define C_SETUP 6
#define C_GO 7
#define C_TICK 8

#define N_FORMS 9

char* codeBody[N_FORMS];		/* code bodies for each entity */
int inlineFlag[N_FORMS];		/* marks which are to be inline */
char destNameBuf[MEDBUFSIZE];		/* storage for destructor name */

/* types and names of standard member funcs */
char* codeType[] = {"","int ","void ","void ","void ","","void ","void ", "void "};
char* codeFuncName[] = {
"","myExecTime","wrapup","initCode","begin",destNameBuf,"setup","go","tick"};

int methKey;			/* signals which of the standard funcs */

#define consCode codeBody[C_CONS]	/* extra constructor code */

char* hInclude[NINC];		/* include files in .h file */
int   nHInclude;		/* number of such files */
char* ccInclude[NINC];		/* include files in .cc file */
int   nCcInclude;		/* number of such files */
char* derivedFrom;		/* class obj is derived from */
char* alsoDerivedFrom[NALSODERIVED]; /* Other non star classes derived from */
int   nAlsoDerivedFrom;		/* number of such files */
char* seeAlsoList[NSEE];	/* list of pointers to other manual sections */
int   nSeeAlso;			/* number of such pointers */

/* all tokens with values are type char *.  Keyword tokens
 * have their names as values.
 */
typedef char * STRINGVAL;
#define YYSTYPE STRINGVAL

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		429
#define	YYFLAG		-32768
#define	YYNTBASE	80

#define YYTRANSLATE(x) ((unsigned)(x) <= 323 ? yytranslate[x] : 151)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    76,    72,    78,     2,     2,    75,
    77,     2,     2,    73,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    79,     2,     2,    74,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    71,     2,    70,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     8,     9,    12,    15,    18,    21,    24,
    27,    28,    34,    35,    41,    42,    48,    49,    55,    57,
    60,    62,    65,    67,    70,    75,    76,    82,    83,    88,
    89,    94,    95,   100,   101,   106,   107,   112,   113,   118,
   119,   124,   129,   131,   132,   138,   142,   145,   148,   151,
   156,   161,   166,   171,   176,   179,   182,   184,   188,   191,
   194,   198,   202,   203,   205,   206,   208,   212,   214,   216,
   218,   220,   222,   224,   226,   228,   230,   232,   242,   249,
   262,   274,   278,   286,   296,   299,   304,   311,   313,   315,
   317,   322,   324,   329,   334,   339,   340,   349,   351,   355,
   356,   359,   362,   364,   367,   368,   371,   376,   381,   386,
   391,   394,   397,   400,   403,   405,   407,   409,   411,   414,
   416,   418,   419,   425,   430,   432,   434,   439,   444,   446,
   448,   450,   455,   460,   462,   464,   466,   468,   470,   472,
   474,   477,   482,   487,   493,   498,   501,   502,   507,   509,
   512,   517,   522,   527,   528,   533,   536,   539,   541,   543,
   545,   547,   550,   552,   554,   556,   559,   564,   569,   571,
   574,   579,   580,   584,   585,   589,   593,   594,   598,   599,
   601,   603,   605,   607,   609,   611,   613,   615,   617,   619,
   621,   623,   625,   627,   629,   631,   633,   635,   637,   639,
   641,   643,   645,   647,   649,   651,   653,   655,   657,   659,
   661,   663,   665,   667,   669,   671,   673,   675,   677,   679,
   681,   683,   685,   687,   689,   691,   693,   695,   697,   699,
   701,   703,   705,   707,   709,   711,   713,   715,   717
};

static const short yyrhs[] = {    81,
     0,    81,    70,     0,    81,   149,     0,     0,    81,    83,
     0,    81,    85,     0,    81,    87,     0,    81,    89,     0,
    82,    47,     0,    38,    71,     0,     0,     3,    84,    71,
    91,    70,     0,     0,     4,    86,    71,   125,    70,     0,
     0,     5,    88,    71,    92,    70,     0,     0,     6,    90,
    71,    93,    70,     0,   111,     0,    91,   111,     0,   128,
     0,    92,   128,     0,   129,     0,    93,   129,     0,     7,
    71,   149,    70,     0,     0,    54,    71,    95,   110,    70,
     0,     0,     8,    71,    96,    47,     0,     0,    55,    71,
    97,    47,     0,     0,    56,    71,    98,    47,     0,     0,
    57,    71,    99,    47,     0,     0,    60,    71,   100,    47,
     0,     0,    58,    71,   101,    47,     0,     0,    53,    71,
   102,    47,     0,    59,    71,   146,    70,     0,    94,     0,
     0,     9,   104,    71,   134,    70,     0,   106,   108,    47,
     0,   123,    47,     0,   121,    47,     0,   122,    47,     0,
   105,    71,   118,    70,     0,    39,    71,   145,    70,     0,
    40,    71,   147,    70,     0,    39,    71,     1,    70,     0,
    40,    71,     1,    70,     0,   117,    47,     0,     1,    70,
     0,    44,     0,    63,   107,    44,     0,    15,    44,     0,
    65,    44,     0,    64,    65,    44,     0,    64,   107,    44,
     0,     0,    64,     0,     0,    15,     0,   109,   119,    71,
     0,    18,     0,    19,     0,    32,     0,    33,     0,    34,
     0,    35,     0,    36,     0,    67,     0,    62,     0,    68,
     0,    72,    69,    48,    72,    72,    69,    69,    69,    72,
     0,    72,    48,    72,    72,    48,    72,     0,    72,    69,
    48,    73,    48,    48,    69,    69,    48,    48,    48,    72,
     0,    72,    69,    48,    73,    48,    48,    69,    69,    48,
    48,    72,     0,    72,    48,    72,     0,    74,    75,    76,
    77,    48,    48,    69,     0,    74,    75,    76,    77,    48,
    48,    48,    48,    48,     0,    48,    69,     0,    48,    48,
    48,    48,     0,    78,    48,    78,    78,    48,    78,     0,
     1,     0,   103,     0,   112,     0,   130,    71,   131,    70,
     0,   113,     0,    12,    71,   149,    70,     0,    16,    71,
   149,    70,     0,    17,    71,   115,    70,     0,     0,    61,
    75,   149,   116,    77,    71,   114,    47,     0,    48,     0,
   115,    73,    48,     0,     0,    73,    49,     0,    50,    71,
     0,   120,     0,   118,   120,     0,     0,    75,    77,     0,
     7,    71,   149,    70,     0,    45,    71,   149,    70,     0,
    29,    71,   149,    70,     0,    25,    71,   124,    70,     0,
   121,    47,     0,    46,    71,     0,    66,    71,     0,   124,
    71,     0,    42,     0,    41,     0,    43,     0,   126,     0,
   125,   126,     0,    94,     0,   112,     0,     0,     9,   127,
    71,   134,    70,     0,   130,    71,   131,    70,     0,   103,
     0,   112,     0,    10,    71,   149,    70,     0,    11,    71,
   149,    70,     0,   103,     0,   113,     0,   103,     0,    20,
    71,   141,    70,     0,    21,    71,   143,    70,     0,    22,
     0,    23,     0,    24,     0,    26,     0,    27,     0,    28,
     0,   132,     0,   131,   132,     0,     7,    71,   149,    70,
     0,    29,    71,   149,    70,     0,    29,    71,    79,   149,
    70,     0,    14,    71,   140,    70,     0,   137,    47,     0,
     0,     8,    71,   133,    47,     0,   135,     0,   134,   135,
     0,     7,    71,   149,    70,     0,    29,    71,   149,    70,
     0,    30,    71,   138,    70,     0,     0,     8,    71,   136,
    47,     0,   137,    47,     0,    51,    71,     0,   139,     0,
    48,     0,   150,     0,    49,     0,   139,    49,     0,    48,
     0,    49,     0,   142,     0,   141,   142,     0,     7,    71,
   149,    70,     0,    31,    71,   149,    70,     0,   144,     0,
   143,   144,     0,     7,    71,   149,    70,     0,     0,   145,
   148,    49,     0,     0,   146,   148,    48,     0,   146,   148,
    69,     0,     0,   147,   148,    49,     0,     0,    73,     0,
   150,     0,    48,     0,    49,     0,     3,     0,     4,     0,
     5,     0,     6,     0,    11,     0,    10,     0,     7,     0,
     8,     0,     9,     0,    12,     0,    13,     0,    16,     0,
    17,     0,    18,     0,    19,     0,    20,     0,    21,     0,
    22,     0,    23,     0,    24,     0,    26,     0,    27,     0,
    28,     0,    29,     0,    30,     0,    32,     0,    33,     0,
    34,     0,    35,     0,    36,     0,    37,     0,    39,     0,
    40,     0,    41,     0,    42,     0,    43,     0,    44,     0,
    45,     0,    46,     0,    25,     0,    55,     0,    56,     0,
    54,     0,    57,     0,    58,     0,    53,     0,    68,     0,
    59,     0,    60,     0,    61,     0,    62,     0,    63,     0,
    64,     0,    66,     0,    67,     0,    65,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   310,   312,   315,   319,   321,   322,   323,   324,   325,   328,
   331,   333,   337,   340,   344,   347,   356,   357,   360,   361,
   364,   365,   369,   370,   374,   376,   377,   379,   380,   383,
   384,   387,   388,   391,   392,   395,   396,   399,   400,   403,
   404,   407,   411,   413,   414,   417,   422,   423,   427,   431,
   432,   433,   434,   435,   436,   444,   449,   450,   451,   452,
   453,   454,   461,   462,   466,   467,   471,   475,   477,   478,
   479,   480,   481,   482,   483,   484,   485,   492,   500,   513,
   521,   529,   541,   550,   560,   566,   572,   585,   589,   591,
   592,   594,   596,   598,   599,   602,   613,   619,   620,   623,
   624,   627,   630,   632,   636,   637,   641,   643,   644,   645,
   646,   650,   655,   660,   666,   667,   668,   671,   672,   675,
   677,   678,   679,   680,   682,   687,   689,   690,   691,   692,
   697,   699,   700,   710,   712,   713,   714,   715,   716,   719,
   721,   724,   726,   727,   728,   729,   730,   730,   735,   737,
   740,   742,   746,   747,   748,   751,   754,   757,   758,   762,
   768,   769,   777,   778,   785,   787,   790,   792,   795,   797,
   800,   805,   806,   811,   812,   814,   818,   819,   823,   824,
   829,   830,   832,   836,   836,   836,   836,   837,   837,   837,
   837,   837,   837,   837,   837,   838,   838,   838,   838,   838,
   839,   839,   839,   839,   839,   839,   840,   841,   841,   841,
   841,   841,   841,   841,   841,   841,   841,   841,   842,   842,
   842,   842,   842,   842,   842,   842,   842,   843,   843,   843,
   844,   844,   844,   844,   844,   844,   844,   844,   844
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DEFSTAR",
"DEFCORONA","DEFCORE","GALAXY","NAME","DESC","DEFSTATE","CORONA","CORECATEGORY",
"DOMAIN","NUMPORTS","NUM","VIRTUAL","DERIVED","ALSODERIVED","CONSTRUCTOR","DESTRUCTOR",
"STAR","ALIAS","INPUT","OUTPUT","INOUT","ACCESS","INMULTI","OUTMULTI","INOUTMULTI",
"TYPE","DEFAULT","CLASS","BEGIN","SETUP","GO","WRAPUP","TICK","CONNECT","ID",
"CCINCLUDE","HINCLUDE","PROTECTED","PUBLIC","PRIVATE","METHOD","ARGLIST","CODE",
"BODY","IDENTIFIER","STRING","CONSCALLS","ATTRIB","LINE","HTMLDOC","VERSION",
"AUTHOR","ACKNOWLEDGE","COPYRIGHT","EXPLANATION","SEEALSO","LOCATION","CODEBLOCK",
"EXECTIME","PURE","INLINE","STATIC","HEADER","INITCODE","START","URL","'}'",
"'{'","'$'","','","'@'","'('","'#'","')'","'%'","'='","full_file","file","id",
"stardef","@1","coronadef","@2","coredef","@3","galdef","@4","starlist","corelist",
"gallist","commonitem","@5","@6","@7","@8","@9","@10","@11","@12","sgitem","@13",
"method","inl","vopt","stdmethkey","stdkey2","version","staritem","domainorderived",
"codeblock","@14","alsoderivedlist","cbargs","conscalls","methlist","optp","methitem",
"code","header","members","protkey","coronalist","coronaitem","@15","coreitem",
"galitem","portkey","portlist","portitem","@16","dstatelist","dstateitem","@17",
"attrib","defval","stringseq","expval","starinstlist","starinstitem","aliaslist",
"aliasitem","cclist","seealso","hlist","optcomma","ident","keyword", NULL
};
#endif

static const short yyr1[] = {     0,
    80,    80,    80,    81,    81,    81,    81,    81,    81,    82,
    84,    83,    86,    85,    88,    87,    90,    89,    91,    91,
    92,    92,    93,    93,    94,    95,    94,    96,    94,    97,
    94,    98,    94,    99,    94,   100,    94,   101,    94,   102,
    94,    94,   103,   104,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   103,   103,   103,   105,   105,   105,   105,
   105,   105,   106,   106,   107,   107,   108,   109,   109,   109,
   109,   109,   109,   109,   109,   109,   109,   110,   110,   110,
   110,   110,   110,   110,   110,   110,   110,   110,   111,   111,
   111,   111,   112,   112,   112,   114,   113,   115,   115,   116,
   116,   117,   118,   118,   119,   119,   120,   120,   120,   120,
   120,   121,   122,   123,   124,   124,   124,   125,   125,   126,
   126,   127,   126,   126,   126,   128,   128,   128,   128,   128,
   129,   129,   129,   130,   130,   130,   130,   130,   130,   131,
   131,   132,   132,   132,   132,   132,   133,   132,   134,   134,
   135,   135,   135,   136,   135,   135,   137,   138,   138,   138,
   139,   139,   140,   140,   141,   141,   142,   142,   143,   143,
   144,   145,   145,   146,   146,   146,   147,   147,   148,   148,
   149,   149,   149,   150,   150,   150,   150,   150,   150,   150,
   150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
   150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
   150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
   150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
   150,   150,   150,   150,   150,   150,   150,   150,   150
};

static const short yyr2[] = {     0,
     1,     2,     2,     0,     2,     2,     2,     2,     2,     2,
     0,     5,     0,     5,     0,     5,     0,     5,     1,     2,
     1,     2,     1,     2,     4,     0,     5,     0,     4,     0,
     4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
     4,     4,     1,     0,     5,     3,     2,     2,     2,     4,
     4,     4,     4,     4,     2,     2,     1,     3,     2,     2,
     3,     3,     0,     1,     0,     1,     3,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     9,     6,    12,
    11,     3,     7,     9,     2,     4,     6,     1,     1,     1,
     4,     1,     4,     4,     4,     0,     8,     1,     3,     0,
     2,     2,     1,     2,     0,     2,     4,     4,     4,     4,
     2,     2,     2,     2,     1,     1,     1,     1,     2,     1,
     1,     0,     5,     4,     1,     1,     4,     4,     1,     1,
     1,     4,     4,     1,     1,     1,     1,     1,     1,     1,
     2,     4,     4,     5,     4,     2,     0,     4,     1,     2,
     4,     4,     4,     0,     4,     2,     2,     1,     1,     1,
     1,     2,     1,     1,     1,     2,     4,     4,     1,     2,
     4,     0,     3,     0,     3,     3,     0,     3,     0,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     4,
     0,     1,     0,    10,    11,    13,    15,    17,   190,   191,
   192,   189,   188,   193,   194,   195,   196,   197,   198,   199,
   200,   201,   202,   203,   223,   204,   205,   206,   207,   208,
   209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
   219,   220,   221,   222,   182,   183,   229,   226,   224,   225,
   227,   228,   231,   232,   233,   234,   235,   236,   239,   237,
   238,   230,     2,     5,     6,     7,     8,     3,   181,     9,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    44,     0,     0,     0,     0,   134,   135,   136,   137,
   138,   139,     0,     0,   116,   115,   117,    57,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    65,
    64,     0,     0,     0,    43,    89,     0,     0,    19,    90,
    92,     0,     0,     0,     0,     0,     0,    44,    43,   125,
   121,     0,   118,     0,     0,     0,     0,   129,   126,   130,
    21,     0,     0,     0,   131,    23,    56,     0,    28,     0,
     0,    59,     0,     0,     0,     0,   112,   102,    40,    26,
    30,    32,    34,    38,   174,    36,     0,    66,     0,     0,
     0,    60,   113,    12,    20,     0,    68,    69,    70,    71,
    72,    73,    74,    76,    75,    77,     0,   105,    55,    48,
    49,    47,   114,     0,     0,    14,   119,     0,     0,     0,
    16,    22,     0,     0,    18,    24,   184,   185,   186,   187,
     0,     0,     0,     0,     0,    98,     0,     0,   179,     0,
   179,     0,     0,     0,     0,     0,     0,   179,     0,   100,
    58,    61,    62,     0,     0,     0,     0,     0,   103,     0,
    46,     0,     0,     0,     0,     0,     0,     0,     0,   140,
     0,     0,     0,     0,     0,     0,     0,     0,   165,     0,
     0,   169,    25,    29,     0,     0,     0,     0,     0,   149,
     0,    93,    94,    95,     0,    53,    51,   180,     0,    54,
    52,     0,    41,    88,     0,     0,     0,     0,     0,    31,
    33,    35,    39,    42,     0,    37,     0,     0,     0,     0,
     0,     0,    50,   104,   111,   106,    67,     0,   147,     0,
     0,   157,    91,   141,   146,     0,   124,   127,   128,     0,
     0,   132,   166,     0,   133,   170,     0,   154,     0,     0,
    45,   150,   156,    99,   173,   178,     0,    85,     0,     0,
     0,     0,    27,   175,   176,   101,     0,     0,     0,     0,
     0,     0,     0,   163,   164,     0,     0,     0,   123,     0,
     0,     0,     0,     0,     0,   159,   161,     0,   158,   160,
     0,    82,     0,     0,     0,    96,   107,   110,   109,   108,
   142,   148,   145,     0,   143,   167,   168,   171,   151,   155,
   152,   153,   162,    86,     0,     0,     0,     0,     0,     0,
   144,     0,     0,     0,     0,     0,    97,    79,     0,     0,
     0,    87,     0,     0,     0,    83,     0,     0,     0,    78,
     0,    84,     0,     0,    81,    80,     0,     0,     0
};

static const short yydefgoto[] = {   427,
     2,     3,    64,    71,    65,    72,    66,    73,    67,    74,
   114,   137,   144,   115,   223,   212,   224,   225,   226,   229,
   227,   222,   116,   150,   117,   118,   169,   187,   188,   289,
   119,   120,   121,   400,   217,   298,   122,   238,   243,   239,
   123,   124,   125,   126,   132,   133,   195,   141,   146,   127,
   249,   250,   353,   269,   270,   364,   251,   368,   369,   356,
   258,   259,   261,   262,   219,   228,   221,   279,    68,    69
};

static const short yypact[] = {    28,
     0,   466,    39,-32768,   111,   113,   129,   130,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    93,    97,   100,   104,   534,   602,   670,   738,    18,   106,
   107,-32768,   109,   142,   114,   116,-32768,-32768,-32768,-32768,
-32768,-32768,   117,   118,-32768,-32768,-32768,-32768,   119,   120,
   121,   122,   123,   124,   125,   137,   154,   161,   166,   229,
    90,   201,   175,   262,-32768,-32768,   193,   921,-32768,-32768,
-32768,   110,   220,   221,   228,   205,   211,-32768,-32768,-32768,
-32768,   330,-32768,   212,   222,   236,    82,-32768,-32768,-32768,
-32768,   239,   240,   398,-32768,-32768,-32768,   804,-32768,   263,
   804,-32768,   804,   251,     7,    11,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   804,-32768,   256,   289,
   292,-32768,-32768,-32768,-32768,    16,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   293,   266,-32768,-32768,
-32768,-32768,-32768,   258,   272,-32768,-32768,   258,   804,   804,
-32768,-32768,    27,   337,-32768,-32768,-32768,-32768,-32768,-32768,
   280,   304,   284,   285,   290,-32768,   -52,   291,    86,   297,
    88,   312,     1,   321,   328,   331,   332,    96,   334,   309,
-32768,-32768,-32768,   306,   320,   333,   338,    -3,-32768,   354,
-32768,   325,   339,   340,   341,   343,   344,   349,    -1,-32768,
   356,   284,     2,   351,   352,   353,   355,     4,-32768,   357,
     8,-32768,-32768,-32768,   358,   364,   365,   372,    17,-32768,
   361,-32768,-32768,-32768,   375,-32768,-32768,-32768,   376,-32768,
-32768,   378,-32768,-32768,   -21,   -10,   370,   399,   379,-32768,
-32768,-32768,-32768,-32768,    -4,-32768,   397,   373,   804,   141,
   804,   804,-32768,-32768,-32768,-32768,-32768,   804,-32768,    54,
   194,-32768,-32768,-32768,-32768,    25,-32768,-32768,-32768,   804,
   804,-32768,-32768,   804,-32768,-32768,   804,-32768,   804,   870,
-32768,-32768,-32768,-32768,-32768,-32768,   411,-32768,   395,   432,
   405,   419,-32768,-32768,-32768,-32768,   433,   443,   446,   447,
   448,   467,   491,-32768,-32768,   469,   804,   470,-32768,   474,
   475,   477,   478,   507,   485,-32768,-32768,   489,   514,-32768,
   516,   493,    47,   494,   501,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   502,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   533,   510,   535,   537,   538,   557,
-32768,   536,   543,   558,   559,   527,-32768,-32768,   544,   546,
    64,-32768,   547,   553,   575,-32768,   555,   583,   584,-32768,
   585,-32768,    -9,   567,-32768,-32768,   640,   647,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   -62,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    30,-32768,-32768,-32768,   539,-32768,-32768,-32768,
   540,    33,   -41,-32768,-32768,-32768,-32768,-32768,-32768,   413,
  -156,-32768,-32768,   363,-32768,   517,-32768,   554,   509,   -47,
   476,  -122,-32768,   420,  -252,-32768,  -212,-32768,-32768,-32768,
-32768,   415,-32768,   414,-32768,-32768,-32768,  -191,  -148,   346
};


#define	YYLAST		989


static const short yytable[] = {   211,
   271,   284,   214,   234,   215,   244,   245,   218,   244,   245,
   256,   220,   246,   129,   260,   246,   332,   274,   230,   240,
   275,   235,   234,   265,   266,   236,   337,   247,   134,   282,
   247,   265,   266,   256,   257,   140,   295,   339,   424,   271,
   235,   237,    99,   344,   236,   267,   268,   338,   285,   248,
   254,   255,   248,   267,   268,  -172,   271,   257,   340,  -177,
   237,    99,   425,   332,   345,     1,   303,   248,   313,   129,
     4,   317,   286,   322,   287,   248,  -172,   325,   288,  -172,
  -177,   240,    79,  -177,   134,    70,   331,   147,    80,    81,
    82,   135,   136,    83,   359,   140,    84,    85,    86,   -63,
   -63,   354,   355,   271,   168,   130,   138,   145,   131,   139,
  -184,   415,  -185,   -63,   -63,   -63,   -63,   -63,   396,   397,
    93,    94,    95,    96,    97,    98,   314,    99,  -186,  -187,
   314,   100,   416,   -65,   101,   102,   103,   104,   105,   106,
   107,   108,   109,   -63,   110,   111,   112,   113,   -63,   -63,
   348,   201,   350,   351,   170,   277,   189,   281,   278,   352,
   278,   130,   358,    75,   131,   294,   138,    76,   278,   139,
    77,   360,   361,   145,    78,   362,   148,   149,   363,   151,
   365,    95,    96,    97,   153,   152,   154,   155,   156,   157,
   158,   159,   160,   161,   162,   163,   207,   208,   209,   210,
     9,    10,    11,    12,    13,    14,    15,   164,   384,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,   165,    31,    32,    33,    34,    35,
    36,   166,    37,    38,    39,    40,    41,    42,    43,    44,
   167,    45,    46,   168,   172,   173,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    79,   176,   244,   245,   190,   191,    80,    81,
    82,   246,   357,    83,   192,   193,    84,    85,    86,   -63,
   -63,   194,   198,    87,    88,    89,   247,    90,    91,    92,
   265,   266,   199,   -63,   -63,   -63,   -63,   -63,   216,   231,
    93,    94,    95,    96,    97,    98,   200,    99,   248,   203,
   204,   100,   267,   268,   101,   102,   103,   104,   105,   106,
   107,   108,   109,   -63,   110,   111,   112,   113,   -63,   -63,
    79,   174,   232,   213,   248,   233,    80,    81,   128,   241,
   242,    83,   252,   260,    84,    85,    86,   -63,   -63,   263,
   264,    87,    88,    89,   272,    90,    91,    92,   283,   273,
   276,   -63,   -63,   -63,   -63,   -63,   280,   290,    93,    94,
    95,    96,    97,    98,   291,    99,   299,   292,   293,   100,
   296,   297,   101,   102,   103,   104,   105,   106,   107,   108,
   300,   -63,   110,   111,   112,   113,   -63,   -63,    79,   196,
   305,   306,   315,   301,    80,    81,    82,   333,   302,   307,
   308,   309,    84,   310,   311,   -63,   -63,   142,   143,   312,
   318,   319,   334,   320,   335,   321,   336,   324,   327,   -63,
   -63,   -63,   -63,   -63,   328,   329,    93,    94,    95,    96,
    97,    98,   330,    99,   341,   346,   342,   100,   343,   347,
   101,   102,   103,   104,   105,   106,   107,   108,   371,   -63,
   110,   111,   112,   113,   -63,   -63,   372,   205,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,   373,
   374,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,   375,    31,    32,    33,
    34,    35,    36,   376,    37,    38,    39,    40,    41,    42,
    43,    44,   377,    45,    46,   378,   379,   380,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    79,    63,   381,   382,   383,   385,
    80,    81,    82,   386,   387,    83,   388,   389,    84,    85,
    86,   -63,   -63,   390,   391,    87,    88,    89,   392,    90,
    91,    92,   393,   394,   395,   -63,   -63,   -63,   -63,   -63,
   398,   401,    93,    94,    95,    96,    97,    98,   399,    99,
   402,   403,   404,   100,   405,   406,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   -63,   110,   111,   112,   113,
   -63,   -63,    79,   407,   412,   410,   411,   408,    80,    81,
   128,   409,   413,    83,   414,   417,    84,    85,    86,   -63,
   -63,   418,   419,    87,    88,    89,   420,    90,    91,    92,
   421,   422,   423,   -63,   -63,   -63,   -63,   -63,   426,   428,
    93,    94,    95,    96,    97,    98,   429,    99,   197,   171,
   304,   100,   206,   175,   101,   102,   103,   104,   105,   106,
   107,   108,   349,   -63,   110,   111,   112,   113,   -63,   -63,
    79,   316,   323,   253,   326,   370,    80,    81,    82,   135,
   136,    83,     0,     0,    84,    85,    86,   -63,   -63,     0,
   202,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   -63,   -63,   -63,   -63,   -63,     0,     0,    93,    94,
    95,    96,    97,    98,     0,    99,     0,     0,     0,   100,
     0,     0,   101,   102,   103,   104,   105,   106,   107,   108,
   109,   -63,   110,   111,   112,   113,   -63,   -63,    79,     0,
     0,     0,     0,     0,    80,    81,    82,     0,     0,     0,
     0,     0,    84,     0,     0,   -63,   -63,   142,   143,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   -63,
   -63,   -63,   -63,   -63,     0,     0,    93,    94,    95,    96,
    97,    98,     0,    99,     0,     0,     0,   100,     0,     0,
   101,   102,   103,   104,   105,   106,   107,   108,     0,   -63,
   110,   111,   112,   113,   -63,   -63,   207,   208,   209,   210,
     9,    10,    11,    12,    13,    14,    15,     0,     0,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,     0,    31,    32,    33,    34,    35,
    36,     0,    37,    38,    39,    40,    41,    42,    43,    44,
     0,    45,    46,     0,     0,     0,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,   207,   208,   209,   210,     9,    10,    11,    12,
    13,    14,    15,     0,     0,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
     0,    31,    32,    33,    34,    35,    36,     0,    37,    38,
    39,    40,    41,    42,    43,    44,     0,   366,   367,     0,
     0,     0,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,   177,   178,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   179,   180,   181,   182,   183,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   184,     0,     0,     0,     0,   185,   186
};

static const short yycheck[] = {   148,
   213,     1,   151,     7,   153,     7,     8,     1,     7,     8,
     7,     1,    14,    76,     7,    14,   269,    70,   167,   176,
    73,    25,     7,     7,     8,    29,    48,    29,    76,   221,
    29,     7,     8,     7,    31,    77,   228,    48,    48,   252,
    25,    45,    46,    48,    29,    29,    30,    69,    48,    51,
   199,   200,    51,    29,    30,    49,   269,    31,    69,    49,
    45,    46,    72,   316,    69,    38,    70,    51,    70,   132,
    71,    70,    72,    70,    74,    51,    70,    70,    78,    73,
    70,   238,     1,    73,   132,    47,    70,    70,     7,     8,
     9,    10,    11,    12,    70,   137,    15,    16,    17,    18,
    19,    48,    49,   316,    15,    76,    77,    78,    76,    77,
     0,    48,     0,    32,    33,    34,    35,    36,    72,    73,
    39,    40,    41,    42,    43,    44,   249,    46,     0,     0,
   253,    50,    69,    44,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
   299,    70,   301,   302,    65,    70,    47,    70,    73,   308,
    73,   132,   311,    71,   132,    70,   137,    71,    73,   137,
    71,   320,   321,   144,    71,   324,    71,    71,   327,    71,
   329,    41,    42,    43,    71,    44,    71,    71,    71,    71,
    71,    71,    71,    71,    71,    71,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    71,   357,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    71,    32,    33,    34,    35,    36,
    37,    71,    39,    40,    41,    42,    43,    44,    45,    46,
    75,    48,    49,    15,    44,    71,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,     1,    71,     7,     8,    47,    47,     7,     8,
     9,    14,    79,    12,    47,    71,    15,    16,    17,    18,
    19,    71,    71,    22,    23,    24,    29,    26,    27,    28,
     7,     8,    71,    32,    33,    34,    35,    36,    48,    44,
    39,    40,    41,    42,    43,    44,    71,    46,    51,    71,
    71,    50,    29,    30,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
     1,    70,    44,    71,    51,    44,     7,     8,     9,    47,
    75,    12,    71,     7,    15,    16,    17,    18,    19,    70,
    47,    22,    23,    24,    70,    26,    27,    28,    47,    70,
    70,    32,    33,    34,    35,    36,    70,    47,    39,    40,
    41,    42,    43,    44,    47,    46,    71,    47,    47,    50,
    47,    73,    53,    54,    55,    56,    57,    58,    59,    60,
    71,    62,    63,    64,    65,    66,    67,    68,     1,    70,
    47,    77,    47,    71,     7,     8,     9,    47,    71,    71,
    71,    71,    15,    71,    71,    18,    19,    20,    21,    71,
    70,    70,    48,    71,    49,    71,    49,    71,    71,    32,
    33,    34,    35,    36,    71,    71,    39,    40,    41,    42,
    43,    44,    71,    46,    75,    49,    48,    50,    70,    77,
    53,    54,    55,    56,    57,    58,    59,    60,    48,    62,
    63,    64,    65,    66,    67,    68,    72,    70,     3,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    48,
    76,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    78,    32,    33,    34,
    35,    36,    37,    71,    39,    40,    41,    42,    43,    44,
    45,    46,    70,    48,    49,    70,    70,    70,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,     1,    70,    70,    47,    70,    70,
     7,     8,     9,    70,    70,    12,    70,    70,    15,    16,
    17,    18,    19,    47,    70,    22,    23,    24,    70,    26,
    27,    28,    49,    48,    72,    32,    33,    34,    35,    36,
    77,    70,    39,    40,    41,    42,    43,    44,    78,    46,
    48,    72,    48,    50,    48,    48,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,     1,    47,    78,    48,    48,    72,     7,     8,
     9,    69,    69,    12,    69,    69,    15,    16,    17,    18,
    19,    69,    48,    22,    23,    24,    72,    26,    27,    28,
    48,    48,    48,    32,    33,    34,    35,    36,    72,     0,
    39,    40,    41,    42,    43,    44,     0,    46,   132,   111,
   238,    50,   144,   114,    53,    54,    55,    56,    57,    58,
    59,    60,   300,    62,    63,    64,    65,    66,    67,    68,
     1,   252,   258,   198,   261,   330,     7,     8,     9,    10,
    11,    12,    -1,    -1,    15,    16,    17,    18,    19,    -1,
   137,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    32,    33,    34,    35,    36,    -1,    -1,    39,    40,
    41,    42,    43,    44,    -1,    46,    -1,    -1,    -1,    50,
    -1,    -1,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,     1,    -1,
    -1,    -1,    -1,    -1,     7,     8,     9,    -1,    -1,    -1,
    -1,    -1,    15,    -1,    -1,    18,    19,    20,    21,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
    33,    34,    35,    36,    -1,    -1,    39,    40,    41,    42,
    43,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
    53,    54,    55,    56,    57,    58,    59,    60,    -1,    62,
    63,    64,    65,    66,    67,    68,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    -1,    -1,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,    32,    33,    34,    35,    36,
    37,    -1,    39,    40,    41,    42,    43,    44,    45,    46,
    -1,    48,    49,    -1,    -1,    -1,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,     3,     4,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    -1,    -1,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    -1,    32,    33,    34,    35,    36,    37,    -1,    39,    40,
    41,    42,    43,    44,    45,    46,    -1,    48,    49,    -1,
    -1,    -1,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    18,    19,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    32,    33,    34,    35,    36,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/users/ptolemy/gnu/common/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/users/ptolemy/gnu/common/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 2:
#line 312 "../../src/ptlang/ptlang.y"
{ yyerror("Too many closing curly braces");
			  exit(1);
			;
    break;}
case 3:
#line 315 "../../src/ptlang/ptlang.y"
{ mismatch(yyvsp[0]);;
    break;}
case 9:
#line 325 "../../src/ptlang/ptlang.y"
{ idBlock = yyvsp[0]; bodyMode = 0;;
    break;}
case 10:
#line 328 "../../src/ptlang/ptlang.y"
{ bodyMode = 1;;
    break;}
case 11:
#line 332 "../../src/ptlang/ptlang.y"
{ clearDefs(0);;
    break;}
case 12:
#line 333 "../../src/ptlang/ptlang.y"
{ genDef();;
    break;}
case 13:
#line 338 "../../src/ptlang/ptlang.y"
{ clearDefs(0);
					  coronaDef = 1; ;
    break;}
case 14:
#line 340 "../../src/ptlang/ptlang.y"
{ genDef();;
    break;}
case 15:
#line 345 "../../src/ptlang/ptlang.y"
{ clearDefs(0);
					  coreDef = 1; ;
    break;}
case 16:
#line 347 "../../src/ptlang/ptlang.y"
{ /* Make sure that corona was set */
					  if (coronaName == (char *)NULL) {
						yyerror("All cores must have"
							" a corona directive");
					  }
					  genDef();
					;
    break;}
case 17:
#line 356 "../../src/ptlang/ptlang.y"
{ clearDefs(1);;
    break;}
case 18:
#line 357 "../../src/ptlang/ptlang.y"
{ genDef();;
    break;}
case 25:
#line 375 "../../src/ptlang/ptlang.y"
{ objName = yyvsp[-1];;
    break;}
case 26:
#line 376 "../../src/ptlang/ptlang.y"
{ versionMode = 1;;
    break;}
case 27:
#line 377 "../../src/ptlang/ptlang.y"
{ versionMode = 0;;
    break;}
case 28:
#line 379 "../../src/ptlang/ptlang.y"
{ descMode = 1; docMode = 1;;
    break;}
case 29:
#line 380 "../../src/ptlang/ptlang.y"
{ objDesc = yyvsp[0];
					  docMode = 0;
					  descMode = 0;;
    break;}
case 30:
#line 383 "../../src/ptlang/ptlang.y"
{ bodyMode = 1; docMode = 1;;
    break;}
case 31:
#line 384 "../../src/ptlang/ptlang.y"
{ objAuthor = yyvsp[0];
					  docMode = 0;
					  bodyMode = 0;;
    break;}
case 32:
#line 387 "../../src/ptlang/ptlang.y"
{ bodyMode = 1; docMode = 1;;
    break;}
case 33:
#line 388 "../../src/ptlang/ptlang.y"
{ objAcknowledge = yyvsp[0];
					  docMode = 0;
					  bodyMode = 0;;
    break;}
case 34:
#line 391 "../../src/ptlang/ptlang.y"
{ bodyMode = 1; docMode = 1;;
    break;}
case 35:
#line 392 "../../src/ptlang/ptlang.y"
{ objCopyright = yyvsp[0];
					  docMode = 0;
					  bodyMode = 0;;
    break;}
case 36:
#line 395 "../../src/ptlang/ptlang.y"
{ bodyMode = 1; docMode = 1;;
    break;}
case 37:
#line 396 "../../src/ptlang/ptlang.y"
{ objLocation = yyvsp[0];
					  docMode = 0;
					  bodyMode = 0;;
    break;}
case 38:
#line 399 "../../src/ptlang/ptlang.y"
{ bodyMode = 1; docMode = 1;;
    break;}
case 39:
#line 400 "../../src/ptlang/ptlang.y"
{ objExpl = yyvsp[0];
					  bodyMode = 0;
					  docMode = 0;;
    break;}
case 40:
#line 403 "../../src/ptlang/ptlang.y"
{ bodyMode = 1; docMode = 1;;
    break;}
case 41:
#line 404 "../../src/ptlang/ptlang.y"
{ objHTMLdoc = yyvsp[0];
					  bodyMode = 0;
					  docMode = 0;;
    break;}
case 42:
#line 407 "../../src/ptlang/ptlang.y"
{ ;
    break;}
case 44:
#line 413 "../../src/ptlang/ptlang.y"
{ clearStateDefs();;
    break;}
case 45:
#line 414 "../../src/ptlang/ptlang.y"
{ genState(); describeState();;
    break;}
case 46:
#line 417 "../../src/ptlang/ptlang.y"
{ inlineFlag[methKey] = yyvsp[-2] ? 1 : 0;
					  codeBody[methKey] = yyvsp[0];
					  bodyMode = 0;
					;
    break;}
case 47:
#line 422 "../../src/ptlang/ptlang.y"
{ addMembers (yyvsp[-1], yyvsp[0]); bodyMode = 0;;
    break;}
case 48:
#line 423 "../../src/ptlang/ptlang.y"
{ strcat (ccCode, yyvsp[0]); 
					  strcat (ccCode, "\n\n");
					  bodyMode = 0;
					;
    break;}
case 49:
#line 427 "../../src/ptlang/ptlang.y"
{ strcat (hCode, yyvsp[0]);
					  strcat (hCode, "\n");
					  bodyMode = 0;
					;
    break;}
case 50:
#line 431 "../../src/ptlang/ptlang.y"
{ wrapMethod();;
    break;}
case 51:
#line 432 "../../src/ptlang/ptlang.y"
{ ;
    break;}
case 53:
#line 434 "../../src/ptlang/ptlang.y"
{ yyerror("Error in ccinclude list");;
    break;}
case 54:
#line 435 "../../src/ptlang/ptlang.y"
{ yyerror("Error in hinclude list");;
    break;}
case 55:
#line 436 "../../src/ptlang/ptlang.y"
{ if(consCalls[0]) {
					     strcat(consCalls,", ");
					     strcat(consCalls,yyvsp[0]);
					  } else {
					     strcpy(consCalls,yyvsp[0]);
					  }
					  bodyMode = 0; 
					;
    break;}
case 56:
#line 444 "../../src/ptlang/ptlang.y"
{ yyerror ("Illegal item");;
    break;}
case 57:
#line 449 "../../src/ptlang/ptlang.y"
{ clearMethodDefs(0);;
    break;}
case 58:
#line 450 "../../src/ptlang/ptlang.y"
{ clearMethodDefs(M_PURE);;
    break;}
case 59:
#line 451 "../../src/ptlang/ptlang.y"
{ clearMethodDefs(M_VIRTUAL);;
    break;}
case 60:
#line 452 "../../src/ptlang/ptlang.y"
{ clearMethodDefs(M_STATIC);;
    break;}
case 61:
#line 453 "../../src/ptlang/ptlang.y"
{ clearMethodDefs(M_STATIC|M_INLINE);;
    break;}
case 62:
#line 454 "../../src/ptlang/ptlang.y"
{ int mode = M_INLINE;
					  if (yyvsp[-1]) mode |= M_VIRTUAL;
					  clearMethodDefs(mode);
					;
    break;}
case 63:
#line 461 "../../src/ptlang/ptlang.y"
{ yyval = 0;;
    break;}
case 64:
#line 462 "../../src/ptlang/ptlang.y"
{ yyval = yyvsp[0];;
    break;}
case 65:
#line 466 "../../src/ptlang/ptlang.y"
{ yyval = 0;;
    break;}
case 66:
#line 467 "../../src/ptlang/ptlang.y"
{ yyval = yyvsp[0];;
    break;}
case 67:
#line 472 "../../src/ptlang/ptlang.y"
{ bodyMode = 1;;
    break;}
case 68:
#line 476 "../../src/ptlang/ptlang.y"
{ methKey = C_CONS;;
    break;}
case 69:
#line 477 "../../src/ptlang/ptlang.y"
{ methKey = C_DEST;;
    break;}
case 70:
#line 478 "../../src/ptlang/ptlang.y"
{ methKey = C_BEGIN;;
    break;}
case 71:
#line 479 "../../src/ptlang/ptlang.y"
{ methKey = C_SETUP;;
    break;}
case 72:
#line 480 "../../src/ptlang/ptlang.y"
{ methKey = C_GO;;
    break;}
case 73:
#line 481 "../../src/ptlang/ptlang.y"
{ methKey = C_WRAPUP;;
    break;}
case 74:
#line 482 "../../src/ptlang/ptlang.y"
{ methKey = C_TICK;;
    break;}
case 75:
#line 483 "../../src/ptlang/ptlang.y"
{ methKey = C_INITCODE;;
    break;}
case 76:
#line 484 "../../src/ptlang/ptlang.y"
{ methKey = C_EXECTIME;;
    break;}
case 77:
#line 486 "../../src/ptlang/ptlang.y"
{ yywarn("start is obsolete, use setup");
		  methKey = C_SETUP;
		;
    break;}
case 78:
#line 495 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
                  objVer = yyvsp[-6];
                  sprintf(b, "\"%s %s\"", yyvsp[-2], yyvsp[-1]);
                  objDate = save(b);
                ;
    break;}
case 79:
#line 502 "../../src/ptlang/ptlang.y"
{
                  char b[SMALLBUFSIZE];
                  long t;
                  objVer = "?.?";
                  t = time((time_t *)0);
                  b[0] = QUOTE;
                  b[1] = 0;
                  strncat(b,ctime(&t),24);
                  strcat(b,"\"");
                  objDate = save(b);
                ;
    break;}
case 80:
#line 516 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
		  objVer = yyvsp[-6];
		  sprintf(b, "\"%s %s\"", yyvsp[-5], yyvsp[-4]);
		  objDate = save(b);
		;
    break;}
case 81:
#line 524 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
		  objVer = yyvsp[-5];
		  sprintf(b, "\"%s %s\"", yyvsp[-4], yyvsp[-3]);
		  objDate = save(b);
		;
    break;}
case 82:
#line 531 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
		  long t;
		  objVer = "?.?";
		  t = time((time_t *)0);
		  b[0] = QUOTE;
		  b[1] = 0;
		  strncat(b,ctime(&t),24);
		  strcat(b,"\"");
		  objDate = save(b);
		;
    break;}
case 83:
#line 545 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
		  objVer = yyvsp[-1];
		  sprintf(b, "\"%s\"", yyvsp[0]);
		  objDate = save(b);
		;
    break;}
case 84:
#line 554 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
		  objVer = yyvsp[-3];
		  sprintf(b, "\"%s %s %s\"", yyvsp[-2], yyvsp[-1], yyvsp[0]);
		  objDate = save(b);
		;
    break;}
case 85:
#line 561 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
		  objVer = yyvsp[-1];
		  sprintf(b, "\"%s\"", yyvsp[0]);
		  objDate = save(b);
		;
    break;}
case 86:
#line 567 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
		  objVer = yyvsp[-3];
		  sprintf(b, "\"%s/%s/%s\"", yyvsp[-2], yyvsp[-1], yyvsp[0]);
		  objDate = save(b);
		;
    break;}
case 87:
#line 573 "../../src/ptlang/ptlang.y"
{
		  char b[SMALLBUFSIZE];
		  long t;
		  objVer = "?.?";
		  t = time((time_t *)0);
		  b[0] = QUOTE;
		  b[1] = 0;
		  strncat(b,ctime(&t),24);
		  strcat(b,"\"");
		  objDate = save(b);
		  /* objDate = "\"checked out\""; */
		;
    break;}
case 88:
#line 585 "../../src/ptlang/ptlang.y"
{ yyerror("Illegal version format");;
    break;}
case 91:
#line 592 "../../src/ptlang/ptlang.y"
{ genPort();
					  describePort(); ;
    break;}
case 93:
#line 597 "../../src/ptlang/ptlang.y"
{ domain = yyvsp[-1];;
    break;}
case 94:
#line 598 "../../src/ptlang/ptlang.y"
{ derivedFrom = yyvsp[-1];;
    break;}
case 95:
#line 599 "../../src/ptlang/ptlang.y"
{;
    break;}
case 96:
#line 604 "../../src/ptlang/ptlang.y"
{ char* b = malloc(SMALLBUFSIZE);
					  blockID = yyvsp[-3];
					  strcpy(b,blockID);
					  codeBlockNames[numBlocks]=b;
					  codeBlockArgs[numBlocks]=save(yyvsp[-2]);
					  codeBlockLines[numBlocks]=
					    savelineref();
					  codeMode = 1;
					;
    break;}
case 97:
#line 613 "../../src/ptlang/ptlang.y"
{ codeBlocks[numBlocks++]=yyvsp[0];
					  codeMode = 0;
					;
    break;}
case 98:
#line 619 "../../src/ptlang/ptlang.y"
{ alsoDerivedFrom[nAlsoDerivedFrom++] = yyvsp[0]; ;
    break;}
case 99:
#line 620 "../../src/ptlang/ptlang.y"
{ alsoDerivedFrom[nAlsoDerivedFrom++] = yyvsp[0]; ;
    break;}
case 100:
#line 623 "../../src/ptlang/ptlang.y"
{ yyval = (char*)0; ;
    break;}
case 101:
#line 624 "../../src/ptlang/ptlang.y"
{ yyval = stripQuotes(yyvsp[0]); ;
    break;}
case 102:
#line 628 "../../src/ptlang/ptlang.y"
{ bodyMode = 1;;
    break;}
case 107:
#line 642 "../../src/ptlang/ptlang.y"
{ methodName = yyvsp[-1];;
    break;}
case 108:
#line 643 "../../src/ptlang/ptlang.y"
{ methodArgs = checkArgs(yyvsp[-1]);;
    break;}
case 109:
#line 644 "../../src/ptlang/ptlang.y"
{ methodType = yyvsp[-1];;
    break;}
case 110:
#line 645 "../../src/ptlang/ptlang.y"
{ methodAccess = yyvsp[-1];;
    break;}
case 111:
#line 646 "../../src/ptlang/ptlang.y"
{ methodCode = yyvsp[0]; bodyMode = 0;;
    break;}
case 112:
#line 651 "../../src/ptlang/ptlang.y"
{ bodyMode = 1;;
    break;}
case 113:
#line 656 "../../src/ptlang/ptlang.y"
{ bodyMode = 1;;
    break;}
case 114:
#line 661 "../../src/ptlang/ptlang.y"
{ bodyMode = 1; yyval = yyvsp[-1];;
    break;}
case 122:
#line 678 "../../src/ptlang/ptlang.y"
{ clearStateDefs();;
    break;}
case 123:
#line 679 "../../src/ptlang/ptlang.y"
{ genState(); describeState();;
    break;}
case 124:
#line 680 "../../src/ptlang/ptlang.y"
{ genPort();
					  describePort(); ;
    break;}
case 127:
#line 689 "../../src/ptlang/ptlang.y"
{ coronaName = yyvsp[-1]; ;
    break;}
case 128:
#line 690 "../../src/ptlang/ptlang.y"
{ coreCategory = yyvsp[-1]; ;
    break;}
case 132:
#line 699 "../../src/ptlang/ptlang.y"
{ genInstance();;
    break;}
case 133:
#line 700 "../../src/ptlang/ptlang.y"
{ genAlias();;
    break;}
case 134:
#line 711 "../../src/ptlang/ptlang.y"
{ initPort(0,0);;
    break;}
case 135:
#line 712 "../../src/ptlang/ptlang.y"
{ initPort(1,0);;
    break;}
case 136:
#line 713 "../../src/ptlang/ptlang.y"
{ initPort(2,0);;
    break;}
case 137:
#line 714 "../../src/ptlang/ptlang.y"
{ initPort(0,1);;
    break;}
case 138:
#line 715 "../../src/ptlang/ptlang.y"
{ initPort(1,1);;
    break;}
case 139:
#line 716 "../../src/ptlang/ptlang.y"
{ initPort(2,1);;
    break;}
case 142:
#line 725 "../../src/ptlang/ptlang.y"
{ portName = yyvsp[-1];;
    break;}
case 143:
#line 726 "../../src/ptlang/ptlang.y"
{ portType = portDataType(yyvsp[-1]);;
    break;}
case 144:
#line 727 "../../src/ptlang/ptlang.y"
{ portInherit = yyvsp[-1];;
    break;}
case 145:
#line 728 "../../src/ptlang/ptlang.y"
{ portNum = yyvsp[-1];;
    break;}
case 146:
#line 729 "../../src/ptlang/ptlang.y"
{ portAttrib = yyvsp[0]; bodyMode = 0;;
    break;}
case 147:
#line 730 "../../src/ptlang/ptlang.y"
{ descMode = 1; docMode = 1;;
    break;}
case 148:
#line 730 "../../src/ptlang/ptlang.y"
{ portDesc = yyvsp[0]; docMode = 0;
					  descMode = 0;;
    break;}
case 151:
#line 741 "../../src/ptlang/ptlang.y"
{ stateName = yyvsp[-1];;
    break;}
case 152:
#line 742 "../../src/ptlang/ptlang.y"
{ int tc = stateTypeClass(yyvsp[-1]);
					  stateMarks[tc]++;
					  stateClass = stateClasses[tc];
					;
    break;}
case 153:
#line 746 "../../src/ptlang/ptlang.y"
{ stateDef = yyvsp[-1];;
    break;}
case 154:
#line 747 "../../src/ptlang/ptlang.y"
{ descMode = 1; docMode = 1;;
    break;}
case 155:
#line 748 "../../src/ptlang/ptlang.y"
{ stateDesc = yyvsp[0];
					  docMode = 0;
					  descMode = 0;;
    break;}
case 156:
#line 751 "../../src/ptlang/ptlang.y"
{ stateAttrib = yyvsp[0]; bodyMode=0;;
    break;}
case 157:
#line 754 "../../src/ptlang/ptlang.y"
{ bodyMode=1;;
    break;}
case 158:
#line 757 "../../src/ptlang/ptlang.y"
{ yyval = yyvsp[0];;
    break;}
case 159:
#line 758 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
					  sprintf (b, "\"%s\"", yyvsp[0]);
					  yyval = save(b);
					;
    break;}
case 160:
#line 762 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
					  sprintf (b, "\"%s\"", yyvsp[0]);
					  yyval = save(b);
					;
    break;}
case 161:
#line 768 "../../src/ptlang/ptlang.y"
{ yyval = save(yyvsp[0]);;
    break;}
case 162:
#line 769 "../../src/ptlang/ptlang.y"
{ char* b = malloc(MEDBUFSIZE);
					  strcpy(b,yyvsp[-1]);
					  strcat(b," ");
					  strcat(b,yyvsp[0]);
					  yyval = b; ;
    break;}
case 163:
#line 777 "../../src/ptlang/ptlang.y"
{ yyval = yyvsp[0];;
    break;}
case 164:
#line 778 "../../src/ptlang/ptlang.y"
{ char b[SMALLBUFSIZE];
					  strcpy (b, yyvsp[0]+1);
					  b[strlen(yyvsp[0])-2] = 0;
					  yyval = save(b);
					;
    break;}
case 167:
#line 791 "../../src/ptlang/ptlang.y"
{ instName = yyvsp[-1];;
    break;}
case 168:
#line 792 "../../src/ptlang/ptlang.y"
{ instClass = yyvsp[-1];;
    break;}
case 171:
#line 801 "../../src/ptlang/ptlang.y"
{ galPortName = yyvsp[-1];;
    break;}
case 173:
#line 806 "../../src/ptlang/ptlang.y"
{ checkIncludes(nCcInclude-1); 
					  ccInclude[nCcInclude++] = yyvsp[0];;
    break;}
case 175:
#line 812 "../../src/ptlang/ptlang.y"
{ checkSeeAlsos(nSeeAlso-1);
					  seeAlsoList[nSeeAlso++] = yyvsp[0];;
    break;}
case 176:
#line 814 "../../src/ptlang/ptlang.y"
{ checkSeeAlsos(nSeeAlso-1);
					  seeAlsoList[nSeeAlso++] = yyvsp[0];;
    break;}
case 178:
#line 819 "../../src/ptlang/ptlang.y"
{ checkIncludes(nHInclude-1); 
					  hInclude[nHInclude++] = yyvsp[0];;
    break;}
case 183:
#line 832 "../../src/ptlang/ptlang.y"
{ yyval = stripQuotes (yyvsp[0]);;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/users/ptolemy/gnu/common/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 847 "../../src/ptlang/ptlang.y"


/*****************************************************************************
 *
 *			C functions
 *
 * These are 3 classes of functions in this section:
 * 
 * 1. Utility functions called by the grammar (above) to store away
 *    pieces of data.
 * 2. "Generation" functions that write out all the data we've collected
 *    to produce the .h, .cc, and doc files.
 * 3. The yacc driver and lexer.
 *
 *****************************************************************************/

/* Reset for a new star or galaxy class definition.  If arg is TRUE
   we are defining a galaxy.
 */
void clearDefs (g)
int g;
{
	int i;
	for (i = 0; i < NSTATECLASSES; i++) stateMarks[i] = 0;
	galDef = g;
	objName = objVer = objDesc = coronaName = coreCategory =
		domain = derivedFrom = objAuthor = objCopyright =
		objExpl = objHTMLdoc = objLocation = NULL;
	consStuff[0] = ccCode[0] = hCode[0] = consCalls[0] = 0;
	publicMembers[0] = privateMembers[0] = protectedMembers[0] = 0;
	inputDescriptions[0] = outputDescriptions[0] = inoutDescriptions[0] = 0;
	stateDescriptions[0] = 0;
	inputDescHTML[0] = outputDescHTML[0] = inoutDescHTML[0] = 0;
	stateDescHTML[0] = 0;
	nCcInclude = nAlsoDerivedFrom = nHInclude = nSeeAlso = 0;
	pureFlag = 0;
	for (i = 0; i < N_FORMS; i++) {
		codeBody[i] = 0;
		inlineFlag[i] = 0;
	}
}

/* Generate a state definition */
void clearStateDefs ()
{
	stateName = stateClass = stateDef = stateDesc = stateAttrib = NULL;
}

char*
cvtToLower (name)
char* name;
{
	static char buf[128];
	char* p = buf, c;
	while ((c = *name++) != 0) {
		if (isupper(c)) *p++ = tolower(c);
		else *p++ = c;
	}
	*p = 0;
	return buf;
}

char*
cvtToUpper (name)
char* name;
{
	static char buf[128];
	char* p = buf, c;
	while ((c = *name++) != 0) {
		if (islower(c)) *p++ = toupper(c);
		else *p++ = c;
	}
	*p = 0;
	return buf;
}


char* whichMembers (type)
char* type;
{
/* type must be "protected", "public", or "private" */
	switch (type[2]) {
	case 'o':
		return protectedMembers;
	case 'b':
		return publicMembers;
	case 'i':
		return privateMembers;
	default:
		fprintf (stderr, "Internal error in whichMembers\n");
		exit (1);
		/* NOTREACHED */
	}
}

/* add declarations of extra members to the class definition */
void addMembers (type, defs)
char* type;
char* defs;
{
	char * p = whichMembers (type);
	strcat (p, defs);
	strcat (p, "\n");
	return;
}

/* get "real" state class name from an argument */
int
stateTypeClass (nameArg)
char* nameArg;
{
	char* name = cvtToLower (nameArg);
 
	if (strcmp (name, "int") == 0)
		return T_INT;
	if (strcmp (name, "float") == 0)
		return T_FLOAT;
	if (strcmp (name, "complex") == 0)
		return T_COMPLEX;
	if (strcmp (name, "string") == 0)
		return T_STRING;
	if (strcmp (name, "intarray") == 0)
		return T_INTARRAY;
	if (strcmp (name, "floatarray") == 0)
		return T_FLOATARRAY;
	if (strcmp (name, "complexarray") == 0)
		return T_COMPLEXARRAY;
	if (strcmp (name, "fix") == 0)
		return T_FIX;
	if (strcmp (name, "fixarray") == 0)
		return T_FIXARRAY;
	if (strcmp (name, "stringarray") == 0)
		return T_STRINGARRAY;
	if (strcmp (name, "precision") == 0)
		return T_PRECISION;
	fprintf (stderr, "state class %s\n", name);
	yyerror ("bad state class: assuming int");
		return T_INT;
}

/* generate code for a state defn */
void genState ()
{
	char* stateDescriptor;
	char* stateDefault;
	/* test that all fields are known */
	if (stateName == NULL) {
		yyerror ("state name not defined");
		return;
	}
	if (stateClass == NULL) {
		yyerror ("state class not defined");
		return;
	}
	if (stateDef == NULL)
		stateDefault = "\"\"";
	else
		stateDefault = stateDef;
	if (stateDesc == NULL)
		stateDescriptor = stateName;
	else
		stateDescriptor = stateDesc;
	sprintf (str1,"\t%s %s;\n", stateClass, stateName);
	sprintf (str2,"\taddState(%s.setState(\"%s\",this,%s,\"%s\"",
		 stateName, stateName, stateDefault, stateDescriptor);
	if (stateAttrib) {
		strcat (str2, ",\n");
		strcat (str2, stateAttrib);
	}
	strcat (str2, "));\n");
	strcat (protectedMembers, str1);
	strcat (consStuff, str2);
}

/* describe the states */
void describeState ()
{
	char descriptString[MEDBUFSIZE];

        /* troff version */
	sprintf(str1,".NE\n\\fI%s\\fR (%s)",stateName,stateClass);
	strcat(stateDescriptions,str1);
	if (stateDesc) {
	    if(unescape(descriptString, stateDesc, MEDBUFSIZE))
		yywarn("warning: Descriptor too long. May be truncated.");
	    sprintf(str1,": %s\n",descriptString);
	} else
	    sprintf(str1,"\n");
	strcat(stateDescriptions,str1);
	if (stateDef) {
		sprintf(str1,".DF %s\n",stateDef);
	}
	strcat(stateDescriptions,str1);

        /* html version */
        sprintf(str1,
                 "<tr>\n<td><i><b><font color=blue>%s</font></b></i></td><td>%s</td>\n",
                 stateName,stateClass);
	strcat(stateDescHTML,str1);
	if (stateDesc) {
	    if(unescape(descriptString, stateDesc, MEDBUFSIZE))
		yywarn("warning: Descriptor too long. May be truncated.");
	    sprintf(str1,"<td>%s</td>\n",descriptString);
            strcat(stateDescHTML,str1);
	}
	if (stateDef) {
            sprintf(str1,"<td>%s</td>\n",stateDef);
            strcat(stateDescHTML,str1);
	}
        sprintf(str1,"</tr>\n");
	strcat(stateDescHTML,str1);
}

/* set up for port definition */
void initPort (dir, multi)
int dir, multi;
{
	portDir = dir;
	portMulti = multi;
	portName = portNum = portInherit = portDesc = portAttrib = NULL;
	portType = "ANYTYPE";
}

char* portDataType (name)
char* name;
{
	/* do better checking later */
	return save(name);
}

void genPort ()
{
	/* test that all fields are known */
	char* dir = portDir==2 ? "InOut" : (portDir==1?"Out" : "In");
	char* m = portMulti ? "Multi" : "";
	char* d = galDef ? "" : domain;
	char* port = galDef ? "PortHole" : "Port";

	sprintf (str1,"\t%s%s%s%s %s;\n", m, dir, d, port, portName);
	if (portNum)
		sprintf (str2, "\taddPort(%s.setPort(\"%s\",this,%s,%s));\n",
			portName, portName, cvtToUpper(portType), portNum);
	else
		sprintf (str2, "\taddPort(%s.setPort(\"%s\",this,%s));\n",
			portName, portName, cvtToUpper(portType));
	strcat (publicMembers, str1);
	strcat (consStuff, str2);
	if (portAttrib) {
		sprintf (str2, "\t%s.setAttributes(\n%s);\n", portName,
			portAttrib);
		strcat (consStuff, str2);
	}
	if (portInherit) {
		sprintf (str2, "\t%s.inheritTypeFrom(%s);\n", portName,
			 portInherit);
		strcat (consStuff, str2);
	}
}

void describePort ()
{
	char *dest, *destHTML, *color;
	char descriptString[MEDBUFSIZE];
        if (portDir==2) {
            dest = inoutDescriptions;
            color = "darkviolet";
            destHTML = inoutDescHTML;
        } else if (portDir==1) {
            dest = outputDescriptions;
            color = "firebrick";
            destHTML = outputDescHTML;
        } else {
            dest = inputDescriptions;
            color = "forestGreen";
            destHTML = inputDescHTML;
        }
	if (portMulti) {
	    sprintf(str1,".NE\n\\fI%s\\fR (multiple), (%s)",portName,portType);
            strcat(dest,str1);
	    sprintf(str1,"<tr>\n<td><i><b><font color=%s>%s</font></b></i> (multiple)</td><td>%s</td>\n", color, portName,portType);	
            strcat(destHTML,str1);
	} else {
	    sprintf(str1,".NE\n\\fI%s\\fR (%s)",portName,portType);
            strcat(dest,str1);
	    sprintf(str1,"<tr>\n<td><i><b><font color=%s>%s</font></b></i></td><td>%s</td>\n", color, portName,portType);	
	     strcat(destHTML,str1);
	}

	if (portDesc) {
	    if(unescape(descriptString, portDesc, MEDBUFSIZE))
		yywarn("warning: Descriptor too long. May be truncated.");
	    sprintf(str1,": %s\n",descriptString);
            strcat(dest,str1);
	    sprintf(str1,"<td>%s</td>\n",descriptString);
            strcat(destHTML,str1);
	} else {
	    sprintf(str1,"\n");
            strcat(dest,str1);
        }
        sprintf(str1,"</tr>\n");
        strcat(destHTML,str1);
}

/* set up for user-supplied method */
void clearMethodDefs (mode)
int mode;
{
	methodName = NULL;
	methodArgs = "()";
	methodAccess = "protected";
	methodCode = NULL;
	methodType = "void";
	methodMode = mode;
}

/* generate code for user-defined method */
void wrapMethod ()
{
	char * p = whichMembers (methodAccess);
	char * mkey = "";
/* add decl to class body */
	if (methodMode == M_PURE) {
		if (methodCode) yyerror ("Code supplied for pure method");
		/* pure virtual function case */
		sprintf (str1, "\tvirtual %s %s %s = 0;\n",
			methodType, methodName, methodArgs);
		strcat (p, str1);
		pureFlag++;
		return;
	}
	if (methodCode == NULL) {
		yyerror ("No code supplied for method");
		methodCode = "";
	}
	/* form declaration in class body */
	if (methodMode & M_STATIC) mkey = "static ";
	else if (methodMode & M_VIRTUAL) mkey = "virtual ";
	sprintf (str1, "\t%s%s %s %s", mkey, methodType,
		 methodName, methodArgs);
	strcat (p, str1);
	/* handle inline functions */
	if (methodMode & M_INLINE) {
		strcat (p, " {\n");
		strcat (p, methodCode);
		strcat (p, "\n\t}\n");
		return;
	}
	/* not inline: put it into the .cc file */
	strcat (p, ";\n");
	sprintf (str2, "\n\n%s %s%s%s::%s ", methodType,
		 galDef ? "" : domain, objName, coreDef ? coreCategory : "", methodName);
	strcat (miscCode, str2);
	stripDefaultArgs (str2, methodArgs);
	strcat (miscCode, str2);
	strcat (miscCode, "\n{\n");
	strcat (miscCode, methodCode);
	strcat (miscCode, "\n}\n");
}

/* generate an instance of a block within a galaxy */
void genInstance ()
{
	sprintf (str1, "\t%s %s;\n", instClass, instName);
	strcat (protectedMembers, str1);
	sprintf (str2,"addBlock(%s.setBlock(\"%s\",this)", instName, instName);
	strcat (consStuff, str2);
}


void genAlias () {
	/* FILL IN */
}

void genConnect () {
	/* FILL IN */
}



/* fn to write out standard methods in the header */
void genStdProto(fp,i)
FILE* fp;
int i;
{
	if (codeBody[i])
		fprintf (fp, "\t/* virtual */ %s%s()%s\n", codeType[i],
			 codeFuncName[i], inlineFlag[i] ? " {" : ";");
	if (inlineFlag[i])
		fprintf (fp, "%s\n\t}\n", codeBody[i]);
}

/* return true if the n characters beginning at s are whitespace. */
static int
isStrnSpace( s, n)
    char	*s;
    int		n;
{
    int		c;

    for (; n > 0; n--, s++) {
	c = *s;
	if ( c!=' ' && c!='\t' )
	    return 0;
    }


    return 1;
}


void cvtCodeBlockExpr( src, src_len, pDst)
    char *src, **pDst;
    int src_len;
{
    char *dst = *pDst;

    /*IF*/ if ( src_len==6 && strncmp(src,"ATSIGN",6)==0 ) {
	*dst++ = '@';		/* */
    } else if ( src_len==6 && strncmp(src,"LBRACE",6)==0 ) {
	*dst++ = '{';		/* } (to balance 'vi') */
    } else if ( src_len==6 && strncmp(src,"RBRACE",6)==0 ) {
	/* { (to balance 'vi') */
	*dst++ = '}';
    } else if ( src_len==9 && strncmp(src,"BACKSLASH",9)==0 ) {
	*dst++ = '\\';
    } else if ( isStrnSpace( src, src_len) ) {
	; /* just drop it */
    } else {
	strcpy(dst,"\" << ("); dst += strlen(dst);
	strncpy( dst, src, src_len); dst += src_len;
	strcpy(dst,") << \""); dst += strlen(dst);
    }
    *pDst = dst;
}

/**
    Convert a codeblock.  If extendB is FALSE, a set of string literals
    will be produced, with the contents of {src} escaped into proper C
    strings.  If extendB is TRUE, The ``@'' substitutions below will
    be processed and merged with the string literals using the "<<" syntax.
    Syntax rules:
	@@	==> @			(double ``@'' goes to single)
	@LBRACE ==> {			(LBRACE is literal string)
	@RBRACE ==> }			(RBRACE is literal string)
	@id	==> C++ token {id}	(id is one or more alphanumerics)
	@(expr) ==> C++ expr {expr}	(expr is arbitrary with balanced parens)
        @anything_else is passed through unchanged (including the @).
    If extendB is FALSE, then none of the ``@'' process occurs.

    The above list is prob. out of date.
**/
void cvtCodeBlock( src_in, dst_in, extendB)
    char *src_in, *dst_in;
    int extendB;
{
    char	*src = src_in, *dst = dst_in;
    char	*src_expr;
    int		c, parenCnt;

    *dst++ = QUOTE;
    for (; (c = *src++) != '\0'; ) {
	switch ( c ) {
	case ESC:
	    c = *src;
	    if ( extendB && c == '\n' ) {
		++src;	/* strip the newline */
	    } else if ( extendB && c == '\0' ) {
		; /* nothing */
	    } else {
		/* one backslash in input becomes two in output */
		*dst++ = ESC;
		*dst++ = ESC;
	    }
	    break;
	case QUOTE:
	    *dst++ = ESC;
	    *dst++ = QUOTE;
	    break;
	case NEWLINE:
	    *dst++ = ESC; *dst++ = 'n';
	    if ( *src == '\0' )	break;
	    *dst++ = '"'; *dst++ = NEWLINE; *dst++ = '"';
	    break;
	case '@':
	    if ( ! extendB ) {
		*dst++ = c;
		break;
	    }
	    c = *src++;
	    /*IF*/ if ( c=='@' || c=='\\' || c=='{' || c=='}' ) {
		*dst++ = c;
	    } else if ( c == '(' ) {
		for ( src_expr=src, parenCnt=1; parenCnt > 0; ) {
		    c = *src++;
		    /*IF*/ if ( c=='\0' ) {
			fprintf(stderr,"Unbalanced parans in @() codeblock\n");
			exit(1);
		    } else if ( c=='(' ) {
			++parenCnt;
		    } else if ( c==')' ) {	
		        if ( --parenCnt == 0 )
			    break;
		    }
		}
		cvtCodeBlockExpr( src_expr, src-src_expr-1, &dst);
	    } else if ( isalpha(c) || (c == '_')) {
		    /* underscores are ok in the names of identifiers
		     * after a '@'.  Jeurgen Weiss 9/96
		     */
		    for ( src_expr=src-1; c=*src++, isalnum(c); )
			;
		    cvtCodeBlockExpr( src_expr, src-src_expr-1, &dst);
		    --src;	/* for() loop will advance */
	    } else {
	        *dst++ = '@';
		*dst++ = c;
	    }
	    break;
	default:
	    *dst++ = c;
	}
    }
    *dst++ = QUOTE;
    *dst = '\0';
}


void genCodeBlock( fp, src, extendB)
    FILE *fp;
    char *src;
    int extendB;
{
    char *dst = malloc(strlen(src)*2+MEDBUFSIZE);
    cvtCodeBlock( src, dst, extendB);
    fputs( dst, fp);
    free(dst);
}


/**
    Convert a method body (standard (e.g., go) or custom).  Primarily
    involves processing for in-line codeblocks.
    Lines starting with a "@" will have the leading white-space striped,
    and the remainder of the line processed by cvtCodeBlock(),
    with the result added to the default code stream.
**/
void cvtMethod( src_in, dst_in)
    char *src_in, *dst_in;
{
    char	*src = src_in, *dst = dst_in;
    char	*src_line, *src_start, *src_end;
    int		c, c_end;
    int		codeblockB = 0;

    for (; *src!='\0' || codeblockB; ) {
	src_line = src;
	for ( ; (c=*src++) != '\0' && isspace(c) && c!=NEWLINE; )
	    ;
	src_start = --src;
	for ( ; (c=*src++) != '\0' && c!=NEWLINE; )
	    ;
	src_end = c==NEWLINE ? src : --src;
	c_end = *src_end; *src_end = '\0';
	if ( src_start[0] == '@' ) {
	    src = src_start+1;
	    if ( !codeblockB ) {
		codeblockB = 1;
	        strcpy(dst, "\t{ StringList _str_; _str_ << \n");
		dst+=strlen(dst);
	    }
	    cvtCodeBlock( src, dst, 1);		dst+=strlen(dst);
	    *dst++ = NEWLINE;
	} else {
	    if ( codeblockB ) {
	        strcpy(dst, ";\n\t addCode(_str_); }\n"); dst+=strlen(dst);
		codeblockB = 0;
	    }
	    strcpy( dst, src_line); dst += strlen(dst);
	}
	*src_end = c_end;
	src = src_end;
    }
    *dst = '\0';
}

void genMethod( fp, src)
    FILE *fp;
    char *src;
{
    char *dst = malloc(strlen(src)*2+MEDBUFSIZE);
    cvtMethod( src, dst);
    fputs( dst, fp);
    free(dst);
}

/* This is the main guy!  It outputs the complete class definition. */
void genDef ()
{
	FILE *fp;
	int i;
	char fname[FLEN], hname[FLEN], ccname[FLEN];
	char baseClass[SMALLBUFSIZE];
	char fullClass[SMALLBUFSIZE];
	char descriptString[MEDBUFSIZE];
        char *derivedSimple;
	char *startp, *copyrightStart; 
	char srcDirectory[SMALLBUFSIZE];

/* temp, until we implement this */
	if (galDef) {
		fprintf (stderr, "Sorry, galaxy definition is not yet supported.\n");
		exit (1);
	}
	if (objName == NULL) {
		yyerror ("No class name defined");
		return;
	}
	if (!galDef && !domain) {
		yyerror ("No domain name defined");
		return;
	}
/* All cores must define a core category. */
	if ( coreDef == 1 ) {
/* Core category determines base-class of core and full Classname */
		if ( coreCategory != (char *)NULL )
			sprintf( fullClass, "%s%s%s", galDef ? "" : domain, objName, coreCategory );
		else
			yyerror("All cores must have"
							" a coreCategory directive");
	} else
		sprintf (fullClass, "%s%s", galDef ? "" : domain, objName );

   if (!htmlOnly) {
/***************************************************************************
			CREATE THE .h FILE
*/
	sprintf (hname, "%s.h", fullClass);
	if ((fp = fopen (hname, "w")) == 0) {
		perror (hname);
		exit (1);
	}
/* Surrounding ifdef stuff */
	fprintf (fp, "#ifndef _%s_h\n#define _%s_h 1\n", fullClass, fullClass);
	fprintf (fp, "// header file generated from %s by %s\n",
		 inputFile, progName);

/* Special GNU pragmas for increased efficiency */
	fprintf (fp, "\n#ifdef __GNUG__\n#pragma interface\n#endif\n\n");

/* copyright */
	if (objCopyright) {
	    if ( strncasecmp(objCopyright,"copyright",9)==0 ) {
		fprintf (fp, "/*\n%s\n */\n", objCopyright);
	    } else {
		fprintf (fp, "/*\n * copyright (c) %s\n */\n", objCopyright);
	    }
	}

/* ID block */
	if (idBlock)
		fprintf (fp, "%s\n", idBlock);
/* The base class */
/* For stars, we append the domain name to the beginning of the name,
   unless it is already there */
	if (derivedFrom) {
		if (domain &&
		    strncmp (domain, derivedFrom, strlen (domain)) != 0) {
			/* Core category determines base class of cores */
			if ( coreDef == 1 ) {
				if ( coreCategory != (char *)NULL )
					sprintf( baseClass, "%s%s%s", galDef ? 
					"" : domain, derivedFrom, coreCategory );
				else
					yyerror("All cores must have"
						" a coreCategory directive");
			} else
				sprintf (baseClass, "%s%s", galDef ? "" : 
				domain, derivedFrom);
		}
		else {
			/* Core category determines base class of cores */
			if ( coreDef == 1 ) {
				if ( coreCategory != (char *)NULL )
					sprintf( baseClass, "%s%s", galDef ? 
					"" : derivedFrom, coreCategory );
				else
					yyerror("All cores must have"
						" a coreCategory directive");
			} else
				(void) strcpy (baseClass, derivedFrom);
		}
	}
/* Not explicitly specified: baseclass is Galaxy or XXXStar */
	else if (galDef)
		(void)strcpy (baseClass, "Galaxy");
	else if ( coreDef == 1 ) {
			/* Core category determines base class of cores */
				if ( coreCategory != (char *)NULL )
					sprintf( baseClass, "%s%sCore", galDef ? 
					"" : domain, coreCategory );
				else
					yyerror("All cores must have"
						" a coreCategory directive");
	} else {
/* Base class of corona is a corona */
		sprintf (baseClass, coronaDef ? "%sCorona" : "%sStar", domain);
	}

/* Include files */
	checkIncludes(nHInclude);
	for (i = 0; i < nHInclude; i++) {
		fprintf (fp, "#include %s\n", hInclude[i]);
	}
	if ( coreDef == 1 )
		fprintf(fp, "#include \"%s%s.h\"\n", domain, objName);
	fprintf (fp, "#include \"%s.h\"\n", baseClass);
	for( i = 0; i < nAlsoDerivedFrom; i++ ) {
		fprintf (fp, "#include \"%s.h\"\n", alsoDerivedFrom[i] );
        }
	
/* Include files for states */
	for (i = 0; i < NSTATECLASSES; i++)
		if (stateMarks[i])
			fprintf (fp, "#include \"%s.h\"\n", stateClasses[i]);

/* extra header code */
	fprintf (fp, "%s\n", hCode);
/* The class template */
	fprintf (fp, "class %s : public %s", fullClass, baseClass);
	for( i = 0; i < nAlsoDerivedFrom; i++ ) {
		fprintf( fp, ", %s", alsoDerivedFrom[i] );
        }
	fprintf( fp, "\n{\n" );

	sprintf (destNameBuf, "~%s", fullClass);
/* Core constructor takes Corona as argument */
	if ( coreDef == 1 ) {
		fprintf (fp, "public:\n\t%s(%sCorona & );\n", fullClass, domain);
/* Corona constructor takes Core init flag are argument ( 0 = don't construct cores which is the default ). */
	} else if ( coronaDef == 1 ) {
		fprintf (fp, "public:\n\t%s( int doCoreInitFlag = 0);\n", fullClass);
	} else {
		fprintf (fp, "public:\n\t%s();\n", fullClass);
	}
/* The makeNew function: only if the class isn't a pure virtual */
	if (!pureFlag) {
/* makeNew() for cores takes a Corona as argument. */
		if ( coreDef == 1 )
			fprintf (fp, "\t/* virtual */ %sCore* makeNew( %sCorona & ) const;\n", domain, domain);
		else
			fprintf (fp, "\t/* virtual */ Block* makeNew() const;\n");
	}
/* Corona keeps source directory for loading cores from same directory. */
	if ( coronaDef == 1 )
		fprintf (fp, "\t/* virtual*/ const char* getSrcDirectory() const;\n");
        fprintf (fp, "\t/* virtual*/ const char* className() const;\n");
        fprintf (fp, "\t/* virtual*/ int isA(const char*) const;\n");
/* The code blocks */
	for (i=0; i<numBlocks; i++) {
		if ( codeBlockArgs[i] == NULL ) {
		    fprintf (fp, "\tstatic CodeBlock %s;\n",codeBlockNames[i]);
		} else {
		    fprintf (fp, "\tconst char* %s(%s);\n",
		      codeBlockNames[i], codeBlockArgs[i]);
		}
	}
	for (i = C_EXECTIME; i <= C_DEST; i++)
		genStdProto(fp,i);
	if (publicMembers[0])
		fprintf (fp, "%s\n", publicMembers);
	if ( coronaDef != 1 )
		fprintf (fp, "protected:\n");
	if ( coreDef == 1 ) {
		fprintf(fp, "\n\t%s%s& corona;\n", domain, objName );
		fprintf(fp, "\n\t/* virtual */ %sCorona& getCorona() const { return (%sCorona&)corona; }\n", domain, domain );
	}
	for (i = C_SETUP; i < N_FORMS; i++)
		genStdProto(fp,i);
	if (protectedMembers[0])
		fprintf (fp, "%s\n", protectedMembers);
	if (privateMembers[0])
		fprintf (fp, "private:\n%s\n", privateMembers);
/* that's all, end the class def and put out an #endif */
	fprintf (fp, "};\n#endif\n");
	(void) fclose (fp);

/**************************************************************************
		CREATE THE .cc FILE
*/
	sprintf (ccname, "%s.cc", fullClass);
	if ((fp = fopen (ccname, "w")) == 0) {
		perror (ccname);
		exit (1);
	}
	fprintf (fp, "static const char file_id[] = \"%s\";\n", inputFile);
	fprintf (fp, "// .cc file generated from %s by %s\n",
		 inputFile, progName);
/* copyright */
	if (objCopyright) {
	    if ( strncasecmp(objCopyright,"copyright",9)==0 ) {
		fprintf (fp, "/*\n%s\n */\n", objCopyright);
	    } else {
		fprintf (fp, "/*\n * copyright (c) %s\n */\n", objCopyright);
	    }
	}

/* special GNU pragma for efficiency */
	fprintf (fp, "\n#ifdef __GNUG__\n#pragma implementation\n#endif\n\n");
	fprintf (fp, "\n# line 1 \"%s\"\n", inputFile);
/* ID block */
	if (idBlock)
		fprintf (fp, "%s\n", idBlock);
/* include files */
	fprintf (fp, "#include \"%s.h\"\n", fullClass);
	if ( coreDef == 1 )
		fprintf(fp, "#include \"%s%s.h\"\n", domain, objName);
	for (i = 0; i < nCcInclude; i++)
		fprintf (fp, "#include %s\n", ccInclude[i]);
/* generate className and (optional) makeNew function */
/* also generate a global identifier with name star_nm_DDDNNN, where DDD is
   the domain and NNN is the name */
	fprintf (fp, "\nconst char *star_nm_%s = \"%s\";\n", fullClass, fullClass);
/* Corona keeps source directory for loading cores. */
	if ( coronaDef == 1 ) { 
		if (getcwd(srcDirectory, SMALLBUFSIZE) == NULL) {
			perror("ptlang: getcwd() error"); exit(2);
		}
		fprintf (fp, "\nconst char *src_dir_%s = \"%s\";\n", fullClass, srcDirectory);
		fprintf (fp, "\nconst char* %s :: getSrcDirectory() const { return src_dir_%s; }\n", fullClass, fullClass);
	}
/* FIXME: Corona uses className virtual method as secondary init. */
	if ( coronaDef == 1 ) 
        	fprintf (fp, "\nconst char* %s :: className() const { %sCorona* ptr = (%sCorona* )this; if ( initCoreFlag == 0 ) ptr->addCores(); return star_nm_%s;}\n",
		fullClass, domain, domain, fullClass);
	else
        fprintf (fp, "\nconst char* %s :: className() const {return star_nm_%s;}\n",
		fullClass, fullClass);
	fprintf (fp, "\nISA_FUNC(%s,%s);\n",fullClass,baseClass);
	if (!pureFlag) {
/* makeNew() for cores takes a corona as argument. */
		if ( coreDef ) {
			fprintf (fp, "\n%sCore* %s :: makeNew( %sCorona & corona_) const { LOG_NEW; return new %s(corona_); }\n",
			 domain, fullClass, domain, fullClass );
/* Corona constructor takes do core init argument. */
		} else if ( coronaDef == 1 ) {
			fprintf (fp, "\nBlock* %s :: makeNew() const { LOG_NEW; return new %s(1);}\n",
			 fullClass, fullClass);
		} else {
			fprintf (fp, "\nBlock* %s :: makeNew() const { LOG_NEW; return new %s;}\n",
			 fullClass, fullClass);
		}
	}
/* generate the CodeBlock constructor calls */
	for (i=0; i<numBlocks; i++) {
	    if ( codeBlockArgs[i] == NULL ) {
		fprintf (fp, "\nCodeBlock %s :: %s (\n%s\n",
			fullClass,codeBlockNames[i],codeBlockLines[i]);
		genCodeBlock( fp, codeBlocks[i], 0);
		fprintf (fp, ");\n");
	    } else {
		fprintf (fp, "\nconst char* %s :: %s(%s) {\n%s\n",
			fullClass,codeBlockNames[i],codeBlockArgs[i],
			codeBlockLines[i]);
		fprintf (fp, "\tstatic StringList _str_; _str_.initialize(); _str_ << \n");
		genCodeBlock( fp, codeBlocks[i], 1);
		fprintf (fp, ";\n\treturn (const char*)_str_;\n}\n");
	    }
	}
/* prefix code and constructor */
/* Core constructor takes corona as argument. */
	if ( coreDef == 1 ) {
		fprintf (fp, "\n%s%s::%s ( %sCorona & corona_) : %s%sCore(corona_), corona((%s%s&)corona_)", ccCode, fullClass, fullClass, domain, domain, coreCategory, domain, objName);
/* Corona takes do core init flag and calls parent constructor. */
	} else if ( coronaDef == 1 ) {
		fprintf (fp, "\n%s%s::%s (int doCoreInitFlag) : %sCorona(0)", ccCode, fullClass, fullClass, domain);
	} else {
		fprintf (fp, "\n%s%s::%s ()", ccCode, fullClass, fullClass);
	}
	if (consCalls[0]) {
/* Core constructor has initializer for corona reference. */
		if ( coreDef == 1 )
			fprintf (fp, ",\n\t%s", consCalls);
		else
			fprintf (fp, " :\n\t%s", consCalls);
	}
	fprintf (fp, "\n{\n");
	if (objDesc)
		fprintf (fp, "\tsetDescriptor(\"%s\");\n", objDesc);
	/* define the class name */
	if (!consCode) consCode = "";
	fprintf (fp, "%s\n%s\n", consStuff, consCode);
/* Corona conditionally constructs coreList */
	if ( coronaDef == 1 )
		fprintf (fp, "\n\tif (doCoreInitFlag == 1 ) addCores();\n");
	fprintf (fp, "}\n");
	for (i = 1; i < N_FORMS; i++) {
		if (codeBody[i] && !inlineFlag[i]) {
		    char *dst = malloc(2*strlen(codeBody[i])+MEDBUFSIZE);
		    cvtMethod( codeBody[i], dst);
		    fprintf (fp, "\n%s%s::%s() {\n%s\n}\n",
		      codeType[i], fullClass, codeFuncName[i], dst);
		    free(dst);
		}
	}
	if (miscCode[0])
		fprintf (fp, "%s\n", miscCode);
	if (pureFlag) {
		fprintf (fp,
			 "\n// %s is an abstract class: no KnownBlock entry\n",
			 fullClass);
	} else if (coreDef) {
		/* FIXME:, these are all the same */
		fprintf (fp,
			"\n// Core prototype instance for known block list\n");
		fprintf (fp, "static %s%s dummy;\n", domain, objName);
		fprintf (fp, "static %s proto(dummy);\n", fullClass);
		fprintf (fp, 
			"static RegisterBlock registerBlock(proto,\"%s%s\");\n",
			objName, coreCategory);
	} else if (coronaDef) {
		fprintf (fp, "\n// Corona prototype instance for known block list\n");
		fprintf (fp, "static %s proto;\n", fullClass);
		fprintf (fp, 
			"static RegisterBlock registerBlock(proto,\"%s\");\n",
			objName);
	} else {
		fprintf (fp, "\n// prototype instance for known block list\n");
		fprintf (fp, "static %s proto;\n", fullClass);
		fprintf (fp, 
			"static RegisterBlock registerBlock(proto,\"%s\");\n",
			objName );
	}
	(void) fclose(fp);
    }  /* htmlOnly */

/**************************************************************************
		CREATE THE HTML DOCUMENTATION FILE
*/

	sprintf (fname, "%s.htm", fullClass);
	if ((fp = fopen (fname, "w")) == 0) {
		perror (fname);
		exit (1);
	}

	fprintf (fp, "<!-- documentation file generated from %s by %s -->\n",
		 inputFile, progName);
	fprintf (fp, "<html>\n<head>\n<title>%s %s star</title>\n</head>\n",
                 domain, objName);
/* Background color (this color is called "ivory2") */
        fprintf (fp, "<body bgcolor=\"#eeeee0\">\n");

/* Name */
	fprintf (fp,
        "<h1><a name=\"%s star, %s domain\">%s star in %s domain</a></h1>\n",
        objName, domain, objName, domain);

/* short descriptor */
	fprintf (fp, "<p>\n");
	if (objDesc) {
		/*
		 * print descriptor with "\n" replaced with NEWLINE,
		 * and "\t" replaced with a tab.
		 * Any other escaped character will be printed as is.
		 */
		if(unescape(descriptString, objDesc, MEDBUFSIZE))
		    yywarn("warning: Descriptor too long. May be truncated.");
		fprintf (fp, "%s\n", descriptString);
	}
	fprintf (fp, "<p>\n");

/* base class and domain */
	/* For stars, we append the domain name to the beginning of the name,
	   unless it is already there */
	if (derivedFrom) {
		if (domain &&
		    strncmp (domain, derivedFrom, strlen (domain)) != 0) {
			sprintf (baseClass, "%s%s", galDef ? "" : domain,
				 derivedFrom);
                        derivedSimple = derivedFrom;
		} else {
			(void) strcpy (baseClass, derivedFrom);
                        derivedSimple = derivedFrom + strlen(domain);
                }
                /* Put in a hyperlink to the domain index */
                fprintf (fp, "<b>Derived from:</b> <a href=\"$PTOLEMY/src/domains/%s/domain.idx#%s \">%s</a><br>\n", cvtToLower(domain), derivedSimple, baseClass);
	}
	/* Not explicitly specified: baseclass is Galaxy or XXXStar */
	else if (galDef) {
		(void)strcpy (baseClass, "Galaxy");
                fprintf (fp, "<b>Derived from:</b> Galaxy<br>\n");
        } else {
		sprintf (baseClass, "%sStar", domain);
                fprintf (fp, "<b>Derived from:</b> <a href=\"$PTOLEMY/src/domains/%s/kernel/%sStar.cc\">%sStar</a><br>\n",
			cvtToLower(domain), domain, domain);
        }
	/* Since we don't know if the values in alsoDerivedFrom
	 * are stars, we cannot easily include hyperlinks
	 */
	if (nAlsoDerivedFrom > 0) {
                /* Put in a hyperlink to the domain index */
                fprintf (fp, "<b>Also Derived from:</b>");
		for( i = 0; i < nAlsoDerivedFrom; i++ ) {
		    if (i>0) fprintf(fp,", ");
	            fprintf (fp, "<code>%s</code>", alsoDerivedFrom[i]);
		}
	}

/* location */
	if (objLocation)
		fprintf (fp, "<b>Location:</b> %s<br>\n", objLocation);

/* version */
	if (objVer && objDate)
		fprintf (fp, "<b>Version:</b> %s %s<br>\n", objVer, objDate);

/* author */
	if (objAuthor)
		fprintf (fp, "<b>Author:</b> %s<br>\n", objAuthor);

/* acknowledge */
	if (objAcknowledge)
		fprintf (fp, "<b>Acknowledgements:</b> %s<br>\n",
                objAcknowledge);

/* inputs */
	if ((int)strlen(inputDescHTML) > 0)
            fprintf (fp, "<h2>Inputs</h2>\n<table BORDER=\"1\">\n%s</table>\n",
            inputDescHTML);

/* outputs */
	if ((int)strlen(outputDescHTML) > 0)
            fprintf (fp, "<h2>Outputs</h2>\n<table BORDER=\"1\">\n%s</table>\n",
            outputDescHTML);

/* inouts */
	if ((int)strlen(inoutDescHTML) > 0)
            fprintf (fp, "<h2>InOut Ports</h2>\n<table BORDER=\"1\">\n%s</table>\n",
            inoutDescHTML);

/* states */
	if ((int)strlen(stateDescHTML) > 0)
            fprintf (fp, "<h2>States</h2>\n<table BORDER=\"1\">\n%s</table>\n",
            stateDescHTML);

/* htmldoc */
	if (objHTMLdoc)
		fprintf (fp, "<h2>Details</h2><p>\n%s\n<p>\n", objHTMLdoc);

/* ID block (will appear in .h and .cc files only. */

/* See Also list */
	if (nSeeAlso > 0) fprintf (fp, "<p><b>See also:</b> ");
	if (nSeeAlso > 2) {
	    checkSeeAlsos(nSeeAlso);
	    for (i = 0; i < (nSeeAlso - 2); i++) {
		seeAlsoGenerate(fp, domain, seeAlsoList[i]);
                fprintf (fp, ",\n");
	    }
	}
	if (nSeeAlso > 1) {
		seeAlsoGenerate(fp, domain, seeAlsoList[nSeeAlso-2]);
                fprintf (fp, " and\n");
	}
	if (nSeeAlso > 0) {
		seeAlsoGenerate(fp, domain, seeAlsoList[nSeeAlso-1]);
                fprintf (fp, ".\n<br>\n");
	}

/* Hyperlink to the source code.  Note that this assumes the source */
/* is in the same directory. */
        fprintf(fp,
                "<br><b>See:</b> <a href=\"%s%s.pl\">source code</a>,\n",
                domain, objName);

/* Hyperlink to the users of this star.
 * Note that the index file might say 'foo facet, XXX user' or
 * 'foo facet, XXX users' depending on whether there is one or more users
 */
	fprintf (fp,
		 " <a href=\"$PTOLEMY/src/domains/%s/domain.idx#%s facet, %s user\">%s users</a>\n",
		 cvtToLower(domain), objName, cvtToUpper(domain), objName);

/* copyright */
	if (objCopyright) {
                fprintf (fp, "<p><hr><p>\n");
		if ( (startp = strstr(objCopyright,"$PTOLEMY/copyright"))) {
			/* Substitute in a hyperlink */
			copyrightStart = strdup(objCopyright);
			copyrightStart[startp-objCopyright] = '\0';

			fprintf(fp, "%s<a href=\"$PTOLEMY/copyright\">$PTOLEMY/copyright</a>%s\n",
				copyrightStart,
				startp+strlen("$PTOLEMY/copyright"));
			free(copyrightStart);
		} else {
			fprintf(fp, "%s\n", objCopyright);
		}
	}

	fprintf(fp, "</body>\n</html>\n");

/* close the file */
	(void) fclose (fp);
}



struct tentry {
	char* key;
	int code;
};

/* keyword table */
struct tentry keyTable[] = {
	{"access", ACCESS},
	{"acknowledge", ACKNOWLEDGE},
	{"alias", ALIAS},
	{"alsoderived", ALSODERIVED},
	{"alsoDerivedFrom", ALSODERIVED},
	{"alsoderivedfrom", ALSODERIVED},
	{"arglist", ARGLIST},
	{"attrib", ATTRIB},
	{"attributes", ATTRIB},
	{"author", AUTHOR},
	{"begin", BEGIN},
	{"ccinclude", CCINCLUDE},
	{"class", CLASS},
	{"code", CODE},
	{"codeblock", CODEBLOCK},
	{"conscalls", CONSCALLS},
	{"consCalls", CONSCALLS},
	{"constructor", CONSTRUCTOR},
	{"copyright", COPYRIGHT},
	{"corecategory", CORECATEGORY},
	{"coreCategory", CORECATEGORY},
	{"corona", CORONA},
	{"default", DEFAULT},
	{"defcore", DEFCORE},
	{"defcorona", DEFCORONA},
	{"defstar", DEFSTAR},
	{"defstate", DEFSTATE},
	{"derived", DERIVED},
	{"derivedFrom", DERIVED},
	{"derivedfrom", DERIVED},
	{"desc", DESC},
	{"descriptor", DESC},
	{"destructor", DESTRUCTOR},
	{"domain", DOMAIN},
	{"execTime", EXECTIME},
	{"exectime", EXECTIME},
	{"explanation", EXPLANATION},
	{"galaxy", GALAXY},
	{"go", GO},
	{"header", HEADER},
	{"hinclude", HINCLUDE},
	{"htmldoc", HTMLDOC},
	{"ident", ID},
	{"initCode", INITCODE},
	{"initcode", INITCODE},
	{"inmulti", INMULTI},
	{"inline", INLINE},
	{"inout", INOUT},
	{"inoutmulti", INOUTMULTI},
	{"input", INPUT},
	{"location", LOCATION},
	{"method", METHOD},
	{"name", NAME},
	{"num", NUM},
	{"numports", NUMPORTS},
	{"numTokens", NUM},
	{"numtokens", NUM},
	{"outmulti", OUTMULTI},
	{"output", OUTPUT},
	{"private", PRIVATE},
	{"programmer", AUTHOR},
	{"protected", PROTECTED},
	{"public", PUBLIC},
	{"pure", PURE},
	{"seealso", SEEALSO},
	{"setup", SETUP},
	{"star", STAR},
	{"start", START},
	{"state", DEFSTATE},
	{"static", STATIC},
        {"tick", TICK},
	{"type", TYPE},
	{"version", VERSION},
	{"virtual", VIRTUAL},
	{"wrapup", WRAPUP},
	{0, 0},
};

int
yyinput() {
    int	c;
    if ( (c=getc(yyin)) == NEWLINE ) {
	++yyline;
    }
    return c;
}


#ifdef notdef
/*
 * In codeMode, we look for LINEs and return them.
 * A LINE is an exact copy of of line of input that
 * does not contain the closing '}'.
 * When a line is encountered that contains the closing '}'
 * that closing '}' is returned.  Anything else on the line is lost.
 */
int
yylexCode_old(pCurChar)
    int *pCurChar;
{
    int		inQuote = 0;
    char	*p = yytext;
    int		c = *pCurChar;
    int		rettok = LINE;

    /* eat spaces until a newline */
    while (c==0 || (isspace(c) && c != NEWLINE))
	c = yyinput();
    /* now eat the newline */
    if (c == NEWLINE) c = yyinput();
    /* now transfer characters to yytext until the next newline,
       or the closing brace. */
    while (c != NEWLINE) {
	*p++ = c;
	switch (c) {
		/* one backslash in input becomes two in output */
	  case ESC:
	    *p++ = c;
	    break;
	  case QUOTE:
	    /* quote in input is escaped */
	    p[-1] = ESC;
	    *p++ = c;
	    inQuote = !inQuote;
	    break;
	  case EOF:
	    yyerror ("Unexpected EOF in body!");
	    exit (1);
	  default:
	    if (!inQuote) {
		if (c == '{') codeModeBraceCount++;
		else if (c == '}') {
		    if ( --codeModeBraceCount == 0 ) {
			/*{*/ rettok = '}';
			c = 0;
			goto done;
		    }
		}
	    }
	}
	c = yyinput();
    }
done:
    /* output doesn't include the NEWLINE or right-brace */
    *pCurChar = c;
    p[0] = 0;
    yylval = save(yytext);
    return rettok;
}

#endif

/*
 * new-style codeMode.  Very much like body-mode: we return an entire
 * multi-line brace-delimined token.  We dont handle ``//'' style comments
 * (or any comments, for that matter).
 */
int
yylexCode(pCurChar)
    int *pCurChar;
{
    int		inQuote = 0;
    char	*p = yytext;
    int		c = *pCurChar;
    int		brace = 1;
    int		startLine = yyline;
    int		startQLine = yyline;

    /* eat spaces until a newline */
    while (c==0 || (isspace(c) && c != NEWLINE))
	c = yyinput();
    /* now eat the newline */
    if (c == NEWLINE) c = yyinput();
    /* now transfer characters to yytext until the next newline,
       or the closing brace. */
    while (1) {
	*p++ = c;
	switch (c) {
#ifdef notdef
	case ESC:
	    c = yyinput();
	    *p++ = c;
	    break;
#endif
	case EOF:
	    sprintf(yytext, "Unterminated %s: it began on line %d",
	     inQuote ? "codeblock string" : "codeblock",
	     inQuote ? startQLine : startLine);
	    yyerror(yytext);
	    exit(1);
	case QUOTE:
	    inQuote = !inQuote;
	    startQLine = yyline;
	    break;
	case '{':
	    if ( inQuote )	break;
	    ++brace;
	    break;
	case '}':
	    if ( inQuote )	break;
	    if ( --brace == 0 ) {
		/* strip last brace and white space up to but not including
		 * the last newline.  (e.g., last char will be newline)
		 */
		*--p = ' ';
		for ( ; p>yytext && isspace(*p) && *p!=NEWLINE; --p)
		    ;
		*++p = 0;
		*pCurChar = 0;
		yylval = save(yytext);
		return BODY;
	    }
	    break;
	}
	c = yyinput();
    }
}




/* bodyMode causes a whole function or document
 * body to be returned as a single token.
 * Leading and trailing spaces are removed
 */
int
yylexBody(pCurChar)
    int *pCurChar;
{
    int		c = *pCurChar;
    char*	p = yytext;
    int		brace = 1;
    int		inQuote = 0;
    int		inComment = 0;
    int		inAt = 0, inAtNext = 0;
    int		startLine = yyline;
    int		startQLine = yyline;

    /* if !docMode, put a "#line" directive in the token */
    if (!docMode) {
	sprintf(p, "# line %d \"%s\"\n", yyline, inputFile);
	p += strlen(p);
    }

    while (brace > 0) {
	*p++ = c;
	switch (c) {
	case ESC:
	    c = yyinput();
	    *p++ = c;
	    break;
	case QUOTE:
	    if (!inComment) {
		    inQuote = !inQuote;
		    startQLine = yyline;
	    }
	    break;
	case EOF:
	    sprintf (yytext,
	      "Unterminated %s at EOF: it began on line %d",
	      inQuote ? "string" : "body block",
	      inQuote ? startQLine : startLine);
	    yyerror(yytext);
	    exit(1);
	case '/':
	    if (inQuote) break;
	    c = getc(yyin);
	    if (c == '/') {
		    inComment = 1;
		    *p++ = c;
	    } else {
		ungetc(c,yyin);
	    }
	    break;
	case NEWLINE:
	    inComment = 0;
	    break;
	default:
	    if (!inQuote && !inComment) {
	       if ( !inAt ) {
		   if (c == '@')	inAtNext = 1;
	           else if (c == '{') brace++;
	           else if (c == '}') brace--;
	       }
	    }
	}
	c = yyinput();
	inAt = inAtNext;
	inAtNext = 0;
    }
    /* The BODY token does not include the closing '}' though it is removed
     * from the input.
     */
    --p;
    /* trim trailing whitespace */
    --p;
    while (isspace(*p)) --p;
    p[1] = 0;
    yylval = save(yytext);
    *pCurChar = 0;
    return BODY;
}

/**
    Return an entire descriptor.
    descMode causes a whole descriptor body to be returned as a single token
    in the form of a string with newlines indicated as "\n" and quotes
    escaped (\").
**/
int
yylexDesc(pCurChar)
    int *pCurChar;
{
    int		c = *pCurChar;
    char*	p = yytext;
    int		brace = 1;
    int		inQuote = 0;
    while (brace > 0) {
	*p++ = c;
	switch (c) {
	case ESC:
		c = yyinput();
		*p++ = c;
		break;
	case QUOTE:
		/* escape the quote */
		--p;
		*p++ = ESC;
		*p++ = QUOTE;
		inQuote = !inQuote;
		break;
	case NEWLINE:
		/* replace with "\n" */
		--p;
		*p++ = ESC;
		*p++ = 'n';
		break;
	case EOF:
		yyerror ("Unexpected EOF in descriptor!");
		exit (1);
	default:
		if (!inQuote) {
		  if (c == '{') brace++;
		  else if (c == '}') brace--;
		}
		break;
	}
	c = yyinput();
    }
   /* The BODY token does not include the closing '}' though it is removed
    * from the input.
    */
    --p;
    /* trim trailing whitespace or '\n' */
    --p;
    while (isspace(*p) || (*p == 'n' && *(p-1) == ESC)) {
	    if(*p == 'n') p -= 2;
	    else --p;
    }
    p[1] = 0;
    *pCurChar = 0;
    yylval = save(yytext);
    return BODY;
}



/**
    regular code (not BODY mode)
   loop to eat up blanks and comments.  A comment starts with // and
   continues for the rest of the line.  If a single / is seen in the
   loop a '/' token is returned.
**/
int
yylexNormal(pCurChar)
    int *pCurChar;
{
    int		c = *pCurChar;
    char    	*p = yytext;
    int		key, isurl = 0;

    while (1) {
	if (c != '/') {
	    break;
	} else {
		c = yyinput();
		if (c != '/') {
			*yytext = '/';
			yytext[1] = 0;
			*pCurChar = c;
			return '/';
		}
		/* comment -- eat rest of line */
		while ((c=yyinput()) != NEWLINE && c != EOF);
	}
	while (isspace(c)) { c = yyinput(); }
    }
    if (c == EOF) {
	*pCurChar = c;
	return 0;		
    }
    if (c == QUOTE) {
	/*
	 * STRING token includes surrounding quotes
	 * If the STRING includes a NEWLINE, a warning is issued.
	 */
	*p++ = c;
	while (1) {
		*p++ = c = yyinput();
		if (c == QUOTE) {
			*p = 0;
			break;
		}
		else if (c == ESC) {
			*p++ = c = yyinput();
		}
		else if (c == NEWLINE) {
			yywarn ("warning: multi-line string");
		}
		else if (c == EOF) {
			yyerror ("Unexpected EOF in string");
			exit (1);
		}
	}
	*pCurChar = 0;
	yylval = save(yytext);
	return STRING;
    } else if (c == '<') {
        /* Token like <stdio.h> */
	p = yytext;
	*p++ = c;
	while (1) {
	    *p++ = c = yyinput();
	    if (c == '>') {
		    *p = 0;
		    break;
	    } else if (c == EOF) {
		    yyerror ("Unexpected EOF in <> string");
		    exit (1);
	    }
	}
	*pCurChar = 0;
	yylval = save(yytext);
	return STRING;
    } else if (! IDENTCHAR(c) && ! urlchar(c)) {
	    yytext[0] = c;
	    yytext[1] = 0;
	    *pCurChar = 0;
	    return yytext[0];
    } else {
        /* we also return numeric values as IDENTIFIER: 
	 * digits and '.' allowed
	 */
        do {
	    if (urlchar(c))
		isurl = 1;
	    *p++ = c;
	    c = yyinput();
        } while ( IDENTCHAR(c) || urlchar(c));
    }
    *p = 0;
    *pCurChar = c;
    yylval = save(yytext);
    if ((key = lookup (yytext)) != 0) {
	    return key;
    }
    if (isurl) 
	return URL;
    return IDENTIFIER;
}

/* #define input() ((c = getc(yyin))==10?(yyline++,c):c) */

/* The lexical analyzer */
int yylex () {
    static int	c = 0;
    if (c == EOF) return 0;

    if (codeMode) {
	return yylexCode(&c);
    }
    while (c==0 || isspace(c)) {
	    c = yyinput();
    }
    if (bodyMode) {
	return yylexBody(&c);
    }

    if (descMode) {
	return yylexDesc(&c);
    }
    return yylexNormal(&c);
}

int lookup (text)
char* text;
{
	struct tentry *p = keyTable;
	while (p->key && strcmp (p->key, text) != 0) p++;
	return p->code;
}

/*
 * save token in dynamic memory
 * For the processing of codeblocks, its important that NULLs stay
 * NULL and that the empty string should remain an entry string.
 */
char* save(in)
char* in;
{
	char* out;
	if ( in == NULL )	return NULL;
	out = malloc((unsigned)strlen(in)+1);
	strcpy(out,in);
	return out;
}

char* savelineref()
{
    char buf[SMALLBUFSIZE];
    sprintf(buf, "# line %d \"%s\"", yyline, inputFile);
    return save(buf);
}

/**
    strip quotes, save token in dynamic memory
    A previous version of this used to special case the situation where
    the string was only two characters (presumbly just ``""'') and would
    return exactly that.  This breaks things, so now return an empty string
    (instead of ``""'') is returned -- kennard
**/
char* stripQuotes(in)
char* in;
{
	char* out;
	int l = strlen(in);
	if ( l<2 || in[0]!=QUOTE || in[l-1]!=QUOTE ) {
		yyerror("String without quotes in stripQuotes().");
		return save(in);
	}
	if ( l == 2 ) { return save(""); }
	out = malloc((unsigned)l-1);
	strncpy(out,in+1,l-2);
	/* strncpy does not necessarily null-terminate the string */
	out[l-2] = 0;
	return out;
}

/*
 * make sure a function is a valid arglist:
 * for now, must start with (, end with ), and have balanced parens.
 */
char* checkArgs(in)
char* in;
{
	char* ptr = in;
	int parenlevel = 0;

	if (*in != LPAR || in[strlen(in)-1] != RPAR)
		yyerror ("Invalid argument list");

	while (*ptr) {
		if (*ptr == LPAR)
			parenlevel++;
		else if (*ptr == RPAR && --parenlevel < 0)
			yyerror ("Mismatched parentheses in argument list");
		ptr++;
	}
	if (parenlevel != 0)
	  yyerror ("Mismatched parentheses in argument list");

	return in;
}

/*
 * copy arglist at *in to *out, deleting any default-argument specifications.
 * A default arg spec starts with '=' at paren level 1, and extends until a
 * ',' or ')' occuring at paren level 1.
 */
void stripDefaultArgs(out, in)
char* out;
char* in;
{
	int copying = 1;
	int parenlevel = 0;

	while (*in) {
	  switch (*in) {
	  case LPAR:
	    parenlevel++;
	    break;
	  case RPAR:
	    if (parenlevel == 1) copying = 1;
	    parenlevel--;
	    break;
	  case '=':
	    if (parenlevel == 1) copying = 0;
	    break;
	  case ',':
	    if (parenlevel == 1) copying = 1;
	    break;
	  }
	  if (copying) *out++ = *in;
	  in++;
	}
	*out = 0;
}

/*
 * copy one string into another, replacing the pattern "\n" with
 * NEWLINE, "\t" with tab, and "\x" with x for any other x.
 * The third argument is the size of the destination, which will not
 * be exceeded.
 */
int unescape(destination, source, dsize)
char* destination;
char* source;
int dsize;
{
	char* d = destination;
	char* s = source;
	int i = 1;
	while (*s != (char)NULL) {
	    if (*s == ESC) {
		switch (*(s+1)) {
		    case 'n':
			*d++ = '\n';
			break;
		    case 't':
			*d++ = '\t';
			break;
		    default:
			*d++ = *(s+1);
		}
		s += 2;
	    } else
		*d++ = *s++;
	    if(i++ >= dsize) {
		*d = (char)NULL; /* terminate the string */
		return(1);
	    }
	}
	*d = (char)NULL; /* terminate the string */
	return(0);
}

/* main program, just calls parser */
int main (argc, argv)
int argc;
char **argv;
{
	if (argc < 2 || argc > 3) {
		fprintf (stderr, "Usage: %s -htm file\n", *argv);
		exit (1);
	}
	if (argc == 3) {
		if (! strcmp(argv[1],"-htm")) { 
			htmlOnly = 1;
			inputFile = argv[2];
		} else {
			fprintf (stderr, "Usage: %s -htm file\n", *argv);
			exit (1);
		}
	} else {
		inputFile = argv[1];
	}
	if ((yyin = fopen (inputFile, "r")) == NULL) {
		perror (inputFile);
		exit (1);
	}
	yyparse ();
	return nerrs;
}

void yyerr2 (x, y)
char *x, *y;
{
	strcpy (str1, x);
	strcat (str1, y);
	yyerror (str1);
}

void yyerror(s)
char *s;
{
	/* ugly: figure out if yacc is reporting syntax error at EOF */
	if (strcmp(s, "syntax error") == 0 && yychar == 0)
		s = "Unexpected EOF (mismatched curly braces?)";
	fprintf (stderr, "\"%s\", line %d: %s\n", inputFile, yyline, s);
	nerrs++;
	return;
}

void yywarn(s)
char *s;
{
	fprintf (stderr, "\"%s\", line %d: %s\n", inputFile, yyline, s);
	return;
}

void mismatch(s)
char *s;
{
	yyerr2 ("Extra token appears after valid input: ", s);
	exit (1);
}

/* Check that we are not blowing the top off an array */
void checkIncludes(numIncludes)
int numIncludes;
{
	if (numIncludes > NINC) {
		fprintf (stderr, 
    "Too many include files(%d), recompile ptlang with NINC (%d) larger.\n",
		numIncludes, NINC);
		exit(1);
	}
}

/* Check that we are not blowing the top off an array */
void checkSeeAlsos(numSeeAlsos)
int numSeeAlsos;
{
	if (numSeeAlsos > NSEE) {
		fprintf (stderr, 
	 "Too many see alsos (%d), recompile ptlang with NSEE (%d) larger.\n",
		numSeeAlsos, NSEE);
		exit(1);
	}
}


/* A seealso can have a url in it.  If it does, then we just
 * want to use the URL
 */
void seeAlsoGenerate(fp,domain,seeAlso)
FILE *fp;
char *domain;
char *seeAlso;
{
	if (strchr(seeAlso,'/')) {
		/* This is a URL, just print it */
		fprintf(fp, "<a href=\"%s\">%s</a>", seeAlso, seeAlso);
	} else {
		/* This is not a URL, so we use the following conventions:
		 * If it is capitalized, it is a star.
		 * If it is lower case, it is a facet. 
		 */
		if (isupper(seeAlso[0])) {
			/* Interstellar Hyperdrive (A link to a star) */
	                fprintf (fp, "<a href=\"$PTOLEMY/src/domains/%s/domain.idx#%s star, %s domain\">%s</a>",
				cvtToLower(domain), seeAlso, 
				cvtToUpper(domain), seeAlso);

		} else	if (islower(seeAlso[0])) {
			/* A facet */
	                fprintf (fp, "<a href=\"$PTOLEMY/src/domains/%s/domain.idx#%s universe, %s domain\">%s</a>",
				cvtToLower(domain), seeAlso,
				cvtToUpper(domain), seeAlso);
		} else {
			/* Does not start with a alpha numeric, so we
			 * just create a links with a trailing space.
			 */
	                fprintf (fp, "<a href=\"$PTOLEMY/src/domains/%s/domain.idx#%s \">%s</a>",
				cvtToLower(domain), seeAlso, seeAlso);
		}
	}
}

