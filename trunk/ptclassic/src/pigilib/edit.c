/* edit.c  edg
Version identification:
$Id$
*/


/* Includes */
#include <string.h>
#include <sys/file.h>
#include "local.h"
#include "rpc.h"
#include "oh.h"
#include "paramStructs.h"
#include "vemInterface.h"
#include "util.h"
#include "err.h"
#include "octIfc.h"
#include "exec.h"

void FindClear();

#define dmWidth 40
#define dmIncrement 20

/* EditDelayParams  5/27/88
Edit the delay value associated with a delay instance.
Inputs: instPtr = delay instance pointer.
Outputs: return = TRUE if no error.
*/

static boolean			   
EditDelayParams(instPtr)
octObject *instPtr;
{
    octObject delayProp;
    static dmTextItem item = {"Number of delays", 1, 10, NULL, NULL};
    char buf[100];

    GetOrInitDelayProp(instPtr, &delayProp);
    StringizeProp(&delayProp, buf, 100);
    item.value = buf;
    if (dmMultiText("Edit Delay", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        return(TRUE);
    }
    delayProp.contents.prop.type = OCT_STRING;
    delayProp.contents.prop.value.string = item.value;
    IntizeProp(&delayProp);
    (void) octModify(&delayProp);
    return(TRUE);
}

static boolean			   
EditBusParams(instPtr)
octObject *instPtr;
{
    octObject busProp;
    static dmTextItem item = {"Bus Width", 1, 10, NULL, NULL};
    char buf[100];

    GetOrInitBusProp(instPtr, &busProp);
    StringizeProp(&busProp, buf, 100);
    item.value = buf;
    if (dmMultiText("Edit Bus Width", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        return(TRUE);
    }
    busProp.contents.prop.type = OCT_STRING;
    busProp.contents.prop.value.string = item.value;
    IntizeProp(&busProp);
    (void) octModify(&busProp);
    return(TRUE);
}

/* EditSogParams  5/27/88
Outputs: return = TRUE if no error.
*/
static boolean
EditSogParams(instPtr)
octObject *instPtr;
{
    ParamListType pList;

/* set domain corresponding to the instance */
    if (!setCurDomainInst(instPtr)) {
	PrintErr("Invalid domain found");
	return(FALSE);
    }

    ERR_IF1(!GetOrInitSogParams(instPtr, &pList));
    if (pList.length == 0) {
	PrintErr("Star or galaxy has no parameters");
	return(TRUE);
    }
    if (EditParamList(&pList,"Edit Actual Parameters"))
	    SetSogParams(instPtr, &pList);
    return(TRUE);
}

static boolean
EditParamList(pListPtr, dmTitle)
ParamListType *pListPtr;
char* dmTitle;
{
    ParamType *place;
    dmTextItem *items;
    int i, width, maxwidth;

    items = (dmTextItem *) calloc(pListPtr->length, sizeof(dmTextItem));
    place = pListPtr->array;
    width = 0;
    maxwidth = 0;
    for (i = 0; i < pListPtr->length; i++) {
	items[i].itemPrompt = place->name;
	items[i].rows = 1;
	items[i].value = place->value;
	width = strlen(place->value);
	if(maxwidth < width) maxwidth = width;
	items[i].userData = NULL;
	place++;
    }
    for (i = 0; i < pListPtr->length; i++) {
	items[i].cols = maxwidth + dmIncrement;
    }
    if (dmMultiText(dmTitle, pListPtr->length, items) != VEM_OK) {
	PrintCon("Aborted entry");
	return(FALSE);
    }
    /* should free place->value string before assignment */
    place = pListPtr->array;
    for (i = 0; i < pListPtr->length; i++) {
	place->value = items[i].value;
	place++;
    }
    free((char *)items);
    return(TRUE);
}

/* 4/14/89 3/19/89
Edit the formal parameters of a galaxy schematic facet.
Returns 0 if there is an error.
Returns 1 if the parameters were changed or if the user clicked OK.
Returns 2 if the command was aborted by the user.
*/
int
EditFormalParams(galFacetPtr)
octObject *galFacetPtr;
{
    ParamType *place;
    int i, width, maxwidth;
    ParamListType pList;
    dmTextItem *items;

    ERR_IF1(!GetFormalParams(galFacetPtr, &pList));

    if (pList.length == 0) {
        PrintErr("Galaxy or Universe has no parameters");
        return(1);
    }
    items = (dmTextItem *) calloc((unsigned)pList.length, sizeof(dmTextItem));
    place = pList.array;
    width = 0;
    maxwidth = 0;
    for (i = 0; i < pList.length; i++) {
	items[i].itemPrompt = place->name;
	items[i].rows = 1;
	items[i].value = place->value;
	width = strlen(place->value);
	if(maxwidth < width) maxwidth = width;
	items[i].userData = NULL;
	place++;
    }
    for (i = 0; i < pList.length; i++) {
        items[i].cols = maxwidth + dmIncrement;
    }
    if (dmMultiText("Edit Formal Parameters", pList.length, items) != VEM_OK) {
	PrintCon("Aborted entry");
	return(2);
    }
    /* should free place->value string before assignment */
    place = pList.array;
    for (i = 0; i < pList.length; i++) {
	place->value = items[i].value;
	place++;
    }

    free((char *)items);

    ERR_IF1(!SetFormalParams(galFacetPtr, &pList));
    return(1);
}

/* 4/14/89 3/19/89
Edit the formal param list of a galaxy schematic facet.
Returns 0 if there is an error.
Returns 1 if the parameters were changed or if the user clicked OK.
Returns 2 if the command was aborted by the user.
*/
static int
EditFormalParamList(galFacetPtr)
octObject *galFacetPtr;
{
static dmTextItem defaultItem = {NULL, 1, dmIncrement, NULL, NULL};
    int i, j, itemsN, paramsN;
    int width, maxwidth;
    ParamListType pList;
    ParamType *place;
    dmTextItem *items;

    ERR_IF1(!GetFormalParams(galFacetPtr, &pList));

    width = 0;
    maxwidth = 0;

    /* put formal params (if any) into item list */
    itemsN = (pList.length + 1) * 3;
    items = (dmTextItem *) calloc((unsigned)itemsN, sizeof(dmTextItem));
    place = pList.array;
    j = 0;
    for (i = 0 ; i < pList.length; i++) {
	    /* add in the name, type, value of each param */
	items[j] = defaultItem;
	items[j].itemPrompt = "Name";
	items[j++].value = place->name;
	width = strlen(place->name);
	if(maxwidth<width) maxwidth = width;
	items[j] = defaultItem;
	items[j].itemPrompt = "Type";
	items[j++].value = place->type ? place->type : "FLOAT";
	items[j] = defaultItem;
	items[j].itemPrompt = "Value";
	items[j++].value = place->value;
	width = strlen(place->value);
	if(maxwidth<width) maxwidth = width;
	place++;
    }
    items[j] = defaultItem;
    items[j].itemPrompt = "New name";
    items[j++].value = "";
    items[j] = defaultItem;
    items[j].itemPrompt = "New type";
    items[j++].value = "";
    items[j] = defaultItem;
    items[j].itemPrompt = "New value";
    items[j++].value = "";
    /* set the width */
    maxwidth += dmIncrement;
    for (i = 0 ; i < itemsN; i++) {
	items[i].cols = maxwidth;
    }
    if (dmMultiText("Edit Formal Parameters", itemsN, items) != VEM_OK) {
	PrintCon("Aborted entry");
	return(2);
    }
    /* Tranfer params from item list to pList, deleting blank entries.
	Assumes there is an extra empty slot in pList.array (kludgy).
	See paramStructs.c.
    */
    place = pList.array;
    j = 0;
    paramsN = 0;
    for (i = 0; i < pList.length + 1; i++) {
	if (strlen(SkipWhite(items[j].value)) != 0) {
	    /* copy this name-type-value triplet */
	    place->name = items[j++].value;
	    place->type = items[j++].value;
	    place->value = items[j++].value;
	    place++;
	    paramsN++;
	} else {
	    /* skip this name-type-value triplet */
	    j += 3;
	}
    }
    pList.length = paramsN;
    free(items);

    ERR_IF1(!SetFormalParams(galFacetPtr, &pList));
    return(1);
}

/* DefineParams 8/4/91 - eal
Define the formal parameter list and default values for a Universe
or galaxy.
*/
int
DefineParams(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet;

    ViInit("define-params");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    } else if (IsGalFacet(&facet) || IsUnivFacet(&facet)) {
	if (!EditFormalParamList(&facet)) {
	    PrintErr(ErrGet());
	}
    } else {
	PrintErr("Cursor must be over a universe or a galaxy schematic");
    }
    ViDone();
}

/* EditParams  5/27/88 4/24/88
Edit parameters.
*/
int 
EditParams(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet, inst;
    vemStatus status;
    boolean status2;

    ViInit("edit-params");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }

    /* get name of instance under cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
	/* cursor not over an instance... */
	if (IsGalFacet(&facet) || IsUnivFacet(&facet)) {
	    status2 = EditFormalParams(&facet);
	} else {
	    PrintErr("Cursor must be over an instance, galaxy, or universe schematic");
	    ViDone();
	}
    } else {
	/* cursor is over some type of instance... */
	if (IsDelay(&inst)) {
	    status2 = EditDelayParams(&inst);
	} else if (IsBus(&inst)) {
	    status2 = EditBusParams(&inst);
	} else if (IsGal(&inst) || IsStar(&inst)) {
	    /* inst is a sog... */
	    status2 = EditSogParams(&inst);
	} else {
	    PrintErr("Cursor must be over a star, galaxy, or delay instance");
	    ViDone();
	}
    }
    if (!status2) {
	PrintErr(ErrGet());
	ViDone();
    }
    ViDone();
}

