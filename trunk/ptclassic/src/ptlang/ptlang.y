%{
/************************************************************************
 Version: $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>

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
#define NINC 10
#define NSEE 30
#define NSTR 20

/* chars allowed in "identifier" */
#define IDENTCHAR(c) (isalnum(c) || c == '.' || c == '_')

char yytext[BIGBUFSIZE];	/* lexical analysis buffer */
int yyline = 1;			/* current input line */
int bodyMode = 0;		/* special lexan mode flag to read bodies  */
int docMode = 0;		/* flag document bodies  */
int descMode = 0;		/* flag descriptor bodies  */
int codeMode = 0;		/* flag code block bodies  */
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
char* malloc();
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
     yywarn(), mismatch();

char* inputFile;		/* input file name */
char* idBlock;			/* ID block */
char* objName;			/* name of star or galaxy class being decld  */
char* objVer;			/* sccs version number */
char* objDate;			/* date of last update */
char* objDesc;			/* descriptor of star or galaxy */
char* objAuthor;		/* author of star or galaxy */
char* objAcknowledge;		/* acknowledgements (previous authors) */
char* objCopyright;		/* copyright */
char* objExpl;			/* long explanation */
char* objLocation;		/* location string */
int   galDef;			/* true if obj is a galaxy */
char* domain;			/* domain of object (if star) */
char* derivedFrom;		/* class obj is derived from */
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

/* codes for "standard methods".  To add more, add them at the end,
 * modify N_FORMS, and put types in the codeType array and names in
 * the codeFuncName array.  The C_CONS is processed special.
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


#define N_FORMS 8
char* codeBody[N_FORMS];		/* code bodies for each entity */
int inlineFlag[N_FORMS];		/* marks which are to be inline */
char destNameBuf[MEDBUFSIZE];		/* storage for destructor name */

/* types and names of standard member funcs */
char* codeType[] = {"","int ","void ","void ","void ","","void ","void "};
char* codeFuncName[] = {
"","myExecTime","wrapup","initCode","begin",destNameBuf,"setup","go"};

int methKey;			/* signals which of the standard funcs */

#define consCode codeBody[C_CONS]	/* extra constructor code */

char* hInclude[NINC];		/* include files in .h file */
int   nHInclude;		/* number of such files */
char* ccInclude[NINC];		/* include files in .cc file */
int   nCcInclude;		/* number of such files */
char* seeAlsoList[NSEE];	/* list of pointers to other manual sections */
int   nSeeAlso;			/* number of such pointers */

/* all tokens with values are type char *.  Keyword tokens
 * have their names as values.
 */
typedef char * STRINGVAL;
#define YYSTYPE STRINGVAL

%}

%token DEFSTAR GALAXY NAME DESC DEFSTATE DOMAIN NUMPORTS NUM VIRTUAL
%token DERIVED CONSTRUCTOR DESTRUCTOR STAR ALIAS INPUT OUTPUT INOUT ACCESS
%token INMULTI OUTMULTI INOUTMULTI
%token TYPE DEFAULT CLASS BEGIN SETUP GO WRAPUP CONNECT ID
%token CCINCLUDE HINCLUDE PROTECTED PUBLIC PRIVATE METHOD ARGLIST CODE
%token BODY IDENTIFIER STRING CONSCALLS ATTRIB LINE
%token VERSION AUTHOR ACKNOWLEDGE COPYRIGHT EXPLANATION SEEALSO LOCATION
%token CODEBLOCK EXECTIME PURE INLINE STATIC HEADER INITCODE START
%%
/* production to report better about garbage at end */
full_file:
	file
|	file '}'	{ yyerror("Too many closing curly braces");
			  exit(1);
			}
|	file ident			{ mismatch($2);}
;

/* a file consists of a series of definitions. */
file:
	/* nothing */
|	file stardef
|	file galdef
|	id BODY				{ idBlock = $2; bodyMode = 0;}
;

id:	ID '{'				{ bodyMode = 1;}
;

stardef:
	DEFSTAR { clearDefs(0);}
		'{' starlist '}'	{ genDef();}
;

galdef:	GALAXY { clearDefs(1);}
		'{' gallist '}'		{ genDef();}
;

