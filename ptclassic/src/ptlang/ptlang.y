%{
/************************************************************************
 Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Ptolemy "star language" preprocessor.  This is an "alpha" version;
 it doesn't support compiled-in galaxies yet and the language may
 still change slightly.  Caveat hacker.

 Programmer: J. T. Buck
 Alpha version: 9/26/90

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

#define FLEN 256
#define NINC 10
#define NSTR 20

char yytext[BIGBUFSIZE];	/* lexical analysis buffer */
int yyline = 1;			/* current input line */
int bodyMode = 0;		/* special lexan mode flag to read bodies  */
FILE* yyin;			/* stream to read from */

char* progName = "ptlang";	/* program name */
int nerrs = 0;			/* # syntax errors detected */

/* scratch buffers */
char str1[SMALLBUFSIZE];
char str2[SMALLBUFSIZE];
char consStuff[BIGBUFSIZE];
char publicMembers[MEDBUFSIZE];
char protectedMembers[MEDBUFSIZE];
char privateMembers[MEDBUFSIZE];
char ccCode[BIGBUFSIZE];
char miscCode[BIGBUFSIZE];

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
char* objDesc;			/* descriptor of star or galaxy */
int   galDef;			/* true if obj is a galaxy */
char* domain;			/* domain of object (if star) */
char* derivedFrom;		/* class obj is derived from */
char* portName;			/* name of porthole */
char* portType;			/* dataType of porthole */
char* portNum;			/* expr giving # of tokens */
int   portOut;			/* true if porthole is output */
int   portMulti;		/* true if porthole is multiporthole */
char* stateName;		/* name of state */
char* stateClass;		/* class of state */
char* stateDef;			/* default value of state */
char* stateDesc;		/* descriptor for state */
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
%token BODY IDENTIFIER STRING 
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
|	DESC '{' desc '}'		{ objDesc = $3;}
|	DEFSTATE { clearStateDefs();}
		'{' dstatelist '}'	{ genState();}
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
|	error '}'			{ yyerror ("bad sgitem");}
;

/* star items */
staritem:
	sgitem
|	DOMAIN '{' ident '}'		{ domain = $3;}
|	DERIVED '{' ident '}'		{ derivedFrom = $3;}
|	portkey '{' portlist '}'	{ genPort();}
|	go BODY				{ goCode = $2; bodyMode = 0;}
;

constructor:
	CONSTRUCTOR optp '{'		{ bodyMode = 1;}
;

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
|	NUM '{' expval '}'		{ portNum = $3;}
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
|	DESC '{' desc '}'		{ stateDesc = $3;}
;

/* allow single token as a default value */
defval:	STRING				{ $$ = $1;}
|	IDENTIFIER			{ char b[SMALLBUFSIZE];
					  sprintf (b, "\"%s\"", $1);
					  $$ = save(b);
					}
;

/* inverse of defval: we strip the quotes */
expval:	IDENTIFIER			{ $$ = $1;}
|	STRING				{ char b[SMALLBUFSIZE];
					  strcpy (b, $1+1);
					  b[strlen($1)-2] = 0;
					  $$ = save(b);
					}
;

desc:	descstart desclist		{ $$ = combineStrings();}
;

descstart:/* nothing */			{ nStrings = 0;}
;

desclist:/* nothing */
|	desclist STRING			{ stringBuf[nStrings++] = $2;}
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
|PRIVATE|METHOD|ARGLIST|CODE|ACCESS
/* also allow strings; strip parenths */
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
	objName = objDesc = domain = derivedFrom = NULL;
	consStuff[0] = ccCode[0] = 0;
	publicMembers[0] = privateMembers[0] = protectedMembers[0] = 0;
}

/* Generate a state definition */
clearStateDefs ()
{
	stateName = stateClass = stateDef = stateDesc = NULL;
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
		stateDef = "\"\"";
	if (stateDesc == NULL) {
		sprintf (buf, "%c%s%c", QUOTE, stateName, QUOTE);
		stateDesc = buf;
	}
	sprintf (str1,"\t%s %s;\n", stateClass, stateName);
	sprintf (str2,"\taddState(%s.setState(\"%s\",this,%s,%s));\n",
	stateName, stateName, stateDef, stateDesc);
	strcat (protectedMembers, str1);
	strcat (consStuff, str2);
}

