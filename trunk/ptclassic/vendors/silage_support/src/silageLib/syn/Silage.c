/* $Id$ */

extern char *malloc(), *realloc();
# define DBLCOLON 257
# define RIGHTARROW 258
# define DOTDOT 259
# define BOX 260
# define DBLBAR 261
# define ATAT 262
# define PNDPND 263
# define LEFTARROW 264
# define INTEGER 265
# define EQUAL 266
# define NOTEQUAL 267
# define LESSEQUAL 268
# define GREATEREQUAL 269
# define LEFTSHIFT 270
# define RIGHTSHIFT 271
# define FIXEDNUM 272
# define ID 273
# define BEGIN_ 274
# define END 275
# define FUNC 276
# define FIX 277
# define PRAGMA 278
# define IF 279
# define FI 280
# define DIV 281
# define INT 282
# define COS 283
# define SIN 284
# define SQRT 285
# define MIN 286
# define MAX 287
# define BIT 288
# define BITSELECT 289
# define BITMERGE 290
# define WHILE 291
# define DO 292
# define OD 293
# define EXIT 294
# define TIXE 295
# define INTERPOLATE 296
# define DECIMATE 297
# define TIMEMUX 298
# define TIMEDEMUX 299
# define UPSAMPLE 300
# define DOWNSAMPLE 301
# define BOOL 302

# line 32 "Silage.y"
#include <stdio.h>
#include "silage.h"

NodeType *TheTree;


# line 39 "Silage.y"
typedef union  {
    NodeType *node;
    struct { int LineNum,CharNum; char *FileName; int V; } token;
} YYSTYPE;
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 321 "Silage.y"



static int CurrentLine, CurrentChar;
static char *CurrentFileName;
static FILE *fparser;
static int FatalError = 0;


#include "silageLex.c"

/* extern char Version[]; */

SilageParser(FileName)
char *FileName;
{
    char command[128];
    FILE *popen();

    InitParser();
#ifdef APOLLO
    sprintf(command,"/usr/lib/cpp %s",FileName);
#else
    sprintf(command,"/lib/cpp %s", FileName);
#endif
    if ((fparser = popen(command,"r"))==NULL){
	fprintf(stdout, "CAN'T OPEN PIPE TO C-PREPROCESSOR\n");
    }
    if (yyparse() || FatalError == 1) {
	fprintf(stdout, "Fatal errors in Silage Input - Program Exit\n");
	exit (1);
    }
    return (0);
}
int yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 2,
	0, 1,
	-2, 0,
	};
