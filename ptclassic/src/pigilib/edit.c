/* edit.c  edg
Version identification:
$Id$
*/


/* Includes */
#include <stdio.h>
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

#define dmWidth 40
#define dmIncrement 20

/* EditDelayParams  5/27/88
Edit the delay value associated with a delay instance.
Inputs: instPtr = delay instance pointer.
Outputs: return = TRUE if no error.
*/
static boolean
EditDelayParams(facetPtr, instPtr)
octObject *facetPtr, *instPtr;
{
    octObject delayProp;
    static dmTextItem item = {"Number of delays", 1, 10, NULL, NULL};
    int n;
    char buf[100];

    GetOrInitDelayProp(instPtr, &delayProp);
    item.value = sprintf(buf, "%ld", delayProp.contents.prop.value.integer);
    if (dmMultiText("Edit Delay", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        return(TRUE);
    }
    if ((n = atoi(item.value)) <= 0) {
	ErrAdd("Invalid entry: number must be > 0");
        return(FALSE);
    }
    delayProp.contents.prop.value.integer = (long) n;
    (void) octModify(&delayProp);
    return(TRUE);
}

/* EditSogParams  5/27/88
Outputs: return = TRUE if no error.
*/
static boolean
EditSogParams(facetPtr, instPtr)
octObject *facetPtr, *instPtr;
{
    int i, width, maxwidth;
    ParamListType pList;
    ParamType *place;
    dmTextItem *items;

/* set domain corresponding to the instance */
    setCurDomainInst(instPtr);

    ERR_IF1(!GetOrInitSogParams(instPtr, &pList));

    if (pList.length == 0) {
	PrintErr("Star or galaxy has no parameters");
	return(TRUE);
    }
    items = (dmTextItem *) calloc(pList.length, sizeof(dmTextItem));
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
    if (dmMultiText("Edit Actual Parameters", pList.length, items) != VEM_OK) {
	PrintCon("Aborted entry");
	return(TRUE);
    }
    /* should free place->value string before assignment */
    place = pList.array;
    for (i = 0; i < pList.length; i++) {
	if (!IsBalancedParen(items[i].value)) {
	    char buf[512];
	    sprintf(buf, "Unbalanced parentheses in parameter `%s`",
		place->name);
	    ErrAdd(buf);
	    return (FALSE);
	}
	place->value = items[i].value;
	place++;
    }
    free(items);

    SetSogParams(instPtr, &pList);
    return(TRUE);
}

/* 4/14/89 3/19/89
Edit the formal param list of a galaxy schematic facet.
*/
static boolean
EditFormalParams(galFacetPtr)
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
    items = (dmTextItem *) calloc(itemsN, sizeof(dmTextItem));
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
	return(TRUE);
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
    return(TRUE);
}

/* EditParams  5/27/88 4/24/88
Edit parameters.
*/
int 
EditParams(spot, cmdList, userOptionWord)
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
	    PrintErr("Cursor must be over an instance or a galaxy schematic");
	    ViDone();
	}
    } else {
	/* cursor is over some type of instance... */
	if (IsDelay(&inst)) {
	    status2 = EditDelayParams(&facet, &inst);
	} else if (IsGal(&inst) || IsStar(&inst)) {
	    /* inst is a sog... */
	    status2 = EditSogParams(&facet, &inst);
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

    sprintf(buf, "%s.palettes", UAppName);
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
RpcOpenPalette(spot, cmdList, userOptionWord)
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
RpcEditComment(spot, cmdList, userOptionWord)
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
RpcEditDomain(spot, cmdList, userOptionWord)
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
    PrintCon(sprintf(buf, "domain = '%s'", domain));
    free(items);
    ViDone();
}

/* Maximum number of architectures that can be supported by one domain */
#define MAX_NUM_ARCHS 50

/* Declare the functions in kernelCalls that the next func uses */
int numberOfArchs();
char* nthArchName();

int
RpcEditArch(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    dmWhichItem *items;
    octObject facet;
    char *domain, buf[MSG_BUF_MAX];
    char *arch;
    int i, which, nArchs;
    int archIndices[MAX_NUM_ARCHS];

    ViInit("edit-architecture");
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

    /* find out how many target architectures there are, and their indices. */
    nArchs = numberOfArchs(domain, archIndices);

    if(nArchs == 0) {
	PrintErr("No architectures supported by current domain.");
	ViDone();
    }

    /* init data structure for dialog box... */
    items = (dmWhichItem *) malloc(nArchs * sizeof(dmWhichItem));
    for (i = 0; i < nArchs; i++) {
        items[i].itemName = nthArchName(archIndices[i]);
        items[i].userData = NULL;
        items[i].flag = 0;
    }

    if (!GOCArchProp(&facet, &arch, nthArchName(archIndices[0]))) {
        PrintErr(ErrGet());
        ViDone();
    }

    sprintf(buf, "architecture = '%s'", arch);
    if (dmWhichOne(buf, nArchs, items, &which, NULL, NULL) != VEM_OK
        || which == -1) {
        PrintCon("Aborted entry");
        ViDone();
    }
    arch = nthArchName(archIndices[which]);
    if (!SetArchProp(&facet, arch)) {
        PrintErr(ErrGet());
        ViDone();
    }
    PrintCon(sprintf(buf, "architecture = '%s'", arch));
    free(items);
    ViDone();
}
