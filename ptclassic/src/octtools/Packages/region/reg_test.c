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
 * Region package testing
 */

#include "port.h"
#include "oct.h"
#include "oh.h"
#include "region.h"
#include "utility.h"
#include "st.h"
#include "errtrap.h"

#define Fprintf		(void) fprintf
#define Printf  	(void) printf
#define Sprintf		(void) sprintf
#define Strcat		(void) strcat
#define RINT(lo, hi)	(drandom() % ((hi) - (lo))) + (lo)


static void process();
static void recall_test();
static void move_test();
static void detach_test();
static void combo_test();
static void delete_test();
static long drandom();
static void rand_box();
static char *vuDispObject();
static st_table *read_table();

static int tot_count = 0;

static FILE *ref_file = (FILE *) 0;

static void usage(prog_name)
char *prog_name;
{
    Fprintf(stderr, "format: %s cell[:view[:facet]]\n", prog_name);
    Fprintf(stderr, "        cell is input cell for region testing\n");
    exit(1);
}

int
main(argc, argv)
int argc;
char *argv[];
/* Takes the name of a test cell */
{
    octObject f;

    errProgramName(argv[0]);
    switch (argc) {
    case 3:
	ref_file = fopen(argv[2], "r");
    case 2:
	ohUnpackDefaults(&f, "r", ":physical:contents");
	if ((ohUnpackFacetName(&f, argv[1]) != OCT_OK) ||
	    (octOpenFacet(&f) < OCT_OK)) {
	    Fprintf(stderr, "%s: unable to open facet `%s'\n", argv[0],
		    ohFormatName(&f));
	    exit(1);
	}
	break;
    default:
	usage(argv[0]);
	/*NOTREACHED*/
    }
    process(&f);
    return 0;
}


#define RECALL_REGIONS	25
#define MOVE_TESTS	10
#define DETACH_TESTS	10
#define COMBO_TESTS	10
#define DELETE_TESTS	10

static void process(f)
octObject *f;
{
    int i;
    struct octBox initial_size;

    OH_ASSERT(octBB(f, &initial_size));

    /* First: some general random recall */
    for (i = 0;  i < RECALL_REGIONS;  i++) recall_test(f, &initial_size);

    /* Lets move some geometry and test the results */
    OH_ASSERT(octBB(f, &initial_size));
    for (i = 0;  i < MOVE_TESTS;  i++) move_test(f, &initial_size);

    /* Detach items from one layer,  put them on another */
    OH_ASSERT(octBB(f, &initial_size));
    for (i = 0;  i < DETACH_TESTS; i++) detach_test(f, &initial_size);

    /* The combination test */
    OH_ASSERT(octBB(f, &initial_size));
    for (i = 0;  i < COMBO_TESTS;  i++) combo_test(f, &initial_size);

    /* The delete test */
    OH_ASSERT(octBB(f, &initial_size));
    for (i = 0;  i < DELETE_TESTS;  i++) delete_test(f, &initial_size);
    
}

static void recall_test(f, size)
octObject *f;
struct octBox *size;
/*
 * Asks for geometries in random regions inside `size'.
 */
{
    struct octBox region;
    regObjGen gen;
    octObject obj;
    st_table *tbl;
    char *disp, *value;

    rand_box(size, &region, 1, 5);
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, (int) (drandom()&0x1)) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0, "Missing object in recall test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0, "Unclaimed object in recall test");
	}
    }
    if (!tbl) Printf("\n");
}


static void move_obj(obj, offset)
octObject *obj;
struct octPoint *offset;
{
    int j;
    int32 num;
    struct octPoint *pnts;

    switch (obj->type) {
    case OCT_BOX:
	obj->contents.box.lowerLeft.x += offset->x;
	obj->contents.box.lowerLeft.y += offset->y;
	obj->contents.box.upperRight.x += offset->x;
	obj->contents.box.upperRight.y += offset->y;
	break;
    case OCT_INSTANCE:
	obj->contents.instance.transform.translation.x += offset->x;
	obj->contents.instance.transform.translation.y += offset->y;
	break;
    case OCT_PATH:
    case OCT_POLYGON:
	ohGetPoints(obj, &num, &pnts);
	for (j = 0;  j < num;  j++) {
	    pnts[j].x += offset->x;
	    pnts[j].y += offset->y;
	}
	OH_ASSERT(octPutPoints(obj, num, pnts));
	break;
    default:
	errRaise("regtest", 0, "Bad object type");
    }
    OH_ASSERT(octModify(obj));
}

