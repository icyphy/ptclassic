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
#include "internal.h"

#include "default.h"
#include "facet_info.h"
#include "inst.h"
#include "io_procs.h"
#include "oct_utils.h"

#include "master.h"
  /* 
   * Implementation of the oct functions for master-object.
   * 
   * A facet contains a master-object for each unique master-facet 
   * that is instantiated in that facet.  Each instance in a facet of a 
   * particular master-facet is chained off of the associated 
   * master-object in that facet.  The master-object is in turn 
   * chained of its associated master-facet.  For example, if a 
   * facet `adder' contains to instances, `p1' and p2' of a facet
   * `transistor' and one instance `r1' of a facet `resistor', then there
   * be two master-objects in `adder', one for `transistor' with 
   * instances `p1' and `p2' chained off of it and one 
   * for `resistor' with `r1' chained off it.  The master-object in 
   * `adder' representing `transistor' will be chained off of the 
   * `transistor' facet structure, and the `resistor' master-object in 
   * `adder' will be similarly chained off of the 'resistory' facet.
   * 
   * This arraignment allows finding all the facets that instantiate 
   * a particular facet easy as well as finding all the instaces of 
   * that particular facet.
   */
extern struct object_desc oct_default_desc; 

void
oct_master_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = oct_default_desc;
    
    object_desc->internal_size = sizeof(struct master);
}

#define SAFE_CMP(p1,p2)\
  ((p1) == NIL(char) ? ((p1) == (p2) ? 0 : -1) :\
                       ((p2) == NIL(char) ? 1 : strcmp(p1,p2)))

static int
master_compare(c_a, c_b)
char *c_a;
char *c_b;
{
    struct master *a = (struct master *) c_a;
    struct master *b = (struct master *) c_b;
    int retval;

    (void) ((retval = SAFE_CMP(a->cell, b->cell)) ||
      (retval = SAFE_CMP(a->view, b->view)) ||
      (retval = SAFE_CMP(a->facet, b->facet)) ||
      (retval = SAFE_CMP(a->version, b->version)));

    return retval;
}

#define SAFE_HASH(ptr, max) ((ptr) == NIL(char) ? 0 : st_strhash(ptr,max))
  
static int
master_hash(c_master, max)
char *c_master;
int max;
{
    struct master *master = (struct master *) c_master;
    int retval;

    retval = SAFE_HASH(master->cell, max) +
      SAFE_HASH(master->view, max) +
      SAFE_HASH(master->facet, max) +
      SAFE_HASH(master->version, max);
    
    return retval%max;
}
    
master_table *oct_make_master_table()
{
    return (master_table *) st_init_table(master_compare, master_hash);
}

void
oct_free_master_table(table)
master_table *table;
{
    st_free_table((st_table *) table);
    return;
}

/*
 * oct_make_master is used by translate.c.  Because of some timing
 * problems, the making and binding have to be done at seperate times
 * during translation.
 * Normally only oct_get_master should be used.
 */

int
oct_make_master(desc, inst, master_p)
struct facet *desc;
struct octInstance *inst;
struct master **master_p;
{
    struct master dummy_master, *new;
    char *junk;

    dummy_master.cell = inst->master;
    dummy_master.view = inst->view;
    dummy_master.facet = inst->facet;
    dummy_master.version = inst->version;

    if (st_lookup((st_table *) desc->masters, (char *) &dummy_master, &junk)) {
	if (master_p != NIL(struct master *)) {
	    *master_p = (struct master *) junk;
	}
	return OCT_ALREADY_OPEN;
    }

    new = ALLOC(struct master, 1);
    
    *new = dummy_master;
    
    new->cell = oct_str_intern(new->cell);
    new->view = oct_str_intern(new->view);
    new->facet = oct_str_intern(new->facet);
    new->version = oct_str_intern(new->version);

    new->instances = NIL(struct instance);
    new->master_bb_date = new->master_formal_date = 0;
    new->master = NIL(struct facet);
    
    if (master_p != NIL(struct master *)) {
	*master_p = new;
    }

    (void) st_add_direct((st_table *) desc->masters, (char *) new, (char *) new);
    
    return OCT_OK;
}

octStatus
oct_remove_master(desc, master)
struct facet *desc;
struct master *master;
{
    char *junk;

    (void) st_delete((st_table *) desc->masters, (char **) &master, &junk);

    return OCT_OK;
}


octStatus
oct_bind_master(desc, master)
struct facet *desc;
struct master *master;
{
    struct octFacet dummy_facet;
    struct facet *master_desc;
    int retval;
    
    dummy_facet.cell = master->cell;
    dummy_facet.view = master->view;
    dummy_facet.facet = master->facet;
    dummy_facet.version = master->version;

    retval = oct_get_facet_info(&dummy_facet, &master_desc, desc);
    
    if (retval < OCT_OK) {
	return retval;
    }

    master->master_bb_date = master_desc->bb_date;
    master->master_formal_date = master_desc->formal_date;
    master->master = master_desc;

    DLL_ATTACH(master_desc->instantiations, master, struct master, next, last);
    
    return OCT_OK;
}

/*
 * get or create a master object on the facet `desc'
 * for the instance `inst'.
 */
int
oct_get_master(desc, inst, master_p)
struct facet *desc;
struct octInstance *inst;
struct master **master_p;
{
    int retval;
    
    if ((retval = oct_make_master(desc, inst, master_p)) < OCT_OK) {
	(void) oct_remove_master(desc, *master_p);
	return retval;
    }

    if (retval == OCT_ALREADY_OPEN) {
	return retval;
    }

