/*
 * VE Technology package
 * David Harrison,  September 1985
 * UC Berkeley
 *
 * A technology describes how objects on each layer appear on a graphics
 * display or plotter.  This package reads and writes human readable 
 * technology descriptions which are written in an form similar to
 * the TECHNOLOGY statement in version 1.0.0 of the Electronic Design
 * Interchange Format.  Internally,  the package builds a hash table
 * of technology descriptions each of which have a hash table of
 * layer descriptions.
 *
 * TECHNOLOGY FILE FORMAT
 *
 * technology_file    ::= "(" tech_header tech_body ")"
 * tech_header        ::= "technology" <name>
 * tech_body          ::= tech_statement | tech_body tech_statement
 * tech_statement     ::= figureGroupDefault
 *
 * figureGroupDefault ::= "(" FGD_header FGD_body ")"
 * FGD_header	      ::= "figureGroupDefault" <name>
 * FGD_body	      ::= FGD_statement | FGD_body FGD_statement
 * FGD_statement      ::= pathType | width | color | fillPattern | borderPat |
 *			  userData
 *
 * pathType           ::= "(" "pathType" pathOption pathOption ")"
 * pathOption         ::= "extend" | "truncate" | "round"
 *
 * width              ::= "(" "width" int ")"
 *
 * color              ::= "(" "color" int int int ")"
 *
 * fillPattern        ::= "(" "fillPattern" int int str ")"
 *
 * borderPat          ::= "(" "borderPattern" int str ")"
 *
 * userData           ::= "(" userHeader userBody ")"
 * userHeader         ::= "userData" name
 * userBody           ::= "(" number_list ")"
 * number_list        ::= int | number_list int
 *
 * Where name is an identifier,  int is an integer constant,  and str is
 * a bit string constant (stream of 0 or 1 enclosed in double quotes).
 *
 * The format is close to the technology description in EDIF but not
 * exactly.  The following components are ignored: define, rename, 
 * numberDefinition,  gridMap, simulationInfo, simulationMap, and comment.  
 * Others are ignored.  figureGroupDefault specifies the display attributes 
 * of a layer.  Comment statements are ignored.  Only one userData statement
 * is defined:  "priority" which must be followed by a single integer
 * representing layer priority.  Higher priority layers will appear "over
 * the top of" lower priority layers.  A sample technology can be found in 
 * tech.test.
 *
 * This format was subsumed by the technology access package (tap).
 * This program is now provided to convert the old pattern specification
 * files into tap views.
 */


#include "port.h"
#include "general.h"
#include "message.h"		/* Uses message logging stuff for errors */
#include "list.h"		/* Uses the list handling package        */
#include "tech.h"		/* Self declaration 			 */
#include "oct.h"		/* Oct data manager                      */

static lsList techList = 0;	/* List of technologies */

#define SPRINTF	(void) sprintf

#define strsave(s)    (strcpy(malloc((unsigned) (strlen(s)+1)), s))

/*
 * Since this package reads a text format,  it is useful to have
 * very explicit error messages.  Whenever an abnormal condition occurs,
 * this package logs the errors to both the VEM log file and to the
 * user.
 */


int findTech(techFile, name)
FILE *techFile;			/* Technology file stream */
STR name;			/* Technology name        */
/*
 * Looks for the technology description in 'techFile'.  If it finds
 * it,  it returns a non-zero status and leaves the stream positioned
 * just after the technology name.  If it doesn't find it,  it
 * returns zero and the stream is left at end of file.  Logs error
 * messages.
 */
{
    lexUnit nextLex;
    int retCode;

    /* Look for technology in the file */
    while ((retCode = scanToOpen(techFile)) == 0) {
	if (scanItem(techFile, &nextLex) != 0) {
	    SPRINTF(errMsgArea,
		    "Technology '%s' not found:  file ends abnormally.\n",
		    name);
	    (void) vemMessage(errMsgArea, MSG_DISP);
	    return(VEM_CANTFIND);
	}
	if ((nextLex.lexType == IDENTIFIER) &&
	    (stricmp(nextLex.lexUnion.strData, "technology") == 0)) {
		if (((retCode = scanItem(techFile, &nextLex)) == 0) &&
		    (nextLex.lexType == IDENTIFIER) &&
		    (stricmp(nextLex.lexUnion.strData, name) == 0)) {
			return(1);
		    }
		if (retCode == SCANEOF) {
		    SPRINTF(errMsgArea,
		       "Technology '%s' not found:  file ends on technology.\n",
		       name);
		    (void) vemMessage(errMsgArea, MSG_DISP);
		    return(0);
		}
	    }
    }
    if (retCode != 0) {
	SPRINTF(errMsgArea,
		"Technology '%s' not not in file.\n",
		name);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    } else return(1);
}


