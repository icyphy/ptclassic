/* $Id$ */

# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
# undef input
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(fparser))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
 
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
#define LexReturn(n) { yylval.token.LineNum = CurrentLine; \
			       yylval.token.CharNum = CurrentChar; \
			       yylval.token.FileName = CurrentFileName; \
			       return(n); }
extern char *Internal();
extern NodeType *ConvCSD(),*ConvFrac();
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
	{ CurrentChar++; }
break;
case 2:
	{ CurrentChar = (CurrentChar+8) & 0xfffffff8; }
break;
case 3:
	{ CurrentChar = 0; CurrentLine++; }
break;
case 4:
{ if (yytext[yyleng-2] == '\\')
			yymore();
		  else {
		      yylval.token.V = StringSave(yytext);
		      CurrentChar += yyleng;
		      LexReturn(ID);
		  }
		}
break;
case 5:
{ int Key;
			  if ((Key = LookupKey(yytext)) != -1) {
			        CurrentChar += strlen(yytext);
			  	LexReturn (Key);
                          }
			  yylval.token.V = (int) Internal(yytext);
			  CurrentChar += strlen(yytext);
			  LexReturn(ID);
			}
break;
case 6:
	{ int n; sscanf(yytext,"%d",&n); yylval.token.V = n;
		  CurrentChar += strlen(yytext); 
		  LexReturn(INTEGER);
		}
break;
case 7:
	{ CurrentChar += strlen(yytext);
		  yytext[strlen(yytext)-1] = '.';
	          yylval.node = ConvCSD(yytext,CurrentLine,
					CurrentChar,CurrentFileName);
		  return FIXEDNUM;
		}
break;
case 8:
case 9:
{ CurrentChar += strlen(yytext);
		  yytext[strlen(yytext)-1] = NULL;
	          yylval.node = ConvCSD(yytext,CurrentLine,
					CurrentChar,CurrentFileName);
		  return FIXEDNUM;
			}
break;
case 10:
{ CurrentChar += strlen(yytext);
		  yylval.node = ConvFrac(yytext,CurrentLine,CurrentChar,
		  			 CurrentFileName);
		  return FIXEDNUM;
		}
break;
case 11:
{ CurrentChar += strlen(yytext);
		  yylval.node = ConvFrac(yytext,CurrentLine,CurrentChar,
		  			 CurrentFileName);
		  return FIXEDNUM;
		}
break;
case 12:
	{ CurrentChar += 2; LexReturn (DBLCOLON); }
break;
case 13:
	{ CurrentChar += 2; LexReturn (RIGHTARROW); }
break;
case 14:
	{ CurrentChar += 2; LexReturn (LEFTARROW); }
break;
case 15:
	{ CurrentChar += 2; LexReturn (DBLBAR); }
break;
case 16:
	{ CurrentChar += 2; LexReturn (DOTDOT); }
break;
case 17:
	{ CurrentChar += 2; LexReturn (BOX); }
break;
case 18:
	{ CurrentChar += 2; LexReturn (LESSEQUAL); }
break;
case 19:
	{ CurrentChar += 2; LexReturn (GREATEREQUAL); }
break;
case 20:
	{ CurrentChar += 2; LexReturn (EQUAL); }
break;
case 21:
	{ CurrentChar += 2; LexReturn (NOTEQUAL); }
break;
case 22:
           { CurrentChar += 2; LexReturn (LEFTSHIFT); }
break;
case 23:
           { CurrentChar += 2; LexReturn (RIGHTSHIFT); }
break;
case 24:
           { CurrentChar += 2; LexReturn (ATAT); }
break;
case 25:
           { CurrentChar += 2; LexReturn (PNDPND); }
break;
case 26:
{ CurrentChar++; LexReturn(yytext[0]); }
break;
case 27:
	{ ERROR(CurrentLine,CurrentChar,CurrentFileName,
			"Illegal character."); 
		  CurrentChar++; }
break;
case 28:
{ char FileName[100];
		  sscanf(yytext+1,"%d %s",&CurrentLine,FileName);
		  CurrentChar = 0; CurrentFileName = Internal(FileName);
		  }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