static void move_test(f, size)
octObject *f;
struct octBox *size;
/*
 * Chooses a random region and moves all objects in that region
 * a random distance then tests both regions.
 */
{
    struct octBox region;
    struct octPoint offset;
    regObjGen gen;
    octObject obj;
    st_table *tbl;
    char *disp, *value;

    rand_box(size, &region, 1, 4);
    offset.x = RINT(size->lowerLeft.x, size->upperRight.x);
    offset.y = RINT(size->lowerLeft.y, size->upperRight.y);
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    move_obj(&obj, &offset);
	}
	regObjFinish(gen);
    }
    /* Now print out those in the old region and in the new region */
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in old region of move test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0, "Unclaimed object in move test");
	}
    }
    if (!tbl) Printf("\n");
    /* Now handle the new region */
    region.lowerLeft.x += offset.x;
    region.lowerLeft.y += offset.y;
    region.upperRight.x += offset.x;
    region.upperRight.y += offset.y;
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in new region of move test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0,
		     "Unclaimed object in new region of move test");
	}
    }
    if (!tbl) Printf("\n");
}


static void detach_test(f, size)
octObject *f;
struct octBox *size;
/* 
 * Chooses a random region and layer.  Generates all geometries
 * on that layer (using low level interface) and detaches them
 * from that layer and puts them onto another.
 */
{
    octObject source, dest, obj;
    int32 num;
    struct octPoint *pnts;
    struct octBox obj_size;
    struct octBox region;
    static char *layers[] = { "MET1", "MET2", "POLY" };
    regGenerator gen_it;
    regObjGen gen;
    st_table *tbl;
    char *disp, *value;

    rand_box(size, &region, 1, 4);
    source.type = dest.type = OCT_LAYER;
    dest.contents.layer.name = source.contents.layer.name = layers[RINT(0, 3)];
    while (dest.contents.layer.name == source.contents.layer.name) {
	dest.contents.layer.name = layers[RINT(0, 3)];
    }
    OH_ASSERT(octGetOrCreate(f, &source));
    OH_ASSERT(octGetOrCreate(f, &dest));
    regInit(&source, &region, OCT_GEO_MASK, &gen_it);
    while (regNext(&gen_it, &obj, &num, &pnts, &obj_size) == REG_OK) {
	OH_ASSERT(octDetach(&source, &obj));
	OH_ASSERT(octAttach(&dest, &obj));
    }
    /* Print out objects in region */
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0, "Missing object in detach test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0, "Unclaimed object in detach test");
	}
    }
    if (!tbl) Printf("\n");
}