starlist:staritem		
|	starlist staritem
;

gallist:galitem			
|	gallist galitem
;

/* items allowed in both stars and galaxies */
sgitem:
	NAME '{' ident '}'		{ objName = $3;}
|	VERSION '{' version '}'		{ }
|	DESC '{' 			{ descMode = 1; docMode = 1;}
		BODY			{ objDesc = $4;
					  docMode = 0;
					  descMode = 0;}
|	AUTHOR '{' 			{ bodyMode = 1; docMode = 1;}
		BODY			{ objAuthor = $4;
					  docMode = 0;
					  bodyMode = 0;}
|	ACKNOWLEDGE '{' 		{ bodyMode = 1; docMode = 1;}
		BODY			{ objAcknowledge = $4;
					  docMode = 0;
					  bodyMode = 0;}
|	COPYRIGHT '{'			{ bodyMode = 1; docMode = 1;}
		BODY			{ objCopyright = $4;
					  docMode = 0;
					  bodyMode = 0;}
|	LOCATION '{'			{ bodyMode = 1; docMode = 1;}
		BODY			{ objLocation = $4;
					  docMode = 0;
					  bodyMode = 0;}
|	EXPLANATION '{'			{ bodyMode = 1; docMode = 1;}
		BODY			{ objExpl = $4;
					  bodyMode = 0;
					  docMode = 0;}
|	SEEALSO '{' seealso '}'		{ }
|	DEFSTATE 			{ clearStateDefs();}
		'{' dstatelist '}'	{ genState(); describeState();}

/* definitions of the constructor, destructor, etc. */
|	inl 	stdmethkey BODY		{ inlineFlag[methKey] = $1 ? 1 : 0;
					  codeBody[methKey] = $3;
					  bodyMode = 0;
					}

|	members BODY			{ addMembers ($1, $2); bodyMode = 0;}
|	code BODY			{ strcat (ccCode, $2); 
					  strcat (ccCode, "\n\n");
					  bodyMode = 0;
					}
|	header BODY			{ strcat (hCode, $2);
					  strcat (hCode, "\n");
					  bodyMode = 0;
					}
|	method '{' methlist '}'		{ wrapMethod();}
|	CCINCLUDE '{' cclist '}'	{ }
|	HINCLUDE '{' hlist '}'
|	CCINCLUDE '{' error '}'		{ yyerror("Error in ccinclude list");}
|	HINCLUDE '{' error '}'		{ yyerror("Error in hinclude list");}
|	conscalls BODY			{ if(consCalls[0]) {
					     strcat(consCalls,", ");
					     strcat(consCalls,$2);
					  } else {
					     strcpy(consCalls,$2);
					  }
					  bodyMode = 0; 
					}
|	error '}'			{ yyerror ("Illegal item");}
;

/* method introducer */
/* if VIRTUAL is combined with another keyword, it must be second */
method:	METHOD				{ clearMethodDefs(0);}
|	PURE vopt METHOD		{ clearMethodDefs(M_PURE);}
|	VIRTUAL METHOD			{ clearMethodDefs(M_VIRTUAL);}
|	STATIC METHOD			{ clearMethodDefs(M_STATIC);}
|	INLINE STATIC METHOD		{ clearMethodDefs(M_STATIC|M_INLINE);}
|	INLINE vopt METHOD		{ int mode = M_INLINE;
					  if ($2) mode |= M_VIRTUAL;
					  clearMethodDefs(mode);
					}
;

/* optional inline keyword */
inl:	/* nothing */			{ $$ = 0;}
|	INLINE				{ $$ = $1;}
;

/* optional virtual keyword */
vopt:	/* nothing */			{ $$ = 0;}
|	VIRTUAL				{ $$ = $1;}
;

/* keywords for standard methods */
stdmethkey:
	stdkey2 optp '{'		{ bodyMode = 1;}
;

stdkey2:
	CONSTRUCTOR			{ methKey = C_CONS;}