# define YYNPROD 120
# define YYLAST 2204
int yyact[]={

    26,    48,   261,   186,   215,   154,    46,    28,   215,   213,
   220,   214,    24,   275,   271,   238,    45,   264,    99,   259,
   269,   215,   213,    42,   214,   173,    47,   258,   219,   100,
   117,   258,   111,   215,   213,   223,   214,   255,    26,   236,
   215,   213,   222,   214,   263,    28,   215,   213,   116,   214,
    24,    82,     4,   253,     4,   257,    99,   114,   277,   273,
   223,   268,   147,   225,   119,   146,   212,    78,    53,   100,
    81,   241,   274,    52,   156,   209,    26,   224,   173,    51,
    98,   208,   113,    28,   173,    80,   178,   110,    24,   207,
    41,   177,   173,    25,   182,   206,    58,   205,   173,   204,
   173,   203,   173,   109,   173,   202,   172,   100,   173,   201,
   199,   108,   173,   173,    26,   159,   107,   106,   160,   105,
   104,    28,   103,   102,    94,    56,    24,    90,    41,    89,
    88,    25,   161,    87,    86,   163,   164,   165,   166,   167,
   168,   169,   170,   171,     3,    10,    11,    50,   265,   115,
   244,   144,    49,   243,   143,     8,    18,    58,    15,    43,
     9,     7,     5,     2,     1,   142,    41,     0,     0,    25,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    26,    58,     0,     0,     0,    56,     0,    28,    65,
    63,     0,    64,    24,    66,     0,   256,     0,   101,   118,
   188,     0,     0,     0,    41,     0,     0,    25,     0,     0,
     0,    56,     0,   144,     0,     0,     0,     0,     0,    26,
     0,     0,     0,    83,     0,     0,    28,     0,     0,     0,
    45,    24,    44,     0,     0,     0,     0,     0,   101,    30,
    45,     0,   280,   216,    48,    14,    27,   216,   242,    46,
    19,    20,    21,    22,    23,    32,    33,    34,    17,    16,
   216,    83,   242,    35,    36,    37,    38,    39,    40,    47,
    44,    41,   216,    68,    25,   227,   101,    30,    45,   216,
   279,   270,    48,    14,    27,   216,   183,    46,    19,    20,
    21,    22,    23,    32,    33,    34,    17,    16,   144,    83,
   260,    35,    36,    37,    38,    39,    40,    47,    44,    41,
   254,    44,    25,     0,     0,    30,    45,   239,   247,    45,
    48,    14,    27,    57,    59,    46,    19,    20,    21,    22,
    23,    32,    33,    34,    17,    16,     0,    83,     0,    35,
    36,    37,    38,    39,    40,    47,    44,   144,     0,   144,
     0,   246,     0,    30,    45,   251,   185,   276,    48,    14,
    27,   278,     0,    46,    19,    20,    21,    22,    23,    32,
    33,    34,    17,    16,     0,     0,     0,    35,    36,    37,
    38,    39,    40,    47,    57,    59,     0,    29,     0,     0,
    58,     0,    69,    70,     0,     0,     0,    65,     0,    26,
     0,     0,    66,     0,     6,     0,    28,     0,     0,    57,
    59,    24,     0,    44,     0,     0,     0,    69,    70,    56,
    30,    45,     0,     0,    12,    48,    14,    27,    67,     0,
    46,    19,    20,    21,    22,    23,    32,    33,    34,    17,
    16,     0,    83,     0,    35,    36,    37,    38,    39,    40,
    47,    44,     0,     0,    26,     0,     0,     0,    30,    45,
     0,    28,     0,    48,    14,    27,    24,     0,    46,    19,
    20,    21,    22,    23,    32,    33,    34,    17,    16,     0,
     0,     0,    35,    36,    37,    38,    39,    40,    47,    41,
     0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
    85,     0,   176,     0,   176,    24,     0,     0,     0,     0,
   240,     0,   245,   175,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    26,     0,     0,     0,     0,     0,     0,    85,     0,
     0,     0,   266,    24,    41,     0,     0,    25,     0,     0,
   266,     0,   272,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   176,   176,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    58,    41,     0,    61,    25,     0,     0,    65,
    63,     0,    64,     0,    66,     0,     0,     0,     0,     0,
     0,   176,   176,   176,   176,     0,     0,    72,   176,    71,
     0,    56,     0,     0,     0,     0,     0,    57,    59,     0,
     0,    41,     0,     0,    25,    69,    70,     0,     0,     0,
   180,    44,     0,     0,     0,     0,    67,     0,    30,    45,
   189,    62,     0,    48,    14,    27,   176,     0,    46,    19,
    20,    21,    22,    23,    32,    33,    34,    17,    16,     0,
     0,     0,    35,    36,    37,    38,    39,    40,    47,     0,
     0,    60,     0,    68,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    44,     0,     0,     0,
     0,   217,   218,    30,    45,    79,     0,     0,    48,    14,
    27,     0,     0,    46,    19,    20,    21,    22,    23,    32,
    33,    34,    17,    16,     0,     0,     0,    35,    36,    37,
    38,    39,    40,    47,     0,    44,     0,   232,   233,   234,
   235,     0,    30,    45,   237,     0,     0,    48,     0,    27,
     0,     0,    46,    19,    20,    21,    22,    23,    32,    33,
    34,     0,     0,     0,   226,     0,    35,    36,    37,    38,
    39,    40,    47,    44,     0,     0,     0,     0,     0,     0,
    30,    45,   267,     0,     0,    48,     0,    27,     0,     0,
    46,    19,    20,    21,    22,    23,    32,    33,    34,     0,
     0,     0,     0,     0,    35,    36,    37,    38,    39,    40,
    47,     0,     0,     0,     0,     0,     0,     0,     0,    57,
    59,     0,     0,    75,    76,    74,    73,    69,    70,     0,
     0,     0,     0,    58,     0,     0,    61,     0,    67,   157,
    65,    63,   158,    64,     0,    66,     0,     0,     0,   148,
     0,     0,     0,     0,     0,     0,     0,     0,    72,     0,
    71,    58,    56,     0,    61,     0,     0,   262,    65,    63,
     0,    64,     0,    66,     0,     0,     0,    58,     0,     0,
    61,     0,     0,     0,    65,    63,    72,    64,    71,    66,
    56,     0,    62,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    72,     0,    71,    58,    56,     0,    61,     0,
     0,   249,    65,    63,     0,    64,     0,    66,     0,     0,
    62,     0,    60,     0,    68,     0,     0,     0,     0,     0,
    72,     0,    71,     0,    56,     0,    62,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    58,     0,
    60,    61,    68,     0,   248,    65,    63,     0,    64,     0,
    66,     0,     0,     0,    62,     0,    60,     0,    68,     0,
     0,     0,     0,    72,     0,    71,     0,    56,     0,     0,
     0,    58,     0,     0,    61,     0,     0,     0,    65,    63,
     0,    64,     0,    66,    60,     0,    68,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    72,    62,    71,     0,
    56,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    58,     0,     0,    61,
     0,     0,     0,    65,    63,     0,    64,    60,    66,    68,
    62,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    72,     0,    71,     0,    56,     0,     0,     0,     0,
    57,    59,     0,     0,    75,    76,    74,    73,    69,    70,
    60,     0,    68,     0,     0,     0,     0,     0,     0,    67,
     0,     0,     0,     0,     0,    62,     0,     0,    57,    59,
     0,     0,    75,    76,    74,    73,    69,    70,     0,     0,
   155,     0,     0,     0,    57,    59,     0,    67,    75,    76,
    74,    73,    69,    70,     0,    60,     0,    68,     0,     0,
     0,     0,     0,    67,     0,     0,     0,     0,     0,     0,
     0,     0,    57,    59,     0,     0,    75,    76,    74,    73,
    69,    70,    58,     0,     0,    61,     0,     0,     0,    65,
    63,    67,    64,     0,    66,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    72,     0,    71,
     0,    56,     0,     0,     0,    57,    59,     0,     0,    75,
    76,    74,    73,    69,    70,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    67,     0,     0,     0,     0,     0,
   221,    62,     0,     0,     0,   231,     0,     0,    57,    59,
     0,     0,    75,    76,    74,    73,    69,    70,    58,     0,
     0,    61,     0,     0,   211,    65,    63,    67,    64,     0,
    66,    60,     0,    68,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    72,     0,    71,     0,    56,     0,     0,
     0,     0,   230,    57,    59,     0,     0,    75,    76,    74,
    73,    69,    70,    58,     0,     0,    61,     0,     0,     0,
    65,    63,    67,    64,     0,    66,     0,    62,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    72,     0,
    71,    58,    56,     0,    61,     0,     0,     0,    65,    63,
   194,    64,     0,    66,     0,     0,     0,    60,     0,    68,
     0,     0,     0,     0,     0,     0,    72,     0,    71,     0,
    56,   200,    62,     0,    58,     0,     0,    61,     0,     0,
     0,    65,    63,   193,    64,     0,    66,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    72,
    62,    71,    60,    56,    68,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    57,
    59,     0,     0,    75,    76,    74,    73,    69,    70,     0,
    60,     0,    68,    62,     0,     0,     0,     0,    67,    58,
     0,     0,    61,     0,     0,   192,    65,    63,     0,    64,
     0,    66,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    60,    72,    68,    71,     0,    56,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    59,     0,    62,    75,
    76,    74,    73,    69,    70,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    67,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    58,     0,    60,    61,
    68,     0,   191,    65,    63,     0,    64,     0,    66,     0,
    57,    59,     0,     0,    75,    76,    74,    73,    69,    70,
     0,    72,     0,    71,    58,    56,     0,    61,     0,    67,
   190,    65,    63,     0,    64,     0,    66,     0,    57,    59,
     0,     0,    75,    76,    74,    73,    69,    70,     0,    72,
     0,    71,     0,    56,     0,    62,     0,    67,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    57,    59,     0,     0,    75,    76,    74,    73,    69,
    70,     0,     0,    62,     0,    60,     0,    68,     0,     0,
    67,    58,     0,     0,    61,     0,     0,     0,    65,    63,
     0,    64,     0,    66,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    60,     0,    68,    72,   187,    71,     0,
    56,     0,    58,     0,     0,    61,     0,     0,     0,    65,
    63,     0,    64,     0,    66,     0,    57,    59,     0,     0,
    75,    76,    74,    73,    69,    70,     0,    72,    55,    71,
    62,    56,     0,     0,     0,    67,     0,     0,    58,     0,
     0,    61,     0,     0,     0,    65,    63,     0,    64,     0,
    66,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    60,    62,    68,    72,     0,    71,     0,    56,     0,    58,
     0,     0,    61,     0,     0,     0,    65,    63,     0,    64,
     0,    66,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    60,     0,    68,    72,     0,    71,    62,    56,     0,
     0,     0,     0,    57,    59,     0,     0,    75,    76,    74,
    73,    69,    70,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    67,     0,     0,     0,     0,    60,    62,    68,
     0,    57,    59,     0,    58,    75,    76,    74,    73,    69,
    70,    65,    63,     0,    64,     0,    66,     0,     0,     0,
    67,     0,     0,     0,     0,     0,     0,     0,    58,    72,
    68,    71,     0,    56,     0,    65,    63,     0,    64,     0,
    66,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    72,     0,    71,     0,    56,     0,     0,
     0,     0,     0,    62,     0,     0,     0,     0,    57,    59,
     0,     0,    75,    76,    74,    73,    69,    70,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    67,     0,     0,
     0,     0,     0,     0,     0,    68,     0,     0,     0,    57,
    59,     0,     0,    75,    76,    74,    73,    69,    70,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    67,    68,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    57,    59,     0,     0,    75,
    76,    74,    73,    69,    70,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    67,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    57,    59,    31,     0,
    75,    76,    74,    73,    69,    70,     0,     0,     0,     0,
     0,    54,     0,    77,     0,    67,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    96,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    57,    59,     0,     0,    75,    76,    74,    73,    69,
    70,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    67,    13,     0,     0,     0,    57,    59,     0,     0,    75,
    76,    74,    73,    69,    70,     0,     0,     0,     0,    84,
     0,     0,     0,     0,    67,     0,    91,    92,    93,    95,
    97,     0,     0,   179,     0,   179,     0,     0,   184,     0,
     0,     0,     0,   112,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   120,   121,   122,
   123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,     0,
     0,     0,   145,     0,     0,     0,     0,    97,   149,   150,
   151,   152,   153,     0,     0,     0,   179,   179,     0,     0,
     0,   112,   162,     0,   112,   112,   112,   112,   112,   112,
   112,   112,   112,     0,     0,   174,     0,     0,     0,   181,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   179,   179,   179,   179,     0,     0,     0,   179,
     0,   184,   184,     0,   184,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   195,   196,     0,
   197,     0,   198,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   184,   210,     0,   179,     0,     0,
     0,     0,   184,     0,   184,     0,     0,     0,     0,   120,
     0,     0,     0,     0,     0,   228,   229,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    95,     0,
     0,     0,   250,   252 };