static void combo_test(f, size)
octObject *f;
struct octBox *size;
/* 
 * Chooses a random region.  Both modifies objects multiple times
 * then deletes some of them (random).
 */
{
    octObject obj, container, lyr, bag;
    struct octBox region;
    static char *layers[] = { "MET1", "MET2", "POLY" };
    regObjGen gen;
    struct octPoint off1, off2;
    st_table *tbl;
    char *disp, *value;

    rand_box(size, &region, 1, 4);
    off1.x = RINT(size->lowerLeft.x, size->upperRight.x);
    off1.y = RINT(size->lowerLeft.y, size->upperRight.y);
    off2.x = RINT(size->lowerLeft.x, size->upperRight.x);
    off2.y = RINT(size->lowerLeft.y, size->upperRight.y);
    lyr.type = OCT_LAYER;
    lyr.contents.layer.name = layers[RINT(0,3)];
    OH_ASSERT(octGetOrCreate(f, &lyr));
    bag.type = OCT_BAG;
    bag.contents.bag.name = "Region Test Bag";
    OH_ASSERT(octCreate(f, &bag));
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    /* Move it by first offset */
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in region generation of combo test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	    move_obj(&obj, &off1);
	    /* Detach it from its first container */
	    if (obj.type == OCT_INSTANCE) {
		OH_ASSERT(octGenFirstContainer(&obj, OCT_ALL_MASK, &container));
		OH_ASSERT(octDetach(&container, &obj));
		OH_ASSERT(octAttach(&bag, &obj));
	    } else {
		OH_ASSERT(octGenFirstContainer(&obj, OCT_LAYER_MASK, &container));
		OH_ASSERT(octDetach(&container, &obj));
		OH_ASSERT(octAttach(&lyr, &obj));
	    }
	    /* Move by second offset */
	    move_obj(&obj, &off2);
	    /* 50% chance of delete */
	    if (obj.objectId & 0x1) {
		OH_ASSERT(octDelete(&obj));
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0,
		     "Unclaimed object in generation of combo test");
	}
    }
    if (!tbl) Printf("\n");
    /* Print out objects in destination regions */
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in old region of combo test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0,
		     "Unclaimed object in old region of combo test");
	}
    }
    if (!tbl) Printf("\n");
    
    region.lowerLeft.x += off1.x;
    region.lowerLeft.y += off1.y;
    region.upperRight.x += off1.x;
    region.upperRight.y += off1.y;
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in middle region of combo test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0,
		     "Unclaimed object in middle region of combo test");
	}
    } 
    if (!tbl) Printf("\n");
    region.lowerLeft.x += off2.x;
    region.lowerLeft.y += off2.y;
    region.upperRight.x += off2.x;
    region.upperRight.y += off2.y;
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in final region of combo test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0,
		     "Unclaimed object in final region of combo test");
	}
    } 
    if (!tbl) Printf("\n");
}

static void delete_test(f, size)
octObject *f;
struct octBox *size;
/*
 * Asks for geometries in a random region based on size.  Moves
 * all geometries in that region then deletes them.
 */
{
    octObject obj;
    struct octBox region;
    st_table *tbl;
    char *disp, *value;
    struct octPoint offset;
    regObjGen gen;

    rand_box(size, &region, 1, 5);
    offset.x = RINT(size->lowerLeft.x, size->upperRight.x);
    offset.y = RINT(size->lowerLeft.y, size->upperRight.y);
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    move_obj(&obj, &offset);
	    OH_ASSERT(octDelete(&obj));
	}
	regObjFinish(gen);
    }
    /* Now print out those in the old region and in the new region */
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in old region of delete test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0, "Unclaimed object in old region of delete test");
	}
    }
    if (!tbl) Printf("\n");
    /* Now handle the new region */
    region.lowerLeft.x += offset.x;
    region.lowerLeft.y += offset.y;
    region.upperRight.x += offset.x;
    region.upperRight.y += offset.y;
    tbl = read_table();
    if (regObjStart(f, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &obj) == REG_OK) {
	    disp = vuDispObject(&obj);
	    if (tbl) {
		if (st_delete(tbl, &disp, &value)) {
		    FREE(disp);
		} else {
		    errRaise("regtest", 0,
			     "Missing object in new region of delete test");
		}
	    } else {
		Printf("%s\n", disp);
	    }
	}
	regObjFinish(gen);
	if (tbl && st_count(tbl)) {
	    errRaise("regtest", 0,
		     "Unclaimed object in new region of delete test");
	}
    }
    if (!tbl) Printf("\n");
}


/*
 * Deterministic random numbers
 */