|	DESTRUCTOR			{ methKey = C_DEST;}
|	BEGIN				{ methKey = C_BEGIN;}
|	SETUP				{ methKey = C_SETUP;}
|	GO				{ methKey = C_GO;}
|	WRAPUP				{ methKey = C_WRAPUP;}
|	INITCODE			{ methKey = C_INITCODE;}
|	EXECTIME			{ methKey = C_EXECTIME;}
|	START
		{ yywarn("start is obsolete, use setup");
		  methKey = C_SETUP;
		}
;

/* version identifier */
version:
        '$' IDENTIFIER ':' IDENTIFIER '$'
   '$' IDENTIFIER ':' IDENTIFIER '/' IDENTIFIER '/' IDENTIFIER
                 IDENTIFIER ':' IDENTIFIER ':' IDENTIFIER '$'
                { char b[SMALLBUFSIZE];
                  objVer = $4;
                  sprintf(b, "\"%s/%s/%s %s:%s:%s\"", $9, $11, $13, $14, $16, $18);
                  objDate = save(b);
                }
|
        '$' IDENTIFIER '$' '$' IDENTIFIER '$'
                {
                  char b[SMALLBUFSIZE];
                  long t;
                  objVer = "?.?";
                  t = time((time_t *)0);
                  b[0] = QUOTE;
                  strncat(b,ctime(&t),24);
                  strcat(b,"\"");
                  objDate = save(b);
                }
|
	'@' '(' '#' ')' IDENTIFIER
		IDENTIFIER
		IDENTIFIER '/' IDENTIFIER '/' IDENTIFIER
		{ char b[SMALLBUFSIZE];
		  objVer = $6;
		  sprintf(b, "\"%s/%s/%s\"", $7, $9, $11);
		  objDate = save(b);
		}
|	IDENTIFIER IDENTIFIER '/' IDENTIFIER '/' IDENTIFIER
		{ char b[SMALLBUFSIZE];
		  objVer = $1;
		  sprintf(b, "\"%s/%s/%s\"", $2, $4, $6);
		  objDate = save(b);
		}
|	'%' IDENTIFIER '%' '%' IDENTIFIER '%'	
		{
		  char b[SMALLBUFSIZE];
		  long t;
		  objVer = "?.?";
		  t = time((time_t *)0);
		  b[0] = QUOTE;
		  strncat(b,ctime(&t),24);
		  strcat(b,"\"");
		  objDate = save(b);
		  /* objDate = "\"checked out\""; */
		}
|	error				{ yyerror("Illegal version format");}
;


/* star items */
staritem:
	sgitem
|	DOMAIN '{' ident '}'		{ domain = $3;}
|	DERIVED '{' ident '}'		{ derivedFrom = $3;}
|	portkey '{' portlist '}'	{ genPort();
					  describePort(); }
|	CODEBLOCK '(' ident cbargs ')' '{'
					{ char* b = malloc(SMALLBUFSIZE);
					  blockID = $3;
					  strcpy(b,blockID);
					  codeBlockNames[numBlocks]=b;
					  codeBlockArgs[numBlocks]=save($4);
					  codeBlockLines[numBlocks]=
					    savelineref();
					  codeMode = 1;
					}
		BODY	 		{ codeBlocks[numBlocks++]=$8;
					  codeMode = 0;
					}
;

cbargs: /*nothing*/		{ $$ = (char*)0; }
|	',' STRING		{ $$ = stripQuotes($2); }
;

conscalls:
	CONSCALLS '{'			{ bodyMode = 1;}

methlist:
	methitem
|	methlist methitem
;

/* optional parentheses: so you can say go() { code } or go { code } */
optp:	/* nothing */
|	'(' ')'
;

/* user-method declarations */
methitem:
	NAME '{' ident '}'		{ methodName = $3;}
|	ARGLIST '{' ident '}'		{ methodArgs = checkArgs($3);}
|	TYPE '{' ident '}'		{ methodType = $3;}
|	ACCESS '{' protkey '}'		{ methodAccess = $3;}
|	code BODY			{ methodCode = $2; bodyMode = 0;}
;

/* a code block */
code:
	CODE '{'			{ bodyMode = 1;}
;

/* a header code block */
header:
	HEADER '{'			{ bodyMode = 1;}
;

/* declare extra members */
members:
	protkey '{'			{ bodyMode = 1; $$ = $1;}
;

/* protection keywords */

protkey:PUBLIC
|	PROTECTED
|	PRIVATE
;