int yypact[]={

   148, -1000,   148, -1000,    20,    14,     9, -1000, -1000, -1000,
 -1000, -1000,  -257,  1557,  -257,   421,   186,   498, -1000,    94,
    93,    90,    89,    87,   498,   498,   498,   498,   498, -1000,
 -1000,   -22,    83,    82,    80,    79,    77,    76,    71,    63,
    47,   498,    42, -1000, -1000, -1000,    -3, -1000,   -12,   -61,
 -1000, -1000, -1000, -1000,    24,   498,   498,   498,   498,   498,
   498,   498,   498,   498,   498,   498,   498,   498,   498,   498,
   498,   498,   498,   498,   498,   498,   498, -1000, -1000,   186,
   186, -1000,     6,     3,   547,   498,   498,   498,   498,   498,
   498,   355,   355,  1689,  -275,   832,    16,   788,    74,   498,
   498, -1000,   498,   498,   498,   498,   498,   498,   498,   498,
   498,   -19,  1593,   498,    46, -1000,    46,   498, -1000,  -257,
  1593, -1000, -1000, -1000, -1000,  1624,  1689,  1713,   355,   355,
   122,   122,   122,   355,    61,    61,   147,   147,   147,   147,
   147,   147,    81,  -290, -1000,  1526, -1000, -1000,   366,  1459,
  1431,  1344,  1279,  1246, -1000,   498,   498, -1000,   498, -1000,
   498,    69,  1218,    68,    64,    60,    58,    56,    54,    48,
    40,    34, -1000,   498,  1173,     4, -1000,    46,    46,   -62,
   -34,  1097,     1, -1000,    19, -1000, -1000,   460, -1000,   186,
 -1000, -1000, -1000,   498,   498,   981,   936,  1593,  1593, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
  1593, -1000, -1000,    46,    46,    46,    46,   -38,    -2, -1000,
    46, -1000,   -43,  -257,  -276,  -257,   498,    43,   903,   860,
   498,   498,   -38,   -38, -1000, -1000, -1000,    -9,  -276,   -24,
 -1000, -1000,   -64,  -241,   -60, -1000,  -293, -1000, -1000, -1000,
   832, -1000,   816, -1000,   -17,  -257, -1000, -1000,    46, -1000,
 -1000,     2,  -237,  -257,  -260,  -257,     0,   -21, -1000, -1000,
  -261,   186,    -1, -1000, -1000,   186,     5, -1000,   -33, -1000,
 -1000 };
int yypgo[]={

     0,   164,   163,   144,    51,   162,   161,   160,   159,  1971,
   158,    85,    70,    94,    71,   286,   156,    32,  1888,   155,
   124,   154,    23,   153,   152,    55,   150,   513,   387,   149,
    17,   148,   146,   145,    80 };