static long random_numbers[] = {
    1604626114, 212650944, 61667691, 182366316, 688864012, 1792477936,
    437906218, 770530584, 187018583, 2048299624, 1099037907, 433767727,
    980538126, 249562238, 1901149345, 837025061, 571945722, 476145177,
    143460666, 567459915, 770425455, 109730913, 2071720955, 367574713,
    55172515, 1064489052, 2080211656, 522911081, 762903755, 259780620,
    579528598, 117487482, 1260898754, 865497158, 2104927106, 813789186,
    496430840, 1038293808, 1119777147, 199573641, 980019640, 1550407538,
    5149739, 1900643541, 1709491811, 2017798022, 378424278, 1258857341,
    2138399884, 1909472526, 1238054244, 1136608299, 1114241176, 810963404,
    1986454146, 497882069, 1238568154, 1039825809, 1387366915, 623123727,
    1157589079, 701317678, 1179093917, 557209555, 1531354585, 1921519624,
    1541714500, 1738290815, 858620705, 783710342, 1583110461, 1586359018,
    1965459205, 133792691, 1491052439, 1988369396, 864519224, 1126527889,
    2042456326, 1689916311, 1862041560, 1270653728, 1950065195, 949961772,
    1075015674, 1095348013, 1942111346, 1377293587, 382742297, 2040275403,
    701490699, 1154734775, 290278225, 1840333283, 619484624, 208506314,
    279793881, 1893912707, 925728737, 1499169848, 538090542, 1684064860,
    1380489362, 1782718173, 207267476, 1793796698, 588518424, 1563612689,
    722605665, 553199191, 584579082, 1529727151, 2122250721, 173059977,
    307040017, 1116196070, 926730826, 841102120, 1191387898, 620995670,
    9413667, 1058528888, 1929307667, 1818446862, 2108750749, 1208040511,
    602913600, 2104247053, 451637215, 2018220679, 1375659436, 1512199969,
    43533853, 1110693850, 1399739535, 70443310, 1306144865, 1693395475,
    202566041, 1873783224, 64340790, 1634004621, 2069582233, 937956007,
    2050168242, 595508675, 259381372, 1816734088, 1660100347, 379981277,
    1717891154, 270566137, 1082814822, 566191229, 937038285, 1664774563,
    916702869, 1473541439, 899630569, 1770934494, 381723029, 1762980703,
    778931799, 1178600065, 284552630, 2022793337, 315229462, 117239200,
    1154634843, 1138000628, 976685203, 1799852042, 621241951, 1442019879,
    44017075, 958464018, 1126133503, 1330818483, 718782949, 1658569686,
    1819099354, 1296126361, 1896661544, 747914260, 793242789, 1425006698,
    161014222, 416222987, 950183277, 73427500, 592829719, 1697113982,
    413821806, 527142937, 1925764499, 1835310971, 1331654147, 185166585,
    574785575, 248510615, 751466681, 151190247, 1919038145, 635714633,
    1008674320, 1683193443, 1870804913, 170993464, 1355038102, 1570982572,
    1686991433, 2056389830, 1993725946, 829460135, 1918581456, 1048165393,
    1334738948, 81689241, 1174693401, 1890839888, 154436338, 1104654061,
    545103101, 799314832, 137566866, 930221742, 365409061, 556546170,
    1968522774, 1315514473, 1310306746, 308958570, 993000717, 650684244,
    78186999, 1665288785, 15645210, 1000874208, 1541669175, 1573016814,
    400987778, 2036388359, 171299996, 175229792, 1707466593, 1985642447,
    1533161273, 217051203, 908584049, 1166638612, 705727622
};

static int rand_spot = 0;
static int num_rand = sizeof(random_numbers)/sizeof(long);    

static long drandom()
/* Returns a random long integer from the list of numbers above */
{
    if (rand_spot >= num_rand) rand_spot = 0;
    tot_count++;
    return random_numbers[rand_spot++];
}

static void rand_box(domain, box, num, dem)
struct octBox *domain;
struct octBox *box;
int num, dem;
/*
 * Generates a random box in `box' that intersects `domain'.
 * `num' and `dem' specify the maximum fraction of `domain'
 * the sides of the `box' should be.
 */
{
    long tw, th;

    box->lowerLeft.x = RINT(domain->lowerLeft.x, domain->upperRight.x);
    box->lowerLeft.y = RINT(domain->lowerLeft.y, domain->upperRight.y);
    tw = (domain->upperRight.x-domain->lowerLeft.x)*num/dem;
    th = (domain->upperRight.y-domain->lowerLeft.y)*num/dem;
    box->upperRight.x = box->lowerLeft.x + RINT(-tw, th);
    box->upperRight.y = box->lowerLeft.y + RINT(-th, th);
    if (box->lowerLeft.x > box->upperRight.x) {
	tw = box->upperRight.x;
	box->upperRight.x = box->lowerLeft.x;
	box->lowerLeft.x = tw;
    }
    if (box->lowerLeft.y > box->upperRight.y) {
	th = box->upperRight.y;
	box->upperRight.y = box->lowerLeft.y;
	box->lowerLeft.y = th;
    }
}

