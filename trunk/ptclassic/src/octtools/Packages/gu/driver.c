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
/* Test Driver for Geometric Update package
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This program exercises the geometric update package.  It reads
 * a sample cell and rearranges things in it.  It then prints
 * out the changes using the geometric update package.
 */

#include "port.h"
#include "gu.h"
#include "oh.h"
#include "utility.h"
#include "errtrap.h"
#include "st.h"

#define Printf		(void) printf
#define Fprintf		(void) fprintf

#define MOD_FREQ	5
#define MOVE_FREQ	7
#define DEL_FREQ	3
#define DINST_FREQ	4

/*
 * For checking the correctness of the changes,  this module
 * maintains a global hash table of changes.  This table is
 * built by the actual change operations (e.g. the modify, delete, etc).
 * The items are hashed by a combination of the parent xid and
 * the object xid. The table contains the following structures:
 */
typedef struct geo_update_defn {
    gu_op op;			/* Operation   */
    struct octBox *old_size;	/* Oldest size */
} geo_update;

typedef struct geo_key_defn {
    int32 parent_xid;		/* Parent object */
    int32 obj_xid;		/* Object itself */
} geo_key;

static st_table *updates = (st_table *) 0;
static st_table *sizes = (st_table *) 0;

static void set_markers();
static void mod_test();
static void move_test();
static void dinst_test();
static void del_test();
static void new_test();
static int gu_test();
static int check_changes();
static char *write_box();

static octObject *bag_test
	ARGS((octObject *fct));


static void usage(prog_name)
char *prog_name;
{
    Fprintf(stderr, "usage: %s cell[:view[:facet]]\n", prog_name);
    Fprintf(stderr, "       cell is input for geometric update testing\n");
    exit(1);
}

int
main(argc, argv)
int argc;
char *argv[];
{
    octObject f;

    errProgramName(argv[0]);
    errCore(1);
    if (argc != 2) usage(argv[0]);
    ohUnpackDefaults(&f, "r", ":physical:contents");
    if ((ohUnpackFacetName(&f, argv[1]) != OCT_OK) ||
	(octOpenFacet(&f) < OCT_OK)) {
	Fprintf(stderr, "%s: unable to open facet `%s'\n", argv[0],
		ohFormatName(&f));
	exit(1);
    }
    return gu_test(&f);
}

#define KEY(ptr, field)	((geo_key *) ptr)->field

static int xid2cmp(key1, key2)
char *key1, *key2;
/* Parameters are actually (geo_key *) */
{
    int32 diff;

    diff = KEY(key1, obj_xid) - KEY(key2, obj_xid);
    if (diff) return diff;
    else return KEY(key1, parent_xid) - KEY(key2, parent_xid);
}

static int xid2hash(key, modulus)
char *key;
int modulus;
/* `key' is actually (geo_key *) */
{
    return (KEY(key, obj_xid) ^ KEY(key, parent_xid)) % modulus;
}

static int gu_test(fct)
octObject *fct;
{
    octObject *bag;

    updates = st_init_table(xid2cmp, xid2hash);
    sizes = st_init_table(st_numcmp, st_numhash);
    set_markers(fct);
    bag = bag_test(fct);    
    mod_test(fct);
    move_test(fct);
    dinst_test(fct);
    del_test(fct);
    new_test(fct);
    (void) octDelete(bag);
    return check_changes(fct);
}

static void store_update(parent, obj, op)
octObject *parent;		/* Parent object  */
octObject *obj;			/* Object itself  */
gu_op op;			/* Operation type */
/* 
 * This routine looks up the parent/obj combination in the table `updates'.
 * If found, the operator is upgraded based on the value of `op'.  If
 * not found,  it is inserted and the object size is saved.
 */
{
    geo_key *key = ALLOC(geo_key, 1);
    geo_update **record;
    struct octBox *bb;

    octExternalId(parent, &(key->parent_xid));
    octExternalId(obj, &(key->obj_xid));
    if (st_find_or_add(updates, (char *) key, (char ***) &record)) {
	/* Already there */
	FREE(key);
	switch ((*record)->op) {
	case GU_NOOP:
	    (*record)->op = op;
	    break;
	case GU_ADD:
	    if (op == GU_REMOVE) {
		(*record)->op = GU_NOOP;
	    }
	    break;
	case GU_UPDATE:
	    if (op == GU_REMOVE) {
		(*record)->op = GU_REMOVE;
	    }
	    break;
	case GU_REMOVE:
	    if (op == GU_ADD) {
		(*record)->op = GU_UPDATE;
	    }
	    break;
	default:
	    errRaise("gu test", 1, "Bad operator");
	    /*NOTREACHED*/
	}
    } else {
	/* Not already there */
	*record = ALLOC(geo_update, 1);
	(*record)->op = op;
	if (st_lookup(sizes, (char *) key->obj_xid, (char **) &bb)) {
	    (*record)->old_size = bb;
	} else {
	    (*record)->old_size = ALLOC(struct octBox, 1);
	    if (octBB(obj, (*record)->old_size) == OCT_OK) {
		(void) st_insert(sizes, (char *) key->obj_xid,
				 (char *) ((*record)->old_size));
	    } else {
		FREE((*record)->old_size);
		(*record)->old_size = (struct octBox *) 0;
	    }
	}
    }
}