int yyr1[]={

     0,     1,     2,     2,     3,     3,     3,     4,     4,     4,
     4,     4,     6,    19,     7,     7,    10,    33,    21,    32,
    32,    11,    11,    12,    12,     5,     5,    30,    30,    31,
    31,    13,    13,    15,    15,    14,    14,    14,    22,    22,
    22,    22,    22,    29,    29,    23,    23,    26,    26,    25,
     9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
     9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
     9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
     9,     9,    16,    16,    16,    16,    16,    16,    16,    16,
    16,    16,    16,    16,    16,    16,    16,    27,    27,    27,
    27,    27,    27,    27,    28,    28,    20,    20,    20,     8,
     8,    24,    24,    24,    24,    34,    34,    17,    17,    18 };
int yyr2[]={

     0,     3,     3,     5,     5,     5,     5,     2,     2,     2,
     2,     2,     7,     5,     5,     9,    17,     9,    13,     9,
    13,     3,     5,     5,     5,    25,    23,     1,     2,     4,
     7,     2,     7,     7,     7,     3,     3,     3,     3,     9,
     3,     3,     5,     7,    11,     5,     5,     5,     5,     7,
     2,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     9,     9,     9,    13,    13,     7,     7,     7,
     7,     5,     5,     5,     7,     7,     7,     7,     7,     7,
     7,     7,     2,     2,     9,     9,     9,     9,     9,     9,
     9,     9,     9,     9,     7,     7,     9,     2,     7,     7,
     7,     7,     5,     7,     2,     3,     7,    11,    11,     3,
     3,     9,     5,     9,     5,     7,     7,     3,     7,     3 };
int yychk[]={

 -1000,    -1,    -2,    -3,    -4,    -5,   256,    -6,   -19,    -7,
   -33,   -32,   276,    -9,   278,   -10,   292,   291,   -16,   283,
   284,   285,   286,   287,    45,   126,    33,   279,    40,   -28,
   272,   -18,   288,   289,   290,   296,   297,   298,   299,   300,
   301,   123,   -22,    -8,   265,   273,   282,   302,   277,   -24,
    -3,    59,    59,    59,   -18,    61,    64,   262,    35,   263,
   124,    38,    94,    43,    45,    42,    47,   281,   126,   270,
   271,    62,    60,   269,   268,   266,   267,   -18,    -4,   274,
   -11,   -12,    -4,   256,    -9,    40,    40,    40,    40,    40,
    40,    -9,    -9,    -9,   -20,    -9,   -18,    -9,   -34,    40,
    91,   260,    40,    40,    40,    40,    40,    40,    40,    40,
    40,   -17,    -9,    40,    60,   -29,    60,    91,   260,    40,
    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
    -9,    -9,   -11,   -21,   -12,    -9,    59,    59,   292,    -9,
    -9,    -9,    -9,    -9,   280,   258,    58,    41,    44,    41,
    44,   -17,    -9,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
   -17,   -17,   125,    44,    -9,   -27,   -28,    45,    40,   -18,
   -27,    -9,   -13,   -15,   -18,   275,   293,    61,    -4,   274,
    41,    41,    41,    44,    44,    -9,    -9,    -9,    -9,    41,
    93,    41,    41,    41,    41,    41,    41,    41,    41,    41,
    -9,    41,    62,    43,    45,    42,   281,   -27,   -27,    62,
    44,    93,    41,    59,    58,    44,   294,   -11,    -9,    -9,
   261,   259,   -27,   -27,   -27,   -27,    41,   -27,    58,   -13,
   -15,   -14,   -22,   -23,   -26,   -15,   -20,   275,    41,    41,
    -9,   -20,    -9,    62,   -14,    61,   260,   -25,    91,   260,
   -25,   295,    41,    61,   -30,   -31,   -15,   -27,    59,   257,
   -30,   274,   -15,    59,    93,   274,   -11,    59,   -11,   275,
   275 };