/*
 * Printing
 */
#define MAX_OBJ_SIZE	4096
#define END(m)	((m)+strlen(m))

static char *disp_inst(obj)
octObject *obj;
/* Returns a textual representation of an instance */
{
    static char ibuf[MAX_OBJ_SIZE];

    (void) sprintf(ibuf, "OCT_INSTANCE_%s[%s:%s", octFormatId(obj->objectId),
	    obj->contents.instance.master,
	    obj->contents.instance.view);
    if (strlen(obj->contents.instance.name) > 0) {
	(void) sprintf(END(ibuf), " N=`%s'", obj->contents.instance.name);
    }
    if (strlen(obj->contents.instance.version) > 0) {
	(void) sprintf(END(ibuf), " V=`%s'", obj->contents.instance.version);
    }
    switch (obj->contents.instance.transform.transformType) {
    case OCT_NO_TRANSFORM:
	(void) sprintf(END(ibuf), " OCT_NO_TRANSFORM");
	break;
    case OCT_MIRROR_X:
	(void) sprintf(END(ibuf), " OCT_MIRROR_X");
	break;
    case OCT_MIRROR_Y:
	(void) sprintf(END(ibuf), " OCT_MIRROR_Y");
	break;
    case OCT_ROT90:
	(void) sprintf(END(ibuf), " OCT_ROT90");
	break;
    case OCT_ROT180:
	(void) sprintf(END(ibuf), " OCT_ROT180");
	break;
    case OCT_ROT270:
	(void) sprintf(END(ibuf), " OCT_ROT270");
	break;
    case OCT_MX_ROT90:
	(void) sprintf(END(ibuf), " OCT_MX_ROT90");
	break;
    case OCT_MY_ROT90:
	(void) sprintf(END(ibuf), " OCT_MY_ROT90");
	break;
    case OCT_FULL_TRANSFORM:
	(void) sprintf(END(ibuf), " OCT_FULL_TRANSFORM");
	break;
    }
    (void) sprintf(END(ibuf), "  (%ld,%ld)]",
		   (long)obj->contents.instance.transform.translation.x,
		   (long)obj->contents.instance.transform.translation.y);
    return ibuf;
}



#define MAX_PNTS	5
#define MAX_POINTS	1024

static char *disp_points(obj)
octObject *obj;
/* Displays some points of a path or polygon */
{
    static char pntbuf[MAX_OBJ_SIZE];
    struct octPoint points[MAX_POINTS];
    int32 num = MAX_POINTS, i;

    pntbuf[0] = '\0';
    if (octGetPoints(obj, &num, points) != OCT_OK) {
	(void) sprintf(pntbuf, "[too many points]");
	return pntbuf;
    }
    if (num == 0) {
	(void) sprintf(pntbuf, "[no points]");
	return pntbuf;
    }
    pntbuf[0] = '\0';
    if (num > MAX_PNTS) {
	for (i = 0;  i < MAX_PNTS-1;  i++) {
	    (void) sprintf(END(pntbuf), " %ld,%ld",
			   (long)points[i].x, (long)points[i].y);
	}
	(void) sprintf(END(pntbuf), " ... %ld,%ld", 
		       (long)points[num-1].x, (long)points[num-1].y);
    } else {
	for (i = 0;  i < num;  i++) {
	    (void) sprintf(END(pntbuf), " %ld,%ld",
			   (long)points[i].x, (long)points[i].y);
	}
    }
    return pntbuf;
}