/* galaxy items */
galitem:
	sgitem
|	STAR '{' starinstlist '}'	{ genInstance();}
|	ALIAS '{' aliaslist '}'		{ genAlias();}
/*
|	CONNECT '{' connectlist '}'	{ genConnect();}
|	NUMPORTS '{' numportlist '}'	{ genNumPort();}
*/
;

/* porthole info */
portkey:
	INPUT				{ initPort(0,0);}
|	OUTPUT				{ initPort(1,0);}
|	INOUT				{ initPort(2,0);}
|	INMULTI				{ initPort(0,1);}
|	OUTMULTI			{ initPort(1,1);}
|	INOUTMULTI			{ initPort(2,1);}
;

portlist:
	portitem
|	portlist portitem
;

portitem:
	NAME '{' ident '}'		{ portName = $3;}
|	TYPE '{' ident '}'		{ portType = portDataType($3);}
|	TYPE '{' '=' ident '}'		{ portInherit = $4;} 
|	NUM '{' expval '}'		{ portNum = $3;}
|	attrib BODY			{ portAttrib = $2; bodyMode = 0;}
|	DESC '{' 			{ descMode = 1; docMode = 1;} BODY			{ portDesc = $4; docMode = 0;
					  descMode = 0;}
;

/* state info (for defining) */
dstatelist:
	dstateitem
|	dstatelist dstateitem
;

dstateitem:
	NAME '{' ident '}'		{ stateName = $3;}
|	TYPE '{' ident '}'		{ int tc = stateTypeClass($3);
					  stateMarks[tc]++;
					  stateClass = stateClasses[tc];
					}
|	DEFAULT '{' defval '}'		{ stateDef = $3;}
|	DESC '{' 			{ descMode = 1; docMode = 1;}
		BODY			{ stateDesc = $4;
					  docMode = 0;
					  descMode = 0;}
|	attrib BODY			{ stateAttrib = $2; bodyMode=0;}
;

attrib:	ATTRIB '{'			{ bodyMode=1;}

/* allow single token, a string, or a sequence of strings as a default value */
defval:	stringseq			{ $$ = $1;}
|	IDENTIFIER			{ char b[SMALLBUFSIZE];
					  sprintf (b, "\"%s\"", $1);
					  $$ = save(b);
					}
|	keyword				{ char b[SMALLBUFSIZE];
					  sprintf (b, "\"%s\"", $1);
					  $$ = save(b);
					}
;

stringseq: STRING			{ $$ = save($1);}
|	stringseq STRING		{ char* b = malloc(MEDBUFSIZE);
					  strcpy(b,$1);
					  strcat(b," ");
					  strcat(b,$2);
					  $$ = b; }
;

/* inverse of defval: we strip the quotes */
expval:	IDENTIFIER			{ $$ = $1;}
|	STRING				{ char b[SMALLBUFSIZE];
					  strcpy (b, $1+1);
					  b[strlen($1)-2] = 0;
					  $$ = save(b);
					}
;

starinstlist:
	starinstitem
|	starinstlist starinstitem
;

starinstitem:
	NAME '{' ident '}'		{ instName = $3;}
|	CLASS '{' ident '}'		{ instClass = $3;}
;

aliaslist:
	aliasitem
|	aliaslist aliasitem
;

aliasitem:
	NAME '{' ident '}'		{ galPortName = $3;}
;

/* include files */
cclist: /* nothing */
|	cclist optcomma STRING		{ ccInclude[nCcInclude++] = $3;}
;

/* see also list */
seealso: /* nothing */
|	seealso optcomma IDENTIFIER	{ seeAlsoList[nSeeAlso++] = $3;}
;

hlist:	/* nothing */
|	hlist optcomma STRING		{ hInclude[nHInclude++] = $3;}
;

optcomma:/* nothing */
|	','
;

/* this production allows keywords as idents in some places */

ident:	keyword
|	IDENTIFIER
/* also allow strings; strip quotation marks */
|	STRING					{ $$ = stripQuotes ($1);}
;