/***** Begin of RpcOpenPalette routines */

/* Structures to store palette list and dialog box */
static char **palettes;
static int palettes_n;

/* function to determine default palettes */
char *defaultPalettes();

static dmWhichItem *items;

/* ListLength  4/28/88
Counts number of ':' in string, adds one, and returns this value.
*/
static int
ListLength(pathStr)
char *pathStr;
{
    int n = 0;

    while ((pathStr = strchr(pathStr, ':')) != NULL) {
	n++;
	pathStr++;
    }
    return(++n);
}

/* 8/2/89 7/23/89 4/28/88 2/27/88
Read the palette list from an entry in .Xdefaults, if no list exists,
then use the hardcoded default.  The list is specified as a colon
separated string of palette pathnames that can be relative to the
current VEM directory.  (The paths are relative to the current VEM
directory because facets are opened on the VEM side of the IPC
interface.)
Caveats: Should only be called once at the beginning of the program.
*/
boolean
OpenPaletteInit()
{
    char *a, *b, *copy, *techDir, buf[FILENAME_MAX];
    int i;

/* dots in resource names no longer work??? */
/*    sprintf(buf, "%s.palettes", UAppName); */
    sprintf(buf, "%sPalettes", UAppName);
    if ((b = RPCXGetDefault("vem", buf)) == NULL) {
	b = defaultPalettes();
    }
    palettes_n = ListLength(b) + 1;
    palettes = (char **) malloc(palettes_n * sizeof(char *));
    copy = malloc((strlen(b) + 1)*sizeof(char));
    strcpy(copy, b);
    a = SkipWhite(copy);
    i = 0;
    while (TRUE) {
	b = strchr(a, ':');
	if (b == NULL) {
	    palettes[i++] = a;  /* one last time */
	    break;
	}
	*b = '\0';
	palettes[i++] = a;
	a = SkipWhite(++b);
    }
    /* tack on system palette at the end */
    ERR_IF1(!UGetFullTechDir(&techDir));
    sprintf(buf, "%s/system", techDir);
    ERR_IF1(!StrDup(&(palettes[i]), buf));

    /* init data structure for dialog box... */
    items = (dmWhichItem *) malloc((palettes_n) * sizeof(dmWhichItem));
    for (i = 0; i < palettes_n; i++) {
	items[i].itemName = palettes[i];
	items[i].userData = NULL;
	items[i].flag = 0;
    }
    return (TRUE);
}

