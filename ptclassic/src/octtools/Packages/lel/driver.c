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
 * Testing driver for lelFindProp and lelLabelEval
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This routine opens a test cell and evaluates all labels in the cell
 * and prints out their representations textually.
 */

#include "port.h"
#include "oct.h"
#include "oh.h"
#include "options.h"
#include "lel.h"

optionStruct optionList[] = {
    { OPT_RARG,	"cell{:view{:facet}}",	"Input cell"	},
    { OPT_DESC, 0,
	"Prints out all labels in the specified cell after processing " },
    { OPT_CONT, 0,
	"them using lelEvalLabel.  Used for testing lelEvalLabel and " },
    { OPT_CONT, 0, "lelFindProp." },
    { 0, 0, 0 }
};

static void print_facet();
static void trace_labels();
static void print_label();

main(argc, argv)
int argc;
char *argv[];
{
    octObject fct;
    int option;

    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	default:
	    optUsage();
	}
    }
    if (optind != argc-1) {
	(void) fprintf(stderr, "%s: no input cell specified\n",
		       optProgName);
	optUsage();
    }
    (void) ohUnpackDefaults(&fct, "r", ":physical:contents");
    if ((ohUnpackFacetName(&fct, argv[optind]) < OCT_OK) ||
	(octOpenFacet(&fct) < OCT_OK)) {
	(void) fprintf(stderr, "%s: cannot open cell `%s':\n  %s\n",
		       optProgName, argv[optind], octErrorString());
	exit(1);
    }
    print_facet(&fct, 0);
    trace_labels(&fct, (octObject *) 0, 1);
    return 0;
}	

static void print_facet(fct, indent)
octObject *fct;
int indent;
/*
 * Prints out a nice representation of the facet name with
 * the specified indent.
 */
{
    int i;

    for (i = 0;  i < indent;  i++) putchar(' ');
    (void) printf("%s:%s:%s\n", fct->contents.facet.cell,
		  fct->contents.facet.view, fct->contents.facet.facet);
}
  
static void trace_labels(parent, inst, indent)
octObject *parent;
octObject *inst;
int indent;
/* 
 * Generates through all labels connected to `parent' and displays
 * them textually.  If this is looking in a master of an instance,
 * `inst' should be passed for correct evaluation of property values.
 * This routine is called recursively on each object encountered.
 */
{
    octObject obj, master;
    octGenerator gen;

    OH_ASSERT(octInitGenContents(parent, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	switch (obj.type) {
	case OCT_LABEL:
	    print_label(parent, inst, &obj, indent);
	    break;
	case OCT_INSTANCE:
	    trace_labels(&obj, inst, indent);
	    master.type = OCT_FACET;
	    master.contents.facet.facet = "interface";
	    master.contents.facet.mode = "r";
	    if (octOpenMaster(&obj, &master) >= OCT_OK) {
		print_facet(&master, indent+1);
		trace_labels(&master, &obj, indent+2);
	    }
	    break;
	default:
	    trace_labels(&obj, inst, indent);
	    break;
	}
    }
}

static void print_label(parent, inst, lbl, indent)
octObject *parent, *inst, *lbl;
int indent;
/*
 * Evaluates label and prints it out textually.
 */
{
    int i;

    for (i = 0;  i < indent;  i++) putchar(' ');
    (void) lelEvalLabel(inst, lbl, "interface");
    (void) printf("Container %s: `%s'\n", ohFormatName(parent),
		  lbl->contents.label.label);
}