/* keyword in identifier position */
keyword:	DEFSTAR|GALAXY|NAME|DESC|DEFSTATE|DOMAIN|NUMPORTS|DERIVED
|CONSTRUCTOR|DESTRUCTOR|STAR|ALIAS
|INPUT|OUTPUT|INOUT|INMULTI|OUTMULTI|INOUTMULTI
|TYPE
|DEFAULT|BEGIN|SETUP|GO|WRAPUP|CONNECT|CCINCLUDE|HINCLUDE|PROTECTED|PUBLIC
|PRIVATE|METHOD|ARGLIST|CODE|ACCESS|AUTHOR|ACKNOWLEDGE|VERSION|COPYRIGHT
|EXPLANATION|START
|SEEALSO|LOCATION|CODEBLOCK|EXECTIME|PURE|INLINE|HEADER|INITCODE|STATIC
;

%%

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
	objName = objVer = objDesc = domain = derivedFrom =
		objAuthor = objCopyright = objExpl = objLocation = NULL;
	consStuff[0] = ccCode[0] = hCode[0] = consCalls[0] = 0;
	publicMembers[0] = privateMembers[0] = protectedMembers[0] = 0;
	inputDescriptions[0] = outputDescriptions[0] = inoutDescriptions[0] = 0;
	stateDescriptions[0] = 0;
	nCcInclude = nHInclude = nSeeAlso = 0;
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
	char *dest;
	char descriptString[MEDBUFSIZE];
	dest = portDir==2 ? inoutDescriptions 
	  : (portDir==1 ? outputDescriptions : inputDescriptions);
	if (portMulti) {
	    sprintf(str1,".NE\n\\fI%s\\fR (multiple), (%s)",portName,portType);
	} else {
	    sprintf(str1,".NE\n\\fI%s\\fR (%s)",portName,portType);
	}
	strcat(dest,str1);

	if (portDesc) {
	    if(unescape(descriptString, portDesc, MEDBUFSIZE))
		yywarn("warning: Descriptor too long. May be truncated.");
	    sprintf(str1,": %s\n",descriptString);
	} else
	    sprintf(str1,"\n");
	strcat(dest,str1);
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
	sprintf (str2, "\n\n%s %s%s::%s %s\n{\n", methodType,
		 galDef ? "" : domain, objName,
		 methodName, methodArgs);
	strcat (miscCode, str2);
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
	    } else if ( isalpha(c) ) {
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
	sprintf (fullClass, "%s%s", galDef ? "" : domain, objName);

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
			sprintf (baseClass, "%s%s", galDef ? "" : domain,
				 derivedFrom);
		}
		else
			(void) strcpy (baseClass, derivedFrom);
	}
/* Not explicitly specified: baseclass is Galaxy or XXXStar */
	else if (galDef)
		(void)strcpy (baseClass, "Galaxy");
	else
		sprintf (baseClass, "%sStar", domain);

/* Include files */
	fprintf (fp, "#include \"%s.h\"\n", baseClass);
	
	for (i = 0; i < nHInclude; i++) {
		fprintf (fp, "#include %s\n", hInclude[i]);
	}
/* Include files for states */
	for (i = 0; i < NSTATECLASSES; i++)
		if (stateMarks[i])
			fprintf (fp, "#include \"%s.h\"\n", stateClasses[i]);

/* extra header code */
	fprintf (fp, "%s\n", hCode);
/* The class template */
	fprintf (fp, "class %s : public %s\n{\n", fullClass, baseClass);
	sprintf (destNameBuf, "~%s", fullClass);
	fprintf (fp, "public:\n\t%s();\n", fullClass);
/* The makeNew function: only if the class isn't a pure virtual */
	if (!pureFlag)
		fprintf (fp, "\t/* virtual */ Block* makeNew() const;\n");
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
	fprintf (fp, "protected:\n");
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

/* ID block */
	if (idBlock)
		fprintf (fp, "%s\n", idBlock);
/* include files */
	fprintf (fp, "#include \"%s.h\"\n", fullClass);
	for (i = 0; i < nCcInclude; i++)
		fprintf (fp, "#include %s\n", ccInclude[i]);
