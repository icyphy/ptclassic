/*
 * octfix
 * Hacked version of octmvlib.
 * Improved to support multiple (old,new) pairs by Joe Buck.
 * Improved to support view-name matching and replacement by Kennard.
 * Improved to support full string matches (instead of partial) by Kennard.
 *
 * $Id$
 */
#include "port.h"
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "oh.h"

optionStruct optionList[] = {
    {"N",	"newpath",	"new path name"},
    {"O",	"oldpath",	"old path name"},
    {"r",       0,              "refresh instances"},
    {"i",       0,              "instantiate \"interface\" facet" },
    {"c",       0,              "instantiate \"contents\" facet" },
    {"o",	"[cell]:view",	"output cell:view"},
    {"v",       0,              "verbose"},
    { OPT_RARG,	"cell:view",	"facet defaults to `contents'" },
    {0, 0, 0}};

void bomb(str)
char * str;
{
    fprintf(stderr, "octmvlib: fatal: %s\n");
}

/* 
* ohReplaceInstance() is almost good, except that it does not allow
* us to force a change of instance when the terminals have changed.
* The output of this routine may be incorrect, but at least
* we have updated the instances.
*/
void
replaceInstance(newInst, oldInst, force)
    octObject *newInst, *oldInst;
    int force;
{
    octObject new_term, term, facet;
    octGenerator gen;
    octStatus status;

    octGetFacet(oldInst, &facet);
    if ( octCreate(&facet, newInst) != OCT_OK ) {
	errRaise( "octmvlib", 1, "New path %s is not valid",
		 newInst->contents.instance.master );
    }

    /*
     *  Copy attachements for objects attached to the instance
     */
    ohAttachContents(oldInst, newInst, OCT_ALL_MASK &~ OCT_TERM_MASK);
    ohAttachContainers(oldInst, newInst, OCT_ALL_MASK &~ OCT_FACET_MASK);
    /*
     *  Copy all objects attached to each terminal
     */
    OH_ASSERT(octInitGenContents(oldInst, OCT_TERM_MASK, &gen));
    while ((status = octGenerate(&gen, &term)) == OCT_OK) {
	/* Find corresponding terminal on the new cell */
	new_term = term;
	status = octGetByName(newInst, &new_term);
	if (status == OCT_OK) {
	    ohAttachContents(&term, &new_term, OCT_ALL_MASK);
	    ohAttachContainers(&term, &new_term, OCT_ALL_MASK&~OCT_INSTANCE_MASK);
	} else if (status == OCT_NOT_FOUND) {
	    if ( force ) {
		fprintf( stderr,  "Formal/Actual Terminal mismatch: %s",
			ohFormatName(&term) );
	    } else {
		errRaise(OH_PKG_NAME, OCT_ERROR, "Formal/Actual Terminal mismatch: %s",
			 ohFormatName(&term));
	    }
	} else {
	    OH_ASSERT(status);
	}
    }
    OH_ASSERT(status);
    OH_ASSERT(octDelete(oldInst));		/* delete the old instance */
    return;
}

#define MAX_PATHS 50

