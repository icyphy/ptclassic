%{
/************************************************************************
 Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Ptolemy "star language" preprocessor.  This is an "alpha" version;
 it doesn't support compiled-in galaxies yet and the language may
 still change slightly.  Caveat hacker.

 Programmer: J. T. Buck and E. A. Lee

************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Symbols for special characters*/
#define LPAR '('
#define RPAR ')'
#define QUOTE '"'
#define ESC '\\'
#define NEWLINE '\n'

/* buffer sizes */
#define BIGBUFSIZE 32768
#define MEDBUFSIZE 4096
#define SMALLBUFSIZE 512

/* number of code blocks allowed */
#define NUMCODEBLOCKS 100

#define FLEN 256
#define NINC 10
#define NSEE 30
#define NSTR 20

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
char* blockNames[NUMCODEBLOCKS];
char* codeBlocks[NUMCODEBLOCKS];
int numBlocks = 0;

/* scratch buffers */
char str1[SMALLBUFSIZE];
char str2[SMALLBUFSIZE];
char consStuff[BIGBUFSIZE];
char publicMembers[MEDBUFSIZE];
char protectedMembers[MEDBUFSIZE];
char privateMembers[MEDBUFSIZE];
char inputDescriptions[MEDBUFSIZE];
char outputDescriptions[MEDBUFSIZE];
char stateDescriptions[MEDBUFSIZE];
char ccCode[BIGBUFSIZE];
char miscCode[BIGBUFSIZE];
char codeBlock[MEDBUFSIZE];
char consCalls[BIGBUFSIZE];

/* state classes */
#define T_INT 0
#define T_FLOAT 1
#define T_COMPLEX 2
#define T_STRING 3
#define T_INTARRAY 4
#define T_FLOATARRAY 5
#define T_COMPLEXARRAY 6
#define NSTATECLASSES 7

/* note: names must match order of symbols above */
char* stateClasses[] = {
"IntState", "FloatState", "ComplexState", "StringState",
"IntArrayState", "FloatArrayState", "ComplexArrayState"
};

/* bookkeeping for state include files */
int stateMarks[NSTATECLASSES];

/* external functions */
char* save();			/* duplicate a string */
char* malloc();

/* forward declarations for functions */

char* whichMembers();
char* checkArgs();
char* stripQuotes();
char* portDataType();
int   stateTypeClass();

char* inputFile;		/* input file name */
char* idBlock;			/* ID block */
char* objName;			/* name of star or galaxy class being decld  */
char* objVer;			/* sccs version number */
char* objDate;			/* date of last update */
char* objDesc;			/* descriptor of star or galaxy */
char* objAuthor;		/* author of star or galaxy */
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
int   portOut;			/* true if porthole is output */
int   portMulti;		/* true if porthole is multiporthole */
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
char* galPortName;		/* name of galaxy port */
char* consCode;			/* extra constructor code */
char* destCode;			/* destructor */
char* startCode;		/* start method */
char* goCode;			/* go method */
char* wrapupCode;		/* wrapup method */
char* hInclude[NINC];		/* include files in .h file */
int   nHInclude;		/* number of such files */
char* ccInclude[NINC];		/* include files in .cc file */
int   nCcInclude;		/* number of such files */
char* seeAlsoList[NSEE];	/* list of pointers to other manual sections */
int   nSeeAlso;			/* number of such pointers */
char* stringBuf[NSTR];		/* bufs for multi-line descriptors */
int   nStrings;			/* # of strings */

/* all tokens with values are type char *.  Keyword tokens
 * have their names as values.
 */
typedef char * STRINGVAL;
#define YYSTYPE STRINGVAL

%}