static void modify(parent, obj)
octObject *parent;
octObject *obj;
{
#ifdef PRINT_TEST
    int32 xid;
    struct octBox bb;
    char box_buf[100];

    octExternalId(obj, &xid);
    if (octBB(obj, &bb) >= OCT_OK) {
	Printf("Modifying %d, old size: %s\n", xid, write_box(box_buf, &bb));
    } else {
	Printf("Modifying %d, no size\n", xid);
    }
#endif
    store_update(parent, obj, GU_UPDATE);
    OH_ASSERT(octModify(obj));
}

static void attach(container, contents)
octObject *container, *contents;
{
#ifdef PRINT_TEST
    int32 up, down;

    octExternalId(container, &up);
    octExternalId(contents, &down);
    Printf("Attaching %d to %d\n", down, up);
#endif
    store_update(container, contents, GU_ADD);
    OH_ASSERT(octAttach(container, contents));
}

static void detach(container, contents)
octObject *container, *contents;
{
#ifdef PRINT_TEST
    int32 up, down;

    octExternalId(container, &up);
    octExternalId(contents, &down);
    Printf("Detaching %d from %d\n", down, up);
#endif
    store_update(container, contents, GU_REMOVE);
    OH_ASSERT(octDetach(container, contents));
}

static void putpoints(parent, obj, num, pnts)
octObject *parent;
octObject *obj;
int num;
octPoint *pnts;
{
#ifdef PRINT_TEST
    int32 xid;

    octExternalId(obj, &xid);
    Printf("Changing points on %d\n", xid);
#endif
    store_update(parent, obj, GU_UPDATE);
    OH_ASSERT(octPutPoints(obj, num, pnts));
}


static void delete(parent, obj)
octObject *parent;
octObject *obj;
{
#ifdef PRINT_TEST
    int32 xid;

    octExternalId(obj, &xid);
    Printf("Deleting %d\n", xid);
#endif
    store_update(parent, obj, GU_REMOVE);
    OH_ASSERT(octDelete(obj));
}

static void create(container, contents)
octObject *container, *contents;
{
#ifdef PRINT_TEST
    int32 up, down;
#endif
    OH_ASSERT(octCreate(container, contents));
#ifdef PRINT_TEST
    octExternalId(container, &up);
    octExternalId(contents, &down);
    Printf("Created %d attached to %d\n", down, up);
#endif
}

