#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "options.h"
#include "oct.h"
#include "oh.h"
#include "vov.h"

extern void process();		/* process.c */

int force_remake = 0;
int process_subcells = 0;
int remove_connectors = 0;
extern char *optProgName;

optionStruct optionList[] = {
    { "c",	0,		"flatten 'CONNECTORS'"},
    { "f",	"file",		"filename containing the 'program'"},
    { "m",	0,		"force re-make of any subcells"},
    { "o",	":view",	"specify output view name"},
    { "r",	0,		"process subcells recursively"},
    { "s",	"factor",	"scale all geometry by 'factor'"},
    { OPT_RARG,	"cell:view",	"facet to modify (view defaults to 'physical')"},
    { OPT_DESC,	0,		"Peform mask modifications on the facet"},
    { 0,	0,		0}
};


int
main(argc, argv)
int argc;
char **argv;
{
    extern char *optarg;
    extern int optind;
    int option;
    char *out_view, *scale, *filename;
    extern void process();
    octObject facet;

    /* parse command line options*/
    filename = NIL(char);
    scale = NIL(char);
    out_view = "";
    while ((option = optGetOption(argc, argv)) != EOF) {
	switch(option) {
	case 'c':
	    remove_connectors = 1;
	    break;
	case 'f':
	    filename = util_strsav(optarg);
	    break;
	case 'm':
	    force_remake = 1;
	    process_subcells = 1;
	    break;
	case 'o':
	    out_view = strchr(optarg, ':');
	    if ( out_view == 0 ) {
		errRaise( "octmm", 1, "If you use the option -o you must specify\n\tthe output view name ('cell:view' or ':view').\n" );
	    }
	    out_view = util_strsav(out_view+1); /* Move over the column  */
	    break;
	case 'r':
	    process_subcells = 1;
	    break;
	case 's':
	    scale = util_strsav(optarg);
	    break;
	}
    }

    /*
     *  Finish the command line argument parsing 
     */
    if (optind + 1 != argc) {
	optUsage();
    }

    VOVbegin( argc, argv );
    ohUnpackDefaults(&facet, "a", ":physical:contents");
    if (ohUnpackFacetName(&facet, argv[optind]) != OCT_OK) {
	optUsage();
    }

    if (out_view == NIL(char)) {
	out_view = facet.contents.facet.view;
    }

    process(facet.contents.facet.cell, facet.contents.facet.view,
				       out_view, scale, filename);
    octEnd();
    VOVend(0);
    return 0;
}