%token DEFSTAR GALAXY NAME DESC DEFSTATE DOMAIN NUMPORTS NUM
%token DERIVED CONSTRUCTOR DESTRUCTOR STAR ALIAS OUTPUT INPUT ACCESS
%token OUTMULTI INMULTI TYPE DEFAULT CLASS START GO WRAPUP CONNECT ID
%token CCINCLUDE HINCLUDE PROTECTED PUBLIC PRIVATE METHOD ARGLIST CODE
%token BODY IDENTIFIER STRING CONSCALLS ATTRIB LINE
%token VERSION AUTHOR COPYRIGHT EXPLANATION SEEALSO LOCATION CODEBLOCK
%%
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
|	constructor BODY		{ consCode = $2; bodyMode = 0;}
|	destructor BODY			{ destCode = $2; bodyMode = 0;}
|	start BODY			{ startCode = $2; bodyMode = 0;}
|	wrapup BODY			{ wrapupCode = $2; bodyMode = 0;}
|	members BODY			{ addMembers ($1, $2); bodyMode = 0;}
|	code BODY			{ strcat (ccCode, $2); 
					  strcat (ccCode, "\n\n");
					  bodyMode = 0;
					}
|	METHOD { clearMethodDefs();}
		'{' methlist '}'	{ genMethod();}
|	CCINCLUDE '{' cclist '}'	{ }
|	HINCLUDE '{' hlist '}'
|	conscalls BODY			{ if(consCalls[0]) {
					     strcat(consCalls,", ");
					     strcat(consCalls,$2);
					  } else {
					     strcpy(consCalls,$2);
					  }
					  bodyMode = 0; 
					}
|	error '}'			{ yyerror ("bad sgitem");}
;

/* version identifier */
version:
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
		  t = time(0);
		  b[0] = QUOTE;
		  strncat(b,ctime(&t),24);
		  strcat(b,"\"");
		  objDate = save(b);
		  /* objDate = "\"checked out\""; */
		}
;


/* star items */
staritem:
	sgitem
|	DOMAIN '{' ident '}'		{ domain = $3;}
|	DERIVED '{' ident '}'		{ derivedFrom = $3;}
|	portkey '{' portlist '}'	{ genPort();
					  describePort(); }
|	go BODY				{ goCode = $2; bodyMode = 0;}
|	CODEBLOCK '(' IDENTIFIER ')' '{'
					{ char* b = malloc(SMALLBUFSIZE);
					  blockID = $3;
					  strcpy(b,blockID);
					  blockNames[numBlocks]=b;
					  codeMode = 1;
					}
		lines '}'		{ char* b = malloc(strlen(codeBlock));
					  strcpy(b,codeBlock);
					  codeBlocks[numBlocks++]=b;
					  codeMode = 0; 
					  codeBlock[0] = 0;
					}
;

lines:	/* nothing */
|	lines LINE		{ char b[SMALLBUFSIZE];
		sprintf(b,"\"%s\\n\"\n",$2);
		strcat(codeBlock,b);
				}
;

constructor:
	CONSTRUCTOR optp '{'		{ bodyMode = 1;}
;

conscalls:
	CONSCALLS '{'			{ bodyMode = 1;}

destructor:
	DESTRUCTOR optp '{'		{ bodyMode = 1;}

start:
	START optp '{'			{ bodyMode = 1;}
;

go:
	GO optp	'{'			{ bodyMode = 1;}
;

wrapup:
	WRAPUP optp '{'			{ bodyMode = 1;}