static void foreach(container, mask, func, data)
octObject *container;
octObjectMask mask;
void (*func)();
char *data;
{
    octGenerator gen;
    octObject obj;

    OH_ASSERT(octInitGenContents(container, mask, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	(*func)(&obj, data);
    }
}

#define MAX_LAYERS	20
typedef struct lyr_info_defn {
    char *layer_name;
    int32 layer_id;
    gu_marker mark;
} lyr_info;
static lyr_info marked_layers[MAX_LAYERS];
static int num_layers = 0;
static gu_marker inst_marker;

/*ARGSUSED*/
static void mark_layer(lyr, data)
octObject *lyr;
char *data;
{
    if (num_layers < MAX_LAYERS) {
	marked_layers[num_layers].layer_name = lyr->contents.layer.name;
	octExternalId(lyr, &(marked_layers[num_layers].layer_id));
	marked_layers[num_layers].mark = guMark(lyr, OCT_GEO_MASK);
	num_layers++;
    }
}

static void set_markers(fct)
octObject *fct;
{
    foreach(fct, OCT_LAYER_MASK, mark_layer, (char *) 0);
    inst_marker = guMark(fct, OCT_INSTANCE_MASK);
}

typedef struct bag_info_defn {
    octObject bag1, bag2;
} bag_info;

static void bag_geo(geo, data)
octObject *geo;
char *data;
{
    bag_info *info = (bag_info *) data;

    (void) octAttach(&(info->bag1), geo);
    (void) octAttach(&(info->bag2), geo);
}

static void bag_lyr(lyr, data)
octObject *lyr;
char *data;
{
    foreach(lyr, OCT_GEO_MASK, bag_geo, data);
}

static octObject *bag_test(fct)
octObject *fct;
/*
 * This routine attaches all geometry onto two bags one of
 * which will be deleted.  This is to exercise the rejection
 * of spurious attach/detach records.  The bag that is
 * to be deleted will be returned.
 */
{
    static bag_info info;

    info.bag1.type = OCT_BAG;
    info.bag1.contents.bag.name = "Deleted bag";
    (void) octCreate(fct, &info.bag1);
    info.bag2.type = OCT_BAG;
    info.bag2.contents.bag.name = "Undeleted bag";
    (void) octCreate(fct, &info.bag2);
    foreach(fct, OCT_LAYER_MASK, bag_lyr, (char *) &info);
    return &(info.bag1);
}

static void mod_box(box)
octObject *box;
{
    box->contents.box.lowerLeft.x -= 100;
    box->contents.box.lowerLeft.y -= 100;
    box->contents.box.upperRight.x += 100;
    box->contents.box.upperRight.y += 100;
}

static void mod_points(parent, obj)
octObject *parent;
octObject *obj;
{
    int32 num;
    int i;
    octPoint *pnts;

    ohGetPoints(obj, &num, &pnts);
    for (i = 0;  i < num;  i++) {
	if (i % 2) {
	    pnts[i].x += 100;
	    pnts[i].y -= 100;
	} else {
	    pnts[i].x -= 100;
	    pnts[i].y += 100;
	}
    }
    putpoints(parent, obj, num, pnts);
}

static void mod_path(parent, path)
octObject *parent;
octObject *path;
{
    path->contents.path.width += 10;
    mod_points(parent, path);
}

static void mod_poly(parent, poly)
octObject *parent;
octObject *poly;
{
    mod_points(parent, poly);
}

static void mod_inst(inst)
octObject *inst;
{
    inst->contents.instance.transform.translation.x += 500;
    inst->contents.instance.transform.translation.y -= 200;
}

typedef struct obj_info_defn {
    int count;
    octObject *parent;
} obj_info;

static void mod_geo(geo, data)
octObject *geo;
char *data;
{
    obj_info *info = (obj_info *) data;

    if ((info->count % MOD_FREQ) == 0) {
	/* Modify this one */
	switch (geo->type) {
	case OCT_BOX:
	    mod_box(geo);
	    break;
	case OCT_PATH:
	    mod_path(info->parent, geo);
	    break;
	case OCT_POLYGON:
	    mod_poly(info->parent, geo);
	    break;
	case OCT_INSTANCE:
	    mod_inst(geo);
	    break;
	default:
	    errRaise("gu test", 1, "Bad object type");
	    /*NOTREACHED*/
	}
	modify(info->parent, geo);
    }
    (info->count)++;
}

static void mod_layer(lyr, data)
octObject *lyr;
char *data;
{
    obj_info *info = (obj_info *) data;
#ifdef PRINT_TEST
    int32 xid;

    octExternalId(lyr, &xid);
    Printf("LAYER: %s (%d)\n", lyr->contents.layer.name, xid);
#endif
    info->parent = lyr;
    foreach(lyr, OCT_GEO_MASK, mod_geo, data);
}

static void mod_test(fct)
octObject *fct;
/*
 * Generates through all geometries and modifies every MOD_FREQ
 * object in some fashion.
 */
{
    obj_info info;

    info.count = 0;
    foreach(fct, OCT_LAYER_MASK, mod_layer, (char *) &info);
    info.parent = fct;
    foreach(fct, OCT_INSTANCE_MASK, mod_geo, (char *) &info);
}

struct move_info {
    int count;
    octObject bag;
    octObject *lyr;
};

static void trans_to_bag(obj, data)
octObject *obj;
char *data;
{
    struct move_info *info = (struct move_info *) data;

    attach(info->lyr, obj);
    OH_ASSERT(octDetach(&(info->bag), obj));
}

static void move_obj(obj, data)
octObject *obj;
char *data;
{
    struct move_info *info = (struct move_info *) data;

    if ((info->count % MOVE_FREQ) == 0) {
	/* Transfer this one - intentionally don't record bag trans. */
	OH_ASSERT(octAttach(&(info->bag), obj));
	detach(info->lyr, obj);
    }
    (info->count)++;
}

static void move_layer(lyr, data)
octObject *lyr;
char *data;
{
    struct move_info *info = (struct move_info *) data;
#ifdef PRINT_TEST
    int32 xid;

    octExternalId(lyr, &xid);
    Printf("LAYER: %s (%d)\n", lyr->contents.layer.name, xid);
#endif
    info->lyr = lyr;
    foreach(&(info->bag), OCT_GEO_MASK, trans_to_bag, data);
    foreach(lyr, OCT_GEO_MASK, move_obj, data);
}

static void move_test(fct)
octObject *fct;
/*
 * Moves objects between layers to test attach/detach.
 */
{
    struct move_info info;

    info.count = 0;
    info.bag.type = OCT_BAG;
    info.bag.contents.bag.name = "Move Test";
    create(fct, &(info.bag));
    foreach(fct, OCT_LAYER_MASK, move_layer, (char *) &info);
}



typedef struct det_info_defn {
    octObject *fct;
    octObject bag;
    int count;
} det_info;

static void det_inst(obj, data)
octObject *obj;
char *data;
{
    det_info *info = (det_info *) data;

    if ((info->count % DINST_FREQ) == 0) {
	(void) octAttach(&(info->bag), obj);
	detach(info->fct, obj);
    }
    info->count += 1;
}

static void on_inst(obj, data)
octObject *obj;
char *data;
{
    det_info *info = (det_info *) data;

    if ((info->count % DINST_FREQ) == 0) {
	attach(info->fct, obj);
	(void) octDetach(&(info->bag), obj);
    }
    info->count += 1;
}

static void dinst_test(fct)
octObject *fct;
/*
 * Detaches some instances from facet and puts them on a bag.
 * Some of these instances are then put back on the facet from the
 * bag.
 */
{
    det_info info;

    info.fct = fct;
    info.bag.type = OCT_BAG;
    info.bag.contents.bag.name = "Instance detach bag";
    info.count = 0;
    (void) octCreate(fct, &info.bag);
    foreach(fct, OCT_INSTANCE_MASK, det_inst, (char *) &info);
    info.count = 0;
    foreach(&info.bag, OCT_INSTANCE_MASK, on_inst, (char *) &info);
    (void) octDelete(&info.bag);
}



static void del_obj(obj, data)
octObject *obj;
char *data;
{
    obj_info *info = (obj_info *) data;

    if ((info->count % DEL_FREQ) == 0) {
	/* Do it */
	delete(info->parent, obj);
    }
    (info->count)++;
}

static void del_layer(lyr, data)
octObject *lyr;
char *data;
{
    obj_info *info = (obj_info *) data;

#ifdef PRINT_TEST
    int32 xid;

    octExternalId(lyr, &xid);
    Printf("LAYER: %s (%d)\n", lyr->contents.layer.name, xid);
#endif
    info->parent = lyr;
    foreach(lyr, OCT_GEO_MASK, del_obj, (char *) info);
}

static void del_test(fct)
octObject *fct;
/*
 * Deletes selected objects.
 */
{
    obj_info info;

    info.count = 0;
    foreach(fct, OCT_LAYER_MASK, del_layer, (char *) &info);
    info.parent = fct;
    foreach(fct, OCT_INSTANCE_MASK, del_obj, (char *) &info);
}

static void new_test(fct)
octObject *fct;
{
    /* Not implemented */
}

#define Strcpy	(void) strcpy

static char *write_op(buf, op)
char *buf;
gu_op op;
{
    switch (op) {
    case GU_NOOP:
	Strcpy(buf, "GU_NOOP");
	break;
    case GU_ADD:
	Strcpy(buf, "GU_ADD");
	break;
    case GU_UPDATE:
	Strcpy(buf, "GU_UPDATE");
	break;
    case GU_REMOVE:
	Strcpy(buf, "GU_REMOVE");
	break;
    default:
	Strcpy(buf, "UNKNOWN");
	break;
    }
    return buf;
}

#define Sprintf	(void) sprintf

static char *write_box(buf, box)
char *buf;
struct octBox *box;
{
    if (box) {
	Sprintf(buf, "(%ld,%ld) (%ld,%ld)",
		(long)box->lowerLeft.x, (long)box->lowerLeft.y,
		(long)box->upperRight.x,(long)box->upperRight.y);
    } else {
	Strcpy(buf, "no_size");
    }
    return buf;
}

static int check_record(change, parent_xid, obj_xid)
gu_change change;
int32 parent_xid;
int32 obj_xid;
/* Returns a non-zero value if a bad record is detected. */
{
    geo_key *key, key_buf;
    geo_update *record;
    char buf1[30], buf2[30], bb1[100], bb2[100];
    int errs = 0;
    struct octBox *rec_box;

    key_buf.parent_xid = parent_xid;
    key_buf.obj_xid = obj_xid;
    key = &key_buf;
    if (st_delete(updates, (char **) &key, (char **) &record)) {
	/* The operators should match */
	if (guOp(change) != record->op) {
	    Fprintf(stderr, "ERROR: Operator mismatch for %ld/%ld: %s != %s\n",
		    (long)parent_xid, (long)obj_xid,
		    write_op(buf1, guOp(change)),
		    write_op(buf2, record->op));
	    errs++;
	}
	/* The old sizes should match */
	if ((rec_box = guOldSize(change)) && record->old_size) {
	    if ((rec_box->lowerLeft.x != record->old_size->lowerLeft.x) ||
		(rec_box->lowerLeft.y != record->old_size->lowerLeft.y) ||
		(rec_box->upperRight.x != record->old_size->upperRight.x) ||
		(rec_box->upperRight.y != record->old_size->upperRight.y)) {
		/* Bad box */
		Fprintf(stderr, "ERROR: Size mismatch for %ld/%ld: %s != %s\n",
			(long)parent_xid, (long)obj_xid,
			write_box(bb1, rec_box),
			write_box(bb2, record->old_size));
		errs++;
	    }
	} else {
	    if ((rec_box && !record->old_size)) {
		Fprintf(stderr,
			"ERROR: No size in update record for %ld/%ld\n",
			(long)parent_xid, (long)obj_xid);
		errs++;
	    } else if ((!rec_box && record->old_size)) {
		Fprintf(stderr,
			"ERROR: No size in change record for %ld/%ld\n",
			(long)parent_xid, (long)obj_xid);
		errs++;
	    }
	}
	FREE(key);
	FREE(record);
    } else {
	/* Not found */
	Fprintf(stderr, "ERROR: Unsolicited change record for %ld/%ld\n",
		(long)parent_xid, (long)obj_xid);
	errs++;
    }
    return errs;
}

static int check_extras()
/* Returns a non-zero value if extras were found in table */
{
    int count = 0;
    st_generator *gen;
    geo_key *key;
    geo_update *record;
    char op_buf[30], box_buf[100];

    gen = st_init_gen(updates);
    while (st_gen(gen, (char **) &key, (char **) &record)) {
	if (record->op != GU_NOOP) {
	    Fprintf(stderr, "ERROR: Undetected change: %ld/%ld %s %s\n",
		    (long)key->parent_xid, (long)key->obj_xid,
		    write_op(op_buf, record->op),
		    write_box(box_buf, record->old_size));
	    count++;
	}
    }
    st_free_gen(gen);
    return count;
}


static int check_changes(fct)
octObject *fct;
{
    gu_gen gen;
    gu_change change;
    int i;
#ifdef PRINT_TEST
    char op_buf[30], box_buf[100];
#endif
    int32 fct_id;
    int err_count = 0;

    for (i = 0;  i < num_layers;  i++) {
#ifdef PRINT_TEST
	Printf("Changes on layer `%s' (%d):\n",
	       marked_layers[i].layer_name,
	       marked_layers[i].layer_id);
#endif
	gen = guStart(marked_layers[i].mark);
	while ( (change = guNext(gen)) ) {
#ifdef PRINT_TEST
	    Printf("op: %s, xid: %d, old_size: %s\n",
		   write_op(op_buf, guOp(change)),
		   guXid(change),
		   write_box(box_buf, guOldSize(change)));
#endif
	    err_count += check_record(change, marked_layers[i].layer_id,
				      guXid(change));
	}
	guEnd(gen);
	guUnmark(marked_layers[i].mark);
    }
#ifdef PRINT_TEST
    Printf("Changes to instances:\n");
#endif
    octExternalId(fct, &fct_id);
    gen = guStart(inst_marker);
    while ( (change = guNext(gen)) ) {
#ifdef PRINT_TEST
	Printf("op: %s, xid: %d, old_size: %s\n",
	       write_op(op_buf, guOp(change)),
	       guXid(change),
	       write_box(box_buf, guOldSize(change)));
#endif
	err_count += check_record(change, fct_id, guXid(change));
    }
    err_count += check_extras();
    guEnd(gen);
    guUnmark(inst_marker);
    return err_count;
}
