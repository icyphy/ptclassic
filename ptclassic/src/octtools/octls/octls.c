/*
 * octls.c
 * Original version is by Phil Lapsley for Gabriel.
 * Tweaked by kennard
 *
 * $Id$
 */

#include "port.h"
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "oh.h"
#include <sys/file.h>

optionStruct optionList[] = {
    {"c",       0,              "check masters"},
    {"f",       0,              "full master name"},
    {0, 0, 0}
};

int	checkmasters = 0;
int	printfull = 0;


printFacetPath( pntname, kid)
char *pntname;
struct octInstance *kid;
{
	if ( printfull ) {
		printf("%s:%s:%s\n", kid->master, kid->view, kid->facet);
	} else {
		printf("%s %s\n", pntname, kid->master);
	}
}

main(argc, argv)
int argc;
char **argv;
{
    char *input_name;
    octObject facet, instance;
    octGenerator gen;
    int count = 0;
    extern int optind;
    extern char *optarg;

    if (options(argc, argv) == -1) {
	exit(1);
    }

    if (argc - optind != 1) {
	optUsage();
    }
    input_name = argv[optind];

    octBegin();
    
    /*
     *  Get the input and output facet names
     */
    ohUnpackDefaults(&facet, "r", ":schematic:contents");
    OH_ASSERT(ohUnpackFacetName(&facet, input_name));

    /* 
     *  Open the facet 
     */
    if (octOpenFacet(&facet) < OCT_OK) {
	octError("can not open the facet");
	exit(-1);
    }

    OH_ASSERT(octInitGenContents(&facet, OCT_INSTANCE_MASK, &gen));

    /* Avoid infinite loops. */
    count = ohCountContents( &facet, OCT_INSTANCE_MASK );
    while (octGenerate(&gen, &instance) == OCT_OK) {
	if ( count-- == 0 ) break;
	if (checkmasters) {
		if (bad_master(instance.contents.instance.master)) {
			printFacetPath(input_name,&instance.contents.instance);
		}
	} else {
		printFacetPath(input_name,&instance.contents.instance);
	}
    }
    octFreeGenerator(&gen);
    OH_ASSERT(octCloseFacet(&facet));
    
    exit(0);
}

/*
 * Return 1 if the master is bad (doesn't exist), 0 otherwise.
 */
int
bad_master(master)
	char	*master;
{
	int	ret;
	char	*p;

	p = util_tilde_expand(master);
	ret = access(p, F_OK);

	return ((ret == -1) ? 1 : 0);
}


int
options(argc, argv)
	int	argc;
	char	*argv[];
{
	int	option;

	while ((option = optGetOption(argc, argv)) != EOF) {
		switch(option) {
		case 'c':
			checkmasters = 1;
			break;
		case 'f':
			printfull = 1;
			break;
		}
	}

	return (0);
}