#define StringTableSize 1024
#define EndOfTextChar '\0' 

static struct KEYSTRUCT { char *key; int val; } KeywordTable[] =
   {  { "begin", BEGIN_ },
      { "BEGIN", BEGIN_ },
      { "div", DIV },
      { "DIV", DIV },
      { "end", END },
      { "END", END },
      { "fi", FI },
      { "FI", FI },
      { "func", FUNC },
      { "FUNC", FUNC },
      { "if", IF },
      { "IF", IF },
      { "fix", FIX },
      { "FIX", FIX },
      { "num", FIX },
      { "NUM", FIX },
      { "int", INT },
      { "INT", INT },
      { "bool", BOOL },
      { "BOOL", BOOL },
      { "pragma", PRAGMA },
      { "PRAGMA", PRAGMA },
      { "WHILE", WHILE },
      { "while", WHILE },
      { "DO", DO },
      { "do", DO },
      { "OD", OD },
      { "od", OD },
      { "EXIT", EXIT },
      { "exit", EXIT },
      { "TIXE", TIXE },
      { "tixe", TIXE },
      { "cos", COS },
      { "COS", COS },
      { "sin", SIN },
      { "SIN", SIN },
      { "sqrt", SQRT },
      { "SQRT", SQRT },
      { "min", MIN },
      { "MIN", MIN },
      { "max", MAX },
      { "MAX", MAX },
      { "bit", BIT },
      { "BIT", BIT },
      { "bitselect", BITSELECT },
      { "BITSELECT", BITSELECT },
      { "bitmerge", BITMERGE },
      { "BITMERGE", BITMERGE },
      { "interpolate", INTERPOLATE },
      { "INTERPOLATE", INTERPOLATE },
      { "decimate", DECIMATE },
      { "DECIMATE", DECIMATE },
      { "timemux", TIMEMUX },
      { "TIMEMUX", TIMEMUX },
      { "timedemux", TIMEDEMUX },
      { "TIMEDEMUX", TIMEDEMUX },
      { "upsample", UPSAMPLE },
      { "UPSAMPLE", UPSAMPLE },
      { "downsample", DOWNSAMPLE },
      { "DOWNSAMPLE", DOWNSAMPLE },
      { NULL, 0 }
   };

int LookupKey(str)
    char *str;
{
	struct KEYSTRUCT *p;

	for (p = KeywordTable; p -> key != 0 && strcmp(p->key,str) != 0; p++);
	if (p -> key == 0) return -1;
	return p->val;
}

char *StringTable[StringTableSize+1];
int StringCount = 0;

InitParser()
{
    int i;

    for (i = 0; i <= StringTableSize; i++)
	StringTable[i] = NULL;
}

int
Hash(s)
    char *s;
    /* Return hashed value for s in range 0 .. StringTableSize-1.  
       StringTableSize must be power of 2.
       */
{
	int Sum = 0;
	for (; *s != NULL;) Sum += *s++;
	return (Sum & (StringTableSize-1));
}

char *Internal(str)
    char *str;
{
    char **p;

    for (p = StringTable+Hash(str); *p != NULL && strcmp(*p,str) != 0; p++);
    if (p == StringTable+StringTableSize)
        for (p = StringTable; *p != NULL && strcmp(*p,str) != 0; p++);
    if (*p == NULL) {
	if (++StringCount == StringTableSize)
	   { Punt("String table overflow."); }
        *p = (char *) tmalloc((unsigned) (strlen(str)+1));
	strcpy(*p,str);
    }
    return *p;
}

char *
External(str)
char *str;
{
    char **p;

    for (p = StringTable+Hash(str); *p != NULL && strcmp(*p,str) != 0; p++);
    if (p == StringTable+StringTableSize)
        for (p = StringTable; *p != NULL && strcmp(*p,str) != 0; p++);
    return *p;
}

