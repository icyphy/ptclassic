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
/*
 * Implementations of the the oct functions for instances.
 * 
 * Each instance in a facet is chained off of a master-object 
 * representing the master-facet of that instance.  See master.c for 
 * further details.
 * 
 * Most of the code here is for maintaining the actual terminals of an 
 * instance in sync with the formal terminals of the master
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "io.h"
#include "tr.h"
#include "attach.h"
#include "bb.h"
#include "chain.h"
#include "change_record.h"
#include "create.h"
#include "get.h"
#include "inst.h"
#include "io_procs.h"
#include "master.h"
#include "oct_utils.h"
#include "term.h"

/* Forward references (should be static?) */
octStatus oct_compute_instance_bb
	ARGS((struct instance  *inst));

static int instance_write_fields();
static int instance_read_fields();
static int instance_free_fields();
static int instance_create();
static int instance_uncreate();
static int instance_modify();
static int instance_undelete();
static int instance_delete();
static int instance_free();
static int instance_bb();

extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;

void
oct_instance_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    object_desc->internal_size = sizeof(struct instance);
    object_desc->external_size = sizeof(struct octInstance);
    object_desc->contain_mask = (octObjectMask) (OCT_ALL_MASK & ~OCT_FACET_MASK);
    object_desc->name_offset = FIELD_OFFSET(struct octInstance, name);
    object_desc->user_offset =
      FIELD_OFFSET(struct instance, name) - object_desc->name_offset;
    object_desc->flags |= HAS_NAME;
    object_desc->flags |= HAS_BB_FUNC;
    object_desc->create_func = instance_create;
    object_desc->uncreate_func = instance_uncreate;
    object_desc->delete_func = instance_delete;
    object_desc->undelete_func = instance_undelete;
    object_desc->free_func = instance_free;
    object_desc->write_fields_func = instance_write_fields;
    object_desc->read_fields_func = instance_read_fields;
    object_desc->free_fields_func = instance_free_fields;
    object_desc->modify_func = instance_modify;
    object_desc->unmodify_func = instance_modify;
    object_desc->bb_func = instance_bb;
}


static void check_transform();