#ifdef ENDTYPES
int readPType(techFile, newLayer)
FILE *techFile;			/* Input technology */
vemTechLayer *newLayer;		/* Layer structure  */
/*
 * Assumes techFile is positioned after "pathType" in path type structure.
 * Returns non-zero if successful, zero if not.  Logs messages using
 * (void) vemMessage.
 */
{
}
#endif

int readWidth(techFile, newLayer)
FILE *techFile;			/* Input technology */
vemTechLayer *newLayer;		/* Layer structure  */
{
    lexUnit nextLex;

    /* Read one integer into width field skip the rest */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Can't read width of layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea,
		"width of layer '%s' has non-numerical value\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->width = nextLex.lexUnion.numData;
    if (scanOverList(techFile, 1) != 0) {
	SPRINTF(errMsgArea,
		"width of layer '%s' is corrupted (abnormal EOF)\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    return(1);
}



int readColor(techFile, newLayer)
FILE *techFile;			/* Input technology */
vemTechLayer *newLayer;		/* Layer structure  */
/*
 * Assumes techFile is positioned just after the beginning of a color
 * structure.  Reads the appropriate information into the specified
 * layer structure.  Returns zero and logs an error message if there
 * were problems.
 */
{
    lexUnit nextLex;

    /* Read three integer values, check for validity */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in color specification for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea,
		"Color specification for layer '%s' has non-numerical values\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if ((nextLex.lexUnion.numData < 0) || (nextLex.lexUnion.numData > 1000)) {
	SPRINTF(errMsgArea,
		"Color for layer '%s' has a value out of range.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->red = nextLex.lexUnion.numData;
    /* Number two */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in color specification for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea,
		"Color specification for layer '%s' has non-numerical values\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if ((nextLex.lexUnion.numData < 0) || (nextLex.lexUnion.numData > 1000)) {
	SPRINTF(errMsgArea,
		"Color for layer '%s' has a value out of range.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->green = nextLex.lexUnion.numData;
    /* Number three */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in color specification for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea,
		"Color specification for layer '%s' has non-numerical values\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if ((nextLex.lexUnion.numData < 0) || (nextLex.lexUnion.numData > 1000)) {
	SPRINTF(errMsgArea,
		"Color for layer '%s' has a value out of range.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->blue = nextLex.lexUnion.numData;
    if (scanOverList(techFile, 1) != 0) {
	SPRINTF(errMsgArea,
		"Color for layer '%s' ends abnormally.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    return(1);
}



int readFillP(techFile, newLayer)
FILE *techFile;			/* Input technology */
vemTechLayer *newLayer;		/* Layer structure  */
/*
 * Assumes techFile is positioned at the beginning of the fill pattern
 * data section.  Reads in the fill pattern and sets relevant fields
 * of 'newLayer'.  If there are problems,  it returns zero and
 * logs a message using (void) vemMessage.
 */
{
    lexUnit nextLex;
    int rows, cols;

    /* This one is interesting.  First, two numbers: */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in fill pattern specification for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea,
		"Fill pattern for layer '%s' has non-numerical values\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    rows = nextLex.lexUnion.numData;
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in fill pattern specification for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea,
		"Fill pattern for layer '%s' has non-numerical values\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    cols = nextLex.lexUnion.numData;
    /* Now,  the fill pattern itself: */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in fill pattern specification for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != STRINGCONST) {
	SPRINTF(errMsgArea,
		"Fill pattern for '%s' has no string constant.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (scanOverList(techFile, 1) != 0) {
	SPRINTF(errMsgArea,
		"Fill pattern for layer '%s' is corrupt.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->fillData = flNewPattern(cols, rows, nextLex.lexUnion.strData);
    if (newLayer->fillData == 0) {
	SPRINTF(errMsgArea,
		"Could not decode fill pattern from layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    return(1);
}


int doPatt(newLayer, patConst)
vemTechLayer *newLayer;		/* Layer structure       */
STR patConst;			/* Border pattern constant */
/*
 * Interprets string pattern constant and places it in the layer
 * specification.  Returns zero and logs errors using (void) vemMessage
 * if there are problems.
 */
{
    if (newLayer->borderLength > 16) {
	SPRINTF(errMsgArea,
		"In layer '%s',  there is a border pattern longer than 16.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (strlen(patConst) != newLayer->borderLength) {
	SPRINTF(errMsgArea,
		"In layer '%s',  border pattern length does not match data.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->borderData = strsave(patConst);

    return(1);
}

int readBorder(techFile, newLayer)
FILE *techFile;			/* Input technology */
vemTechLayer *newLayer;		/* Layer structure  */
/*
 * Assumes techFile is just after the keyword "borderPattern" in a
 * border pattern structure.  Reads and fills in the border pattern
 * information.  Returns zero and logs an error message.
 */
{
    lexUnit nextLex;

    /* Read length first */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in border pattern for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea,
		"Border pattern for layer '%s' has non-numerical values\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->borderLength = nextLex.lexUnion.numData;
    /* Read data from string */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea,
		"Abnormal EOF in border pattern specification for layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != STRINGCONST) {
	SPRINTF(errMsgArea,
		"Border pattern for '%s' has no string constant.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (scanOverList(techFile, 1) != 0) {
	SPRINTF(errMsgArea,
		"Border pattern for layer '%s' is corrupt.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    return( doPatt(newLayer, nextLex.lexUnion.strData) );
}



int readUserD(techFile, newLayer)
FILE *techFile;			/* Input technology */
vemTechLayer *newLayer;		/* Layer structure  */
/*
 * This routine assumes techFile is positioned just after the keyword
 * "userData" in a user data section.  It looks for the keyword
 * "priority" and reads the integer priority into 'newLayer'.  Returns
 * zero and puts a message in errMsgArea if there are problems.
 */
{
    lexUnit nextLex;

    /* expects a name next */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea, "Expecting name in user data field of layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != IDENTIFIER) {
	SPRINTF(errMsgArea, "Expecting name in user data field of layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (stricmp(nextLex.lexUnion.strData, "priority") != 0) {
	if (scanOverList(techFile, 1) != 0) {
	    SPRINTF(errMsgArea,
		    "User data section of layer '%s' is corrupt.\n",
		    newLayer->layerName);
	    (void) vemMessage(errMsgArea, MSG_DISP);
	    return(0);
	}
	return(1);
    }
    /* Read the number */
    if (scanItem(techFile, &nextLex) != 0) {
	SPRINTF(errMsgArea, "Expecting value for priority of layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    if (nextLex.lexType != NUMBERCONST) {
	SPRINTF(errMsgArea, "Non numerical value for priority of layer '%s'\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    newLayer->priority = nextLex.lexUnion.numData;
    if (scanOverList(techFile, 1) != 0) {
	SPRINTF(errMsgArea, "User data section for layer '%s' ends abnormally.\n",
		newLayer->layerName);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(0);
    }
    return(1);
}


int readLayer(techFile, newLayer)
FILE *techFile;			/* Input technology */
vemTechLayer *newLayer;		/* Layer structure  */
/*
 * This routine assumes 'techFile' is positioned just after the
 * keyword 'figureGroupDefault' in a figure group default structure.
 * It reads the layer information and places it in 'newLayer'.  Returns
 * non-zero if there were no problems,  zero if there was.  Writes
 * its own messages in errMsgArea.
 */
{
    lexUnit nextLex;

    /* Initialize structure */
    (void) memset((Pointer) newLayer, 0, sizeof(vemTechLayer));

    /* Name should be next */
    if ((scanItem(techFile, &nextLex) != 0) ||
	(nextLex.lexType != IDENTIFIER)) {
	SPRINTF(errMsgArea, "Missing name for figureGroupDefault.\n");
	return(0);
    }
    newLayer->layerName = VEMSTRCOPY(nextLex.lexUnion.strData);

    /*
     * Read the lists in figureGroupDefault.  The ones we are interested in:
     * pathType, width, color, fillPattern, borderPat, and userData.
     */

    for (;;) {
	if (scanItem(techFile, &nextLex) != 0) {
	    SPRINTF(errMsgArea,
		    "While reading layer, scanner reached EOF.\n");
	    (void) vemMessage(errMsgArea, MSG_DISP);
	    return(0);
	}
	if (nextLex.lexType == RIGHTPAREN) break;
	if (nextLex.lexType == LEFTPAREN) {
	    /* read identifier to see what's in the list */
	    if (scanItem(techFile, &nextLex) != 0) {
		SPRINTF(errMsgArea,
			"While reading layer '%s', scanner reached EOF.\n",
			newLayer->layerName);
		(void) vemMessage(errMsgArea, MSG_DISP);
		return(0);
	    }
	    if (nextLex.lexType == IDENTIFIER) {
		if (stricmp(nextLex.lexUnion.strData, "width") == 0) {
		    if (!readWidth(techFile, newLayer))
		      return(0);
		}
#ifdef ENDTYPES
		if (stricmp(nextLex.lexUnion.strData, "pathType") == 0) {
		    if (!readPType(techFile, newLayer))
		      return(0);
		}
#endif
		else if (stricmp(nextLex.lexUnion.strData, "color") == 0) {
		    if (!readColor(techFile, newLayer))
		      return(0);
		} else if (stricmp(nextLex.lexUnion.strData,"fillPattern") == 0) {
		    if (!readFillP(techFile, newLayer))
		      return(0);
		} else if (stricmp(nextLex.lexUnion.strData, "borderPattern") == 0) {
		    if (!readBorder(techFile, newLayer))
		      return(0);
		} else if (stricmp(nextLex.lexUnion.strData, "userData") == 0) {
		    if (!readUserD(techFile, newLayer))
		      return(0);
		} else {
		    /* Not interested.  Skip the list */
		    if (scanOverList(techFile, 1) != 0) {
			SPRINTF(errMsgArea,
				"scanner reached abnormal EOF in layer '%s'\n",
				newLayer->layerName);
			(void) vemMessage(errMsgArea, MSG_DISP);
			return(0);
		    }
		}
	    } else {
		if (scanOverList(techFile, 1) != 0) {
		    SPRINTF(errMsgArea,
			    "scanner reached abnormal EOF in layer '%s'\n",
			    newLayer->layerName);
		    (void) vemMessage(errMsgArea, MSG_DISP);
		    return(0);
		}
	    }
	} else {
	    SPRINTF(errMsgArea,
		    "layer '%s' is corrupt.\n",
		    newLayer->layerName);
	    (void) vemMessage(errMsgArea, MSG_DISP);
	    return(0);
	}
    }
    return(1);
}
	


void insertLayer(list, layer)
lsList list;			/* List to insert item */
vemTechLayer *layer;		/* Layer to insert     */
/*
 * This routine inserts the specified layer into the specified list
 * ordered by priority.  Layers are ordered from lowest to highest
 * in priority.  This is basically an insertion sort routine.
 */
{
    lsGen generator;
    vemTechLayer *nextLayer;

    generator = lsStart(list);
    while (lsNext(generator, (Pointer *) &nextLayer, LS_NH) == LS_OK) {
	if (layer->priority < nextLayer->priority) {
	    (void) lsPrev(generator, (Pointer *) &nextLayer, LS_NH);
	    lsInBefore(generator, (Pointer) layer, LS_NH);
	    break;
	}
    }
    lsFinish(generator);
    if (nextLayer == NIL(vemTechLayer))   /* Insert at end of list */
	lsNewEnd(list, (Pointer) layer, LS_NH);
}





int readTech(techFile, layerList)
FILE *techFile;			/* Input file positioned in technology */
lsList layerList;		/* List of layers (see list package)   */
/*
 * This routine assumes 'techFile' has been positioned to just after the
 * technology name.  It reads the description looking for 'figureGroupDefault'
 * structures and ignoring everything else.  Each 'figureGroupDefault'
 * section is read into a new layer and positioned in the list in
 * order of priority (see insertLayer).  It returns a non-zero status if 
 * the scanning was sucessful.  Otherwise,  it returns zero and logs
 * an error using vemMessage.
 */
{
    lexUnit nextLex;
    vemTechLayer *newLayer;

    for (;;) {
	if (scanItem(techFile, &nextLex) != 0) {
	    SPRINTF(errMsgArea,
		    "Reached EOF before closing outer most list.\n");
	    (void) vemMessage(errMsgArea, MSG_DISP);
	    return(0);
	}
	if (nextLex.lexType == RIGHTPAREN) break;
	if (nextLex.lexType == LEFTPAREN) {
	    /* read identifier to see what's in the list */
	    if (scanItem(techFile, &nextLex) != 0) {
		SPRINTF(errMsgArea,
			"Reached EOF inside sub-list of technology.\n");
		(void) vemMessage(errMsgArea, MSG_DISP);
		return(0);
	    }
	    if ((nextLex.lexType == IDENTIFIER) &&
		(stricmp(nextLex.lexUnion.strData, "figureGroupDefault") == 0)) {
		    newLayer = VEMALLOC(vemTechLayer);
		    newLayer->layerAttrs = (vemAttrs) 0;
		    if (!readLayer(techFile, newLayer)) {
			VEMFREE(newLayer);
			return(0);
		    }
		    insertLayer(layerList, newLayer);
		} else {
		    scanOverList(techFile, 1);
		}
	} else {
	    SPRINTF(errMsgArea, "technology is corrupt.\n");
	    (void) vemMessage(errMsgArea, MSG_DISP);
	    return(0);
	}
    }
    return(1);
}
			



vemStatus loadTech(name, patFile, newTech)
STR name;			/* in: Technology name            */
STR patFile;			/* in: File for technology        */
vemTech **newTech;		/* out: Technology description    */
/*
 * Loads technology named 'name' from the file 'file' in directory 'dir'.  
 * Returns a record which contains the technology name and file and a list of 
 * layers ordered from lowest to highest in priority.  There is one 
 * non-fatal diagnostic:
 *   VEM_DUPLICATE:  Attempt to load a technology that's already loaded
 * Fatal diagnostics:
 *   VEM_NOFILE:   File or directory not found
 *   VEM_CANTFIND: Named technology not in specified file
 */
{
    vemTech *tempTech;
    vemStatus returnCode = VEM_OK, remTech();
    FILE *techFile;

    /* Initialize list of technologies (if needed) */
    if (techList == LS_NIL) techList = lsCreate();

    /* Check to make sure the file is around */
    if ((techFile = fopen(patFile, "r")) == NIL(FILE)) {
	SPRINTF(errMsgArea, "Could not find technology file '%s'.\n", patFile);
	(void) vemMessage(errMsgArea, MSG_DISP);
	return(VEM_NOFILE);
    }

    /* Find the technology */
    if (!findTech(techFile, name)) return(VEM_CANTFIND);

    /* Create and fill new technology */
    (*newTech) = VEMALLOC(vemTech);
    (*newTech)->techName = VEMSTRCOPY(name);
    (*newTech)->techFile = VEMSTRCOPY(patFile);
    (*newTech)->layerList = lsCreate();
    if (!readTech(techFile, (*newTech)->layerList))
      return(VEM_CORRUPT);

    /* Remove any other technologies named 'name' */
    if ((tempTech = getTech(name, patFile)) != NIL(vemTech)) {
	returnCode = VEM_DUPLICATE;
	SPRINTF(errMsgArea, "A technology named '%s' has already been loaded.\n",
		name);
	(void) vemMessage(errMsgArea, 0);
	remTech(tempTech);
    }

    /* Put it in the list and return */
    lsNewBegin(techList, (Pointer) (*newTech), LS_NH);
    SPRINTF(errMsgArea, "Loaded technology '%s' from file '%s'\n",
	    name, patFile);
    (void) vemMessage(errMsgArea, 0);
    return(returnCode);
}





vemTech *getTech(name, techFile)
STR name;			/* in: Technology name */
STR techFile;			/* in: Technology file */
/*
 * Returns a previously loaded technology.  If the technology cannot
 * be found,  it returns NULL.  Warning:  linear lookup.
 */
{
    lsGen generator;
    vemTech *oneTech;

    if (techList == LS_NIL) {
	techList = lsCreate();
	return(NULL);
    } else {
	generator = lsStart(techList);
	oneTech = NULL;
	while (lsNext(generator, (Pointer *) &oneTech, LS_NH) == LS_OK) {
	    if ((stricmp(oneTech->techName, name) == 0) &&
		(stricmp(oneTech->techFile, techFile) == 0))
	      break;
	}
	lsFinish(generator);
	return(oneTech);
    }
}





vemTechLayer *getLayer(tech, layerName)
vemTech *tech;			/* in: Pointer to technology */
STR layerName;			/* in: Name of layer         */
/*
 * Returns a pointer to a layer description for the specified technology
 * and layer name.  Returns NULL if there is no such layer for the
 * supplied technology.  Warning:  linear lookup.  Since the list is
 * provided in the record,  this routine is for convenience only.
 */
{
    lsGen generator;
    vemTechLayer *nextLayer;

    generator = lsStart(tech->layerList);
    while (lsNext(generator, (Pointer *) &nextLayer, LS_NH) == LS_OK) {
	if (strcmp(nextLayer->layerName, layerName) == 0) {
	    lsFinish(generator);
	    return(nextLayer);
	}
    }
    lsFinish(generator);
    return(NIL(vemTechLayer));
}




int remLayer(ptr)
vemTechLayer *ptr;
/* Frees the storage associated with 'ptr' */
{
    VEMFREE(ptr);
}

vemStatus remTech(tech)
vemTech *tech;			/* Technology */
/*
 * Releases all resources associated with the specified technology.
 * It should not be referenced again in any way.  Returns VEM_CANTFIND
 * if the technology is not in the current list of technologies.
 */
{
    lsGen generator;
    vemTech *nextTech;

    /* Find it in the technology list and remove it */
    generator = lsStart(techList);
    while (lsNext(generator, (Pointer *) &nextTech, LS_NH) == LS_OK) {
	if (nextTech == tech) {
	    lsDelBefore(generator, (Pointer *) &nextTech);
	    break;
	}
    }
    if (nextTech != tech) {
	SPRINTF(errMsgArea,
		"remTech: can't find technology '%s' in list.\n",
		tech->techName);
	(void) vemMessage(errMsgArea, 0);
	return(VEM_CANTFIND);
    }

    /* Next,  get rid of all the layers */
    lsDestroy(tech->layerList, (void (*)()) remLayer);

    /* Finally,  get rid of technology itself */
    VEMFREE(tech);
    return(VEM_OK);
}



/*ARGSUSED*/
vemStatus saveTech(tech, file)
vemTech *tech;			/* in: Technology to save */
STR file;			/* in: File to save it in */
/*
 * This routine writes out a technology in the format described on
 * the title page.  Some warnings:  the routine writes only the
 * technology information and it uses its own formatting.  A technology
 * read in which is part of a larger EDIF specification,  contains
 * comments or special formatting,  will be LOST if saveTech is used
 * to write a changed technology to the same file.
 */
{
    /* Note: not implemented yet */
    (void) vemMessage("Saving technologies is not implemented yet.\n",
	       MSG_DISP);
}