    if ((retval = oct_bind_master(desc, *master_p)) < OCT_OK) {
	(void) oct_remove_master(desc, *master_p);
	return retval;
    }

    return retval;
}


/*
 * write the master out
 */
static octStatus
write_master(master)
struct master *master;
{
    return oct_put_string(master->cell) &&
          oct_put_string(master->view) &&
          oct_put_string(master->facet) &&
          oct_put_string(master->version) &&
          oct_put_32(master->master_formal_date) &&
          oct_put_32(master->master_bb_date);
}

/*
 * read the master in
 */
static int
read_master(desc, master_p)
struct facet *desc;
struct master **master_p;
{
    struct master *new = ALLOC(struct master, 1);
    int val;

    val = oct_get_string(&new->cell) &&
          oct_get_string(&new->view) &&
          oct_get_string(&new->facet) &&
          oct_get_string(&new->version) &&
          oct_get_32(&new->master_formal_date) &&
          oct_get_32(&new->master_bb_date);

    if (!val) {
	return 0;
    }

    new->master = NIL(struct facet);
    new->instances = NIL(struct instance);
    
    (void) st_add_direct((st_table *) desc->masters, (char *) new, (char *) new);
    
    if (master_p != NIL(struct master *)) {
	*master_p = new;
    }
    
    return 1;
}

/*ARGSUSED*/
static enum st_retval
do_dump_instances(c_master, junk1, c_retval)
char *c_master;
char *junk1;
char *c_retval;
{
    struct master *master = (struct master *) c_master;
    struct instance *instance;
    int *retval_p = (int *) c_retval;

    if (master->instances == NIL(struct instance)) {
	return ST_CONTINUE;
    }
	
    if (!oct_put_nl() || !oct_put_type(OCT_MASTER) || !write_master(master)) {
	*retval_p = 1;
	return ST_STOP;
    }

    instance = master->instances;
    while (instance != NIL(struct instance)) {
	if (oct_write_object((generic *) instance) != OCT_OK) {
	    *retval_p = 1;
	    return ST_STOP;
	}
	instance = instance->last_inst;
    }
    
    return ST_CONTINUE;
}

int
oct_dump_instances(desc)
struct facet *desc;
{
    int retval = 1;

    (void) st_foreach((st_table *) desc->masters, do_dump_instances, (char *) &retval);
    
    return retval;
}

int
oct_restore_instances(desc, next_type)
struct facet *desc;
int *next_type;
{
    int type;

    if (!oct_get_type(&type)) {
	return 0;
    }
    
    while (type == OCT_MASTER) {
	struct master *master;
	generic *inst;
	
	if (!read_master(desc, &master)) {
	    return 0;
	}
	desc->object_count++;
	
	if (!oct_get_type(&type)) {
	    return 0;
	}
    
	while (type == OCT_INSTANCE) {
	    if (!oct_read_object(desc, type, &inst)) {
		return 0;
	    }
	    desc->object_count++;
	    DLL_ATTACH(master->instances, ((struct instance *) inst),
		       struct instance, next_inst, last_inst);
	    
	    ((struct instance *) inst)->master = master;
	    
	    if (!oct_get_type(&type)) {
		return 0;
	    }
	}
    }

    *next_type = type;
    return 1;
}

octStatus
oct_update_all_instance_bbs(desc)
struct facet *desc;
{
    struct master *master = desc->instantiations;
    int retval;

    while (master != NIL(struct master)) {
	struct instance *instance = master->instances;
	master->master_bb_date = desc->bb_date;
	while (instance != NIL(struct instance)) {
	    retval = oct_recompute_instance_bb(instance, NIL(struct octBox));
	    if (retval < OCT_OK) {
		return retval;
	    }
	    instance = instance->last_inst;
	}
	master = master->last;
    }
    
    return OCT_OK;
}

octStatus
oct_invalidate_all_instance_bbs(desc)
struct facet *desc;
{
    struct master *master = desc->instantiations;

    while (master != NIL(struct master)) {
	struct instance *instance = master->instances;
	if (instance != NIL(struct instance)) {
	    if (!instance->facet->facet_flags.bb_invalid_instances) {
		instance->facet->facet_flags.bb_invalid_instances = 1;
		(void) oct_invalidate_all_instance_bbs(instance->facet);
	    }
	}
	while (instance != NIL(struct instance)) {
	    instance->flags.bb_valid = 0;
	    instance = instance->last_inst;
	}
	master = master->last;
    }
    return OCT_OK;
}

/*ARGSUSED*/
static enum st_retval
do_validate_instance_bbs(c_master, junk1, c_retval)
char *c_master;
char *junk1;
char *c_retval;
{
    struct master *master = (struct master *) c_master;
    struct instance *instance;
    int retval, *retval_p = (int *) c_retval;

       /*
	* XXX: linear search to allow lazy facet bb evaluation.
	* Is this a good trade off?
	*/

    instance = master->instances;
    
    if (instance != NIL(struct instance) && !VALID_BB(&instance->bbox)) {
	while (instance != NIL(struct instance)) {
	    retval = oct_recompute_instance_bb(instance, NIL(struct octBox));
	    if (retval != OCT_OK) {
		oct_prepend_error("Updating instance bounding boxes:");
		*retval_p = retval;
		return ST_STOP;
	    }
	    instance = instance->last_inst;
	}
    }
    return ST_CONTINUE;
}

octStatus
oct_validate_all_instance_bbs(facet)
struct facet *facet;
{
    int retval = OCT_OK;

    (void) st_foreach((st_table *) facet->masters, do_validate_instance_bbs,
	       (char *) &retval);
    
    return retval;
}
