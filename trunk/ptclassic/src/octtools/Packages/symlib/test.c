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
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "region.h"
#include "tap.h"
#include "symlib.h"


      
static int
printProp(prop)
octObject *prop;
{
    (void) printf("\t\t%s=", prop->contents.prop.name);
    switch (prop->contents.prop.type) {
	case OCT_INTEGER:
	(void) printf("%ld\n", (long)prop->contents.prop.value.integer);
	break;

	case OCT_REAL:
	(void) printf("%g\n", prop->contents.prop.value.real);
	break;

	case OCT_STRING:
	(void) printf("%s\n", prop->contents.prop.value.string);
	break;

	default:
	(void) printf("...\n");
	break;
    }
    return 1;
}

/*ARGSUSED*/
int
main(argc, argv)
int argc;
char **argv;
{
    octObject facet, fterm, net, t1, t2;
    octObject ndif, met1, met2, pdif, poly, inst1, inst2;
    octObject loadSource, loadDrain, loadGate, loadCon;
    octObject driverSource, driverDrain, driverGate, driverCon;
    octObject input, output;
    struct octPoint pt, dummyPt1, dummyPt2;
    char *techdir;
    char buffer[4096];
    symSegment array[1000];
    int i;
    octObject inst, term, prop;
    octGenerator gen, tgen, pgen;

    errProgramName(argv[0]);

    /* open the facet */
    OH_ASSERT(ohUnpackFacetName(&facet, "bob:symbolic:contents"));
    symOpenFacet(&facet, "w", "scmos");
    techdir = tapGetDirectory(&facet);
    (void) printf("techdir is %s\n", techdir);

    /* build an inverter */

    OH_ASSERT(ohGetOrCreateLayer(&facet, &poly, "POLY"));
    OH_ASSERT(ohGetOrCreateLayer(&facet, &met1, "MET1"));
    OH_ASSERT(ohGetOrCreateLayer(&facet, &met2, "MET2"));
    OH_ASSERT(ohGetOrCreateLayer(&facet, &ndif, "NDIF"));
    OH_ASSERT(ohGetOrCreateLayer(&facet, &pdif, "PDIF"));

    /* load */
    inst1.type = OCT_INSTANCE;
    (void) sprintf(buffer, "%s/primitives/pmos.8x4", techdir);
    inst1.contents.instance.master = buffer;
    inst1.contents.instance.view = "physical";
    inst1.contents.instance.facet = "contents";
    inst1.contents.instance.version = OCT_CURRENT_VERSION;
    inst1.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    inst1.contents.instance.transform.translation.x = 0;
    inst1.contents.instance.transform.translation.y = 2000;
    symCreateInstance(&facet, &inst1);


    OH_ASSERT(ohCreatePropStr(&inst1, &prop, "MOSFETTYPE", "overridden"));
    OH_ASSERT(ohCreatePropStr(&inst1, &prop, "NEW-PROP", "added"));
    OH_ASSERT(ohGetByTermName(&inst1, &loadDrain, "DRAIN"));
    symNameObject(&inst1, "abc");

    pt.x = 0;
    pt.y = 2500;
    symCreateSegment(&pdif, 
		     &dummyPt1, &loadDrain, SYM_USE_EXISTING,
		     &pt, &loadCon, SYM_CREATE,
		     80, SYM_VERTICAL);
    symCreateFormalTerm(&facet, &fterm, "VDD");
    symImplementFormalTerm(&fterm, &loadCon);

    OH_ASSERT(ohGetByTermName(&inst1, &loadSource, "SOURCE"));
    pt.x = 0;
    pt.y = 1500;
    symCreateSegment(&pdif, 
		     &dummyPt1, &loadSource, SYM_USE_EXISTING,
		     &pt, &loadCon, SYM_CREATE, 80, SYM_VERTICAL);

    /* driver */
    inst2.type = OCT_INSTANCE;
    (void) sprintf(buffer, "%s/primitives/nmos.4x4", techdir);
    inst2.contents.instance.master = buffer;
    inst2.contents.instance.view = "physical";
    inst2.contents.instance.facet = "contents";
    inst2.contents.instance.version = OCT_CURRENT_VERSION;
    inst2.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    inst2.contents.instance.transform.translation.x = 0;
    inst2.contents.instance.transform.translation.y = 500;
    symCreateInstance(&facet, &inst2);
    

    symSideConstraint( &inst1, SYM_PUT_CONST_LEFT, 2000, 40000 );
    symNormalConstraint( SYM_VERTICAL, &inst1, &inst2, 0, SYM_PUT_CONST_UNDEFINED );
    symActiveConstraint( SYM_VERTICAL, &inst1, &inst2, &inst1, &inst2  );
    symActiveSideConstraint( SYM_PUT_CONST_RIGHT, &inst1, &inst2  );

    OH_ASSERT(ohGetByTermName(&inst2, &driverSource, "SOURCE"));
    OH_ASSERT(ohCreatePropStr(&driverSource, &prop, "TERMTYPE", "overridden"));
    OH_ASSERT(ohCreatePropStr(&driverSource, &prop, "NEW-TPROP", "added"));
    pt.x = 0;
    pt.y = 0;
    symCreateSegment(&ndif, 
		     &dummyPt1, &driverSource, SYM_USE_EXISTING,
		     &pt, &driverCon, SYM_CREATE,
		     80, SYM_VERTICAL);
    symCreateFormalTerm(&facet, &fterm, "GND");
    symImplementFormalTerm(&fterm, &driverCon);
    symGetNet(&fterm, &net);
    symNameObject(&net, "source");
   
    OH_ASSERT(ohGetByTermName(&inst2, &driverDrain, "DRAIN"));
    pt.x = 0;
    pt.y = 1000;
    symCreateSegment(&ndif, 
		     &dummyPt1, &driverDrain, SYM_USE_EXISTING,
		     &pt, &driverCon, SYM_CREATE,
		     80, SYM_VERTICAL);
    symCreateSegment(&met1, 
		     &dummyPt1, &loadCon, SYM_USE_EXISTING,
		     &dummyPt2, &driverCon, SYM_USE_EXISTING,
		     80, SYM_VERTICAL);
    /* output */
    pt.x = 500;
    pt.y = 1500;
    symCreateSegment(&met1, 
		     &dummyPt1, &loadCon, SYM_USE_EXISTING,
		     &pt, &output, SYM_CREATE,
		     80, SYM_VERTICAL);
    symCreateFormalTerm(&facet, &fterm, "OUTPUT");
    symImplementFormalTerm(&fterm, &output);
    symGetNet(&fterm, &net);
    symNameObject(&net, "fred");
    
    /* connect inputs */
    OH_ASSERT(ohGetByTermName(&inst1, &loadGate, "GATE"));
    pt.x = -500;
    pt.y = 2000;
    symCreateSegment(&poly, 
		     &dummyPt1, &loadGate, SYM_USE_EXISTING,
		     &pt, &loadCon, SYM_CREATE,
		     80, SYM_VERTICAL);
    
    OH_ASSERT(ohGetByTermName(&inst2, &driverGate, "GATE"));
    pt.x = -500;
    pt.y = 500;
    symCreateSegment(&poly, 
		     &dummyPt1, &driverGate, SYM_USE_EXISTING,
		     &pt, &driverCon, SYM_CREATE,
		     80, SYM_VERTICAL);
    symCreateSegment(&poly, 
		     &dummyPt1, &loadCon, SYM_USE_EXISTING,
		     &dummyPt2, &driverCon, SYM_USE_EXISTING,
		     80, SYM_VERTICAL);
    
    pt.x = -1000;
    pt.y = 2000;
    symCreateSegment(&poly, 
		     &dummyPt1, &loadCon, SYM_USE_EXISTING,
		     &pt, &input, SYM_CREATE,
		     80, SYM_VERTICAL);
    
    symCreateFormalTerm(&facet, &fterm, "INPUT");
    symImplementFormalTerm(&fterm, &input);
    
    symCreateNet(&facet, &net);
    symNameObject(&net, "bigWilly");
    symAddTermToNet(&fterm, &net);

    /* do a route */
    dummyPt1.x = 5000;
    dummyPt1.y = 0;
    dummyPt2.x = 5000;
    dummyPt2.y = 1000;
    t1.objectId = t2.objectId = oct_null_id;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_USE_EXISTING_OR_CREATE,
		     &dummyPt2, &t2, SYM_USE_EXISTING_OR_CREATE,
		     -1, SYM_VERTICAL);

    pt.x = 6000;
    pt.y = 1000;
    symCreateSegment(&met1, 
		     &dummyPt1, &t2, SYM_USE_EXISTING,
		     &pt, &t1, SYM_CREATE,
		     -1, SYM_HORIZONTAL);
    pt.x = 6000;
    pt.y = 2000;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_USE_EXISTING,
		     &pt, &t2, SYM_CREATE,
		     200, SYM_VERTICAL);
    pt.x = 5500;
    pt.y = 2000;
    symCreateSegment(&met1, 
		     &dummyPt1, &t2, SYM_USE_EXISTING,
		     &pt, &t1, SYM_CREATE,
		     -1, SYM_HORIZONTAL);
    pt.x = 5500;
    pt.y = 3000;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_USE_EXISTING,
		     &pt, &t2, SYM_CREATE,
		     -1, SYM_VERTICAL);
    dummyPt1.x = 7000;
    dummyPt1.y = 0;
    dummyPt2.x = 7000;
    dummyPt2.y = 3000;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_CREATE,
		     &dummyPt2, &t2, SYM_CREATE,
		     -1, SYM_VERTICAL);
    dummyPt1.x = 4000;
    dummyPt1.y = 0;
    dummyPt2.x = 4000;
    dummyPt2.y = 1500;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_CREATE,
		     &dummyPt2, &t2, SYM_CREATE,
		     -1, SYM_VERTICAL);
    pt.x = 8000;
    pt.y = 1500;
    symCreateSegment(&met1, 
		     &dummyPt1, &t2, SYM_USE_EXISTING,
		     &pt, &t1, SYM_CREATE,
		     -1, SYM_HORIZONTAL);
    pt.x = 8000;
    pt.y = 3000;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_USE_EXISTING,
		     &pt, &t2, SYM_CREATE,
		     -1, SYM_HORIZONTAL);

    /* test 'pt' use existing */
    dummyPt1.x = 5000;
    dummyPt1.y = 1000;
    dummyPt2.x = 5000;
    dummyPt2.y = 3000;
    t1.objectId = t2.objectId = oct_null_id;
    symCreateSegment(&met2,
		     &dummyPt1, &t1, SYM_USE_EXISTING,
		     &dummyPt2, &t2, SYM_CREATE,
		     -1, SYM_VERTICAL);

    /* do a another route */
    /* put down a formal terminal */
    inst1.type = OCT_INSTANCE;
    (void) sprintf(buffer, "%s/primitives/m2m2.4x4", techdir);
    inst1.contents.instance.master = buffer;
    inst1.contents.instance.view = "physical";
    inst1.contents.instance.facet = "contents";
    inst1.contents.instance.version = OCT_CURRENT_VERSION;
    inst1.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    inst1.contents.instance.transform.translation.x = -4000;
    inst1.contents.instance.transform.translation.y = -1000;
    symCreateInstance(&facet, &inst1);
    symNameObject(&inst1, "BOB1");
    symCreateFormalTerm(&facet, &fterm, "BOB2");
    OH_ASSERT(ohGetByTermName(&inst1, &t1, "t"));
    symImplementFormalTerm(&fterm, &t1);
    symCreateNet(&facet, &net);
    symNameObject(&net, "BOB3");
    symAddTermToNet(&fterm, &net);

    dummyPt1.x = -4000;
    dummyPt1.y = -1000;
    dummyPt2.x = -5000;
    dummyPt2.y = -1000;
    t2.objectId = oct_null_id;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_USE_EXISTING_OR_CREATE,
		     &dummyPt2, &t2, SYM_USE_EXISTING_OR_CREATE,
		     -1, SYM_HORIZONTAL);
    
    dummyPt1.x = -5000;
    dummyPt1.y = -2000;
    dummyPt2.x = -6000;
    dummyPt2.y = -2000;
    t1.objectId = t2.objectId = oct_null_id;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_USE_EXISTING_OR_CREATE,
		     &dummyPt2, &t2, SYM_USE_EXISTING_OR_CREATE,
		     -1, SYM_HORIZONTAL);

    /* merge two seperate nets */
    dummyPt1.x = -5000;
    dummyPt1.y = -1000;
    dummyPt2.x = -5000;
    dummyPt2.y = -2000;
    t1.objectId = t2.objectId = oct_null_id;
    symCreateSegment(&met2, 
		     &dummyPt1, &t1, SYM_USE_EXISTING_OR_CREATE,
		     &dummyPt2, &t2, SYM_USE_EXISTING_OR_CREATE,
		     -1, SYM_VERTICAL);

    /* test out symRoute */
    array[0].pt1.x = 10000;
    array[0].pt1.y = 0;
    array[0].pt2.x = 10300;
    array[0].pt2.y = 0;
    array[0].layer = met1;
    array[0].width = -1;
    array[0].direction = SYM_HORIZONTAL;
    for (i = 1; i < 100; i++) {
	array[i].pt1.x = array[i-1].pt2.x;
	array[i].pt1.y = array[i-1].pt2.y;
	switch (i % 4) {
	    case 0:
	    array[i].layer = met1;
	    array[i].pt2.x = array[i].pt1.x + 300;
	    array[i].pt2.y = array[i].pt1.y;
	    array[i].direction = SYM_HORIZONTAL;
	    break;

	    case 1:
	    array[i].layer = met2;
	    array[i].pt2.x = array[i].pt1.x;
	    array[i].pt2.y = array[i].pt1.y + 300;
	    array[i].direction = SYM_VERTICAL;
	    break;
	    
	    case 2:
	    array[i].layer = met1;
	    array[i].pt2.x = array[i].pt1.x + 300;
	    array[i].pt2.y = array[i].pt1.y;
	    array[i].direction = SYM_HORIZONTAL;
	    break;
	    
	    case 3:
	    array[i].layer = poly;
	    array[i].pt2.x = array[i].pt1.x;
	    array[i].pt2.y = array[i].pt1.y + 300;
	    array[i].direction = SYM_VERTICAL;
	    break;
	}
	array[i].width = -1;
    }
    symRoute(array, 100);

    /* netlist */
    symInitInstanceGen(&facet, SYM_INSTANCES, &gen);
    while (octGenerate(&gen, &inst) == OCT_OK) {
	if (inst.contents.instance.name == NIL(char)) {
	    inst.contents.instance.name = "";
	}
	(void) printf("%s (%s)\n", inst.contents.instance.master,
		   inst.contents.instance.name);
	OH_ASSERT(octInitGenContents(&inst, OCT_TERM_MASK, &tgen));
	while (octGenerate(&tgen, &term) == OCT_OK) {
	    symGetNet(&term, &net);
	    if (net.contents.net.name == NIL(char)) {
		net.contents.net.name = "";
	    }
	    if (symGetProperty(&term, &prop, "TERMTYPE") == OCT_OK) {
		(void) printf("\t%s=%s (%s)\n", term.contents.term.name,
		       net.contents.net.name, prop.contents.prop.value.string);
	    } else {
		(void) printf("\t%s=%s\n", term.contents.term.name, net.contents.net.name);
	    }		
	    symInitPropertyGen(&term, &pgen);
	    while (octGenerate(&pgen, &prop) == OCT_OK) {
		(void) printProp(&prop);
	    }
	    (void) printf("\n");
	}
	if (symGetProperty(&inst, &prop, "MOSFETWIDTH") == OCT_OK) {
	    (void) printProp(&prop);
	}
	if (symGetProperty(&inst, &prop, "MOSFETLENGTH") == OCT_OK) {
	    (void) printProp(&prop);
	}
	(void) printf("\n");
	symInitPropertyGen(&inst, &pgen);
	while (octGenerate(&pgen, &prop) == OCT_OK) {
	    (void) printProp(&prop);
	}
    }

    /* statistics */
    OH_ASSERT(octInitGenContents(&facet, OCT_NET_MASK, &gen));
    i = 0;
    while (octGenerate(&gen, &net) == OCT_OK) {
	i++;
    }
    (void) printf("\nnets: %d\n", i);
    symInitInstanceGen(&facet, SYM_INSTANCES, &gen);
    i = 0;
    while (octGenerate(&gen, &inst) == OCT_OK) {
	i++;
    }
    (void) printf("instances: %d\n", i);
    symInitInstanceGen(&facet, SYM_CONNECTORS, &gen);
    i = 0;
    while (octGenerate(&gen, &inst) == OCT_OK) {
	i++;
    }
    (void) printf("connectors: %d\n", i);

    OH_ASSERT(octCloseFacet(&facet));

    return 0;
}