main(argc, argv)
int argc;
char **argv;
{
    char *input_name, *output_name;
    char *oldpath[MAX_PATHS];
    int oldlen[MAX_PATHS];
    char *oldview[MAX_PATHS];
    char *newpath[MAX_PATHS];
    char *newview[MAX_PATHS];
    int n_oldp = 0, n_newp = 0;
    octObject outfacet, facet, instance, newinstance;
    octGenerator gen;
    int option;
    int verbose = 0;
    int force = 0;
    char* forceFacet = 0;
    int count = 0;		/* Used to prevent infinite looping in replace loop. */
    int subst = 0;		/* Number of substitutions. */
    extern int optind;
    extern char *optarg;

    /* parse command line options*/
    output_name = NIL(char);
    
    while ((option = optGetOption(argc, argv)) != EOF) {
	switch(option) {
	case 'i':
	    forceFacet = "interface";
	    break;
	case 'c':
	    forceFacet = "contents";
	    break;
	case 'r':
	    force = 1;
	    oldpath[0] = "";
	    newpath[0] = "";
	    n_oldp = n_newp = 1;
	    oldlen[0] = 0;
	    break;
	case 'O':
	    if (n_oldp >= MAX_PATHS) bomb("Too many -O options");
	    oldpath[n_oldp] = util_strsav(optarg);
	    if ( (oldview[n_oldp]=strchr(oldpath[n_oldp],':')) != NULL ) {
		*(oldview[n_oldp]) = '\0';
		if ( *(++(oldview[n_oldp])) == '\0' ) {
		    oldview[n_oldp] = NULL;
		}
		oldlen[n_oldp] = -1;
	    } else {
	        oldlen[n_oldp] = strlen(oldpath[n_oldp]);
	    }
	    ++n_oldp;
	    break;

	case 'N':
	    if (n_newp >= MAX_PATHS) bomb("Too many -N options");
	    newpath[n_newp] = util_strsav(optarg);
	    if ( (newview[n_newp]=strchr(newpath[n_newp],':')) != NULL ) {
		*(newview[n_newp]) = '\0';
		if ( *(++(newview[n_newp])) == '\0' ) {
		    newview[n_newp] = NULL;
		}
	    }
	    ++n_newp;
	    break;

	case 'o':
	    output_name = util_strsav(optarg);
	    break;
	case 'v':
	    verbose = 1;
	    break;
	}
    }

    if (n_newp == 0) {
	(void) fprintf(stderr, "no newpath was specified\n");
	optUsage();
    }
    if (n_newp != n_oldp) {
	(void) fprintf(stderr, "number of -N and -O options must be equal\n");
	optUsage();
    }

    /*
     *  Finish the command line argument parsing 
     */
    if (optind != argc-1) {
	optUsage();
    } else {
	input_name = argv[optind];
    }

    octBegin();
    
    /*
     *  Get the input and output facet names
     */
    ohUnpackDefaults(&facet, "a", ":schematic:contents");
    OH_ASSERT(ohUnpackFacetName(&facet, input_name));

    /*
     *  Unpack the output name (if present), and make a copy of the facet
     */
    if (output_name != NIL(char)) {
	outfacet = facet;
	if (ohUnpackFacetName(&outfacet, output_name) == OCT_OK) {
	    /* Copy the contents facet */
	    facet.contents.facet.facet = "contents";
	    outfacet.contents.facet.facet = "contents";
	    if (octCopyFacet(&outfacet, &facet) != OCT_OK) {
		octError("copying the contents facet");
		exit(-1);
	    }

	    /* Copy the interface facet if it exists */
	    facet.contents.facet.facet = "interface";
	    outfacet.contents.facet.facet = "interface";
	    if (octCopyFacet(&outfacet, &facet) != OCT_OK) {
		octError("copying the interface facet");
		(void) fprintf(stderr, "continuing\n");
	    }

	    facet = outfacet;
	    facet.contents.facet.facet = "contents";
	}
    }

    /* 
     *  Open the facet 
     */

    {
	octStatus   status = octOpenFacet(&facet);
	if ( status != OCT_OLD_FACET && status != OCT_INCONSISTENT ) {
	    errRaise( "octmvlib", 1, "Cannot open %s\n%s\n",
		     ohFormatName( &facet ), octErrorString);
	}
    }

    OH_ASSERT(octInitGenContents(&facet, OCT_INSTANCE_MASK, &gen));

    /* Avoid infinite loops. */
    count = ohCountContents( &facet, OCT_INSTANCE_MASK );
    while (octGenerate(&gen, &instance) == OCT_OK) {
	int old_len;
        char * new_path, *new_view, *old_path, *old_view;
	int idx;
	char newname[2048];

	if ( count-- == 0 ) break;

	new_path = NULL;
        for (idx = 0; idx < n_oldp; idx++) {
	    old_path = instance.contents.instance.master;
	    old_view = instance.contents.instance.view;
	    old_len = oldlen[idx];
	    if((old_len>0 && strncmp(old_path,oldpath[idx],old_len)==0)
	      || strcmp(old_path,oldpath[idx])==0 ) {
		if ( oldview[idx]!=NULL && strcmp(old_view,oldview[idx])!=0 )
		    continue;
		new_path = newpath[idx];
		new_view = newview[idx];
		break;
	    }
	}
	if (new_path==NULL)
	    continue;


	(void) strcpy(newname, new_path);
	if ( old_len > 0 ) {
	    (void) strcat(newname, &old_path[old_len]);
	}
	newinstance = instance;
	newinstance.contents.instance.master = newname;
	if ( new_view ) {
	    newinstance.contents.instance.view = new_view;
	}
	if ( forceFacet ) {
	    newinstance.contents.instance.facet = forceFacet;
	}
	if ( verbose ) {
	    printf( "Replacing \n\tOLD: %s\n\tNEW: %s\n", ohFormatName(&instance), ohFormatName(&newinstance) );
	    fflush( stdout );
	}
	replaceInstance(&newinstance, &instance, force);
	subst++;
    }
    printf( "octmvlib: %d substitutions.\n", subst );
    octFreeGenerator( &gen );
    OH_ASSERT(octCloseFacet(&facet));
    
    exit(0);
}