static char *disp_prop(obj)
octObject *obj;
/* Displays a property */
{
    static char propbuf[MAX_OBJ_SIZE];
    int i;

    (void) sprintf(propbuf, "OCT_PROP_%s[`%s'", octFormatId(obj->objectId),
	    obj->contents.prop.name);
    switch (obj->contents.prop.type) {
    case OCT_NULL:
	(void) sprintf(END(propbuf), " OCT_NULL]");
	break;
    case OCT_INTEGER:
	(void) sprintf(END(propbuf), " OCT_INTEGER=%ld]",
		       (long)obj->contents.prop.value.integer);
	break;
    case OCT_REAL:
	(void) sprintf(END(propbuf), " OCT_REAL=%g]",
		obj->contents.prop.value.real);
	break;
    case OCT_STRING:
	(void) sprintf(END(propbuf), " OCT_STRING=`%s']",
		obj->contents.prop.value.string);
	break;
    case OCT_ID:
	(void) sprintf(END(propbuf), " OCT_ID=%d]",
		(int) obj->contents.prop.value.id);
	break;
    case OCT_STRANGER:
	(void) sprintf(END(propbuf), " OCT_STRANGER]");
	break;
    case OCT_REAL_ARRAY:
	(void) sprintf(END(propbuf), " OCT_REAL_ARRAY=<");
	for (i = 0;  i < obj->contents.prop.value.real_array.length;  i++) {
	    (void) sprintf(END(propbuf), " %g",
		    obj->contents.prop.value.real_array.array[i]);
	}
	(void) sprintf(END(propbuf), ">]");
	break;
    case OCT_INTEGER_ARRAY:
	(void) sprintf(END(propbuf), " OCT_INTEGER_ARRAY=<");
	for (i = 0;  i < obj->contents.prop.value.integer_array.length;  i++) {
	    (void) sprintf(END(propbuf), " %ld",
		   (long)obj->contents.prop.value.integer_array.array[i]);
	}
	(void) sprintf(END(propbuf), ">]");
	break;
    }
    return propbuf;
}

static char *disp_omask(mask)
octObjectMask mask;
/* Returns a string representation of an object mask */
{
    static char omask_buf[500];

    omask_buf[0] = '\0';
    if (mask & OCT_FACET_MASK) Strcat(omask_buf, "|OCT_FACET_MASK");
    if (mask & OCT_TERM_MASK) Strcat(omask_buf, "|OCT_TERM_MASK");
    if (mask & OCT_NET_MASK) Strcat(omask_buf, "|OCT_NET_MASK");
    if (mask & OCT_INSTANCE_MASK) Strcat(omask_buf, "|OCT_INSTANCE_MASK");
    if (mask & OCT_PROP_MASK) Strcat(omask_buf, "|OCT_PROP_MASK");
    if (mask & OCT_BAG_MASK) Strcat(omask_buf, "|OCT_BAG_MASK");
    if (mask & OCT_POLYGON_MASK) Strcat(omask_buf, "|OCT_POLYGON_MASK");
    if (mask & OCT_BOX_MASK) Strcat(omask_buf, "|OCT_BOX_MASK");
    if (mask & OCT_CIRCLE_MASK) Strcat(omask_buf, "|OCT_CIRCLE_MASK");
    if (mask & OCT_PATH_MASK) Strcat(omask_buf, "|OCT_PATH_MASK");
    if (mask & OCT_LABEL_MASK) Strcat(omask_buf, "|OCT_LABEL_MASK");
    if (mask & OCT_LAYER_MASK) Strcat(omask_buf, "|OCT_LAYER_MASK");
    if (mask & OCT_POINT_MASK) Strcat(omask_buf, "|OCT_POINT_MASK");
    if (mask & OCT_EDGE_MASK) Strcat(omask_buf, "|OCT_EDGE_MASK");
    if (mask & OCT_FORMAL_MASK) Strcat(omask_buf, "|OCT_FORMAL_MASK");
    if (mask & OCT_CHANGE_LIST_MASK) Strcat(omask_buf, "|OCT_CHANGE_LIST_MASK");
    if (mask & OCT_CHANGE_RECORD_MASK) Strcat(omask_buf, "|OCT_CHANGE_RECORD_MASK");
    if (mask & OCT_FORMAL_MASK) Strcat(omask_buf, "|OCT_FORMAL_MASK");
    if (omask_buf[0] == '|') return &omask_buf[1];
    else return (char *) omask_buf;
}