/* 7/23/89 4/28/88 2/88
Uses global palettes variable.
*/
int 
RpcOpenPalette(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    int i;
    octObject facet;

    ViInit("open-palette");
    ErrClear();
    if (dmMultiWhich("Palettes", palettes_n, items, NULL, NULL) != VEM_OK) {
	PrintCon("Aborted entry");
	ViDone();
    }
    for (i = 0; i < palettes_n; i++) {
	if (items[i].flag != 0) {
	    if (ohOpenFacet(&facet, palettes[i], "schematic", "contents", "r")
		< 0) {
		PrintErr(octErrorString());
		ViDone();
	    }
	    vemOpenWindow(&facet, NULL);
	}
    }
    ViDone();
}

/***** End of RpcOpenPalette routines */


int 
RpcEditComment(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"Comment", 1, 80, NULL, NULL};
    char *comment;
    octObject facet, obj;
    vemStatus status;

    ViInit("edit-comment");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }

    status = vuFindSpot(spot, &obj, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
        ViDone();
    } else if (status == VEM_CANTFIND) {
	/* This should not be necessary, according to the manual, but it
	    doesn't seem to work without it.
	*/
	obj = facet;
    }

    /* At this point, obj is either an instance or the facet */
    if (!GetCommentProp(&obj, &comment)) {
	PrintErr(ErrGet());
        ViDone();
    }
    item.value = (comment == NULL) ? "" : comment;
    if (dmMultiText("Edit Comment", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        ViDone();
    }
    if (!SetCommentProp(&obj, item.value)) {
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}

/* Declare the functions in kernelCalls that the next two funcs use */
int numberOfDomains();
char* nthDomainName();

#ifdef BUG_FIXED		/* comment out for now */
/* This function builds the default palettes list.  It has one entry
 * for each domain, plus the user.pal and init.pal palettes and */
static char *defaultPalettes () {
    static char buf[1024];
    char line[128];
    int i, j, nDomains = numberOfDomains();
    buf[0] = 0;
    for (i = 0; i < nDomains; i++) {
	char* dn = nthDomainName(i);
	/* convert domain name to lowercase */
	char* ldn;
	for (j = 0; dn[j]; j++) {
		char c = dn[j];
		if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
		ldn[j] = c;
	}
	ldn[j] = 0;
	sprintf (line, "~ptolemy/src/domains/%s/icons/%s.pal:", ldn, ldn);
	strcat (buf, line);
    }
    strcat (buf, "./user.pal:./init.pal");
    return buf;
}
#endif

int
RpcEditDomain(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    dmWhichItem *items;
    octObject facet;
    char *domain, buf[MSG_BUF_MAX];
    int i, which, nDomains;
    char *temp;    /* for swapping two pointers to char */
    int current = 0;    /* actual index of current domain */

    ViInit("edit-domain");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    if (!GOCDomainProp(&facet, &domain, DEFAULT_DOMAIN)) {
        PrintErr(ErrGet());
        ViDone();
    }

    nDomains = numberOfDomains();

    /* init data structure for dialog box... */
    items = (dmWhichItem *) malloc(nDomains * sizeof(dmWhichItem));
    for (i = 0; i < nDomains; i++) {
        items[i].itemName = nthDomainName(i);
        items[i].userData = NULL;
        items[i].flag = 0;
	/* Because dmWhichOne always makes items[0] as the default
	   selected item, I put the current domain in items[0] so
	   that the current domain is the default new domain.  So
	   0 becomes the temperary index of the current domain.  The
	   actual index of the current domain is saved in "current". */
	if (i > 0 && !strcmp(items[i].itemName, domain)) {
	    current = i;  /* actual index of current domain is "i" */
	    /* swap items[0] and items[current] */
	    temp = items[0].itemName;
	    items[0].itemName = items[current].itemName;
	    items[current].itemName = temp;
        }
    }

    sprintf(buf, "domain = '%s'", domain);
    /* dmWhichOne has a bug.  It doesn't always set 'which' to -1 when
       no item is selected.  If the dialog box pops out with no default
       selected item and the user doesn't touch any selection button and
       directly clicks the "OK" or "Cancel" button, 'which' will be set
       to a large integer.  So I add the test 'which >= nDomains'. */
    if (dmWhichOne(buf, nDomains, items, &which, NULL, NULL) != VEM_OK
        || which < 0 || which >= nDomains) {
        PrintCon("Aborted entry");
        ViDone();
    }
    /* Since we swapped items[0] and items[current], we must 
       do the following adjustment on "which". */
    if (which == 0) {
	which = current;
    } else if (which == current) {
	which = 0;
    }
    domain = nthDomainName(which);
    if (!SetDomainProp(&facet, domain)) {
        PrintErr(ErrGet());
        ViDone();
    }
    sprintf(buf, "domain = '%s'", domain);
    PrintCon(buf);
    free(items);
    
    ViDone();
}

/* kernel Call */
void KcEditSeed();

int
RpcEditSeed(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    int n;
    char buf[64];
    static oldN = 1;
    static dmTextItem item = {"Seed for Random number", 1, 20,
				NULL, NULL};

    ViInit("edit-seed");
    ErrClear();
    FindClear();

    /* default seed is 1 */
    sprintf(buf, "%d", oldN);
    item.value = buf;
    if (dmMultiText("Edit-Seed", 1, &item) != VEM_OK) {
            PrintCon("Aborted entry");
            PrintErr(ErrGet());
	    ViDone();
    }
    ERR_IF2((n = atoi(item.value)) <= 0,
        "Invalid entry, number must be > 0");

    oldN = n;
    KcEditSeed(n);
    ViDone();
}

/* Maximum number of targets that can be supported by one domain */
#define MAX_NUM_TARGETS 50

/* Declare the functions in kernelCalls that the next func uses */
int KcDomainTargets();
char* KcDefTarget();

int
RpcEditTarget(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    dmWhichItem *items;
    octObject facet;
    char buf[MSG_BUF_MAX];
    char *target, *defaultTarget;
    int i, which, nTargets, galFlag = 0, nChoices;
    char *targetNames[MAX_NUM_TARGETS];

    ViInit("edit-target");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    if (!setCurDomainF(&facet)) {
	PrintErr("Unknown domain found; correct the domain first");
	ViDone();
    }
    nTargets = KcDomainTargets(targetNames,MAX_NUM_TARGETS);

    if(nTargets == 0) {
	PrintErr("No targets supported by current domain.");
	ViDone();
    }

    /* for a galaxy, add "<parent>" as an option */

    if (IsGalFacet(&facet)) {
	    galFlag = 1;
	    targetNames[nTargets] = defaultTarget = "<parent>";
    }
    else defaultTarget = KcDefTarget();

    nChoices = nTargets + galFlag;

    /* init data structure for dialog box... */
    items = (dmWhichItem *) malloc(nChoices * sizeof(dmWhichItem));

    for (i = 0; i < nChoices; i++) {
        items[i].itemName = targetNames[i];
        items[i].userData = NULL;
        items[i].flag = 0;
    }
    
    if (!GOCTargetProp(&facet, &target, defaultTarget)) {
        PrintErr(ErrGet());
        ViDone();
    }

    sprintf(buf, "target = '%s'", target);
/* put current target in "0" position, because of the way dmWhichOne works */
    for (i = 1; i < nChoices; i++) {
	if (strcmp(items[i].itemName, target) == 0) {
	    items[i].itemName = items[0].itemName;
	    items[0].itemName = target;
	}
    }
    if (dmWhichOne(buf, nChoices, items, &which, NULL, NULL) != VEM_OK
        || which == -1) {
        PrintCon("Aborted entry");
        ViDone();
    }

    target = items[which].itemName;

    if (!SetTargetProp(&facet, target)) {
        PrintErr(ErrGet());
        ViDone();
    }
    sprintf(buf, "target = '%s'", target);
    PrintCon(buf);
    free(items);
/* now edit target parameters */
    if (strcmp(target, "<parent>") != 0) {
	ParamListType pList;
	if (!GetTargetParams(target, &facet, &pList)) {
	    PrintErr(ErrGet());
	    ViDone();
	}
	if (pList.length > 0 && EditParamList(&pList,"Edit Target Params")) {
	    SetTargetParams(&facet, &pList);
        }
    }
    ViDone();
}