;

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
portkey:OUTPUT				{ initPort(1,0);}
|	INPUT				{ initPort(0,0);}
|	OUTMULTI			{ initPort(1,1);}
|	INMULTI				{ initPort(0,1);}
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
|	DESC '{' 			{ descMode = 1; docMode = 1;}
		BODY			{ portDesc = $4;
					  docMode = 0;
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
;

stringseq: STRING			{ char* b = malloc(MEDBUFSIZE);
					  strcpy(b, $1);
					  $$ = b;
					}
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

ident:	IDENTIFIER|DEFSTAR|GALAXY|NAME|DESC|DEFSTATE|DOMAIN|NUMPORTS|DERIVED
|CONSTRUCTOR|DESTRUCTOR|STAR|ALIAS|OUTPUT|INPUT|OUTMULTI|INMULTI|TYPE
|DEFAULT|START|GO|WRAPUP|CONNECT|CCINCLUDE|HINCLUDE|PROTECTED|PUBLIC
|PRIVATE|METHOD|ARGLIST|CODE|ACCESS|AUTHOR|VERSION|COPYRIGHT|EXPLANATION
|SEEALSO|LOCATION|CODEBLOCK
/* also allow strings; strip quotation marks */
|STRING					{ $$ = stripQuotes ($1);}
;

%%
/* Reset for a new star or galaxy class definition.  If arg is TRUE
   we are defining a galaxy.
 */

clearDefs (g)
int g;
{
	int i;
	for (i = 0; i < NSTATECLASSES; i++) stateMarks[i] = 0;
	galDef = g;
	objName = objVer = objDesc = domain = derivedFrom =
		objAuthor = objCopyright = objExpl = objLocation = NULL;
	consStuff[0] = ccCode[0] = codeBlock[0] = consCalls[0] = 0;
	publicMembers[0] = privateMembers[0] = protectedMembers[0] = 0;
	inputDescriptions[0] = outputDescriptions[0] = stateDescriptions[0] = 0;
	nCcInclude = nHInclude = nSeeAlso = 0;
}

/* Generate a state definition */
clearStateDefs ()
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
	fprintf (stderr, "state class %s\n", name);
	yyerror ("bad state class: assuming int");
		return T_INT;
}