static char *disp_fmask(mask)
octFunctionMask mask;
/* Returns a string representation of a function mask */
{
    static char fmask_buf[500];

    fmask_buf[0] = '\0';
    if (mask & OCT_CREATE_MASK) Strcat(fmask_buf, "|OCT_CREATE_MASK");
    if (mask & OCT_DELETE_MASK) Strcat(fmask_buf, "|OCT_DELETE_MASK");
    if (mask & OCT_MODIFY_MASK) Strcat(fmask_buf, "|OCT_MODIFY_MASK");
    if (mask & OCT_PUT_POINTS_MASK) Strcat(fmask_buf, "|OCT_PUT_POINTS_MASK");
    if (mask & OCT_DETACH_CONTENT_MASK) Strcat(fmask_buf, "|OCT_DETACH_CONTENT_MASK");
    if (mask & OCT_DETACH_CONTAINER_MASK) Strcat(fmask_buf, "|OCT_DETACH_CONTAINER_MASK");
    if (mask & OCT_ATTACH_CONTENT_MASK) Strcat(fmask_buf, "|OCT_ATTACH_CONTENT_MASK");
    if (mask & OCT_ATTACH_CONTAINER_MASK) Strcat(fmask_buf, "|OCT_ATTACH_CONTAINER_MASK");
    if (fmask_buf[0] == '|') return &fmask_buf[1];
    else return (char *) fmask_buf;
}


char *disp_crec(obj)
octObject *obj;
/* Displays a change record */
{
    static char crecbuf[MAX_OBJ_SIZE];

    (void) sprintf(crecbuf, "OCT_CHANGE_RECORD_%s[", octFormatId(obj->objectId));
    switch (obj->contents.changeRecord.changeType) {
    case OCT_CREATE:
	(void) sprintf(END(crecbuf), "OCT_CREATE");
	break;
    case OCT_DELETE:
	(void) sprintf(END(crecbuf), "OCT_DELETE");
	break;
    case OCT_MODIFY:
	(void) sprintf(END(crecbuf), "OCT_MODIFY");
	break;
    case OCT_PUT_POINTS:
	(void) sprintf(END(crecbuf), "OCT_PUT_POINTS");
	break;
    case OCT_ATTACH_CONTENT:
	(void) sprintf(END(crecbuf), "OCT_ATTACH_CONTENT");
	break;
    case OCT_ATTACH_CONTAINER:
	(void) sprintf(END(crecbuf), "OCT_ATTACH_CONTAINER");
	break;
    case OCT_DETACH_CONTENT:
	(void) sprintf(END(crecbuf), "OCT_DETACH_CONTENT");
	break;
    case OCT_DETACH_CONTAINER:
	(void) sprintf(END(crecbuf), "OCT_DETACH_CONTAINER");
	break;
    }
    (void) sprintf(END(crecbuf), ", %d, %d]",
		   (int) obj->contents.changeRecord.objectExternalId,
		   (int) obj->contents.changeRecord.contentsExternalId);
    return crecbuf;
}