int yydef[]={

     0,    -2,    -2,     2,     0,     0,     0,     7,     8,     9,
    10,    11,     0,     0,     0,     0,     0,     0,    50,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    82,
    83,   109,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   104,   105,   119,    38,    40,    41,   110,
     3,     4,     5,     6,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    13,    14,     0,
     0,    21,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    71,    72,    73,     0,     0,   109,     0,     0,     0,
     0,   112,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   117,     0,     0,    42,     0,     0,   114,     0,
    12,    51,    52,    53,    54,    55,    56,    57,    58,    59,
    60,    61,    67,    68,    69,    70,    74,    75,    76,    77,
    78,    79,     0,     0,    22,     0,    23,    24,     0,     0,
     0,     0,     0,     0,    80,     0,     0,    81,     0,    94,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    95,     0,     0,     0,    97,     0,     0,   109,
     0,     0,     0,    31,     0,    15,    17,     0,    19,     0,
    62,    63,    64,     0,     0,   106,     0,   115,   116,    84,
   111,    85,    86,    87,    88,    89,    90,    91,    92,    93,
   118,    96,    39,     0,     0,     0,     0,   102,     0,    43,
     0,   113,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    98,    99,   100,   101,   103,     0,     0,     0,
    32,    33,    35,    36,    37,    34,     0,    20,    65,    66,
   107,   108,     0,    44,     0,    27,    45,    47,     0,    46,
    48,     0,     0,    27,     0,    28,     0,     0,    18,    16,
     0,     0,     0,    29,    49,     0,     0,    30,     0,    26,
    25 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"DBLCOLON",	257,
	"RIGHTARROW",	258,
	"DOTDOT",	259,
	"BOX",	260,
	"DBLBAR",	261,
	"ATAT",	262,
	"PNDPND",	263,
	"LEFTARROW",	264,
	"INTEGER",	265,
	"EQUAL",	266,
	"NOTEQUAL",	267,
	"LESSEQUAL",	268,
	"GREATEREQUAL",	269,
	"LEFTSHIFT",	270,
	"RIGHTSHIFT",	271,
	"FIXEDNUM",	272,
	"ID",	273,
	"BEGIN_",	274,
	"END",	275,
	"FUNC",	276,
	"FIX",	277,
	"PRAGMA",	278,
	"IF",	279,
	"FI",	280,
	"DIV",	281,
	"INT",	282,
	"COS",	283,
	"SIN",	284,
	"SQRT",	285,
	"MIN",	286,
	"MAX",	287,
	"BIT",	288,
	"BITSELECT",	289,
	"BITMERGE",	290,
	"WHILE",	291,
	"DO",	292,
	"OD",	293,
	"EXIT",	294,
	"TIXE",	295,
	"INTERPOLATE",	296,
	"DECIMATE",	297,
	"TIMEMUX",	298,
	"TIMEDEMUX",	299,
	"UPSAMPLE",	300,
	"DOWNSAMPLE",	301,
	"BOOL",	302,
	"|",	124,
	"&",	38,
	"!",	33,
	"^",	94,
	"<",	60,
	">",	62,
	"+",	43,
	"-",	45,
	"~",	126,
	"*",	42,
	"/",	47,
	"@",	64,
	"#",	35,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"program : outerDefnSequence",
	"outerDefnSequence : outerDefn",
	"outerDefnSequence : outerDefnSequence outerDefn",
	"outerDefn : valueDefns ';'",
	"outerDefn : functionDefn ';'",
	"outerDefn : error ';'",
	"valueDefns : valueDefn",
	"valueDefns : pragma",
	"valueDefns : iteratedDefn",
	"valueDefns : doDefn",
	"valueDefns : whileDefn",
	"valueDefn : expr '=' expr",
	"pragma : PRAGMA ident",
	"iteratedDefn : iterator valueDefns",
	"iteratedDefn : iterator BEGIN_ valueDefnSequence END",
	"iterator : '(' ident ':' expr DOTDOT expr ')' DBLCOLON",
	"doDefn : DO valueDefnSequence exitDefn OD",
	"exitDefn : expr '=' EXIT if_body TIXE ';'",
	"whileDefn : WHILE expr DO valueDefns",
	"whileDefn : WHILE expr DO BEGIN_ valueDefnSequence END",
	"valueDefnSequence : innerDefn",
	"valueDefnSequence : valueDefnSequence innerDefn",
	"innerDefn : valueDefns ';'",
	"innerDefn : error ';'",
	"functionDefn : FUNC ident '(' formals ')' ':' type '=' declarations BEGIN_ valueDefnSequence END",
	"functionDefn : FUNC ident '(' formals ')' formals '=' declarations BEGIN_ valueDefnSequence END",
	"declarations : /* empty */",
	"declarations : declarationSequence",
	"declarationSequence : formalItem ';'",
	"declarationSequence : declarationSequence formalItem ';'",
	"formals : formalItem",
	"formals : formals ';' formalItem",
	"formalItem : ident ':' type",
	"formalItem : ident ',' formalItem",
	"type : typeNum",
	"type : typeBox",
	"type : typeValuedBox",
	"typeNum : INT",
	"typeNum : INT '<' manexpr '>'",
	"typeNum : BOOL",
	"typeNum : FIX",
	"typeNum : FIX typeSpec",
	"typeSpec : '<' manexpr '>'",
	"typeSpec : '<' manexpr ',' manexpr '>'",
	"typeBox : typeNum BOX",
	"typeBox : typeBox BOX",
	"typeValuedBox : typeNum valuedBox",
	"typeValuedBox : typeValuedBox valuedBox",
	"valuedBox : '[' manexpr ']'",
	"expr : primary",
	"expr : expr '@' expr",
	"expr : expr ATAT expr",
	"expr : expr '#' expr",
	"expr : expr PNDPND expr",
	"expr : expr '|' expr",
	"expr : expr '&' expr",
	"expr : expr '^' expr",
	"expr : expr '+' expr",
	"expr : expr '-' expr",
	"expr : expr '*' expr",
	"expr : expr '/' expr",
	"expr : COS '(' expr ')'",
	"expr : SIN '(' expr ')'",
	"expr : SQRT '(' expr ')'",
	"expr : MIN '(' expr ',' expr ')'",
	"expr : MAX '(' expr ',' expr ')'",
	"expr : expr DIV expr",
	"expr : expr '~' expr",
	"expr : expr LEFTSHIFT expr",
	"expr : expr RIGHTSHIFT expr",
	"expr : '-' expr",
	"expr : '~' expr",
	"expr : '!' expr",
	"expr : expr '>' expr",
	"expr : expr '<' expr",
	"expr : expr GREATEREQUAL expr",
	"expr : expr LESSEQUAL expr",
	"expr : expr EQUAL expr",
	"expr : expr NOTEQUAL expr",
	"expr : IF if_body FI",
	"expr : '(' expr ')'",
	"primary : manprimary",
	"primary : FIXEDNUM",
	"primary : ident '(' exprSequence ')'",
	"primary : BIT '(' exprSequence ')'",
	"primary : BITSELECT '(' exprSequence ')'",
	"primary : BITMERGE '(' exprSequence ')'",
	"primary : INTERPOLATE '(' exprSequence ')'",
	"primary : DECIMATE '(' exprSequence ')'",
	"primary : TIMEMUX '(' exprSequence ')'",
	"primary : TIMEDEMUX '(' exprSequence ')'",
	"primary : UPSAMPLE '(' exprSequence ')'",
	"primary : DOWNSAMPLE '(' exprSequence ')'",
	"primary : '(' TupleExprSeq ')'",
	"primary : '{' exprSequence '}'",
	"primary : typeNum '(' expr ')'",
	"manexpr : manprimary",
	"manexpr : manexpr '+' manexpr",
	"manexpr : manexpr '-' manexpr",
	"manexpr : manexpr '*' manexpr",
	"manexpr : manexpr DIV manexpr",
	"manexpr : '-' manexpr",
	"manexpr : '(' manexpr ')'",
	"manprimary : name",
	"manprimary : INTEGER",
	"if_body : expr RIGHTARROW expr",
	"if_body : expr RIGHTARROW expr DBLBAR expr",
	"if_body : expr RIGHTARROW expr DBLBAR if_body",
	"name : ident",
	"name : arrayName",
	"arrayName : ident '[' expr ']'",
	"arrayName : ident BOX",
	"arrayName : arrayName '[' expr ']'",
	"arrayName : arrayName BOX",
	"TupleExprSeq : expr ',' expr",
	"TupleExprSeq : TupleExprSeq ',' expr",
	"exprSequence : expr",
	"exprSequence : exprSequence ',' expr",
	"ident : ID",
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
#define YYERROR		goto yyerrlab
#define YYACCEPT	{ free(yys); free(yyv); return(0); }
#define YYABORT		{ free(yys); free(yyv); return(1); }
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** static variables used by the parser
*/
static YYSTYPE *yyv;			/* value stack */
static int *yys;			/* state stack */

static YYSTYPE *yypv;			/* top of value stack */
static int *yyps;			/* top of state stack */

static int yystate;			/* current state */
static int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */

int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */


/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */
	unsigned yymaxdepth = YYMAXDEPTH;

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yyv = (YYSTYPE*)malloc(yymaxdepth*sizeof(YYSTYPE));
	yys = (int*)malloc(yymaxdepth*sizeof(int));
	if (!yyv || !yys)
	{
		yyerror( "out of memory" );
		return(1);
	}
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
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
		if ( yydebug )
		{
			register int yy_i;

			(void)printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			yymaxdepth += YYMAXDEPTH;
			yyv = (YYSTYPE*)realloc((char*)yyv,
				yymaxdepth * sizeof(YYSTYPE));
			yys = (int*)realloc((char*)yys,
				yymaxdepth * sizeof(int));
			if (!yyv || !yys)
			{
				yyerror( "yacc stack overflow" );
				return(1);
			}
			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			(void)printf( "Received token " );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				(void)printf( "Received token " );
				if ( yychar == 0 )
					(void)printf( "end-of-file\n" );
				else if ( yychar < 0 )
					(void)printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					(void)printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						(void)printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
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
				if ( yydebug )
				{
					register int yy_i;

					(void)printf( "Error recovery discards " );
					if ( yychar == 0 )
						(void)printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						(void)printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						(void)printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			(void)printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 47 "Silage.y"
{ TheTree = yypvt[-0].node; } break;
case 2:
# line 51 "Silage.y"
{ yyval.node = MakeTree(CommaNode,yypvt[-0].node,NULL); } break;
case 3:
# line 53 "Silage.y"
{ yyval.node = AppendValue(yypvt[-1].node,yypvt[-0].node); } break;
case 4:
# line 57 "Silage.y"
{ yyval.node = yypvt[-1].node; } break;
case 5:
# line 58 "Silage.y"
{ yyval.node = yypvt[-1].node; } break;
case 6:
# line 59 "Silage.y"
{ yyval.node = NULL; } break;
case 12:
# line 71 "Silage.y"
{ yyval.node = MakeTree(DefnNode,yypvt[-2].node,yypvt[-0].node); } break;
case 13:
# line 75 "Silage.y"
{ yyval.node = MakeTree(PragmaNode, yypvt[-0].node, NULL); } break;
case 14:
# line 79 "Silage.y"
{ yyval.node = MakeTree(IterateNode,yypvt[-1].node,yypvt[-0].node); } break;
case 15:
# line 81 "Silage.y"
{ yyval.node = MakeTree(IterateNode,yypvt[-3].node,yypvt[-1].node); } break;
case 16:
# line 86 "Silage.y"
{ yyval.node = MakeTree(IteratorNode,yypvt[-6].node,MakeTree(CommaNode,yypvt[-4].node,yypvt[-2].node)); } break;
case 17:
# line 91 "Silage.y"
{ yyval.node = MakeTree(DoNode, yypvt[-2].node, yypvt[-1].node); } break;
case 18:
# line 95 "Silage.y"
{ yyval.node = MakeTree(ExitNode, yypvt[-5].node, yypvt[-2].node); } break;
case 19:
# line 99 "Silage.y"
{ yyval.node = MakeTree(WhileNode,yypvt[-2].node,yypvt[-0].node); } break;
case 20:
# line 101 "Silage.y"
{ yyval.node = MakeTree(WhileNode,yypvt[-4].node,yypvt[-1].node); } break;
case 21:
# line 105 "Silage.y"
{ yyval.node = MakeTree(CommaNode,yypvt[-0].node,NULL); } break;
case 22:
# line 107 "Silage.y"
{ yyval.node = AppendValue(yypvt[-1].node,yypvt[-0].node); } break;
case 23:
# line 111 "Silage.y"
{ yyval.node = yypvt[-1].node; } break;
case 24:
# line 112 "Silage.y"
{ yyval.node = NULL; } break;
case 25:
# line 118 "Silage.y"
{ yyval.node = MakeTree(FuncDefnNode,yypvt[-10].node,
				MakeTree(CommaNode,MakeTree(CommaNode,yypvt[-8].node,yypvt[-5].node),
					 MakeTree(CommaNode, yypvt[-3].node, yypvt[-1].node))); } break;
case 26:
# line 123 "Silage.y"
{ yyval.node = MakeTree(FuncDefnNode,yypvt[-9].node,
				MakeTree(CommaNode,MakeTree(CommaNode,yypvt[-7].node,yypvt[-5].node),
					 MakeTree(CommaNode, yypvt[-3].node, yypvt[-1].node))); } break;
case 27:
# line 129 "Silage.y"
{ yyval.node = NULL; } break;
case 30:
# line 136 "Silage.y"
{ yyval.node = AppendTree(yypvt[-2].node,yypvt[-1].node); } break;
case 32:
# line 141 "Silage.y"
{ yyval.node = AppendTree(yypvt[-2].node,yypvt[-0].node); } break;
case 33:
# line 145 "Silage.y"
{ yyval.node = MakeTree(CommaNode,
			MakeTree(FormalNode,yypvt[-2].node,yypvt[-0].node),
			NULL); } break;
case 34:
# line 148 "Silage.y"
{ yyval.node = MakeTree(CommaNode,
			MakeTree(FormalNode,yypvt[-2].node, Right(Left(yypvt[-0].node))),
			yypvt[-0].node); } break;
case 35:
# line 154 "Silage.y"
{ yyval.node=yypvt[-0].node ;} break;
case 36:
# line 155 "Silage.y"
{ yyval.node=yypvt[-0].node ;} break;
case 37:
# line 156 "Silage.y"
{ yyval.node=yypvt[-0].node ;} break;
case 38:
# line 160 "Silage.y"
{ yyval.node = MakeTree2(IntTypeNode,NULL,NULL,yypvt[-0].token.LineNum,					yypvt[-0].token.CharNum, yypvt[-0].token.FileName); } break;
case 39:
# line 161 "Silage.y"
{ yyval.node = MakeTree(IntTypeNode,yypvt[-1].node,NULL); } break;
case 40:
# line 162 "Silage.y"
{ yyval.node = MakeTree2(BoolTypeNode,NULL,NULL,
					yypvt[-0].token.LineNum, yypvt[-0].token.CharNum, yypvt[-0].token.FileName); } break;
case 41:
# line 164 "Silage.y"
{ yyval.node = MakeTree2(NumTypeNode,NULL,NULL,
					yypvt[-0].token.LineNum, yypvt[-0].token.CharNum, yypvt[-0].token.FileName); } break;
case 42:
# line 166 "Silage.y"
{ yyval.node = yypvt[-0].node ; } break;
case 43:
# line 170 "Silage.y"
{ yyval.node = MakeTree(NumTypeNode,yypvt[-1].node,NULL); } break;
case 44:
# line 172 "Silage.y"
{ yyval.node = MakeTree(NumTypeNode,yypvt[-3].node,yypvt[-1].node); } break;
case 45:
# line 177 "Silage.y"
{ yyval.node=MakeTree(BoxTypeNode,yypvt[-1].node,MakeTree2(BoxNode,NULL,NULL,
			yypvt[-0].token.LineNum, yypvt[-0].token.CharNum, yypvt[-0].token.FileName)); } break;
case 46:
# line 180 "Silage.y"
{ yyval.node=AppendTree(yypvt[-1].node,MakeTree2(BoxNode,NULL,NULL, yypvt[-0].token.LineNum,
			yypvt[-0].token.CharNum, yypvt[-0].token.FileName));} break;
case 47:
# line 186 "Silage.y"
{ yyval.node=MakeTree(ValuedBoxTypeNode,yypvt[-1].node,yypvt[-0].node); } break;
case 48:
# line 188 "Silage.y"
{ yyval.node=AppendTree(yypvt[-1].node,yypvt[-0].node); } break;
case 49:
# line 193 "Silage.y"
{ yyval.node=MakeTree (ValuedBoxNode,yypvt[-1].node,NULL); } break;
case 51:
# line 198 "Silage.y"
{ yyval.node = MakeTree(DelayNode,yypvt[-2].node,yypvt[-0].node); } break;
case 52:
# line 199 "Silage.y"
{ yyval.node = MakeTree(InitNode,yypvt[-2].node,yypvt[-0].node); } break;
case 53:
# line 200 "Silage.y"
{ yyval.node = MakeTree(LpDelayNode,yypvt[-2].node,yypvt[-0].node); } break;
case 54:
# line 201 "Silage.y"
{ yyval.node = MakeTree(LpInitNode,yypvt[-2].node,yypvt[-0].node); } break;
case 55:
# line 202 "Silage.y"
{ yyval.node = MakeTree(OrNode,yypvt[-2].node,yypvt[-0].node); } break;
case 56:
# line 203 "Silage.y"
{ yyval.node = MakeTree(AndNode,yypvt[-2].node,yypvt[-0].node); } break;
case 57:
# line 204 "Silage.y"
{ yyval.node = MakeTree(ExorNode,yypvt[-2].node,yypvt[-0].node); } break;
case 58:
# line 205 "Silage.y"
{ yyval.node = MakeTree(PlusNode,yypvt[-2].node,yypvt[-0].node); } break;
case 59:
# line 206 "Silage.y"
{ yyval.node = MakeTree(MinusNode,yypvt[-2].node,yypvt[-0].node); } break;
case 60:
# line 207 "Silage.y"
{ yyval.node = MakeTree(MultNode,yypvt[-2].node,yypvt[-0].node); } break;
case 61:
# line 208 "Silage.y"
{ yyval.node = MakeTree(DivNode,yypvt[-2].node,yypvt[-0].node); } break;
case 62:
# line 209 "Silage.y"
{ yyval.node = MakeTree(CosNode,yypvt[-1].node,NULL); } break;
case 63:
# line 210 "Silage.y"
{ yyval.node = MakeTree(SinNode,yypvt[-1].node,NULL); } break;
case 64:
# line 211 "Silage.y"
{ yyval.node = MakeTree(SqrtNode,yypvt[-1].node,NULL); } break;
case 65:
# line 212 "Silage.y"
{ yyval.node = MakeTree(MinNode,yypvt[-3].node,yypvt[-1].node); } break;
case 66:
# line 213 "Silage.y"
{ yyval.node = MakeTree(MaxNode,yypvt[-3].node,yypvt[-1].node); } break;
case 67:
# line 214 "Silage.y"
{ yyval.node = MakeTree(IntDivNode,yypvt[-2].node,yypvt[-0].node); } break;
case 68:
# line 215 "Silage.y"
{ yyval.node = MakeTree(CompMinusNode,yypvt[-2].node,yypvt[-0].node); } break;
case 69:
# line 216 "Silage.y"
{ yyval.node = MakeTree(LeftShiftNode,yypvt[-2].node,yypvt[-0].node); } break;
case 70:
# line 217 "Silage.y"
{ yyval.node = MakeTree(RightShiftNode,yypvt[-2].node,yypvt[-0].node); } break;
case 71:
# line 218 "Silage.y"
{ yyval.node = MakeTree(NegateNode,yypvt[-0].node,NULL); } break;
case 72:
# line 219 "Silage.y"
{ yyval.node = MakeTree(CompNode,yypvt[-0].node,NULL); } break;
case 73:
# line 220 "Silage.y"
{ yyval.node = MakeTree(NotNode,yypvt[-0].node,NULL); } break;
case 74:
# line 221 "Silage.y"
{ yyval.node = MakeTree(GreaterNode,yypvt[-2].node,yypvt[-0].node); } break;
case 75:
# line 222 "Silage.y"
{ yyval.node = MakeTree(SmallerNode,yypvt[-2].node,yypvt[-0].node); } break;
case 76:
# line 223 "Silage.y"
{ yyval.node = MakeTree(GreaterEqualNode,yypvt[-2].node,yypvt[-0].node); } break;
case 77:
# line 224 "Silage.y"
{ yyval.node = MakeTree(SmallerEqualNode,yypvt[-2].node,yypvt[-0].node); } break;
case 78:
# line 225 "Silage.y"
{ yyval.node = MakeTree(EqualNode,yypvt[-2].node,yypvt[-0].node); } break;
case 79:
# line 226 "Silage.y"
{ yyval.node = MakeTree(NotEqualNode,yypvt[-2].node,yypvt[-0].node); } break;
case 80:
# line 227 "Silage.y"
{ yyval.node = yypvt[-1].node; } break;
case 81:
# line 228 "Silage.y"
{ yyval.node = yypvt[-1].node; } break;
case 84:
# line 235 "Silage.y"
{ yyval.node = MakeTree(FuncAppNode,yypvt[-3].node,yypvt[-1].node); } break;
case 85:
# line 237 "Silage.y"
{ yyval.node = MakeTree(BitNode, yypvt[-1].node, NULL); } break;
case 86:
# line 239 "Silage.y"
{ yyval.node = MakeTree(BitSelectNode, yypvt[-1].node, NULL); } break;
case 87:
# line 241 "Silage.y"
{ yyval.node = MakeTree(BitMergeNode, yypvt[-1].node, NULL); } break;
case 88:
# line 243 "Silage.y"
{ yyval.node = MakeTree(InterpolateNode,yypvt[-1].node,NULL); } break;
case 89:
# line 245 "Silage.y"
{ yyval.node = MakeTree(DecimateNode,yypvt[-1].node,NULL); } break;
case 90:
# line 247 "Silage.y"
{ yyval.node = MakeTree(TimeMuxNode, yypvt[-1].node, NULL); } break;
case 91:
# line 249 "Silage.y"
{ yyval.node = MakeTree(TimeDeMuxNode, yypvt[-1].node, NULL); } break;
case 92:
# line 251 "Silage.y"
{ yyval.node = MakeTree(UpSampleNode, yypvt[-1].node, NULL); } break;
case 93:
# line 253 "Silage.y"
{ yyval.node = MakeTree(DownSampleNode, yypvt[-1].node, NULL); } break;
case 94:
# line 254 "Silage.y"
{ yyval.node = MakeTree(TupleNode,yypvt[-1].node,NULL); } break;
case 95:
# line 255 "Silage.y"
{ yyval.node = MakeTree(AggregateNode,yypvt[-1].node,NULL); } break;
case 96:
# line 256 "Silage.y"
{ yyval.node = MakeTree(CastNode,yypvt[-3].node,yypvt[-1].node); } break;
case 98:
# line 261 "Silage.y"
{ yyval.node = MakeTree(PlusNode,yypvt[-2].node,yypvt[-0].node); } break;
case 99:
# line 262 "Silage.y"
{ yyval.node = MakeTree(MinusNode,yypvt[-2].node,yypvt[-0].node); } break;
case 100:
# line 263 "Silage.y"
{ yyval.node = MakeTree(MultNode,yypvt[-2].node,yypvt[-0].node); } break;
case 101:
# line 264 "Silage.y"
{ yyval.node = MakeTree(IntDivNode,yypvt[-2].node,yypvt[-0].node); } break;
case 102:
# line 265 "Silage.y"
{ yyval.node = MakeTree(NegateNode,yypvt[-0].node,NULL); } break;
case 103:
# line 266 "Silage.y"
{ yyval.node = yypvt[-1].node; } break;
case 105:
# line 271 "Silage.y"
{ yyval.node = MakeTree2(IntegerLeaf,yypvt[-0].token.V,NULL,
					 yypvt[-0].token.LineNum,yypvt[-0].token.CharNum,yypvt[-0].token.FileName); } break;
case 106:
# line 277 "Silage.y"
{ yyval.node = MakeTree(CondNode,MakeTree(CommaNode,yypvt[-2].node,yypvt[-0].node),NULL); } break;
case 107:
# line 279 "Silage.y"
{ yyval.node = MakeTree(CondNode,MakeTree(CommaNode,yypvt[-4].node,yypvt[-2].node),yypvt[-0].node); } break;
case 108:
# line 281 "Silage.y"
{ yyval.node = MakeTree(CondNode,MakeTree(CommaNode,yypvt[-4].node,yypvt[-2].node),yypvt[-0].node); } break;
case 109:
# line 285 "Silage.y"
{yyval.node=yypvt[-0].node;} break;
case 110:
# line 286 "Silage.y"
{ yyval.node=yypvt[-0].node;} break;
case 111:
# line 291 "Silage.y"
{ yyval.node = MakeTree(ArrayNameNode, yypvt[-3].node,
					 MakeTree(ArrayIndexNode,yypvt[-1].node,NULL)) ;} break;
case 112:
# line 294 "Silage.y"
{ yyval.node = MakeTree(ArrayNameNode, yypvt[-1].node, MakeTree2(ArrayIndexNode,NULL,
			NULL,yypvt[-0].token.LineNum,yypvt[-0].token.CharNum,yypvt[-0].token.FileName)) ;} break;
case 113:
# line 297 "Silage.y"
{ yyval.node = AppendTree(yypvt[-3].node,MakeTree(ArrayIndexNode,yypvt[-1].node,NULL)); } break;
case 114:
# line 299 "Silage.y"
{ yyval.node = AppendTree(yypvt[-1].node,MakeTree2(ArrayIndexNode,NULL,NULL,
			yypvt[-0].token.LineNum, yypvt[-0].token.CharNum, yypvt[-0].token.FileName)); } break;
case 115:
# line 304 "Silage.y"
{ yyval.node = MakeTree(CommaNode,yypvt[-2].node,
					 MakeTree(CommaNode,yypvt[-0].node,NULL)); } break;
case 116:
# line 307 "Silage.y"
{ yyval.node = AppendValue(yypvt[-2].node,yypvt[-0].node); } break;
case 117:
# line 311 "Silage.y"
{ yyval.node = MakeTree(CommaNode,yypvt[-0].node,NULL); } break;
case 118:
# line 313 "Silage.y"
{ yyval.node = AppendValue(yypvt[-2].node,yypvt[-0].node); } break;
case 119:
# line 317 "Silage.y"
{ yyval.node = MakeTree2(IdentLeaf,yypvt[-0].token.V,NULL,yypvt[-0].token.LineNum,yypvt[-0].token.CharNum,
				 yypvt[-0].token.FileName); } break;
	}
	goto yystack;		/* reset registers in driver code */
}