/* generate code for a state defn */
genState ()
{
	char buf[128];
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
describeState ()
{
	char* descriptString[MEDBUFSIZE];

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
initPort (out, multi)
int out, multi;
{
	portOut = out;
	portMulti = multi;
	portName = portNum = portInherit = portDesc = NULL;
	portType = "ANYTYPE";
}

char* portDataType (name)
char* name;
{
	/* do better checking later */
	return save(name);
}

genPort ()
{
	/* test that all fields are known */
	char* dir = portOut ? "Out" : "In";
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
	if (portInherit) {
		sprintf (str2, "\t%s.inheritTypeFrom(%s);\n", portName,
			 portInherit);
		strcat (consStuff, str2);
	}
}

describePort ()
{
	char *dest, *m;
	char descriptString[MEDBUFSIZE];
	if(portOut)
	    /* describe an output port */
	    dest = outputDescriptions;
	else
	    /* describe an input port */
	    dest = inputDescriptions;
	if (portMulti)
	    sprintf(str1,".NE\n\\fI%s\\fR (multiple), (%s)",portName,portType);
	else
	    sprintf(str1,".NE\n\\fI%s\\fR (%s)",portName,portType);
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
clearMethodDefs ()
{
	methodName = NULL;
	methodArgs = "()";
	methodAccess = "protected";
	methodCode = NULL;
	methodType = "void";
}

/* generate code for user-defined method */
genMethod ()
{
	char * p = whichMembers (methodAccess);
/* add decl to class body */
	sprintf (str1, "\t%s %s %s;\n", methodType, methodName, methodArgs);
	strcat (p, str1);
	sprintf (str2, "\n\n%s %s%s::%s %s\n{\n", methodType,
		 galDef ? "" : domain, objName,
		 methodName, methodArgs);
	strcat (miscCode, str2);
	strcat (miscCode, methodCode);
	strcat (miscCode, "\n}\n");
}

/* generate an instance of a block within a galaxy */
genInstance ()
{
	sprintf (str1, "\t%s $s;\n", instClass, instName);
	strcat (protectedMembers, str1);
	sprintf (str2,"addBlock(%s.setBlock(\"%s\",this)", instName, instName);
	strcat (consStuff, str2);
}


genAlias () {
	/* FILL IN */
}

genConnect () {
	/* FILL IN */
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
	}
}

/* add declarations of extra members to the class definition */
addMembers (type, defs)
char* type;
char* defs;
{
	char * p = whichMembers (type);
	strcat (p, defs);
	strcat (p, "\n");
	return;
}

/* This is the main guy!  It outputs the complete class definition. */
genDef ()
{
	FILE *fp;
	int i;
	char fname[FLEN], hname[FLEN], ccname[FLEN];
	char baseClass[SMALLBUFSIZE];
	char fullClass[SMALLBUFSIZE];
	char descriptString[MEDBUFSIZE];
	char *d;

/* temp, until we implement this */
	if (galDef) {
		fprintf (stderr, "Sorry, galaxy definition is not yet supported.\n");
		exit ();
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
/* copyright */
	if (objCopyright)
		fprintf (fp, "/*\n * copyright (c) %s\n */\n", objCopyright);

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


/* The class template */
	fprintf (fp, "\nclass %s : public %s\n{\n", fullClass, baseClass);
	if (privateMembers[0])
		fprintf (fp, "private:\n%s\n", privateMembers);
	if (protectedMembers[0])
		fprintf (fp, "protected:\n%s\n", protectedMembers);
	fprintf (fp, "public:\n\t%s();\n", fullClass);
	if (startCode)
		fprintf (fp, "\tvoid start();\n");
	if (goCode)
		fprintf (fp, "\tvoid go();\n");
	if (wrapupCode)
		fprintf (fp, "\tvoid wrapup();\n");
	if (destCode)
		fprintf (fp, "\t~%s();\n", fullClass);
	if (publicMembers[0])
		fprintf (fp, "%s\n", publicMembers);
	for (i=0; i<numBlocks; i++)
		fprintf (fp, "\tstatic %sCodeBlock %s;\n",domain,blockNames[i]);
/* The clone function; end of file */
	fprintf (fp, "\tBlock* clone() const { return new %s;}\n};\n", fullClass);
	fprintf (fp, "#endif\n");
	(void) fclose (fp);

/**************************************************************************
		CREATE THE .cc FILE
*/
	sprintf (ccname, "%s.cc", fullClass);
	if ((fp = fopen (ccname, "w")) == 0) {
		perror (ccname);
		exit (1);
	}
	fprintf (fp, "// .cc file generated from %s by %s\n",
		 inputFile, progName);
/* copyright */
	if (objCopyright)
		fprintf (fp, "/*\n * copyright (c) %s\n */\n", objCopyright);

/* ID block */
	if (idBlock)
		fprintf (fp, "%s\n", idBlock);
/* include files */
	fprintf (fp, "#include \"KnownBlock.h\"\n");
	fprintf (fp, "#include \"%s.h\"\n", fullClass);
	for (i = 0; i < nCcInclude; i++)
		fprintf (fp, "#include %s\n", ccInclude[i]);
/* prefix code and constructor */
	fprintf (fp, "\n%s%s::%s ()", ccCode, fullClass, fullClass);
	if (consCalls[0])
		fprintf (fp, " :\n\t%s", consCalls);
	fprintf (fp, "\n{\n");
	if (objDesc)
		fprintf (fp, "\tdescriptor = \"%s\";\n", objDesc);
	if (!consCode) consCode = "";
	fprintf (fp, "%s\n%s\n", consStuff, consCode);
	fprintf (fp, "}\n");
	if (startCode)
		fprintf (fp, "\nvoid %s::start() {\n%s\n}\n", fullClass, startCode);
	if (goCode)
		fprintf (fp, "\nvoid %s::go() {\n%s\n}\n", fullClass, goCode);
	if (wrapupCode)
		fprintf (fp, "\nvoid %s::wrapup() {\n%s\n}\n", fullClass, wrapupCode);
	if (destCode)
		fprintf (fp, "\n%s::~%s() {\n%s\n}\n", fullClass, fullClass, destCode);
	if (miscCode[0])
		fprintf (fp, "%s\n", miscCode);
	/* generate the CodeBlock constructor calls */
	for (i=0; i<numBlocks; i++)
		fprintf (fp, "%sCodeBlock %s :: %s (\n%s);\n",
			domain,fullClass,blockNames[i],codeBlocks[i]);
	fprintf (fp, "\n// prototype instance for known block list\n");
	fprintf (fp, "static %s proto;\n", fullClass);
	fprintf (fp, "static KnownBlock entry(proto,\"%s\");\n", objName);
	(void) fclose(fp);

/**************************************************************************
		CREATE THE DOCUMENTATION FILE
*/

	sprintf (fname, "%s.t", fullClass);
	if ((fp = fopen (fname, "w")) == 0) {
		perror (fname);
		exit (1);
	}

	fprintf (fp, ".\\\" documentation file generated from %s by %s\n",
		 inputFile, progName);

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
	fprintf (fp, ".SV %s %s\n", objVer, objDate);

/* author */
	if (objAuthor)
		fprintf (fp, ".AL \"%s\"\n", objAuthor);

/* copyright */
	if (objCopyright)
		fprintf (fp, ".CO \"%s\"\n", objCopyright);

/* inputs */
	if (strlen(inputDescriptions) > 0)
		fprintf (fp, ".IH\n%s.PE\n", inputDescriptions);

/* outputs */
	if (strlen(outputDescriptions) > 0)
		fprintf (fp, ".OH\n%s.PE\n", outputDescriptions);

/* states */
	if (strlen(stateDescriptions) > 0)
		fprintf (fp, ".SH\n%s.ET\n", stateDescriptions);

/* explanation */
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
	"alias", ALIAS,
	"arglist", ARGLIST,
	"attrib", ATTRIB,
	"attributes", ATTRIB,
	"author", AUTHOR,
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
	"explanation", EXPLANATION,
	"galaxy", GALAXY,
	"go", GO,
	"hinclude", HINCLUDE,
	"ident", ID,
	"inmulti", INMULTI,
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
	"seealso", SEEALSO,
	"star", STAR,
	"start", START,
	"state", DEFSTATE,
	"type", TYPE,
	"version", VERSION,
	"wrapup", WRAPUP,
	0, 0,
};

#define input() ((c = getc(yyin))==10?(yyline++,c):c)

/* The lexical analyzer */
yylex () {
	static int c = 0;
	int key;
	char* p = yytext;
	if (c == EOF) return 0;
/*
 * In codeMode, we look for LINEs and return them.
 * A LINE is an exact copy of of line of input that
 * does not contain the closing '}'.
 * When a line is encountered that contains the closing '}'
 * that closing '}' is returned.  Anything else on the line is lost.
 */
	if (codeMode) {
	    int brace = 1;
	    int inQuote = 0;
	    /* eat spaces until a newline */
	    while (!c || (isspace(c) && c != NEWLINE))
		input();
	    /* now eat the newline */
	    if (c == NEWLINE) input();
	    /* now transfer characters to yytext until the next newline,
	       or the closing brace. */
	    while (c != NEWLINE) {
		*p++ = c;
		switch (c) {
		  case ESC:
		    c = getc(yyin);
		    *p++ = c;
		    break;
		  case QUOTE:
		    inQuote = !inQuote;
		    break;
		  case EOF:
		    yyerror ("Unexpected EOF in body!");
		    exit (1);
		  default:
		    if (!inQuote) {
			if (c == '{') brace++;
			else if (c == '}') {
			    brace--;
			    if (brace == 0) {
				/* output doesn't include the '}' */
				p[0] = 0;
				c = 0;
				yylval = save(yytext);
				return '}';
			    }
			}
		    }
		}
		input();
	    }
	    /* output doesn't include the NEWLINE */
	    p[0] = 0;
	    yylval = save(yytext);
	    return LINE;
	}

/* bodyMode causes a whole function or document
 * body to be returned as a single token.
 * Leading and trailing spaces are removed
 */
	while (!c || isspace(c)) {
		input();
	}
	if (bodyMode) {
		int brace = 1;
		int inQuote = 0;
/* if !docMode, put a "#line" directive in the token */
		if (!docMode) {
		   sprintf (yytext, "# line %d \"%s\"\n", yyline, inputFile);
		   p = yytext + strlen (yytext);
		}

		while (brace > 0) {
			*p++ = c;
			switch (c) {
			case ESC:
				c = getc(yyin);
				*p++ = c;
				break;
			case QUOTE:
				inQuote = !inQuote;
				break;
			case EOF:
				yyerror ("Unexpected EOF in body!");
				exit (1);
			default:
				if (!inQuote) {
				   if (c == '{') brace++;
				   else if (c == '}') brace--;
				}
			}
			input();
		}
/* The BODY token does not include the closing '}' though it is removed
 * from the input.
 */
		--p;
/* trim trailing whitespace */
		--p;
		while (isspace(*p)) --p;
		p[1] = 0;
		c = 0;
		yylval = save(yytext);
		return BODY;
	}

/* descMode causes a whole descriptor body to be returned as a single token
 * in the form of a string with newlines indicated as "\n" and quotes
 * escaped (\").
 */
	if (descMode) {
		int brace = 1;
		int inQuote = 0;
		while (brace > 0) {
			*p++ = c;
			switch (c) {
			case ESC:
				c = getc(yyin);
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
			input();
		}
/* The BODY token does not include the closing '}' though it is removed
 * from the input.
 */
		--p;
/* trim trailing whitespace or '\n' */
		--p;
		while (isspace(*p) || *p == 'n')
			if( (*p == 'n') && (*(p-1) == ESC) )
				p -= 2;
			else --p;
		p[1] = 0;
		c = 0;
		yylval = save(yytext);
		return BODY;
	}

/* regular code (not BODY mode) */

/* loop to eat up blanks and comments.  A comment starts with // and
 * continues for the rest of the line.  If a single / is seen in the
 * loop a '/' token is returned.
 */
	while (1) {
		if (c != '/') break;
		else {
			input();
			if (c != '/') {
				*yytext = '/';
				yytext[1] = 0;
				return '/';
			}
			/* comment -- eat rest of line */
			while (input() != NEWLINE);
		}
		while (isspace(c)) { input(); }
	}
	/*
	 * STRING token includes surrounding quotes
	 * If the STRING includes a NEWLINE, a warning is issued.
	 */
	if (c == QUOTE) {
		p = yytext;
		*p++ = c;
		while (1) {
			*p++ = input();
			if (c == QUOTE) {
				*p = 0;
				break;
			}
			else if (c == ESC) {
				*p++ = input();
			}
			else if (c == NEWLINE) {
				yywarn ("warning: multi-line string");
			}
			else if (c == EOF) {
				yyerror ("Unexpected EOF in string");
				exit (1);
			}
		}
		c = 0;
		yylval = save(yytext);
		return STRING;
	}
	/* Token like <stdio.h> */
	else if (c == '<') {
		p = yytext;
		*p++ = c;
		while (1) {
			*p++ = input();
			if (c == '>') {
				*p = 0;
				break;
			}
			else if (c == EOF) {
				yyerror ("Unexpected EOF in string");
				exit (1);
			}
		}
		c = 0;
		yylval = save(yytext);
		return STRING;
	}
	else if (!isalnum(c) && c != '.') {
		yytext[0] = c;
		yytext[1] = 0;
		c = 0;
		return yytext[0];
	}
/* note: we also return numeric values as IDENTIFIER: digits and '.' allowed */
	else do {
		*p++ = c;
		input();
	} while (isalnum(c) || c == '.');
	*p = 0;
	yylval = save(yytext);
	if ((key = lookup (yytext)) != 0) {
		return key;
	}
	return IDENTIFIER;
}

int lookup (text)
char* text;
{
	struct tentry *p = keyTable;
	while (p->key && strcmp (p->key, text) != 0) p++;
	return p->code;
}

/* save token in dynamic memory */
char* save(in)
char* in;
{
	char* out = malloc(strlen(in)+1);
	return strcpy(out,in);
}

/* strip quotes, save token in dynamic memory */
char* stripQuotes(in)
char* in;
{
	char* out;
	int l = strlen (in);
	if (l <= 2 || *in != QUOTE) return in;
	out = malloc(l-1);
	return strncpy(out,in+1,l-2);
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
main (argc, argv)
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

yyerr2 (x, y)
char *x, *y;
{
	strcpy (str1, x);
	strcat (str1, y);
	yyerror (str1);
}

yyerror(s)
char *s;
{
	fprintf (stderr, "\"%s\", line %d: %s\n", inputFile, yyline, s);
	nerrs++;
	return;
}

yywarn(s)
char *s;
{
	fprintf (stderr, "\"%s\", line %d: %s\n", inputFile, yyline, s);
	return;
}