/* set up for port definition */
initPort (out, multi)
int out, multi;
{
	portOut = out;
	portMulti = multi;
	portName = portType = portNum = NULL;
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
	char hname[FLEN], ccname[FLEN];
	char baseClass[SMALLBUFSIZE];
	char fullClass[SMALLBUFSIZE];

/* temp, until we implement this */
	if (galDef) {
		fprintf (stderr, "Sorry, galaxy definition is not yet supported.\n");
		exit ();
	}
/* First, make the .h file */
	if (objName == NULL) {
		yyerror ("No class name defined");
		return;
	}
	if (!galDef && !domain) {
		yyerror ("No domain name defined");
		return;
	}
	sprintf (fullClass, "%s%s", galDef ? "" : domain, objName);
	sprintf (hname, "%s.h", fullClass);
	if ((fp = fopen (hname, "w")) == 0) {
		perror (hname);
		exit (1);
	}
/* Surrounding ifdef stuff */
	fprintf (fp, "#ifndef _%s_h\n#define _%s_h 1\n", fullClass, fullClass);
	fprintf (fp, "// header file generated from %s by %s\n",
		 inputFile, progName);
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
	if (publicMembers)
		fprintf (fp, "%s\n", publicMembers);
/* The clone function; end of file */
	fprintf (fp, "\tBlock* clone() { return new %s;}\n};\n", fullClass);
	fprintf (fp, "#endif\n");
	(void) fclose (fp);

/* Now do the .cc file */
	sprintf (ccname, "%s.cc", fullClass);
	if ((fp = fopen (ccname, "w")) == 0) {
		perror (ccname);
		exit (1);
	}
	fprintf (fp, "// .cc file generated from %s by %s\n",
		 inputFile, progName);
/* ID block */
	if (idBlock)
		fprintf (fp, "%s\n", idBlock);
/* include files */
	fprintf (fp, "#include \"KnownBlock.h\"\n");
	fprintf (fp, "#include \"%s.h\"\n", fullClass);
	for (i = 0; i < nCcInclude; i++)
		fprintf (fp, "#include %s\n", ccInclude[i]);
/* prefix code and constructor */
	fprintf (fp, "\n%s%s::%s () {\n", ccCode, fullClass, fullClass);
	if (objDesc)
		fprintf (fp, "\tdescriptor = %s;\n", objDesc);
	if (!consCode) consCode = "";
	fprintf (fp, "%s\n%s\n}\n", consStuff, consCode);
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
	fprintf (fp, "\n// prototype instance for known block list\n");
	fprintf (fp, "static %s proto;\n", fullClass);
	fprintf (fp, "static KnownBlock entry(proto,\"%s\");\n", objName);
	(void) fclose(fp);
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
	"ccinclude", CCINCLUDE,
	"class", CLASS,
	"code", CODE,
	"constructor", CONSTRUCTOR,
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
	"galaxy", GALAXY,
	"go", GO,
	"hinclude", HINCLUDE,
	"ident", ID,
	"inmulti", INMULTI,
	"input", INPUT,
	"method", METHOD,
	"name", NAME,
	"num", NUM,
	"numports", NUMPORTS,
	"numTokens", NUM,
	"numtokens", NUM,
	"outmulti", OUTMULTI,
	"output", OUTPUT,
	"private", PRIVATE,
	"protected", PROTECTED,
	"public", PUBLIC,
	"star", STAR,
	"start", START,
	"state", DEFSTATE,
	"type", TYPE,
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
	if (!c) input();
/* bodyMode causes a whole function body to be returned as a single token.*/
	if (bodyMode) {
		int brace = 1;
		int inQuote = 0;
/* put a "#line" directive in the token */
		sprintf (yytext, "# line %d \"%s\"\n", yyline, inputFile);
		p = yytext + strlen (yytext);

		while (brace > 0) {
			*p++ = input();
			if (c == ESC) {
				c = getc(yyin);
				*p++ = c;
				continue;
			}
			else if (c == QUOTE)
				inQuote = !inQuote;
			else if (c == EOF) {
				yyerror ("Unexpected EOF in method!");
				exit (1);
			}
			else if (!inQuote) {
				if (c == '{') brace++;
				else if (c == '}') brace--;
			}
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
/* regular code (not BODY mode) */

/* loop to eat up blanks and comments.  A comment starts with // and
 * continues for the rest of the line.  If a single / is seen in the
 * loop a '/' token is returned.
 */
	while (1) {
		while (isspace(c)) {
			input();
		}
		if (c != '/') break;
		else {
			input();
			if (c != '/') {
				*yytext = '/';
				yytext[1] = 0;
				return '/';
			}
			/* comment -- eat rest of line */
			while (input() != '\n');
		}
	}
	/* STRING token includes surrounding quotes */
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

/* combine multi-strings for descriptors */
char* combineStrings () {
	int i;
	if (nStrings == 0) return "\"\"";
	if (nStrings == 1) return stringBuf[0];
	strcpy (str1, stringBuf[0]);
	for (i = 1; i < nStrings; i++) {
		strcat (str1, "\n\t");
		strcat (str1, stringBuf[i]);
	}
	return save (str1);
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
