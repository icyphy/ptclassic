#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*
 * Key and Menu Binding Resolving System
 *
 * David Harrison
 * University of California, Berkeley
 * 1986, 1987
 *
 * This file contains definitions required for the use of the bindings
 * module.  The bindings module supports multiple binding tables.
 * A binding table contains mappings from keyboard characters,  command
 * names and menu entries to function pointers,  remote application startups,
 * and remote commands.  Bindings tables can be built
 * from an initialization file (called a binding file) or they can
 * be built on the fly.  VEM defines a standard bindings file for
 * the basic commands available for editing various viewtypes.
 *
 * When reading bindings from a file,  the bindings module makes use
 * of the function pointer routines in commands.c to determine the
 * address of functions associated with keys or menu entries.  If
 * the binding specifies a remote function,  this function must
 * be registered using a special binding definition function.
 *
 * The bindings file format defines multiple keyboard, menu, and
 * command name bindings for a uniquely identified command.
 * There are three statement types:  normal binding,  remote declaration,
 * and a copy directive.   A normal binding sets up key, menu, and
 * alias bindings for a specified command.  A remote declaration defines
 * a new command which starts a remote application.  The copy directive
 * copies a previously defined table into a new table.  The format
 * is as follows:
 *
 * viewtype id [KEY keySpec ... ] [MENU menuSpec ... ] [ALIAS aliasSpec ... ]
 *
 *	This is the format for a standard binding.  The viewtype and
 *	unique command name (id) are required.  The KEY, MENU, and
 *	ALIAS clauses may appear in any order and any number of times.
 *      A keyspec must be a literal character,  hat notation, or octal
 *	notation.  A menuspec consists of a pane name (specifing the
 *      pane of the entry) and a selection index (an integer indicating
 *      how far down in the pane the entry will appear.  An alias
 *      spec is simply another name for the command.  If ALIAS
 *      appears without any specifications,  the standard display
 *      name for the command is registered as an alias.  The keywords
 *      may be plural (e.g. KEYS, MENUS, ALIASES).
 *
 * REMOTE id host path
 * 
 * 	 This statement is not a binding but a declaration
 * 	 specifing the id as a remote application.  The
 * 	 host and path specify the location of the program
 * 	 executable.  Once a id has been declared as remote,
 * 	 bindings may be made using the other statement types.
 * 
 * new-viewtype COPY old-viewtype
 *
 *	 This is a directive which tells VEM to create
 *       a new table which is an exact copy of the old table
 *       *up to the definitions specified so far*.
 *
 * INCLUDE file
 *
 *	 This directive tells VEM to read some other file
 *	 bindings file at this point.  The path may include
 *	 references to users using the standard UNIX tilde (~)
 *	 notation.
 *
 * INCLUDESTD
 *
 *	 This directive tells VEM to include the standard definitions
 *       it uses if the user does not specify his own bindings file.
 *	 This can be used to make incremental changes to VEM standard
 *	 definitions.
 *
 * Some explaination about menus is appropriate.  Initially,  VEM uses
 * the XMenu library for menus.  The module supports a call (bdGetMenu)
 * which constructs a menu from a bindings specification.  A XMenu menu 
 * consists of one or more panes each with one or more selections.  When 
 * activated (using XMenuActivate) the menu panes appear as a deck of 
 * cards slightly fanned out.  As the mouse is moved,  the cards riffle 
 * exposing appropriate panes.  As the mouse is moved up and down in a 
 * pane,  selections are highlighted.  When a selection is made (either 
 * by releasing or pushing the menu button),  the routine returns pane and
 * selection indicies.  These are the pane and selection numbers
 * discussed above.
 */

#include "port.h"
#include "general.h"		/* General VEM defns  */
#include "bindings.h"		/* Self-declaration   */
#include "st.h"			/* Hash table package */
#include "message.h"		/* Message display    */
#include "defaults.h"		/* Default handling   */
#include "commands.h"		/* Function bindings  */
#include "buffer.h"		/* Buffer styles      */


#define MAXFIELDSIZE	255
#define MAXLINE		MAXFIELDSIZE*5
#define MAXKEYS		256
#define MAXPANES	10
#define MAXSELECT	20
#define MAXALIASES	100
#define MAXMENUDEPTH	2
#define COMCHAR		'#'

/* Space for input lines */
static char Line[MAXLINE];

/* Hash table of all known binding tables keyed by table name */
static st_table *allTables = (st_table *) 0;

/* Hash table of all registered bindings (bdBinding) keyed by id */
static st_table *nameTable = (st_table *) 0;

/* Definitions for building standard bindings table file */

#define CAD_PATH(pth) \
VEMSTRCOPY(strcat(strcat(strcpy(buf, ""), "$OCTTOOLS"), pth))

#define BDS(bloc) \
CAD_PATH(strcat(strcpy(buf2, bloc), CUR_VER))

/*
 * Forward declarations
 */

static STR bdFindKeys();
static STR bdFindMenu();
static STR bdFindAlias();
static void append_key();
static void append_menu();
static void append_ali();
static int getline();
static int getword();
static int doRemote();
static int doCopy();
static int doKey();
static int doBdMenu();
static int doAlias();
static int strdiff();
static int bdLocKeys();



/* Alias mapping structure */

typedef struct aliasMap_defn {
    STR aliasName;		/* Name of alias      */
    bdBinding *aliasFunc;	/* Associated binding */
} aliasMap;

/* Menu mapping structure */

typedef struct menuMap_defn {
    STR paneName;		/* Pane name of binding */
    bdBinding *menuFunc;	/* Associated binding   */
} menuMap;

/* The binding table structure (bdTable is a hidden pointer to this) */

typedef struct bd_table_defn {
    STR tableName;		/* Name of table                  */
    bdBinding **keyBindings;	/* Keyboard binding table         */
    int nextPane;		/* Next pane number               */
    st_table *paneTable;	/* Hash table of (int) mapping    */
				/* pane names to pane indices     */
    int buildMenu;		/* Menu must be made if set       */
    xpa_menu tableMenu;		/* Associated menu of the table   */
    menuMap menuBindings[MAXPANES][MAXSELECT];
				/* Menu binding table             */
    int last_num;		/* Last depth of selection        */
    int *last_sel;		/* Last selection array           */
    int lastPane, lastSelect;	/* Last choice made on the menu   */
    int isSorted;		/* Non-zero if aliases sorted     */
    aliasMap **aliases;	/* Sorted array of alias pointers */
    int lastAlias;		/* Index one past last alias      */
} bd_table;    


/* Keyword table (REMOTE and INCLUDE are treated specially) */

#define MAXKEYWORDS 6
static char *keywords[] = {
    "KEY", "KEYS", "MENU", "MENUS", "ALIAS", "ALIASES"
  };



vemStatus bdRegFunc(id, dispName, helpStr, func)
STR id;				/* Unique string identifier */
STR dispName;			/* Standard display name    */
STR helpStr;			/* One line help message    */
vemStatus (*func)();		/* Function pointer         */
/*
 * This routine registers a new tightly bound function.  If
 * the function returns normally,  'id' may be used to
 * establish keyboard, menu, and alias bindings for the function.
 * If the 'id' is not unique,  it will return VEM_NOTUNIQ and
 * the function will not be registered.  'dispName' is how
 * the command will appear on a menu and in the console window.
 */
{
    bdBinding *newBinding;

    if (nameTable == (st_table *) 0) {
	/* Create table */
	nameTable = st_init_table(STRCOMP, st_strhash);
    }
    if (st_lookup(nameTable, id, (Pointer *) &newBinding) == 1) {
	return VEM_NOTUNIQ;
    }
    newBinding = VEMALLOC(bdBinding);
    newBinding->id = VEMSTRCOPY(id);
    newBinding->dispName = VEMSTRCOPY(dispName);
    newBinding->bindType = TIGHT_COMMAND;
    newBinding->bindValue.assocFunc = func;
    newBinding->help_str = VEMSTRCOPY(helpStr);
    st_insert(nameTable, newBinding->id, (Pointer) newBinding);
    return VEM_OK;
}

vemStatus bdRegApp(id, dispName, hostname, pathname)
STR id;			/* Unique string identifier    */
STR dispName;		/* Standard display name       */
STR hostname;		/* Host of application         */
STR pathname;		/* Absolute path to executable */
/*
 * This routine registers a remote application.  The hostname
 * and pathname are assumed to be correct (no checking is done).
 * After successful completion,  the 'id' may be used normally
 * to establish bindings for the application.  'dispName' is how
 * the command will appear on a menu and in the console window.
 */
{
    bdBinding *newBinding;

    if (nameTable == (st_table *) 0) {
	/* Create table */
	nameTable = st_init_table(STRCOMP, st_strhash);
    }
    if (st_lookup(nameTable, id, (Pointer *) &newBinding) == 1) {
	return VEM_NOTUNIQ;
    }
    newBinding = VEMALLOC(bdBinding);
    newBinding->id = VEMSTRCOPY(id);
    newBinding->dispName = VEMSTRCOPY(dispName);
    newBinding->bindType = INVOKE_REMOTE;
    newBinding->bindValue.theApp.hostName = VEMSTRCOPY(hostname);
    newBinding->bindValue.theApp.pathName = VEMSTRCOPY(pathname);
    newBinding->help_str = VEMSTRCOPY("Remote application");
    st_insert(nameTable, newBinding->id, (Pointer) newBinding);
    return VEM_OK;
}


vemStatus bdRegAppFunc(id, dispName, appNumber, appIndex)
STR id;			/* Unique String identifier */
STR dispName;		/* Standard display name    */
int appNumber;		/* Application number       */
int appIndex;		/* Command index            */
/*
 * This routine registers a foreign application command.  The
 * application number and index are provided by the RPC library.
 * Once defined,  the 'id' may be used to establish menu and
 * key bindings. 'dispName' is how the command will appear on a 
 * menu and in the console window.
 */
{
    bdBinding *newBinding;

    if (nameTable == (st_table *) 0) {
	/* Create table */
	nameTable = st_init_table(STRCOMP, st_strhash);
    }
    if (st_lookup(nameTable, id, (Pointer *) &newBinding) == 1) {
	return VEM_NOTUNIQ;
    }
    newBinding = VEMALLOC(bdBinding);
    newBinding->id = VEMSTRCOPY(id);
    newBinding->dispName = VEMSTRCOPY(dispName);
    newBinding->bindType = REMOTE_COMMAND;
    newBinding->bindValue.theFun.appNum = appNumber;
    newBinding->bindValue.theFun.appIndex = appIndex;
    newBinding->help_str = VEMSTRCOPY("Remote command");
    st_insert(nameTable, newBinding->id, (Pointer) newBinding);
    return VEM_OK;
}



vemStatus bdCreate(tableName, theTable)
STR tableName;		/* Name of bindings table        */
bdTable *theTable;	/* New bindings table (returned) */
/*
 * This routine allocates a new bindings table and returns a handle
 * which can be used to access the table.  The table can be
 * destroyed using bdDelete.  Binding table names must be unique.
 */
{
    bd_table *newTable;
    int row, col;

    /* Initialize the main table (if needed) */
    if (allTables == (st_table *) 0) {
	int index;
	char id[100], dispName[100], helpStr[1024];
	vemStatus (*func)();

	allTables = st_init_table(STRCOMP, st_strhash);
	/* Registers all tightly bound commands */
	for (index = 0;  index < vemNumFuncs();  index++) {
	    vemGetFunc(index, id, dispName, helpStr, &func);
	    bdRegFunc(id, dispName, helpStr, func);
	}
    }

    if (st_is_member(allTables, tableName)) {
	return VEM_DUPLICATE;
    } else {
	/* Initialize the new table entry */
	newTable = VEMALLOC(bd_table);
	newTable->tableName = VEMSTRCOPY(tableName);
	newTable->keyBindings = VEMARRAYALLOC(bdBinding *, MAXKEYS);
        for (row = 0;  row < MAXKEYS;  row++)
	  newTable->keyBindings[row] = (bdBinding *) 0;
	newTable->nextPane = 0;
	newTable->paneTable = st_init_table(STRCOMP, st_strhash);
	newTable->buildMenu = 1;
	newTable->tableMenu = (xpa_menu) 0;
	for (row = 0;  row < MAXPANES;  row++) {
	    for (col = 0;   col < MAXSELECT;  col++) {
		newTable->menuBindings[row][col].menuFunc = (bdBinding *) 0;
		newTable->menuBindings[row][col].paneName = (STR) 0;
	    }
	}
	newTable->last_num = 0;
	newTable->last_sel = VEMARRAYALLOC(int, MAXMENUDEPTH);
	newTable->isSorted = 0;
	newTable->aliases = VEMARRAYALLOC(aliasMap *, MAXALIASES);
	for (row = 0;  row < MAXALIASES;  row++)
	  newTable->aliases[row] = (aliasMap *) 0;
	newTable->lastAlias = 0;

	/* Put the table in the "table of tables" */
	st_insert(allTables, newTable->tableName, (Pointer) newTable);
	*theTable = (bdTable) newTable;
	return VEM_OK;
    }
}


static enum st_retval copyPaneEntry(key, value, arg)
char *key;			/* Key entry          */
char *value;			/* Value pointer      */
char *arg;			/* Arbitrary argument */
/*
 * This function copies a pane entry in a hash table and
 * inserts it in another hash table.  The other hash table
 * is passed as 'arg'.
 */
{
    int *realValue = (int *) value, *newValue;
    st_table *otherTable = (st_table *) arg;

    /* Make a new copy of the value */
    newValue = VEMALLOC(int);
    *newValue = *realValue;
    /* Insert the entry in the other table */
    st_insert(otherTable, key, (Pointer) newValue);
    return ST_CONTINUE;
}


vemStatus bdCopy(oldTable, newTable)
bdTable oldTable;		/* Previously defined table */
bdTable newTable;		/* New table (modified)     */
/*
 * This routine copies the contents of a previously created
 * binding table into another binding table.  The destination
 * table should be a newly created table which is otherwise
 * empty.
 */
{
    bd_table *realOldTable = (bd_table *) oldTable;
    bd_table *theTable = (bd_table *) newTable;
    int row, col;

    /* Copy old into the new */
    for (row = 0;  row < MAXKEYS;  row++)
      (theTable->keyBindings)[row] = (realOldTable->keyBindings)[row];
    theTable->nextPane = realOldTable->nextPane;
    /* Copy the pane table */
    st_foreach(realOldTable->paneTable, copyPaneEntry,
	       (Pointer) theTable->paneTable);
    theTable->buildMenu = 1;
    theTable->tableMenu = (xpa_menu) 0;
    for (row = 0;  row < MAXPANES;  row++) {
	for (col = 0;   col < MAXSELECT;  col++) {
	    theTable->menuBindings[row][col].paneName =
	      VEMSTRCOPY(realOldTable->menuBindings[row][col].paneName);
	    theTable->menuBindings[row][col].menuFunc =
	      realOldTable->menuBindings[row][col].menuFunc;
	}
    }
    theTable->last_num = realOldTable->last_num;
    theTable->last_sel = VEMARRAYALLOC(int, MAXMENUDEPTH);
    for (col = 0;  col < theTable->last_num;  col++) {
	theTable->last_sel[col] = realOldTable->last_sel[col];
    }
    theTable->isSorted = realOldTable->isSorted;
    for (row = 0;  row < MAXALIASES;  row++) {
	if (realOldTable->aliases[row]) {
	    theTable->aliases[row] = VEMALLOC(aliasMap);
	    theTable->aliases[row]->aliasName =
	      VEMSTRCOPY(realOldTable->aliases[row]->aliasName);
	    theTable->aliases[row]->aliasFunc =
	      realOldTable->aliases[row]->aliasFunc;
	}
    }
    theTable->lastAlias = realOldTable->lastAlias;
    return VEM_OK;
}




vemStatus bdInit(fileName)
STR fileName;		/* Bindings file name */
/*
 * This routine reads all of the bindings in the given file
 * and creates (or adds to) binding tables for them.  These 
 * tables can be retrieved using bdGetTable.  The routine
 * returns VEM_OK if there were no errors,  VEM_NOFILE if
 * the file could not be opened,   and VEM_FALSE if there
 * were other non-fatal errors.
 */
{
    FILE *bindFile;
    int linecount, index, errorFlag = 0;
    /* Space for input */
    char field[MAXFIELDSIZE], newfile[MAXLINE];
    char *ptrIndex;
    /* Basic fields */
    char tblName[MAXFIELDSIZE], uniqueId[MAXFIELDSIZE];
    char buf[VEMMAXSTR*4], buf2[VEMMAXSTR*4];
    bdTable theTable;

    bindFile = fopen( util_file_expand(fileName), "r");
    if (bindFile == NULL) {
	sprintf(errMsgArea, "Can't open bindings file '%s'\n", fileName);
	vemMessage(errMsgArea, MSG_DISP);
	return VEM_NOFILE;
    } else {
	sprintf(errMsgArea, "Reading bindings from file '%s':\n", fileName);
	vemMessage(errMsgArea, 0);
    }

    /* Fill the tables */
    linecount = 1;
    while (getline(bindFile, Line, &linecount) != EOF) {  /* Line is global */
	ptrIndex = Line;
	if (!getword(&ptrIndex, field)) {
	    sprintf(errMsgArea, "  Missing first field on line %d\n",
		    linecount);
	    vemMessage(errMsgArea, 0);
	    errorFlag = 1;
	    continue;
	}
	if (STRCOMP(field, "REMOTE") == 0) {
	    /* It is a remote declaration */
	    if (!doRemote(ptrIndex, linecount)) {
		errorFlag = 1;
	    }
	} else if (STRCOMP(field, "INCLUDE") == 0) {
	    /* Its an include directive */
	    if (!getword(&ptrIndex, field)) {
		sprintf(errMsgArea, "  Missing file name on line %d\n",
			linecount);
		vemMessage(errMsgArea, 0);
		errorFlag = 1;
		continue;
	    }
	    STRMOVE(newfile, util_file_expand(field));
	    /* Recursively read in the defintions */
	    if (bdInit(newfile) != VEM_OK) {
		sprintf(errMsgArea, "There were errors in file %s\n",
			field);
		vemMessage(errMsgArea, 0);
	    } else {
		sprintf(errMsgArea, "Finished reading file %s returning to %s\n",
			field, fileName);
		vemMessage(errMsgArea, 0);
	    }
	} else if (STRCOMP(field, "INCLUDESTD") == 0) {
	    char *stdbind;

	    /* Include the standard bindings file */
	    stdbind = BDS("/lib/vem/vem.bindings.");
	    stdbind = util_file_expand(stdbind);
	    if (bdInit(stdbind) != VEM_OK) {
		sprintf(errMsgArea, "There were errors in file %s\n", stdbind);
		vemMessage(errMsgArea, 0);
	    } else {
		sprintf(errMsgArea, "Finished reading file %s returning to %s\n",
			stdbind, fileName);
		vemMessage(errMsgArea, 0);
	    }
	} else {
	    /* It is a normal line - first item is tblName */
	    STRMOVE(tblName, field);
	    if (bdGetTable(tblName, &theTable) != VEM_OK) {
		/* New table is required */
		if (bdCreate(tblName, &theTable) != VEM_OK) {
		    sprintf(errMsgArea,
			    "  Unable to create new table '%s' on line %d\n",
			    tblName, linecount);
		    vemMessage(errMsgArea, MSG_DISP);
		    errorFlag = 1;
		    continue;
		}
	    }
	    /* Second item is unique id or COPY */
	    if (!getword(&ptrIndex, field)) {
		sprintf(errMsgArea, "  Missing id or COPY on line %d\n",
			linecount);
		vemMessage(errMsgArea, 0);
		errorFlag = 1;
		continue;
	    }
	    if (STRCOMP(field, "COPY") == 0) {
		/* It is a copy directive */
		if (!doCopy(theTable, ptrIndex, linecount))
		  errorFlag = 1;
	    } else {
		/* Its a normal line - field is id */
		STRMOVE(uniqueId, field);
		/* Process all keyword directives */
		while (getword(&ptrIndex, field)) {
		    /* This field should be a keyword */
		    for (index = 0;  index < MAXKEYWORDS;  index++) {
			if (STRCOMP(field, keywords[index]) == 0) break;
		    }
		    if (index >= MAXKEYWORDS) {
			sprintf(errMsgArea,
				"  Unknown keyword '%s' on line %d\n",
				field, linecount);
			vemMessage(errMsgArea, 0);
			errorFlag = 1;
			break;	/* Jumps out to next line */
		    }
		    switch (index) {
		    case 0:
		    case 1:
			/* It is a key definition */
			if (!doKey(theTable, uniqueId, &ptrIndex, linecount))
			  errorFlag = 1;
			break;
		    case 2:
		    case 3:
			/* It is a menu definition */
			if (!doBdMenu(theTable, uniqueId, &ptrIndex, linecount))
			  errorFlag = 1;
			break;
		    case 4:
		    case 5:
			/* It is an alias definition */
			if (!doAlias(theTable, uniqueId, &ptrIndex,linecount))
			  errorFlag = 1;
			break;
		    }
		}
	    }
	}
    }
    if (errorFlag) return VEM_FALSE;
    else {
	vemMessage("  No errors were found\n", 0);
	return VEM_OK;
    }
}



static int getline(file, area, lc)
FILE *file;			/* Stream opened for read */
char *area;			/* Line to put text into  */
int *lc;			/* Current line count     */
/*
 * This routine reads from the given file until a '\n' is found.
 * The resulting string is copied into 'area'.  Returns 0
 * if everything is fine,  EOF if it has reached the end of file.
 */
{
    int ch, count;

    if ((ch = getc(file)) == EOF) return EOF;
    else area[0] = ch;
    for (count = 1; count < MAXLINE; count++) {
	ch = getc(file);
	if ((ch == '\n') || (ch == EOF)) break;
	area[count] = ch;
    }
    area[count] = '\0';
    if (area[0] == COMCHAR) {
	return getline(file, area, lc);
    } else {
	*lc += 1;
	return 0;
    }
}

static int getword(str, word)
char **str;			/* Pointer to string pointer (modified) */
char *word;			/* Returned word                        */
/*
 * This routine extracts a word from 'str' and copies it into 'word'.
 * 'str' is modified so that the next call will return the next word.
 * If there are no more words,  the routine returns 0.  'word'
 * must be preallocated.
 */
{
    char *index;

    index = *str;
    while ((*index == ' ') || (*index == '\t')) {
	/* Skip over white space */
	index++;
    }
    if (*index == '\0') return 0;
    while ((*index != ' ') && (*index != '\t') && (*index != '\0')) {
	*word = *index;
	word++;  index++;
    }
    *word = '\0';
    *str = index;
    return 1;
}

static int pushword(str)
char **str;			/* Pointer to string pointer (modified) */
/*
 * This routine backs up the passed pointer one word.  'str' is
 * modified so that the next call to getword will return the word
 * previously read by getword.  Warning: this does not detect the
 * start of the string!
 */
{
    char *index;

    index = *str;
    while ((*index == ' ') || (*index == '\t')) {
	/* Skip over white space */
	index--;
    }
    while ((*index != ' ') && (*index != '\t')) {
	/* Skip over word */
	index--;
    }
    *str = index;
    return 1;
}



static int doRemote(line, linecount)
STR line;			/* Line identified to be a REMOTE decl */
int linecount;			/* Line number of declaration          */
/*
 * This routine interprets a REMOTE declaration line in a binding
 * file.  It assumes the pointer points past the REMOTE keyword.
 * The routine returns a non-zero value if the line has
 * no errors and zero if it does.  Error messages are logged to
 * the logging stream.
 */
{
    char id[MAXFIELDSIZE], host[MAXFIELDSIZE], path[MAXFIELDSIZE];
    int len;

    len = sscanf(line, "%s %s %s", id, host, path);
    if (len != 3) {
	sprintf(errMsgArea, "  Error in REMOTE declaration on line %d\n",
		linecount);
	vemMessage(errMsgArea, 0);
	return 0;
    }
    if (bdRegApp(id, id, host, path) != VEM_OK) {
	/* Must not be unique */
	sprintf(errMsgArea,
		"  String id for REMOTE declaration on line %d is not unique\n",
		linecount);
	vemMessage(errMsgArea, 0);
	return 0;
    } else return 1;
}





static int decodeKey(keyStr, outKey)
STR keyStr;			/* Key string         */
char *outKey;			/* Returned character */
/*
 * Reads a string which represents a character and decodes it into
 * a regular character.  Representation string may use a literal
 * character,  hat notation (i.e. "^T"),  or standard C
 * octal notation (i.e. "\010").  Returns zero if the character
 * representation is bad.
 */
{
    int temp;

    /* Decode the character */
    if (keyStr[0] == '^') {
	/* WARNING: character set dependency */
	if ((keyStr[1] < ' ') && (keyStr[1] > '_')) {
	    /* Corrupt character */
	    return 0;
	} else *outKey = keyStr[1] - '@';
    } else if (keyStr[0] == '\\') {
	if (sscanf(keyStr, "\\%o", &temp) != 1) {
	    /* Corrupt character */
	    return 0;
	} else *outKey = (int) temp;
    } else if (STRLEN(keyStr) > 1) {
	/* Corrupt character */
	return 0;
    } else *outKey = keyStr[0];
    return 1;
}


static int doKey(theTable, id, ptrIndex, linecount)
bdTable theTable;		/* Table for key binding                  */
STR id;				/* Unique id for binding                  */
char **ptrIndex;		/* Line pointer after KEY                 */
int linecount;			/* The current line count                 */
/*
 * This line parses KEY entries in a binding file (the format is described
 * at the top of this file).  If everything goes well,  the routine returns
 * a non-zero value.  If it failed,  zero is returned and an error message
 * is logged to the logging stream.
 */
{
    char ch[MAXFIELDSIZE], targetCh;
    int count, errors, index;

    count = errors = 0;
    /* Enter loop processing key definitions */
    while (getword(ptrIndex, ch)) {
	count++;
	/* Is it another keyword? */
	for (index = 0;  index < MAXKEYWORDS;  index++) {
	    if (STRCOMP(ch,  keywords[index]) == 0) break;
	}
	if (index < MAXKEYWORDS) {
	    /* Done with this batch of key definitions */
	    pushword(ptrIndex);
	    return 1;
	}
	/* Must be a character definition */
	if (!decodeKey(ch, &targetCh)) {
	    sprintf(errMsgArea,
	"  Character specification %d of KEY definition on line %d is corrupt\n",
		    count, linecount);
	    vemMessage(errMsgArea, 0);
	} else {
	    switch (bdBindKey(theTable, id, targetCh)) {
	    case VEM_OK:
		/* Everything is fine */
		break;
	    case VEM_CANTFIND:
		/* Id can't be found */
		sprintf(errMsgArea, "  KEY definition on line %d:\n",
			linecount);
		vemMessage(errMsgArea, 0);
		sprintf(errMsgArea,
			"    There is no function registered for '%s'\n",
			id);
		vemMessage(errMsgArea, 0);
		/* Fatal error */
		return 0;
	    case VEM_NOTUNIQ:
		/* Already a binding for the key */
		sprintf(errMsgArea, "  KEY definition on line %d:\n",
			linecount);
		vemMessage(errMsgArea, 0);
		sprintf(errMsgArea,
			"    Previous definition for '%s' overridden\n",
			ch);
		vemMessage(errMsgArea, 0);
		/* Non-fatal - but mark as error */
		errors++;
		break;
	    }
	}
    }
    if (errors) return 0;
    else return 1;
}



static int doBdMenu(theTable, id, ptrIndex, linecount)
bdTable theTable;		/* Table for the menu entry                */
STR id;				/* Unique id for command                   */
char **ptrIndex;		/* Pointer into line positioned after MENU */
int linecount;			/* The current line count                  */
/*
 * This line parses MENU entries in a binding file (the format is described
 * at the top of this file).  If everything goes well,  the routine returns
 * a non-zero value.  If it failed,  zero is returned and an error message
 * is logged to the logging stream.
 */
{
    char pane[MAXFIELDSIZE], selectStr[MAXFIELDSIZE];
    int count, errors, selectNum, index;

    count = errors = 0;
    /* Enter loop processing menu definitions */
    for (;;) {
	count++;
	if (!getword(ptrIndex, pane)) break;
	/* Is it another keyword? */
	for (index = 0;  index < MAXKEYWORDS;  index++) {
	    if (STRCOMP(pane, keywords[index]) == 0) break;
	}
	if (index < MAXKEYWORDS) {
	    /* Done with this batch of menu definitions */
	    pushword(ptrIndex);
	    return 1;
	}
	if ((!getword(ptrIndex, selectStr)) ||
	    (sscanf(selectStr, "%d", &selectNum) != 1))
	  {
	      sprintf(errMsgArea, "  MENU definition on line %d:\n", linecount);
	      vemMessage(errMsgArea, 0);
	      sprintf(errMsgArea,
		      "    Specification %d:  missing or corrupt selection\n",
		      count);
	      vemMessage(errMsgArea, 0);
	      continue;
	  }
		
	/* Got all the necessary stuff */
	switch (bdBindMenu(theTable, id, pane, selectNum)) {
	case VEM_OK:
	    /* Everything is fine */
	    break;
	case VEM_CANTFIND:
	    /* Id can't be found */
	    sprintf(errMsgArea, "  MENU definition on line %d:\n",
		    linecount);
	    vemMessage(errMsgArea, 0);
	    sprintf(errMsgArea,
		    "    There is no function registered for '%s'\n",
		    id);
	    vemMessage(errMsgArea, 0);
	    /* Fatal error */
	    return 0;
	case VEM_NOTUNIQ:
	    /* Already a binding for this entry in menu */
	    sprintf(errMsgArea, "  MENU definition on line %d:\n",
		    linecount);
	    vemMessage(errMsgArea, 0);
	    sprintf(errMsgArea,
		    "    Previous definition for specification %d overridden\n",
		    count);
	    vemMessage(errMsgArea, 0);
	    /* Non-fatal - but mark as error */
	    errors++;
	    break;
	}
    }
    if (errors) return 0;
    else return 1;
}




static int doAlias(theTable, id, ptrIndex, linecount)
bdTable theTable;		/* Table for the alias binding              */
STR id;				/* Unique identifier for command            */
char **ptrIndex;		/* Pointer into line positioned after ALIAS */
int linecount;			/* The current line count                   */
/*
 * This line parses ALIAS entries in a binding file (the format is described
 * at the top of this file).  If everything goes well,  the routine returns
 * a non-zero value.  If it failed,  zero is returned and an error message
 * is logged to the logging stream.
 */
{
    char name[MAXFIELDSIZE];
    int errors, count, index;

    count = errors = 0;
    /* Enter loop processing alias definitions */
    while (getword(ptrIndex, name)) {
	count++;
	/* Is it another keyword? */
	for (index = 0;  index < MAXKEYWORDS;  index++) {
	    if (STRCOMP(name,  keywords[index]) == 0) break;
	}
	if (index < MAXKEYWORDS) {
	    /* Done with this batch of alias definitions */
	    pushword(ptrIndex);
	    return 1;
	}
	/* Got one - try to bind it as an alias */
	switch (bdBindAlias(theTable, id, name)) {
	case VEM_OK:
	    /* Everything is fine */
	    break;
	case VEM_CANTFIND:
	    /* Id can't be found */
	    sprintf(errMsgArea, "  ALIAS definition on line %d:\n",
		    linecount);
	    vemMessage(errMsgArea, 0);
	    sprintf(errMsgArea,
		    "    There is no function registered for '%s\n",
		    id);
	    vemMessage(errMsgArea, 0);
	    /* Fatal error */
	    return 0;
	case VEM_NOTUNIQ:
	    /* Already a binding for the key */
	    sprintf(errMsgArea, "  ALIAS definition on line %d:\n",
		    linecount);
	    vemMessage(errMsgArea, 0);
	    sprintf(errMsgArea,
		    "    Previous definition for '%s' overridden\n",
		    name);
	    vemMessage(errMsgArea, 0);
	    /* Non-fatal - but mark as error */
	    errors++;
	    break;
	}
    }
    if (count == 0) {
	/* No alias definitions - make one width display name */
	bdBindAlias(theTable, id, "");
    }
    if (errors) return 0;
    else return 1;
}


static int doCopy(theTable, ptrIndex, linecount)
bdTable theTable;		/* Table to copy into                   */
char *ptrIndex;			/* Pointer into current line after COPY */
int linecount;			/* The current line number              */
/*
 * This routine parses COPY entries in a binding file (the format is
 * described at the top of this file).  Returns a non-zero value if
 * everything went well.  Note the contents of the destination table 
 * up to this point is lost.
 */
{
    bdTable oldTable;
    char tblName[MAXFIELDSIZE];

    if (!getword(&ptrIndex, tblName)) {
	sprintf(errMsgArea, "  COPY definition on line %d has no source table\n",
		linecount);
	vemMessage(errMsgArea, 0);
	return 0;
    }
    if (bdGetTable(tblName, &oldTable) != VEM_OK) {
	sprintf(errMsgArea, "  COPY definition on line %d:\n", linecount);
	vemMessage(errMsgArea, 0);
	sprintf(errMsgArea, "    No table named '%s' has been defined\n",
		tblName);
	vemMessage(errMsgArea, 0);
	return 0;
    }
    /* Ready to do the copy */
    bdCopy(oldTable, theTable);
    return 1;
}


vemStatus bdGetTable(tableName, theTable)
STR tableName;			/* Name of bindings table   */
bdTable *theTable;		/* Binding table (returned) */
/*
 * This routine gets a bindings table given its name.  It returns
 * VEM_CANTFIND if there is no table with the given name.
 */
{
    /* Initialize the main table (if needed) */
    if (allTables == (st_table *) 0) {
	int index;
	char id[100], dispName[100], helpStr[1024];
	vemStatus (*func)();

	allTables = st_init_table(STRCOMP, st_strhash);
	/* Registers all tightly bound commands */
	for (index = 0;  index < vemNumFuncs();  index++) {
	    vemGetFunc(index, id, dispName, helpStr, &func);
	    bdRegFunc(id, dispName, helpStr, func);
	}
    }

    if (st_lookup(allTables, tableName, (Pointer *) theTable) == 0) {
	*theTable = (bdTable) 0;
	return VEM_CANTFIND;
    } else {
	return VEM_OK;
    }
}



vemStatus bdBindKey(theTable, id, commandKey)
bdTable theTable;		/* Binding table         */
STR id;				/* Unique id for command */
int commandKey;		/* Keyboard character    */
/*
 * Adds a new key binding to the specified binding table.  Returns
 * VEM_NOTUNIQ if the key has already been defined elsewhere.  In
 * this case,  the new binding overrides the old.
 */
{
    bd_table *realTable = (bd_table *) theTable;
    bdBinding *theBinding;
    vemStatus retCode;

    /* Look up the unique string identifier to find binding */
    if (st_lookup(nameTable, id, (Pointer *) &theBinding) == 0) {
	return VEM_CANTFIND;
    }
    /* Is there already a binding? */
    if (realTable->keyBindings[commandKey] != (bdBinding *) 0)
      retCode = VEM_NOTUNIQ;
    else
      retCode = VEM_OK;
    realTable->keyBindings[commandKey] = theBinding;
    return retCode;
}

vemStatus bdBindMenu(theTable, id, paneName, select)
bdTable theTable;	/* Binding table         */
STR id;			/* Unique id for command */
STR paneName;		/* Pane of selection     */
int select;		/* Selection index       */
/*
 * Adds a new menu binding to the specified binding table.
 * 'name' will appear on pane 'paneName' as the 'select'
 * entry on the pane.  Pane names are defined in the order
 * they are seen.  The routine will return VEM_NOTUNIQ
 * if the menu binding is already in use.  However,  the
 * new binding will override the old one.
 */
{
    bd_table *realTable = (bd_table *) theTable;
    STR copyStr;
    bdBinding *theBinding;
    int *paneNum;
    vemStatus retCode;

    /* Look up the unique string identifier to find binding */
    if (st_lookup(nameTable, id, (Pointer *) &theBinding) == 0) {
	return VEM_CANTFIND;
    }
    /* Resolve pane name to pane number */
    if (st_lookup(realTable->paneTable, paneName,
		  (Pointer *) &paneNum) == 0)
      {
	  /* Didn't find it.  Create a new one */
	  paneNum = VEMALLOC(int);
	  *paneNum = realTable->nextPane;
	  realTable->nextPane += 1;
	  copyStr = VEMSTRCOPY(paneName);
	  st_insert(realTable->paneTable, copyStr, (Pointer) paneNum);
      } 
    /* Is there already a binding here? */
    if (realTable->menuBindings[*paneNum][select].menuFunc)
      retCode = VEM_NOTUNIQ;
    else
      retCode = VEM_OK;
    /* Put in the binding */
    realTable->menuBindings[*paneNum][select].paneName = VEMSTRCOPY(paneName);
    realTable->menuBindings[*paneNum][select].menuFunc = theBinding;
    return retCode;
}

vemStatus bdBindAlias(theTable, id, name)
bdTable theTable;	/* Binding table        */
STR id;			/* Unique id of command */
STR name;		/* Alias name           */
/*
 * This command adds a new alias binding to the specified
 * bindings table.  The name must be unique within this
 * table.  Note aliases have a built-in mechanism for
 * command completion.  Choice of alias names should take
 * this into account.  If a name is not unique,  the routine
 * will return VEM_NOTUNIQ and the new binding will override
 * the old one.  If 'name' is empty,  the display name
 * for the binding is used.
 */
{
    bd_table *realTable = (bd_table *) theTable;
    bdBinding *theBinding;
    int index;
    
    /* Look up the unique string identifier to find binding */
    if (st_lookup(nameTable, id, (Pointer *) &theBinding) == 0) {
	return VEM_CANTFIND;
    }
    /* Check for empty name */
    if (STRLEN(name) == 0) {
	name = theBinding->dispName;
    }
    /* Is there already a binding here? */
    realTable->isSorted = 0;
    for (index = 0;  index < realTable->lastAlias;  index++)
      if (STRCOMP(realTable->aliases[index]->aliasName, name) == 0) {
	  /* Previous binding here - replace it and break */
	  realTable->aliases[index]->aliasFunc = theBinding;
	  break;
      }

    if (index >= realTable->lastAlias) {
	/* Didn't find a previous one.  Put in a new one. */
	realTable->aliases[realTable->lastAlias] = VEMALLOC(aliasMap);
	realTable->aliases[realTable->lastAlias]->aliasName = VEMSTRCOPY(name);
	realTable->aliases[realTable->lastAlias]->aliasFunc = theBinding;
	realTable->lastAlias += 1;
	return VEM_OK;
    } else return VEM_NOTUNIQ;
}





vemStatus bdKey(theTable, keyChar, binding)
bdTable theTable;		/* Binding table            */
int keyChar;			/* Keyboard character       */
bdBinding *binding;		/* Command binding (return) */
/*
 * Returns the keyboard binding for the specified key in the
 * specified binding table.  If there is no binding for the
 * key,  the routine will return VEM_CANTFIND.
 */
{
    bd_table *realTable = (bd_table *) theTable;
    unsigned char key = (unsigned char) keyChar;

    if ((realTable != NIL(bd_table)) &&
	(realTable->keyBindings[key] != NIL(bdBinding)))
      {
	  *binding = *realTable->keyBindings[key];
	  binding->bd_tbl = theTable;
	  return VEM_OK;
      }
    else return VEM_CANTFIND;
}




int aliasCmp(p1, p2)
aliasMap **p1, **p2;	/* Pointers to two alias maps */
/*
 * This routine is passed to qsort(3) for sorting the alias
 * records.  It compares the two alias names using STRCOMP.
 */
{
    return STRCOMP((*p1)->aliasName, (*p2)->aliasName);
}


vemStatus bdName(Tbl1, Tbl2, commandName, number, completion, binding)
bdTable Tbl1, Tbl2;		/* Binding tables                 */
STR commandName;		/* Name of a command 		  */
int *number;			/* Number of aliases which match  */
STR completion;			/* Completion string              */
bdBinding *binding;		/* alias binding (return)         */
/*
 * Returns the command name binding for the specified command in
 * the specified binding tables.  The second table may be empty.
 * The name may be incomplete.  If so,
 * 'number' indicates how many aliases it matches.  If it matches
 * more than one,  'completion' will contain a list of the ones
 * it matches.  If it matches exactly one,  the binding will be
 * returned in 'binding'.  If there is no binding for the
 * command,  the routine returns VEM_CANTFIND.  Additional characters
 * will be appended to 'commandName' if the possible completions
 * for the command are the same to a certain point.
 */
{
    bd_table *realTable = (bd_table *) Tbl1;
    bd_table *altTable = (bd_table *) Tbl2;
    char maxComp[100];	      /* Maximum completion of possible completions */
    int index, length, maxCompLen;

    if (!realTable->isSorted) {
	/* Sort the table using qsort */
	qsort((char *) realTable->aliases, realTable->lastAlias,
	      sizeof(aliasMap *), aliasCmp);
	realTable->isSorted = 1;
    }

    /* There should be a check here to sort the alias array if necessary */

    *number = 0;
    length = STRLEN(commandName);
    completion[0] = '\0';
    maxComp[0] = '\0';
    maxCompLen = -1;
    binding->bindType = 0;
    /* Check the main table */
    for (index = 0;  index < realTable->lastAlias;  index++) {
	if (STRNCOMP(commandName, realTable->aliases[index]->aliasName,
		    length) == 0)
	  {
	      /* We found one that meets the criteria */
	      (*number)++;
	      STRCONCAT(completion, realTable->aliases[index]->aliasName);
	      STRCONCAT(completion, " ");
	      if (STRLEN(realTable->aliases[index]->aliasName) == length) {
		  *binding = *(realTable->aliases[index]->aliasFunc);
		  binding->bd_tbl = Tbl1;
	      }
	      /* Handle maximum completion */
	      if (maxCompLen == -1) {
		  /* First word that matches */
		  STRMOVE(maxComp, realTable->aliases[index]->aliasName);
		  maxCompLen = STRLEN(maxComp);
	      } else {
		  /* Find first character where the current and max diverge */
		  /* Note strdiff is my own function - not part of Clib     */
		  maxCompLen = strdiff(maxComp,
				       realTable->aliases[index]->aliasName);
		  maxComp[maxCompLen] = '\0';
	      }
	  }
    }
    /* Check the alternate table (if it exists) */
    if (altTable) {
	if (!altTable->isSorted) {
	    /* Sort the table using qsort */
	    qsort((char *) altTable->aliases, altTable->lastAlias,
		  sizeof(aliasMap *), aliasCmp);
	    altTable->isSorted = 1;
	}

	for (index = 0;  index < altTable->lastAlias;  index++) {
	    if (STRNCOMP(commandName, altTable->aliases[index]->aliasName,
			length) == 0)
	      {
		  /* We found one that meets the criteria */
		  (*number)++;
		  STRCONCAT(completion, altTable->aliases[index]->aliasName);
		  STRCONCAT(completion, " ");
		  if (STRLEN(altTable->aliases[index]->aliasName) == length) {
		      *binding = *(altTable->aliases[index]->aliasFunc);
		      binding->bd_tbl = Tbl2;
		  }
		  /* Handle maximum completion */
		  if (maxCompLen == -1) {
		      /* First word that matches */
		      STRMOVE(maxComp, altTable->aliases[index]->aliasName);
		      maxCompLen = STRLEN(maxComp);
		  } else {
		      /* Find first character where the current and max diverge */
		      /* Note strdiff is my own function - not part of Clib     */
		      maxCompLen = strdiff(maxComp,
					   altTable->aliases[index]->aliasName);
		      maxComp[maxCompLen] = '\0';
		  }
	      }
	}
    }
    /* Finished looking */
    if (*number == 0) return VEM_CANTFIND;
    if (*number == 1) {
	/* Put the complete string into 'commandName' */
	STRMOVE(commandName, maxComp);
	return VEM_OK;
    } else {
	/* 
	 * Multiple completions.  Copy maximum possible completion back
	 * into 'commandName'.  If one actually matches,  fill in
	 * the table.
	 */
	STRMOVE(commandName, maxComp);
	return VEM_NOTUNIQ;
    }
}

static int strdiff(s1, s2)
char *s1, *s2;
/* Returns the index of the first character which is different */
{
    int index;

    index = 0;
    while ((*s1) && (*s2) && (*s1 == *s2)) {
	s1++;  s2++;  index++;
    }
    return index;
}



vemStatus bdMenu(theTable, depth, select, commandName, binding)
bdTable theTable;		/* Binding table 		    */
int depth;			/* Depth of selection               */
int *select;			/* Array of selections              */
STR *commandName;		/* Name of the command (returned)   */
bdBinding *binding;		/* Binding for selection (returned) */
/*
 * This routine returns the binding for the specified depth
 * and selection array.  Initially,  this only supports the
 * two level menus used in versions of VEM before 7-0.  If there 
 * is no binding for the given depth and selection array, the routine 
 * returns VEM_CANTFIND.  'commandName' will contain the name of the 
 * command as it appears on the menu.
 */
{
    bd_table *realTable = (bd_table *) theTable;

    if ((depth == 2) && (realTable != NIL(bd_table)) &&
	(select[0] >= 0) && (select[1] >= 0) &&
	(realTable->menuBindings[select[0]][select[1]].menuFunc !=
	 NIL(bdBinding)))
      {
	  *commandName =
	    realTable->menuBindings[select[0]][select[1]].menuFunc->dispName;
	  *binding = *realTable->menuBindings[select[0]][select[1]].menuFunc;
	  binding->bd_tbl = theTable;
	  return VEM_OK;
      } else {
	  return VEM_CANTFIND;
      }
}



static enum st_retval findName(key, value, arg)
char *key;			/* Key of hash table */
char *value;			/* Value of entry    */
char *arg;			/* Argument passed   */
/*
 * This function is used when looking up the name of a pane
 * given its number.  The routine is passed to st_foreach
 * which will pass the key and value of all entries.  This
 * routine compares the value with 'arg' which is specified
 * when calling st_foreach.  After passing the function to
 * st_foreach,  call the routine with an 'arg' of -1 will
 * return the name by copying it into value.
 */
{
    static char *name;
    int *val = (int *) value;
    int *dest = (int *) arg;
    
    if (*dest == -1) {
	char **outName = (char **) value;

	*outName = name;
	return ST_CONTINUE;
    }
    if (*val == *dest) name = key;
    return ST_CONTINUE;
}

vemStatus bdGetMenu(disp, theTable, theMenu, last_num, last_sel)
Display *disp;			/* X Connection      */
bdTable theTable;		/* Binding table     */
xpa_menu *theMenu;		/* Returned menu     */
int **last_num;			/* Last number       */
int ***last_sel;		/* Last selection    */
/*
 * This routine returns the menu associated with the specified
 * binding table.  The returned menu can be used in calls
 * to xpa_post, xpa_unpost, and xpa_moded.  Since this routine 
 * tracks changes to the binding table,  it should always be called 
 * to get associated menus.   When a change occurs,  it may take 
 * a while to reconstruct the associated menu.  However,  as long 
 * as the menu stays the same,  this is a cheap call.  The 
 * `last_num' and `last_select' return the previous selection 
 * made in the menu and can be used to start the menu at that 
 * position if desired.  They should be passed directly to
 * the menu package so they will be updated.  The routine returns 
 * VEM_CORRUPT if there are problems.
 */
{
    bd_table *realTable = (bd_table *) theTable;
    XColor *spec;
    xpa_entry panes[MAXPANES+1];
    xpa_appearance attr;
    int all_keys[MAXKEYS];
    int paneIndex, topSelect, selIndex, temp, numkeys, amask;
    char *paneName;

    if (realTable == NIL(bd_table)) return VEM_CORRUPT;
    if (realTable->buildMenu) {
	/* Must reconstruct the menu */

	if (realTable->tableMenu != (xpa_menu) 0) {
	    xpa_destroy(realTable->tableMenu);
	    realTable->tableMenu = (xpa_menu) 0;
	}

	for (paneIndex = 0;  paneIndex < MAXPANES;  paneIndex++) {
	    /* Any selections on this pane? */
	    for (topSelect = MAXSELECT-1;  topSelect >= 0;  topSelect--) {
		if (realTable->menuBindings[paneIndex][topSelect].menuFunc != 0)
		  break;
	    }
	    if (topSelect >= 0) {
		/* Yes:  determine the name of the pane */
		st_foreach(realTable->paneTable, findName, (char *) &paneIndex);
		temp = -1;
		findName((char *) 0, (char *) &paneName, (char *) &temp);

		/* Assign information to pane */
		panes[paneIndex].item_name = paneName;
		panes[paneIndex].key_char = XPA_NOCHAR;
		panes[paneIndex].sub_title = (char *) 0;
		panes[paneIndex].sub_entrys =
		  VEMARRAYALLOC(xpa_entry, topSelect+2);

		/* Add menu entries */
		for (selIndex = 0;  selIndex <= topSelect;  selIndex++) {
		    /* Is it a non-empty entry? */
		    if (realTable->menuBindings[paneIndex][selIndex].menuFunc
			!= 0) {
			/* Yes: add it as an active entry */
			panes[paneIndex].sub_entrys[selIndex].item_name =
			  realTable->menuBindings[paneIndex][selIndex].
			    menuFunc->dispName;
			numkeys = bdLocKeys(realTable->keyBindings,
			  realTable->menuBindings[paneIndex][selIndex].menuFunc->id,
			  all_keys);
			if (numkeys) {
			    panes[paneIndex].sub_entrys[selIndex].key_char =
			      all_keys[0];
			} else {
			    panes[paneIndex].sub_entrys[selIndex].key_char =
			      XPA_NOCHAR;
			}
			panes[paneIndex].sub_entrys[selIndex].sub_title =
			  (char *) 0;
			panes[paneIndex].sub_entrys[selIndex].sub_entrys =
			  (xpa_entry *) 0;
		    } else {
			/* No: add it as an inactive entry       */
			/* xpa doesn't have inactive entries yet */
			panes[paneIndex].sub_entrys[selIndex].item_name = "";
			panes[paneIndex].sub_entrys[selIndex].key_char =
			  XPA_NOCHAR;
			panes[paneIndex].sub_entrys[selIndex].sub_title =
			  (char *) 0;
			panes[paneIndex].sub_entrys[selIndex].sub_entrys =
			  (xpa_entry *) 0;
		    }
		}
		/* Add null terminating selection */
		panes[paneIndex].sub_entrys[selIndex].item_name = (char *) 0;
	    } else {
		/* No more selections.  Abort out */
		break;
	    }
	}
	/* Add null terminating pane */
	panes[paneIndex].item_name = (char *) 0;

	/* Fill in attributes */
	amask = 0;
	dfGetFont("menu.title.font", &(attr.title_font)); amask |= XPA_T_FONT;
	dfGetFont("menu.item.font", &(attr.item_font)); amask |= XPA_I_FONT;
	dfGetFont("menu.key.font", &(attr.key_font)); amask |= XPA_K_FONT;
	dfGetPixel("menu.title.foreground", &spec); amask |= XPA_T_FG;
	attr.title_fg = spec->pixel;
	dfGetPixel("menu.title.background", &spec); amask |= XPA_T_BG;
	attr.title_bg = spec->pixel;
	dfGetPixel("menu.item.foreground", &spec); amask |= XPA_I_FG;
	attr.item_fg = spec->pixel;
	dfGetPixel("menu.item.background", &spec); amask |= XPA_I_BG;
	attr.item_bg = spec->pixel;
	dfGetPixel("menu.cursor.color", &spec); amask |= XPA_C_FG;
	attr.cur_fg = spec->pixel;

	/* Create menu - should do something about title */
	realTable->tableMenu = xpa_create(disp, realTable->tableName, panes, amask,
					  &attr);
	realTable->buildMenu = 0;
    }
    *theMenu = realTable->tableMenu;
    *last_num = &(realTable->last_num);
    *last_sel = &(realTable->last_sel);
    return VEM_OK;
}


vemStatus bdQuery(cmd, keys, panes, aliases)
bdBinding *cmd;			/* Command binding   */
STR *keys;			/* All key bindings  */
STR *panes;			/* All menu panes    */
STR *aliases;			/* All type-in names */
/*
 * This command takes a binding and produces all of its
 * key, menu, and alias bindings in a textual form suitable
 * for direct output.  These strings are owned by the routine
 * and should not be modified.  The routine assumes `cmd'
 * is a valid binding.
 */
{
    bd_table *realTable = (bd_table *) cmd->bd_tbl;

    *keys = bdFindKeys(realTable, cmd);
    *panes = bdFindMenu(realTable, cmd);
    *aliases = bdFindAlias(realTable, cmd);
    return VEM_OK;
}


static char key_str[MAXKEYS];

static int bdLocKeys(keyBindings, id, keys)
bdBinding **keyBindings;	/* All key bindings    */
STR id;				/* Unique command id   */
int keys[MAXKEYS];		/* Returned keys       */
/* 
 * Traces through the table and finds all keys with the
 * supplied unique id.  The number of keys found is returned.
 * The ASCII value of each key is filled into `keys'.
 */
{
    int count = 0;
    int idx;

    for (idx = 0;  idx < MAXKEYS;  idx++) {
	if (keyBindings[idx] && STRCOMP(keyBindings[idx]->id, id) == 0) {
	    keys[count++] = idx;
	}
    }
    return count;
}

static STR bdFindKeys(tbl, binding)
bd_table *tbl;		/* Real bindings table */
bdBinding *binding;		/* Command binding     */
/*
 * This command tries to find `binding' in the key bindings for
 * the table `tbl'.  It produces an output string of the form:
 * "`%c', `%c', ..." for all key bindings.  Control characters
 * are mapped to hat notation and those above 127 are mapped
 * to octal notation.  If no key bindings are found,  it
 * returns a string of length zero.  The string is owned by
 * this routine and should not be modified.
 */
{
    int keys[MAXKEYS];
    int key_str_len = 0;
    int idx, count;

    key_str[0] = '\0';
    count = bdLocKeys(tbl->keyBindings, binding->id, keys);
    for (idx = 0;  idx < count;  idx++) {
	append_key(keys[idx], &key_str_len, key_str);
    }
    return (STR) key_str;
}

static void append_key(ch, len, key_str)
int ch;				/* Character code           */
int *len;			/* Current length of string */
STR key_str;			/* String to append onto    */
/*
 * Appends a character specification onto the string `str'.  The
 * character will be enclosed in single quotes.  If it is a special
 * character,  it will be denoted that way.  If it is a control character,
 * it will be given in hat notation.  If it is beyond the ASCII range,
 * it will be given in octal.  The routine assumes the ASCII character
 * set.  If there isn't enough space,  it will not append any more
 * results onto the string.
 */
{
    char ch_space[10];

    /* See if there is room */
    if (*len >= MAXKEYS-10) return;

    /* See if we prepend a comma */
    if (*len > 0) {
	STRCONCAT(key_str, ", ");
    }
    if (ch < ' ') {
	/* In control region - check to see if special */
	switch (ch) {
	case 0:
	    STRCONCAT(key_str, "`\\0'");
	    *len += 4;
	    break;
	case '\b':
	    STRCONCAT(key_str, "`\\b'");
	    *len += 4;
	    break;
	case '\f':
	    STRCONCAT(key_str, "`\\f'");
	    *len += 4;
	    break;
	case '\n':
	    STRCONCAT(key_str, "`\\n'");
	    *len += 4;
	    break;
	case '\r':
	    STRCONCAT(key_str, "`\\r'");
	    *len += 4;
	    break;
	case '\t':
	    STRCONCAT(key_str, "`\\t'");
	    *len += 4;
	    break;
	default:
	    /* Standard control notation */
	    STRCONCAT(key_str, "`^");
	    *len += 2;
	    key_str[(*len)++] = 'A' + ch - 1;
	    key_str[(*len)++] = '\'';
	    key_str[*len] = '\0';
	    break;
	}
    } else if (ch > 0177) {
	/* Beyond printable range */
	sprintf(ch_space, "`\\%04o'", ch);
	*len += STRLEN(ch_space);
	STRCONCAT(key_str, ch_space);
    } else {
	/* Normal printing character */
	sprintf(ch_space, "`%c'", ch);
	*len += STRLEN(ch_space);
	STRCONCAT(key_str, ch_space);
    }
}


#define MAX_MENU_SIZE	100

static char menu_str[MAX_MENU_SIZE];

static STR bdFindMenu(tbl, binding)
bd_table *tbl;		/* Real bindings table */
bdBinding *binding;		/* Command binding     */
/*
 * This routine produces a string which lists all of the panes
 * where `binding' appears in `tbl'.  The format of output
 * is: "pane:number, pane:number, ..." for all menu bindings
 * of the command.  The string produced is owned by this
 * routine and should not be modified.
 */
{
    int menu_str_len = 0;
    int pane, select;

    menu_str[0] = '\0';
    for (pane = 0;  pane < MAXPANES;  pane++) {
	for (select = 0;  select < MAXSELECT;  select++) {
	    if ((tbl->menuBindings[pane][select]).menuFunc &&
		(STRCOMP((tbl->menuBindings[pane][select]).menuFunc->id,
			 binding->id) == 0))
	      {
		  append_menu((tbl->menuBindings[pane][select]).paneName,
			      select, &menu_str_len, menu_str);
	      }
	}
    }
    return (STR) menu_str;
}

static void append_menu(pn, sel, len, str)
STR pn;				/* Name of pane                */
int sel;			/* Selection spot on pane      */
int *len;			/* Length of string (modified) */
STR str;			/* Actual menu string          */
/*
 * This routine appends onto `str' a textual decription of the
 * menu binding described by `pn' and `sel'.  If there isn't room
 * for the binding,  it is not appended.
 */
{
    char mn_space[100];
    int new_len;

    sprintf(mn_space, "%s:%d", pn, sel);
    new_len = STRLEN(mn_space);
    if (*len >= MAX_MENU_SIZE - new_len - 3) return;
    if (*len > 0) {
	/* Append preceding comma */
	STRCONCAT(str, ", ");
	*len += 2;
    }
    STRCONCAT(str, mn_space);
    *len += new_len;
}


#define MAX_ALI_SIZE	200

static char ali_str[MAX_ALI_SIZE];

static STR bdFindAlias(tbl, binding)
bd_table *tbl;		/* Real bindings table */
bdBinding *binding;		/* Command binding     */
/*
 * This routine produces a string which lists all of the type-in
 * aliases for `binding' in `tbl'.  The format of output is:
 * "name, name, ..." for all type-in bindings. The string produced 
 * is owned by this routine and should not be modified.
 */
{
    int ali_str_len = 0;
    int idx;

    ali_str[0] = '\0';
    for (idx = 0;  idx < tbl->lastAlias;  idx++) {
	if ((tbl->aliases[idx])->aliasFunc &&
	    (STRCOMP((tbl->aliases[idx])->aliasFunc->id, binding->id) == 0)) {
	    append_ali((tbl->aliases[idx])->aliasName, &ali_str_len, ali_str);
	}
    }
    return (STR) ali_str;
}

static void append_ali(ali, len, str)
STR ali;			/* Name of type-in alias       */
int *len;			/* Length of string (modified) */
STR str;			/* Actual alias list string    */
/*
 * This routine appends onto `str' a textual decription of the
 * type-in binding given in `ali'.  If there isn't enough space
 * for the binding,  it is not appended.
 */
{
    int new_len;

    new_len = STRLEN(ali);
    if (*len >= MAX_ALI_SIZE - new_len - 3) return;
    if (*len > 0) {
	/* Append preceding comma */
	STRCONCAT(str, ", ");
	*len += 2;
    }
    STRCONCAT(str, ali);
    *len += new_len;
}


/*ARGSUSED*/
static enum st_retval paneDestroy(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * Destroys a pane entry in the pane table.
 */
{
    VEMFREE(key);
    VEMFREE(value);
    return ST_CONTINUE;
}


vemStatus bdDestroy(theTable)
bdTable theTable;		/* Binding table */
/*
 * This routine releases all resources associated with the
 * binding table.  Note this routine DOES NOT associate
 * a reference count with the table.  Once it is destroyed,
 * there must never be another reference to the table.
 */
{
    bd_table *realTable = (bd_table *) theTable;
    bdTable *tempTable;
    char *realName;
    int index, subindex;

    if (realTable != NIL(bd_table)) {
	/* Get rid of the table in the hash table */
	realName = realTable->tableName;
	st_delete(allTables,  &realName, (Pointer *) &tempTable);
	/* Free the record contents */
	VEMFREE(realTable->tableName);
	VEMFREE(realTable->keyBindings);
	st_foreach(realTable->paneTable, paneDestroy, (char *) 0);
	st_free_table(realTable->paneTable);
	if (realTable->tableMenu) xpa_destroy(realTable->tableMenu);
	for (index = 0;  index < MAXPANES;  index++) {
	    for (subindex = 0;  subindex < MAXSELECT;  subindex++) {
		if ((realTable->menuBindings[index][subindex]).paneName) {
		    VEMFREE((realTable->menuBindings[index][subindex]).paneName);
		}
	    }
	}
	for (index = 0;  index < realTable->lastAlias;  index++) {
	    VEMFREE(realTable->aliases[index]->aliasName);
	    VEMFREE(realTable->aliases[index]);
	}
	VEMFREE(realTable);
    }
    return VEM_OK;
}



static void outAliases();
static void outMenus();
static void outKeys();

/*ARGSUSED*/
vemStatus bdOutBindings(spot, cmdList)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
/*
 * This command outputs a easily readable representation of 
 * the current bindings of the table given by `spot' to the
 * log file.
 */
{
    char *tableName = (char *)NULL;
    bd_table *table;

    if (spot) {
	switch (bufStyle(spot->theFct)) {
	case BUF_PHYSICAL:
	    tableName = "physical";
	    break;
	case BUF_SYMBOLIC:
	    tableName = "symbolic";
	    break;
	case BUF_SCHEMATIC:
	    tableName = "schematic";
	    break;
	}
    } else {
	tableName = "standard";
    }
    if (st_lookup(allTables, tableName, (char **) &table)) {
	sprintf(errMsgArea, "Bindings for table %s:\n", tableName);
	vemMessage(errMsgArea, 0);
	vemMessage("  Command name aliases:\n", 0);
	outAliases(table);
	vemMessage("  Menu allotments:\n", 0);
	outMenus(table);
	vemMessage("  Key bindings:\n", 0);
	outKeys(table);
    }
    return VEM_ARGRESP;
}

static void outAliases(table)
bd_table *table;
/*
 * Outputs command name aliases for all commands.  Commands are
 * given in sorted order by alias.
 */
{
    int i;

    sprintf(errMsgArea, "    %-20s %-20s\n\n", "ALIAS", "COMMAND");
    vemMessage(errMsgArea, 0);
    for (i = 0;  i < table->lastAlias;  i++) {
	sprintf(errMsgArea, "    %-20s %-20s\n",
		table->aliases[i]->aliasName,
		table->aliases[i]->aliasFunc->id);
	vemMessage(errMsgArea, 0);
    }
    sprintf(errMsgArea, "  %d total aliases\n", table->lastAlias);
    vemMessage(errMsgArea, 0);
}

static void outMenus(table)
bd_table *table;
/*
 * Outputs menu locations for all commands.  Commands are given
 * as they appear in the muliple pane menu.
 */
{
    int p, s, si, ret_val = -1;
    char *paneName;

    for (p = 0;  p < MAXPANES;  p++) {
	/* Check for selections */
	for (s = MAXSELECT-1;  s >= 0;  s--) {
	    if (table->menuBindings[p][s].menuFunc != 0) break;
	}
	if (s > 0) {
	    /* Find the pane name */
	    st_foreach(table->paneTable, findName, (char *) &p);
	    findName((char *) 0, (char *) &paneName, (char *) &ret_val);
	    sprintf(errMsgArea, "    Pane %s:\n", paneName);
	    vemMessage(errMsgArea, 0);
	    for (si = 0;  si <= s;  si++) {
		if (table->menuBindings[p][si].menuFunc) {
		    sprintf(errMsgArea, "      %s\n",
			    table->menuBindings[p][si].menuFunc->id);
		    vemMessage(errMsgArea, 0);
		} else {
		    vemMessage("      *Empty Selection*\n", 0);
		}
	    }
	} else {
	    vemMessage("    *Empty Pane*\n", 0);
	}
    }
}

#define CNTRL_A	'\001'
#define CNTRL_Z '\032'
#define PRTLOW	'\040'
#define PRTHI	'~'

static char *keyFormat(key_table, idx, buf)
bdBinding **key_table;		/* Key mapping table */
int idx;			/* Index into it     */
char *buf;			/* Buffer for format */
/*
 * Returns `buf' after formatting a key binding into it.
 */
{
    if (idx < PRTLOW) {
	/* Control version */
	buf[0] = '^';
	buf[1] = idx+'A'-CNTRL_A;
    } else if (idx > PRTHI) {
	/* Super version */
	buf[0] = 'D';
	buf[1] = 'L';
    } else {
	/* Normal version */
	buf[0] = ' ';
	buf[1] = idx;
    }
    buf[2] = ':';
    buf[3] = ' ';
    buf[4] = '\0';
    if (key_table[idx]) {
	STRNCONCAT(buf, key_table[idx]->id, 20);
    }
    return buf;
}

static void outKeys(table)
bd_table *table;
/*
 * Outputs key bindings for the specified table.  These will be given
 * in two forms:  the basic alphabetic keys will be given in a three
 * column format: unshifted, shifted, and control.  The remaining
 * keys will be given in ASCII order in three column form.
 */
{
    int i, c;
    char buf1[25], buf2[25], buf3[25];

    vemMessage("    Alphabetic key bindings:\n", 0);
    for (i = CNTRL_A;  i <= CNTRL_Z;  i++) {
	sprintf(errMsgArea, "      %-20s %-20s %-20s\n",
		keyFormat(table->keyBindings, i+'a'-CNTRL_A, buf1),
		keyFormat(table->keyBindings, i+'A'-CNTRL_A, buf2),
		keyFormat(table->keyBindings, i, buf3));
	vemMessage(errMsgArea, 0);
    }
    vemMessage("    Remaining key bindings:\n", 0);
    c = 0;
    vemMessage("      ", 0);
    for (i = CNTRL_Z+1;  i < 'A';  i++) {
	sprintf(errMsgArea, "%-20s ", keyFormat(table->keyBindings, i, buf1));
	vemMessage(errMsgArea, 0);
	if (++c >= 3) {
	    c = 0;
	    vemMessage("\n      ", 0);
	}
    }
    for (i = 'Z'+1;  i < 'a';  i++) {
	sprintf(errMsgArea, "%-20s ", keyFormat(table->keyBindings, i, buf1));
	vemMessage(errMsgArea, 0);
	if (++c >= 3) {
	    c = 0;
	    vemMessage("\n      ", 0);
	}
    }
    for (i = 'z'+1;  i < 128;  i++) {
	sprintf(errMsgArea, "%-20s ", keyFormat(table->keyBindings, i, buf1));
	vemMessage(errMsgArea, 0);
	if (++c >= 3) {
	    c = 0;
	    vemMessage("\n      ", 0);
	}
    }
    vemMessage("\n", 0);
}