/* generate className and (optional) makeNew function */
/* also generate a global identifier with name star_nm_DDDNNN, where DDD is
   the domain and NNN is the name */
	fprintf (fp, "\nconst char *star_nm_%s = \"%s\";\n", fullClass, fullClass);
        fprintf (fp, "\nconst char* %s :: className() const {return star_nm_%s;}\n",
		fullClass, fullClass);
	fprintf (fp, "\nISA_FUNC(%s,%s);\n",fullClass,baseClass);
	if (!pureFlag) {
		fprintf (fp, "\nBlock* %s :: makeNew() const { LOG_NEW; return new %s;}\n",
			 fullClass, fullClass);
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
	fprintf (fp, "\n%s%s::%s ()", ccCode, fullClass, fullClass);
	if (consCalls[0])
		fprintf (fp, " :\n\t%s", consCalls);
	fprintf (fp, "\n{\n");
	if (objDesc)
		fprintf (fp, "\tsetDescriptor(\"%s\");\n", objDesc);
	/* define the class name */
	if (!consCode) consCode = "";
	fprintf (fp, "%s\n%s\n", consStuff, consCode);
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
	}
	else {
		fprintf (fp, "\n// prototype instance for known block list\n");
		fprintf (fp, "static %s proto;\n", fullClass);
		fprintf (fp, "static RegisterBlock registerBlock(proto,\"%s\");\n",
			 objName);
	}
	(void) fclose(fp);

/**************************************************************************
		CREATE THE DOCUMENTATION FILE
*/

	sprintf (fname, "%s.t", fullClass);
	if ((fp = fopen (fname, "w")) == 0) {
		perror (fname);
		exit (1);
	}

	/* Turn eqn delimitters off */
	fprintf (fp, ".EQ\ndelim off\n.EN\n");

	fprintf (fp, ".\\\" documentation file generated from %s by %s\n",
		 inputFile, progName);

/* copyright */
	if (objCopyright) {
		char* p = objCopyright;
		while (*p) {
			/* make each line a comment */
			fprintf (fp, ".\\\" ");
			while (*p && *p != NEWLINE) fputc(*p++,fp);
			if (*p == NEWLINE) fputc(*p++,fp);
		}
		fputc(NEWLINE,fp);
	}

/* Name */
	fprintf (fp, ".NA \"%s\"\n", objName);

/* short descriptor */
	fprintf (fp, ".SD\n");
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
	fprintf (fp, ".SE\n");

/* location */
	if (objLocation)
		fprintf (fp, ".LO \"%s\"\n",objLocation);

/* base class and domain */
	/* For stars, we append the domain name to the beginning of the name,
	   unless it is already there */
	if (derivedFrom) {
		if (domain &&
		    strncmp (domain, derivedFrom, strlen (domain)) != 0) {
			sprintf (baseClass, "%s%s", galDef ? "" : domain,
				 derivedFrom);
		}
		else
			(void) strcpy (baseClass, derivedFrom);
	}
	/* Not explicitly specified: baseclass is Galaxy or XXXStar */
	else if (galDef)
		(void)strcpy (baseClass, "Galaxy");
	else
		sprintf (baseClass, "%sStar", domain);

	fprintf (fp, ".DM %s %s\n", domain, baseClass);

/* version */
	if (objVer && objDate)
		fprintf (fp, ".SV %s %s\n", objVer, objDate);

/* author */
	if (objAuthor)
		fprintf (fp, ".AL \"%s\"\n", objAuthor);

/* acknowledge */
	if (objAcknowledge)
		fprintf (fp, ".AC \"%s\"\n", objAcknowledge);

/* inputs */
	if (strlen(inputDescriptions) > 0)
		fprintf (fp, ".IH\n%s.PE\n", inputDescriptions);

/* outputs */
	if (strlen(outputDescriptions) > 0)
		fprintf (fp, ".OH\n%s.PE\n", outputDescriptions);

/* inouts */
	if (strlen(inoutDescriptions) > 0)
		fprintf (fp, ".BH\n%s.PE\n", inoutDescriptions);

/* states */
	if (strlen(stateDescriptions) > 0)
		fprintf (fp, ".SH\n%s.ET\n", stateDescriptions);

/* explanation */
	/* Turn eqn delimitters on */
	fprintf (fp, ".EQ\ndelim $$\n.EN\n");
	if (objExpl)
		fprintf (fp, ".LD\n%s\n", objExpl);