int ExternInt(str)
char *str;
{
    char **p;

    for (p = StringTable+Hash(str); *p != NULL && strcmp(*p,str) != 0; p++);
    if (p == StringTable+StringTableSize)
        for (p = StringTable; *p != NULL && strcmp(*p,str) != 0; p++);
    return (int) (p - StringTable);
}

int
StringSave(str)
char *str;
{
    char *String, *Ptr;

    String = Ptr = (char *) tmalloc(strlen(str));
    for (++str; *str != '\0'; str++, Ptr++)
	if (*str != '\\' || *(str+1) != '"')
	    *Ptr = *str;
    *(Ptr-1) = '\0';
    return ((int) String);
}

int ErrorCount = 0;

ERROR(linenum,charnum,filename,mssg)
    int linenum,charnum;
    char *filename,*mssg;
{
	fprintf(stdout,"Error in %s, line %d, character %d: %s\n",
		filename,linenum,charnum,mssg);
	FatalError = 1;
	ErrorCount++;
}

yyerror(s)
    char *s;
{
	ERROR(CurrentLine,CurrentChar,CurrentFileName,s);
	FatalError = 1;
}

EndParseMessage()
{
char		endOfText;

if (ErrorCount)
	{
	fprintf(stdout,"Compilation Produced Errors.\n");
	}
else	{
	fprintf(stdout,"Successful Compilation.\n");
	};
endOfText=EndOfTextChar;
fprintf(stdout,"%c",endOfText);
};

int yywrap()
{
	return(1);
}

/* The following routines convert strings to internal representations of
 * fixed point numbers.  A fixed point number is represented as a tree
 * of the form MakeTree(FixedLeaf,M,S), where M and S are integers such
 * that the number represented is M * 2**S.
 */

NodeType *
ConvCSD(s,LineNum,Char,File)
    char *s;
    int LineNum,Char;
    char *File;
    /* Convert string s in canonical signed digit form to internal
     * representation.
     */
{
    int Scale = 0;
    int Sum = 0;
    NodeType *CSDNode;

    for (;*s != NULL; s++) {
        if (*s == '.') { Scale = 1; continue; }
	Sum += Sum; Scale += Scale;
	if (*s == '1') Sum++;
	else if (*s == '-') Sum--;
    }
    CSDNode = MakeTree2(FixedLeaf, Sum, Scale, LineNum, Char, File);
    CSDNode->Attrs = NULL;
    return (CSDNode);
}