static octStatus
instance_create(desc, instance, new_p)
struct facet *desc;
struct octObject *instance;
generic **new_p;
{
    struct octInstance *user_instance = &instance->contents.instance;
    struct facet *instance_desc;
    struct term *ptr;
    struct instance *new;
    struct master *master;
    octStatus retval;

    check_transform(&user_instance->transform);
    
    /* Get the master for this instance so we can inherit the terminals. */
    
    retval = oct_get_master(desc, user_instance, &master);

    if (retval < OCT_OK) {
	return retval;
    } 

    instance_desc = master->master;
    
    /*
     * Now that we know we can do our part, have super do his
     */
    
    retval = (*super->create_func)(desc, instance, new_p);

    if (retval < OCT_OK) {
	return retval;
    }

    new = (struct instance *) *new_p;
    new->name = oct_str_intern(user_instance->name);
    new->transform = user_instance->transform;
    
    ptr = (struct term *) instance_desc->object_lists[OCT_FORMAL];
	RECORD_CHANGE(*new_p, OCT_CREATE, oct_null_xid);   
    /* copy the terminals onto the instance structure */
    /* here lies a big problem: the change record for these attachments and creations will precede the
	   cr for the instance! something drastic needs to be done! */
    while (ptr != NIL(struct term)) {
	retval = oct_add_actual(new, ptr);
	if (retval < OCT_OK) {
	    return retval;
	}
	ptr = (struct term *) ptr->last;
    }

    new->master = master;
    /* chain the instance off of its master */
    DLL_ATTACH(master->instances, new, struct instance, next_inst, last_inst); 

    /* XXX RLS - these were (void) originally */
    if (oct_compute_instance_bb(new) != OCT_OK) {
	oct_prepend_error("bad return from oct_compute_instance_bb");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    return OCT_OK;
}

static octStatus
instance_undelete(desc, instance, new_p, old_xid, old_id)
struct facet *desc;
struct octObject *instance;
generic **new_p;
int32 old_xid, old_id;
{
    struct octInstance *user_instance = &instance->contents.instance;
    struct facet *instance_desc;
    struct instance *new;
    struct master *master;
    octStatus retval;

    check_transform(&user_instance->transform);
    
    /* Get the master for this instance so we can inherit the terminals. */
    
    /* XXX - handled by OCT_1_COMPAT user_instance->facet = "contents"; */
    
    retval = oct_get_master(desc, user_instance, &master);

    if (retval < OCT_OK) {
	return retval;
    } 

    instance_desc = master->master;
    
    /*
     * Now that we know we can do our part, have super do his
     */
    
    retval = (*super->undelete_func)(desc, instance, new_p, old_xid, old_id);

    if (retval < OCT_OK) {
	return retval;
    }

    new = (struct instance *) *new_p;
    new->name = oct_str_intern(user_instance->name);
    new->transform = user_instance->transform;
    
    new->master = master;

	/* chain the instance off of its master */
    DLL_ATTACH(master->instances, new, struct instance, next_inst, last_inst); 

    /* XXX RLS - these were (void) originally */
    if (oct_compute_instance_bb(new) != OCT_OK) {
	oct_prepend_error("bad return from oct_compute_instance_bb");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    return OCT_OK;
}

static double abs_temp;

#define EPSILON 1e-4
#define SINGLE_EVAL_ABS(a) (abs_temp = (a), (abs_temp < 0 ? -abs_temp : abs_temp))
#define EQ(a,b) (SINGLE_EVAL_ABS((a)-(b)) < EPSILON)
		     
/* Check the transform to see if it legal */
static void
check_transform(trans)
struct octTransform *trans;
{
    int orthogonal;
    double (*mtx)[2];
    
    if (trans->transformType == OCT_FULL_TRANSFORM) {
	mtx = trans->generalTransform;
	orthogonal = (EQ(mtx[0][0],mtx[1][1]) && EQ(mtx[1][0],-mtx[0][1])) ||
	             (EQ(mtx[0][0],-mtx[1][1]) && EQ(mtx[1][0],mtx[0][1]));
	if (!orthogonal) {
	    oct_error("Instance transformations must be orthogonal");
	    errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	}
    } else {
	switch (trans->transformType) {
            case OCT_NO_TRANSFORM:
            case OCT_MIRROR_X:
            case OCT_MIRROR_Y:
            case OCT_ROT90:
            case OCT_ROT180:
            case OCT_ROT270:
            case OCT_MX_ROT90:
            case OCT_MY_ROT90:
            case OCT_FULL_TRANSFORM:	/* just for completeness */
		return;
	    default:
	    oct_error("Illegal transform type");
	    errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
        }
    }
    return;
}


/* 
 * add a new actual terminal to `inst' corresponding to the formal 
 * terminal in its master, `term'
 * 
 * Used by the master and term code to update instance terminals when 
 * a new formal terminal is created in the master facet
 */
octStatus
oct_add_actual(inst, term)
struct instance *inst;
struct term *term;
{
    generic *copy;
    struct octObject actual;
    int retval;

    actual.type = OCT_TERM;
    actual.contents.term.name = term->user_term.name;
    actual.contents.term.width = term->user_term.width;
    actual.contents.term.instanceId = inst->objectId;
    retval = oct_do_term_create(inst->facet, &actual, &copy, (int32) 0);
    
    if (retval < OCT_OK) {
	return retval;
    }

    ((struct term *)copy)->user_term.formalExternalId = term->externalId;
    
    (void) oct_attach_content((generic *)inst, (generic *)copy, 0);
    inst->facet->object_count++;
	RECORD_ATTACH((generic *)inst, (generic *)copy, retval);
    return OCT_OK;
}
		
/* 
 * Similar to oct_add_actual, but add a new formal terminal to the 
 * interface facet `facet' corresponding to the formal terminal 
 * `contents_formal' in the contents
 */

octStatus
oct_add_interface_formal(facet, contents_formal, new_p)
struct facet *facet;
struct term *contents_formal;
struct term **new_p;
{
    generic *copy;
    struct octObject formal;
    int retval;

    formal.type = OCT_TERM;
    formal.contents.term.name = contents_formal->user_term.name;
    formal.contents.term.width = contents_formal->user_term.width;
    formal.contents.term.instanceId = oct_null_id;
    
    retval = oct_do_term_create(facet, &formal, &copy, contents_formal->externalId);
    if (retval < OCT_OK) {
	return retval;
    }

    retval = oct_do_attach((generic *) facet, copy, 0);
    if (retval != OCT_OK) {
	return retval;
    }

    ((struct term *)copy)->user_term.formalExternalId = contents_formal->externalId;
    if (new_p) {
	*new_p = (struct term *) copy;
    }

    return OCT_OK;
}
		
static octStatus
instance_delete(iptr, by_user)
generic *iptr;
int by_user;
{
    struct instance *instance = (struct instance *) iptr;
    octStatus retval;
    struct chain *ptr, *next;
    struct octBox bbox;

    if (by_user) {
	(void) oct_bb_get(iptr, &bbox);
	(void) oct_bb_delete(instance->facet, &bbox);
    }

    /* delete the actual terminals as well */
    ptr = instance->contents;
    while (ptr != NIL(struct chain)) {
	next = ptr->last;
	if (ptr->object->flags.type == OCT_TERM) {
	    struct term *term = (struct term *) ptr->object;
	    if (term->user_term.instanceId == instance->objectId) {
		retval = oct_do_delete((generic *) term, 0);
		if (retval != OCT_OK) {
		    return retval;
		}
	    }
	}
	ptr = next;
    }

    DLL_DETACH(instance->master->instances, instance, struct instance,
	       next_inst, last_inst);
    
    return (*super->delete_func)(iptr, by_user);
}

static octStatus
instance_uncreate(iptr, by_user)
generic *iptr;
int by_user;
{
    struct instance *instance = (struct instance *) iptr;
    struct octBox bbox;

    if (by_user) {
	(void) oct_bb_get(iptr, &bbox);
	(void) oct_bb_delete(instance->facet, &bbox);
    }

	/* don't waste time with the actuals. they are(were?) in the changelist,
	   and should already be gone by now. */
	
    DLL_DETACH(instance->master->instances, instance, struct instance,
	       next_inst, last_inst);
    
    return (*super->uncreate_func)(iptr, by_user);
}

static octStatus
instance_free(iptr)
generic *iptr;
{
    struct instance *instance = (struct instance *) iptr;

    DLL_DETACH(instance->master->instances, instance, struct instance,
	       next_inst, last_inst);
    
    return (*super->free_func)(iptr);
}

static int instance_bb(iptr, user_box)
generic *iptr;
struct octBox *user_box;
{
    struct instance *inst = (struct instance *) iptr;
    int retval;
    
    if (inst->flags.bb_valid) {
	if (user_box != NIL(struct octBox)) {
	    *user_box = inst->bbox;
	}
	return OCT_OK;
    } else if (!inst->flags.bb_init) {
	/* first time we have asked for the bb of this instance since it was read in */
	if ((retval = oct_compute_instance_bb(inst)) < OCT_OK) {
	    return retval;
	}
	if (user_box != NIL(struct octBox)) {
	    *user_box = inst->bbox;
	}
	return OCT_OK;
    } else {
	return oct_recompute_instance_bb(inst, user_box);
    }
}

/* 
 * recompute the bounding box of an instance by transforming the 
 * bounding box of its master.
 * 
 * Called for each instance of a facet when that facets bounding box 
 * is changed
 */

octStatus
oct_recompute_instance_bb(inst, user_box)
struct instance  *inst;
struct octBox *user_box;
{
    struct octBox old_bb;

    old_bb = inst->bbox;

    if (!oct_compute_instance_bb(inst)) {
	return OCT_ERROR;
    }

    if (user_box != NIL(struct octBox)) {
	*user_box = inst->bbox;
    }

    return oct_bb_modify((generic *) inst, &old_bb, &inst->bbox);
}
    
/* 
 * compute the bounding box of inst and update inst->bbox.
 * oct_bb_modify is NOT called.
 * 
 * This is used to initially set the bounding box of the instance on
 * read in.
 */

octStatus
oct_compute_instance_bb(inst)
struct instance  *inst;
{
    struct octTransform *trans = &inst->transform;
    int32 tmp;
    int retval;
    struct octBox tmpBox;	/* AC  */

    /* XXX what if the master can not be found (i.e. master->master is nil) */
    if (inst->master->master) {
	retval = oct_bb_get((generic *) inst->master->master, &tmpBox);
    } else {
	retval = OCT_NO_BB;
    }

    if (retval == OCT_NO_BB) {
	tmpBox.lowerLeft.x = tmpBox.lowerLeft.y = 0;
	tmpBox.upperRight.x = tmpBox.upperRight.y = 0;
    } else if (retval < OCT_OK) {
	oct_prepend_error(
	      "Getting the bounding box of the master of an instance");
	return retval;
    }
	
    tr_oct_transform(trans, &tmpBox.lowerLeft.x, &tmpBox.lowerLeft.y);
    tr_oct_transform(trans, &tmpBox.upperRight.x, &tmpBox.upperRight.y);
    
    inst->bbox = tmpBox;
    /* 
     * Fix the bounding box so the lowerLeft is actually lower and 
     * lefter than the upperRight
     */
    if (inst->bbox.lowerLeft.x > inst->bbox.upperRight.x) {
	tmp = inst->bbox.lowerLeft.x;
	inst->bbox.lowerLeft.x = inst->bbox.upperRight.x;
	inst->bbox.upperRight.x = tmp;
    }
    
    if (inst->bbox.lowerLeft.y > inst->bbox.upperRight.y) {
	tmp = inst->bbox.lowerLeft.y;
	inst->bbox.lowerLeft.y = inst->bbox.upperRight.y;
	inst->bbox.upperRight.y = tmp;
    }

    inst->flags.bb_init = inst->flags.bb_valid = 1;
    return OCT_OK;
}
    
static int
instance_free_fields(iptr)
generic *iptr;
{
    struct instance *instance = (struct instance *) iptr;
    
    if (instance->name != NIL(char)) {
	oct_str_free(instance->name);
    }
    return 1;
}


/*
 * XXX: NOTE: experimental modify that allows master/view/facet/version
 * fields to be changed - this will not work for changes that have terminal
 * differences.  This may not work with UNDO.
 */

static octStatus
instance_modify(iptr, obj)
generic *iptr;
struct octObject *obj;
{
    struct instance *instance = (struct instance *) iptr;
    struct octInstance *user_instance = &obj->contents.instance;
    struct master *master = instance->master;
    char *name = obj->contents.instance.name;
    octBox dummy_bb;
    int val, retval;

    check_transform(&obj->contents.instance.transform);

    val =
      !STR_EQ(master->cell, obj->contents.instance.master) ||
	!STR_EQ(master->view, obj->contents.instance.view);

    val = val ||
   /* 
    * XXX backwards compat.
      !STR_EQ(master->facet, obj->contents.instance.facet) ||
    */
      !STR_EQ(master->version, obj->contents.instance.version);
    
    if (val) {
#ifdef OLD_STUFF
	oct_error(
	   "The master, view, facet or version of a instance cannot be modified");
	return OCT_ERROR;
#else
	octBox bbox;
	oct_bb_get(iptr, &bbox);
	oct_bb_delete(instance->facet, &bbox);

	retval = oct_get_master(instance->facet, user_instance, &master);
	if (retval < OCT_OK) {
	    oct_error("new master does not exist");
	    return retval;
	}

	DLL_DETACH(instance->master->instances, instance, struct instance,
		   next_inst, last_inst);
	instance->master = master;
	DLL_ATTACH(instance->master->instances, instance, struct instance,
		   next_inst, last_inst);
	oct_bb_get(iptr, &bbox);
	oct_bb_add(instance->facet, &bbox);
#endif
    }

    if (!oct_bb_get(iptr, &dummy_bb)) {
	return OCT_ERROR;
    }

    instance->transform = obj->contents.instance.transform;

    MODIFY_NAME(instance->name, name);
    instance->name = name;

    retval = oct_recompute_instance_bb(instance, NIL(struct octBox));
    if (retval != OCT_OK) {
	(void) fprintf(stderr, "oct_recompute_instance_bb on %s:%s returned %d\n",
			instance->master->cell,
			instance->master->view,
			retval);
    }

    return OCT_OK;
}

static octStatus
instance_read_fields(ptr)
generic *ptr;
{
    struct instance *instance;
    int val, type;
    
    instance = (struct instance *) ptr;
    
    val = (*super->read_fields_func)(ptr) &&
            oct_get_string(&instance->name) &&
	    oct_get_32(&instance->transform.translation.x) &&
	    oct_get_32(&instance->transform.translation.y) &&
	    oct_get_byte(&type);
    
    if (!val) {
	return OCT_ERROR;
    }

    instance->transform.transformType = (octTransformType) type;
    
    if (instance->transform.transformType == OCT_FULL_TRANSFORM) {
	val = oct_get_double(&instance->transform.generalTransform[0][0]) &&
		oct_get_double(&instance->transform.generalTransform[0][1]) &&
		oct_get_double(&instance->transform.generalTransform[1][0]) &&
		oct_get_double(&instance->transform.generalTransform[1][1]);
    }
    
    return val;
}
    
static octStatus
instance_write_fields(iptr)
generic *iptr;
{
    struct instance *instance = (struct instance *) iptr;
    int val;

    val =   (*super->write_fields_func)(iptr) &&
            oct_put_string(instance->name) &&
	    oct_put_32(instance->transform.translation.x) &&
	    oct_put_32(instance->transform.translation.y) &&
	    oct_put_byte((int)instance->transform.transformType);

    if (!val) {
	return val;
    }

    if (instance->transform.transformType == OCT_FULL_TRANSFORM) {
	val = oct_put_double(instance->transform.generalTransform[0][0]) &&
		oct_put_double(instance->transform.generalTransform[0][1]) &&
		oct_put_double(instance->transform.generalTransform[1][0]) &&
		oct_put_double(instance->transform.generalTransform[1][1]);
    }
    
    return val;
}

int oct_only_trivial_attachements(obj, container)
generic *obj;
generic *container;
{
    struct chain *ptr;

    /* nothing is allowed to be attached */
    
    if (obj->contents != NIL(struct chain)) {
	return 0;
    }

    ptr = obj->containers;

    /* and it is only allowed to be attached to `container'*/
    
    if (ptr != NIL(struct chain)) {
	if (ptr->container != container || ptr->next_chain != NIL(struct chain)) {
	    return 0;
	}
    }
    return 1;
}

/* 
 * find the formal terminal in `master' that corresponds to the actual 
 * terminal `term'
 */

static struct term *find_contents_formal(master, term)
struct facet *master;
struct term *term;
{
    int32 xid = term->user_term.formalExternalId;

    if (master->facet_flags.all_loaded) {
	/*XXX currently, xid table is only built with the full facet */
	return (struct term *) oct_xid_to_ptr(master, xid);
    } else {
	struct term *ptr = (struct term *) master->object_lists[OCT_FORMAL];
	
	while (ptr != NIL(struct term)) {
	    if (ptr->externalId == xid) {
		return ptr;
	    }
	    ptr = (struct term *) ptr->last;
	}

	return NIL(struct term);
    }
}

/* 
 * Delete the actual terminal `term' in `inst'.  Complain if the 
 * terminal has anything interesting attached to it.
 * 
 * called when a formal terminal in the master has been deleted.
 */

int
oct_delete_actual(inst, term)
struct instance *inst;
struct term *term;
{
    int retval;
    
    if (oct_only_trivial_attachements((generic *) term, (generic *) inst)) {
	retval = oct_do_delete((generic *) term, 0);
    } else {
	retval = oct_add_to_error_bag((generic *) term);
	if (retval == OCT_INCONSISTENT) {
	    retval = OCT_OK;
	}
    }
    return retval;
}
    
/*
 * the master of inst has changed somehow, update the actuals of inst.
 * 
 * Called at read in of a instance when it has been detected that the 
 * master has changed since the instance has last been read in.
 */

int
oct_update_terminals(master, inst)
struct facet *master;
struct instance *inst;
{
    struct chain *ptr, *next;
    struct term *term;
    int retval, final_val = OCT_OK;

    ptr = inst->contents;

    /* 
     * for each actual on the instance, look for a corresponding 
     * terminal on the master.  If there isn't one, delete the actual.
     * otherwise mark the formal to show that a corresponding actual does exist
     */
    while (ptr != NIL(struct chain)) {
	next = ptr->last;
	if (ptr->object->flags.type == OCT_TERM) {
	    term = (struct term *) ptr->object;
	    if (term->user_term.instanceId == inst->objectId) {
		struct term *formal_term = find_contents_formal(master, term);
		if (formal_term == NIL(struct term)) {
		    /* The corresponding formal has been deleted */
		    retval = oct_delete_actual(inst, term);
		    if (retval != OCT_OK) {
			final_val = retval;
		    }
		} else {
		    char *old_name = term->user_term.name;
		    term->user_term.name = formal_term->user_term.name;
		    MODIFY_NAME(old_name, term->user_term.name);
		    formal_term->flags.is_marked = 1;
		}
	    }
	}
	ptr = next;
    }
    
    term = (struct term *) master->object_lists[OCT_FORMAL];
    
    /* 
     * now, for each formal in the master that hasn't been marked 
     * (i.e. for each one that doesn't have a corresponding actual on 
     * the instance) create a actual
     */
    while (term != NIL(struct term)) {
	if (!term->flags.is_marked) {
	    /* a new terminal from the master */
	    retval = oct_add_actual(inst, term);
	    if (retval < OCT_OK) {
		return retval;
	    }
	} else {
	    term->flags.is_marked = 0;
	}
	term = (struct term *) term->last;
    }
    
    return final_val;
}

/* 
 * The error bag contains obsolete actuals that we couldn't delete 
 * because there was still things attached to them and instances that 
 * we couldnt check because we couldn't open the associated masters
 */
int
oct_add_to_error_bag(obj)
generic *obj;
{
    octObject bag, error_obj;
    generic *facet;

    facet = (generic *) obj->facet;
    
    bag.type = OCT_BAG;
    bag.contents.bag.name = OCT_INCONSISTENT_BAG;

    if (oct_get_or_create(facet, &bag) != OCT_OK) {
	oct_prepend_error("Adding to the error bag:");
	return OCT_ERROR;
    }

    (void) octMarkTemporary(&bag);
    
    error_obj.objectId = obj->objectId;
    error_obj.type = obj->flags.type;

    /* XXX fix this - internal functions... */
    if (octAttachOnce(&bag, &error_obj) != OCT_OK) {
	oct_prepend_error("Adding to the error bag:");
	return OCT_ERROR;
    }
    
    oct_error("A formal or actual terminal is obsolete but still has attachments");
    return OCT_INCONSISTENT;
}
    
/* 
 * Most of the internal objects contain a copy of the associated 
 * user_object, so passing objects back to the user simply involves 
 * copying this sub-structure into his.  But some of the important 
 * information for an instance is stored in the master object, so 
 * instances need a special function for filling in a user structure
 */
 
void
oct_fill_instance(object, g_inst)
octObject *object;
generic *g_inst;
{
    struct instance *instance = (struct instance *) g_inst;
    struct master *master = instance->master;
    struct octInstance *u_inst = &object->contents.instance;

    u_inst->name = instance->name;
    u_inst->master = master->cell;
    u_inst->view = master->view;
    u_inst->facet = master->facet;
    u_inst->version = master->version;
    u_inst->transform = instance->transform;
    object->type = OCT_INSTANCE;
    object->objectId = instance->objectId;
    return;
}

void
oct_copy_instance(instance, u_inst)
struct instance *instance;
struct octInstance *u_inst;
{
    struct master *master = instance->master;

    u_inst->name = oct_str_intern(instance->name);
    u_inst->master = oct_str_intern(master->cell);
    u_inst->view = oct_str_intern(master->view);
    u_inst->facet = oct_str_intern(master->facet);
    u_inst->version = oct_str_intern(master->version);
    u_inst->transform = instance->transform;
    return;
}

void
oct_free_instance_fields(u_inst)
struct octInstance *u_inst;
{
    oct_str_free(u_inst->name);
    oct_str_free(u_inst->master);
    oct_str_free(u_inst->view);
    oct_str_free(u_inst->facet);
    oct_str_free(u_inst->version);
    return;
}