/* ID block (will appear in .h and .cc files only. */

/* See Also list */
	if (nSeeAlso > 0) fprintf (fp, ".SA\n");
	if (nSeeAlso > 2)
	    for (i = 0; i < (nSeeAlso - 2); i++)
		fprintf (fp, "%s,\n", seeAlsoList[i]);
	if (nSeeAlso > 1) fprintf (fp, "%s and\n", seeAlsoList[nSeeAlso-2]);
	if (nSeeAlso > 0) fprintf (fp, "%s.\n", seeAlsoList[nSeeAlso-1]);

/* end the final entry */
	fprintf (fp, ".ES\n");

/* close the file */
	(void) fclose (fp);
}



struct tentry {
	char* key;
	int code;
};

/* keyword table */
struct tentry keyTable[] = {
	"access", ACCESS,
	"acknowledge", ACKNOWLEDGE,
	"alias", ALIAS,
	"arglist", ARGLIST,
	"attrib", ATTRIB,
	"attributes", ATTRIB,
	"author", AUTHOR,
	"begin", BEGIN,
	"ccinclude", CCINCLUDE,
	"class", CLASS,
	"code", CODE,
	"codeblock", CODEBLOCK,
	"conscalls", CONSCALLS,
	"consCalls", CONSCALLS,
	"constructor", CONSTRUCTOR,
	"copyright", COPYRIGHT,
	"default", DEFAULT,
	"defstar", DEFSTAR,
	"defstate", DEFSTATE,
	"derived", DERIVED,
	"derivedFrom", DERIVED,
	"derivedfrom", DERIVED,
	"desc", DESC,
	"descriptor", DESC,
	"destructor", DESTRUCTOR,
	"domain", DOMAIN,
	"execTime", EXECTIME,
	"exectime", EXECTIME,
	"explanation", EXPLANATION,
	"galaxy", GALAXY,
	"go", GO,
	"header", HEADER,
	"hinclude", HINCLUDE,
	"ident", ID,
	"initCode", INITCODE,
	"initcode", INITCODE,
	"inmulti", INMULTI,
	"inline", INLINE,
	"inout", INOUT,
	"inoutmulti", INOUTMULTI,
	"input", INPUT,
	"location", LOCATION,
	"method", METHOD,
	"name", NAME,
	"num", NUM,
	"numports", NUMPORTS,
	"numTokens", NUM,
	"numtokens", NUM,
	"outmulti", OUTMULTI,
	"output", OUTPUT,
	"private", PRIVATE,
	"programmer", AUTHOR,
	"protected", PROTECTED,
	"public", PUBLIC,
	"pure", PURE,
	"seealso", SEEALSO,
	"setup", SETUP,
	"star", STAR,
	"start", START,
	"state", DEFSTATE,
	"static", STATIC,
	"type", TYPE,
	"version", VERSION,
	"virtual", VIRTUAL,
	"wrapup", WRAPUP,
	0, 0,
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
    int		key;

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
    } else if (! IDENTCHAR(c) ) {
	    yytext[0] = c;
	    yytext[1] = 0;
	    *pCurChar = 0;
	    return yytext[0];
    } else {
        /* we also return numeric values as IDENTIFIER: 
	 * digits and '.' allowed
	 */
        do {
	    *p++ = c;
	    c = yyinput();
        } while ( IDENTCHAR(c) );
    }
    *p = 0;
    *pCurChar = c;
    yylval = save(yytext);
    if ((key = lookup (yytext)) != 0) {
	    return key;
    }
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

/* make sure a function is a valid arglist */
char* checkArgs(in)
char* in;
{
	if (*in != LPAR || in[strlen(in)-1] != RPAR)
		yyerror ("Invalid argument list");
	return in;
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
	while (*s != NULL) {
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
		*d = NULL; /* terminate the string */
		return(1);
	    }
	}
	*d = NULL; /* terminate the string */
	return(0);
}

/* main program, just calls parser */
int main (argc, argv)
int argc;
char **argv;
{
	if (argc != 2) {
		fprintf (stderr, "Usage: %s file\n", *argv);
		exit (1);
	}
	inputFile = argv[1];
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

