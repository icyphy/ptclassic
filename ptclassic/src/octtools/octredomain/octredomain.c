/* Version Identification:
 * $Id$
 */
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
 * octredomain
 * Hacked version of octredomain, which is in turn a hacked version of octmvlib
 * redomain a facet
 */
#include "port.h"
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "oh.h"
#include "errtrap.h"

optionStruct optionList[] = {
    {"d",	"SDF",		"new domain"},
    {"o",	"[cell]:view",	"output cell:view"},
    {"v",       0,              "verbose"},
    { OPT_RARG,	"cell:view",	"facet defaults to `contents'" },
    {0, 0, 0}};

void bomb(str)
char * str;
{
    fprintf(stderr, "octredomain: fatal: %s\n", str);
}

int main(int argc, char **argv)
{
    char *input_name = (char *)NULL, *output_name;
    char *newDomainName = (char *)NULL;
    octObject outfacet, facet, paramProp;
    octGenerator gen;
    int option;
    int verbose = 0;
    extern int octoptind;
    extern char *optarg;

    /* parse command line options*/
    output_name = NIL(char);
    
    while ((option = optGetOption(argc, argv)) != EOF) {
	switch(option) {
	case 'd':
	    newDomainName = util_strsav(optarg);
	    break;
	case 'o':
	    output_name = util_strsav(optarg);
	    break;
	case 'v':
	    verbose = 1;
	    break;
	}
    }

    if (newDomainName == (char *)NULL) {
	(void) fprintf(stderr, "no new domain name was specified\n");
	optUsage();
    }

    /*
     *  Finish the command line argument parsing 
     */
    if (octoptind != argc-1) {
	optUsage();
    } else {
	input_name = argv[octoptind];
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
	    errRaise( "octredomain", 1, "Cannot open %s\n%s\n",
		     ohFormatName( &facet ), octErrorString());
	}
    }

    OH_ASSERT(octInitGenContents(&facet, OCT_INSTANCE_MASK, &gen));

    OH_ASSERT(ohGetByPropName( &facet, &paramProp, "domain"));
    printf ("%s\n",paramProp.contents.prop.value.string);
    OH_ASSERT(ohCreateOrModifyPropStr(&facet,&paramProp,"domain",newDomainName));

    octFreeGenerator( &gen );
    OH_ASSERT(octCloseFacet(&facet));
    
    return 0;
}