NodeType *
ConvFrac(s,LineNum,Char,File)
    char *s;
    int LineNum,Char;
    char *File;
    /* Convert s, a decimal rational number, to internal form. Give
     * as many bits of precision as possible
     */
{
    int Acc = 0,B = 1,Scale = 0,Quo;
double d;
    NodeType *FracNode;
    char *s2;

    s2 = s;
    for (; *s != NULL; s++) 
		{
        	if (*s == '.') { B = 1; continue; }
		Acc = 10*Acc+(*s - '0'); B *= 10;
    		}
    /* Now Acc / B is the desired result. Let A0 = A here. */
    if (Acc<0) Acc = - Acc;
    FracNode = MakeTree2(FixedLeaf,Acc,B,LineNum,Char,File);
    FracNode->Attrs = (AttrType *) tmalloc (strlen (s2) + 1);
    strcpy (FracNode->Attrs, s2);
    return (FracNode);
}
int yyvstop[] = {
0,

27,
0,

2,
27,
0,

3,
0,

3,
27,
0,

1,
27,
0,

26,
27,
0,

27,
0,

26,
27,
0,

26,
27,
0,

26,
27,
0,

26,
27,
0,

6,
27,
0,

6,
27,
0,

26,
27,
0,

26,
27,
0,

26,
27,
0,

26,
27,
0,

26,
27,
0,

5,
27,
0,

26,
27,
0,

26,
27,
0,

26,
27,
0,

21,
0,

4,
0,

25,
0,

13,
0,

16,
0,

7,
0,

6,
0,

6,
0,

12,
0,

14,
0,

22,
0,

18,
0,

20,
0,

19,
0,

23,
0,

24,
0,

5,
0,

17,
0,

15,
0,

28,
0,

28,
0,

25,
0,

8,
0,

9,
0,

11,
0,

11,
0,

6,
0,

11,
0,

8,
9,
0,

10,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	1,6,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,7,	1,8,	1,9,	
1,10,	1,3,	10,28,	1,11,	
0,0,	1,11,	1,11,	1,11,	
1,11,	1,11,	1,12,	1,13,	
1,11,	1,14,	2,4,	1,15,	
13,30,	13,31,	58,58,	13,30,	
13,30,	58,58,	58,58,	1,16,	
1,11,	1,17,	1,18,	1,19,	
8,25,	1,20,	1,21,	12,29,	
16,38,	18,42,	1,21,	19,43,	
19,44,	2,7,	17,39,	20,45,	
2,24,	63,66,	0,0,	2,11,	
59,64,	2,11,	2,11,	2,11,	
0,0,	2,11,	0,0,	2,13,	
2,11,	17,40,	17,41,	0,0,	
1,22,	0,0,	1,11,	1,11,	
1,11,	22,47,	60,64,	2,16,	
2,11,	2,17,	2,18,	2,19,	
9,26,	2,20,	33,54,	33,55,	
30,53,	33,56,	33,56,	0,0,	
59,64,	9,26,	30,30,	0,0,	
9,26,	30,30,	30,30,	35,54,	
35,61,	0,0,	35,62,	35,62,	
1,11,	1,23,	1,11,	1,11,	
23,48,	0,0,	60,64,	0,0,	
2,22,	0,0,	2,11,	2,11,	
9,26,	9,27,	54,58,	9,26,	
54,54,	54,54,	0,0,	0,0,	
0,0,	0,0,	9,26,	0,0,	
9,26,	0,0,	0,0,	9,26,	
15,37,	9,26,	15,36,	15,36,	
15,36,	15,36,	15,36,	15,36,	
15,36,	15,36,	15,36,	15,36,	
2,11,	2,23,	2,11,	2,11,	
9,26,	14,32,	56,33,	56,58,	
9,26,	56,33,	56,33,	14,33,	
14,34,	0,0,	14,35,	14,35,	
14,36,	14,36,	14,36,	14,36,	
14,36,	14,36,	14,36,	14,36,	
21,46,	61,63,	0,0,	0,0,	
61,65,	61,65,	0,0,	0,0,	
0,0,	0,0,	9,26,	0,0,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	0,0,	
0,0,	0,0,	0,0,	21,46,	
0,0,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	21,46,	
21,46,	21,46,	21,46,	24,49,	
0,0,	0,0,	0,0,	34,57,	
0,0,	0,0,	0,0,	0,0,	
24,50,	34,58,	0,0,	24,51,	
34,59,	34,59,	34,60,	34,60,	
34,60,	34,60,	34,60,	34,60,	
34,60,	34,60,	0,0,	0,0,	
0,0,	0,0,	0,0,	49,49,	
0,0,	0,0,	0,0,	24,49,	
24,49,	24,52,	24,49,	0,0,	
49,50,	0,0,	0,0,	49,51,	
0,0,	24,49,	0,0,	24,49,	
0,0,	0,0,	24,49,	0,0,	
24,49,	37,60,	37,60,	37,60,	
37,60,	37,60,	37,60,	37,60,	
37,60,	37,60,	37,60,	49,49,	
49,49,	0,0,	49,49,	24,49,	
0,0,	0,0,	0,0,	24,49,	
0,0,	49,49,	0,0,	49,49,	
55,57,	0,0,	49,49,	0,0,	
49,49,	65,66,	55,63,	0,0,	
0,0,	55,63,	55,63,	65,63,	
0,0,	0,0,	65,65,	65,65,	
0,0,	0,0,	0,0,	49,49,	
64,64,	24,49,	64,64,	49,49,	
0,0,	64,67,	64,67,	64,67,	
64,67,	64,67,	64,67,	64,67,	
64,67,	64,67,	64,67,	65,64,	
67,67,	67,67,	67,67,	67,67,	
67,67,	67,67,	67,67,	67,67,	
67,67,	67,67,	0,0,	0,0,	
0,0,	49,49,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	65,64,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-41,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+0,	0,		yyvstop+8,
yycrank+0,	0,		yyvstop+11,
yycrank+3,	0,		yyvstop+14,
yycrank+-103,	0,		yyvstop+17,
yycrank+3,	0,		yyvstop+19,
yycrank+0,	0,		yyvstop+22,
yycrank+5,	0,		yyvstop+25,
yycrank+7,	0,		yyvstop+28,
yycrank+130,	0,		yyvstop+31,
yycrank+106,	0,		yyvstop+34,
yycrank+10,	0,		yyvstop+37,
yycrank+29,	0,		yyvstop+40,
yycrank+8,	0,		yyvstop+43,
yycrank+10,	0,		yyvstop+46,
yycrank+11,	0,		yyvstop+49,
yycrank+152,	0,		yyvstop+52,
yycrank+4,	0,		yyvstop+55,
yycrank+4,	0,		yyvstop+58,
yycrank+-274,	0,		yyvstop+61,
yycrank+0,	0,		yyvstop+64,
yycrank+0,	yysvec+9,	0,	
yycrank+0,	0,		yyvstop+66,
yycrank+0,	0,		yyvstop+68,
yycrank+0,	0,		yyvstop+70,
yycrank+69,	0,		0,	
yycrank+0,	0,		yyvstop+72,
yycrank+0,	0,		yyvstop+74,
yycrank+61,	0,		0,	
yycrank+240,	0,		0,	
yycrank+74,	yysvec+14,	yyvstop+76,
yycrank+0,	yysvec+15,	yyvstop+78,
yycrank+277,	0,		0,	
yycrank+0,	0,		yyvstop+80,
yycrank+0,	0,		yyvstop+82,
yycrank+0,	0,		yyvstop+84,
yycrank+0,	0,		yyvstop+86,
yycrank+0,	0,		yyvstop+88,
yycrank+0,	0,		yyvstop+90,
yycrank+0,	0,		yyvstop+92,
yycrank+0,	0,		yyvstop+94,
yycrank+0,	yysvec+21,	yyvstop+96,
yycrank+0,	0,		yyvstop+98,
yycrank+0,	0,		yyvstop+100,
yycrank+-302,	0,		0,	
yycrank+0,	0,		yyvstop+102,
yycrank+0,	yysvec+49,	yyvstop+104,
yycrank+0,	yysvec+49,	yyvstop+106,
yycrank+0,	0,		yyvstop+108,
yycrank+92,	yysvec+33,	0,	
yycrank+309,	0,		0,	
yycrank+125,	0,		0,	
yycrank+0,	0,		yyvstop+110,
yycrank+9,	yysvec+55,	0,	
yycrank+11,	yysvec+34,	yyvstop+112,
yycrank+29,	yysvec+37,	yyvstop+114,
yycrank+144,	yysvec+34,	0,	
yycrank+0,	yysvec+14,	yyvstop+116,
yycrank+38,	yysvec+55,	0,	
yycrank+325,	0,		0,	
yycrank+314,	yysvec+34,	yyvstop+118,
yycrank+0,	0,		yyvstop+120,
yycrank+336,	0,		yyvstop+123,
0,	0,	0};
struct yywork *yytop = yycrank+415;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,012 ,01  ,01  ,015 ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'!' ,'"' ,'!' ,'$' ,01  ,'!' ,01  ,
'!' ,'!' ,'!' ,'+' ,'!' ,'-' ,'!' ,'!' ,
'0' ,'0' ,'2' ,'2' ,'2' ,'2' ,'2' ,'2' ,
'2' ,'2' ,'!' ,'!' ,'!' ,'!' ,'!' ,01  ,
'!' ,'A' ,'A' ,'A' ,'A' ,'E' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'!' ,01  ,'!' ,'!' ,'_' ,
01  ,'A' ,'A' ,'A' ,'A' ,'E' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'!' ,'!' ,'!' ,'!' ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.6 88/02/08 SMI"; /* from S5R2 1.2 */
#endif

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
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
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