static char *vuDispObject(obj)
octObject *obj;
/* Returns a string suitable for display given an object */
{
    static char final[MAX_OBJ_SIZE];
    octObject layerObj;

    layerObj.contents.layer.name = "No Layer";
    final[0] = '\0';
    switch (obj->type) {
    case OCT_UNDEFINED_OBJECT:
	(void) sprintf(END(final), "OCT_UNDEFINED");
	break;
    case OCT_FACET:
	(void) sprintf(END(final), "OCT_FACET_%s[%s:%s:%s]", octFormatId(obj->objectId),
		obj->contents.facet.cell,
		obj->contents.facet.view,
		obj->contents.facet.facet);
	break;
    case OCT_TERM:
	(void) sprintf(END(final), "OCT_TERMINAL_%s[`%s']", octFormatId(obj->objectId),
		obj->contents.term.name);
	break;
    case OCT_NET:
	(void) sprintf(END(final), "OCT_NET_%s[`%s']", octFormatId(obj->objectId),
		obj->contents.net.name);
	break;
    case OCT_INSTANCE:
	(void) sprintf(END(final), "%s", disp_inst(obj));
	break;
    case OCT_POLYGON:
	(void) octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_POLYGON_%s[%s]:%s", octFormatId(obj->objectId),
		layerObj.contents.layer.name, disp_points(obj));
	break;
    case OCT_BOX:
	(void) octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_BOX_%s[%s; %ld,%ld %ld,%ld]",
		       octFormatId(obj->objectId), 
		       layerObj.contents.layer.name,
		       (long)obj->contents.box.lowerLeft.x,
		       (long)obj->contents.box.lowerLeft.y,
		       (long)obj->contents.box.upperRight.x,
		       (long)obj->contents.box.upperRight.y);
	break;
    case OCT_CIRCLE:
	(void) octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final),
		       "OCT_CIRCLE_%s[%s; %ld,%ld R=%ld,%ld A=%ld,%ld]",
		       octFormatId(obj->objectId),
		       layerObj.contents.layer.name,
		       (long)obj->contents.circle.center.x,
		       (long)obj->contents.circle.center.y,
		       (long)obj->contents.circle.innerRadius,
		       (long)obj->contents.circle.outerRadius,
		       (long)obj->contents.circle.startingAngle,
		       (long)obj->contents.circle.endingAngle);
	break;
    case OCT_PATH:
	(void) octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_PATH_%s[%s; %ld]:%s",
		       octFormatId(obj->objectId), 
		       layerObj.contents.layer.name,
		       (long)obj->contents.path.width, disp_points(obj));
	break;
    case OCT_LABEL:
	(void) octGenFirstContainer(obj, OCT_LAYER_MASK, &layerObj);
	(void) sprintf(END(final), "OCT_LABEL_%s[%s; %ld,%ld `%s']",
		       octFormatId(obj->objectId),
		       layerObj.contents.layer.name,
		       (long)obj->contents.label.region.lowerLeft.x,
		       (long)obj->contents.label.region.lowerLeft.y,
		       obj->contents.label.label);
	break;
    case OCT_PROP:
	(void) sprintf(END(final), "%s", disp_prop(obj));
	break;
    case OCT_BAG:
	(void) sprintf(END(final), "OCT_BAG_%s[`%s']", octFormatId(obj->objectId),
		obj->contents.bag.name);
	break;
    case OCT_LAYER:
	(void) sprintf(END(final), "OCT_LAYER_%s[`%s']", octFormatId(obj->objectId),
		obj->contents.layer.name);
	break;
    case OCT_POINT:
	(void) sprintf(END(final), "OCT_POINT_%s[%ld,%ld]",
		       octFormatId(obj->objectId),
		       (long)obj->contents.point.x,
		       (long)obj->contents.point.y);
	break;
    case OCT_EDGE:
	(void) sprintf(END(final),
		       "OCT_EDGE_%s[%ld,%ld to %ld,%ld]",
		       octFormatId(obj->objectId),
		       (long)obj->contents.edge.start.x,
		       (long)obj->contents.edge.start.y,
		       (long)obj->contents.edge.end.x,
		       (long)obj->contents.edge.end.y);
	break;
    case OCT_CHANGE_LIST:
	(void) sprintf(END(final), "OCT_CHANGE_LIST_%s[%s, %s]",
		       octFormatId(obj->objectId),
		       disp_omask(obj->contents.changeList.objectMask),
		       disp_fmask(obj->contents.changeList.functionMask));
	break;
    case OCT_CHANGE_RECORD:
	(void) sprintf(END(final), "%s", disp_crec(obj));
	break;
    default:
	(void) sprintf(END(final), "OCT_UNKNOWN_%s[%d]", octFormatId(obj->objectId), obj->type);
	break;
    }
    return final;
}


/*ARGSUSED*/
static enum st_retval free_item(key, value, arg)
char *key, *value, *arg;
{
    FREE(key);
    return ST_CONTINUE;
}

static char *get_line()
{
    static char buf[2048];
    int len;

    if (fgets(buf, 2048, ref_file)) {
	len = strlen(buf);
	buf[--len] = '\0';
    } else {
	len = 0;
    }
    if (len) {
	return strcpy(malloc((unsigned) (len+1)), buf);
    } else {
	return (char *) 0;
    }
}

static st_table *read_table()
/*
 * Reads in a reference table from standard input
 */
{
    static st_table *result = (st_table *) 0;
    char *buf;

    if (ref_file) {
	if (result) {
	    (void) st_foreach(result, free_item, (char *) 0);
	    st_free_table(result);
	}
	result = st_init_table(strcmp, st_strhash);
	while ( (buf = get_line()) ) {
	    (void) st_insert(result, buf, (char *) 0);
	}
	return result;
    } else {
	return (st_table *) 0;
    }
}
