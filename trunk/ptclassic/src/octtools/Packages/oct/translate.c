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
#include "internal.h"
#include "io.h"
#include "geo.h"
#include "master.h"

#include "chain.h"
#include "io_procs.h"
#include "oct_utils.h"

#include "translate.h"

static  octStatus fix_actuals();


#define OLD_GET_ID_AND_PTR(id, ptr, type) \
{\
    if (!oct_get_32(&id)) {\
	return 0;\
    }\
    id += oct_id_offset;\
    ptr = oct_id_to_ptr(id);\
    if (ptr == NIL(generic)) {\
	ptr = (struct generic *) malloc((unsigned) oct_object_descs[type].internal_size);\
	ptr->contents = ptr->containers = NIL(struct chain);\
	CLEAR_FLAGS(ptr);\
	if (id != oct_new_id(ptr)) {\
	    oct_error("Panic: Corrupted facet: id's out of sequence");\
	    return 0;\
	}\
    }\
}

static octStatus term_read(), net_read(), instance_read(), polygon_read(),
	  box_read(), circle_read(), path_read(), label_read(),
	  prop_read(), bag_read(), layer_read(), point_read(),
	  edge_read(), term_read();
	  
static octStatus (*read_funcs[])() = {
    oct_illegal_op,
    oct_illegal_op,
    term_read,
    net_read,
    instance_read,
    polygon_read,
    box_read,
    circle_read,
    path_read,
    label_read,
    prop_read,
    bag_read,
    layer_read,
    point_read,
    edge_read,
    term_read,
};

octId oct_id_offset;

#define BB_UNINITIALIZED 0
#define BB_INIT 1
#define BB_VALID 2

int
oct_translate(version_number, desc)
int version_number;
struct facet *desc;
{
    generic *ptr;
    int32 id;
    int type, val, i;
    int bb_flags;

    if (version_number != 0 && version_number != 1) {
	oct_error("Panic: oct version mismatch. Possibly corrupt facet");
	return 0;
    }

    desc->facet_flags.old_facet = 1;

    ((generic *) desc)->contents = NIL(struct chain);
    val = oct_get_type(&type) &&
	   oct_get_32(&id) &&
	   oct_get_byte(&bb_flags) &&
	   oct_get_box(&desc->bbox) &&
	   oct_get_props((generic *) desc) &&
	   oct_get_contents((generic *) desc);

    if (!val || id != oct_null_id || type != OCT_FACET) {
	oct_error("The facet is corrupted");
	return 0;
    }

    if (bb_flags&BB_INIT) desc->facet_flags.bb_init = 1;
    if (bb_flags&BB_VALID) desc->facet_flags.bb_valid = 1;
    
    while (oct_get_type(&type) && type != OCT_UNDEFINED_OBJECT) {
	if ((*read_funcs[type])(desc) != OCT_OK) {
	    return 0;
	}
	desc->object_count++; /* recompute, to keep sane */
    }
    
    if (type != OCT_UNDEFINED_OBJECT) {
	return 0;
    }

    /*
     * this is important: the formal terminals must have their xids
     * assigned first, since that is how fix_actuals assumes it will
     * happen
     */
    
    ptr = desc->object_lists[OCT_FORMAL]; 
    while (ptr != NIL(generic)) {
	ptr->externalId = oct_next_xid(desc, ptr);
	((struct term *)ptr)->user_term.formalExternalId = ptr->externalId;
	ptr = ptr->last;
    }
    
    for(i = 2; i <= OCT_MAX_TYPE; i++) {
	if (i == OCT_FORMAL) {
	    continue;
	}
	ptr = desc->object_lists[i];
	while (ptr != NIL(generic)) {
	    ptr->externalId = oct_next_xid(desc, ptr);
	    ptr = ptr->last;
	}
    }

    ptr = desc->object_lists[OCT_INSTANCE];
    while (ptr != NIL(generic)) {
	if (fix_actuals((struct instance *) ptr) != OCT_OK) {
	    return 0;
	}
	ptr = ptr->last;
    }

    desc->facet_flags.all_loaded = 1;
    desc->facet_flags.info_loaded = 1;
    desc->objectId = oct_new_id((generic *)desc);
    return 1;
}

struct term *find_formal(facet, actual)
struct facet *facet;
struct term *actual;
{
    char *target = actual->user_term.name;
    struct term *term = (struct term *) facet->object_lists[OCT_FORMAL];

    while (term != NIL(struct term)) {
	if (strcmp(term->user_term.name, target) == 0) {
	    return term;
	}
	term = (struct term *) term->last;
    }
    
    return NIL(struct term);
}

static octStatus
fix_actuals(inst)
struct instance *inst;
{
    struct chain *ptr;
    int no_master = 0;

    ptr = inst->contents;
    
    if (inst->master->master == NIL(struct facet)) {
	if (oct_bind_master(inst->facet, inst->master) < OCT_OK) {
	    no_master = 1;
	}
    }

    while (ptr != NIL(struct chain)) {
	if (ptr->object->flags.type == OCT_TERM) {
	    struct term *term = (struct term *) ptr->object;
	    if (term->user_term.instanceId == inst->objectId) {
		struct term *formal;
		if (no_master || (formal = find_formal(inst->master->master, term)) == NIL(struct term)) {
		    term->user_term.formalExternalId = oct_null_id;
		} else {
		    term->user_term.formalExternalId = formal->externalId;
		}
	    }
	}
	ptr = ptr->last;
    }
    return OCT_OK;
}

static octStatus
instance_read(desc)
struct facet *desc;
{
    struct instance *instance;
    struct octTransform *trans;
    struct octInstance u_inst;
    struct master *master;
    generic *ptr;
    int32 id;
    int val, type, retval;
    
    OLD_GET_ID_AND_PTR(id, ptr, OCT_INSTANCE);
    instance = (struct instance *) ptr;

    trans = &instance->transform;
    
    val = oct_get_string(&instance->name) &&
	    oct_get_string(&u_inst.master) &&
	    oct_get_string(&u_inst.view) &&
	    oct_get_string(&u_inst.version) &&
	    oct_get_32(&trans->translation.x) &&
	    oct_get_32(&trans->translation.y) &&
	    oct_get_byte(&type);
    
    if (!val) {
	return OCT_ERROR;
    }

    u_inst.facet = oct_str_intern("contents");

    trans->transformType = (octTransformType) type;
    
    if (trans->transformType == OCT_FULL_TRANSFORM) {
	val = oct_old_get_double(&trans->generalTransform[0][0]) &&
		oct_old_get_double(&trans->generalTransform[0][1]) &&
		oct_old_get_double(&trans->generalTransform[1][0]) &&
		oct_old_get_double(&trans->generalTransform[1][1]);
    }
    
    ptr->contents = NIL(struct chain);
    val = val && oct_get_box(&instance->bbox) &&
            oct_get_props((generic *) instance) &&
	    oct_get_contents((generic *) instance);
    
    if (!val) {
	return OCT_ERROR;
    }

    retval = oct_make_master(desc, &u_inst, &master);
    
    if (retval < OCT_OK) {
	return retval;
    }
    
    oct_str_free(u_inst.master);
    oct_str_free(u_inst.view);
    oct_str_free(u_inst.version);
    oct_str_free(u_inst.facet);
    
    instance->flags.type = OCT_INSTANCE;
    instance->objectId = id;
    instance->facet = desc;
    instance->master = master;
    
    DLL_ATTACH(master->instances, instance, struct instance,
	       next_inst, last_inst);
    
    oct_attach_generic_list(desc->object_lists[OCT_INSTANCE], instance);
    return OCT_OK;
}

static octStatus
label_read(desc)
struct facet *desc;
{
    struct label *label;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_LABEL);
    label = (struct label *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_string(&label->user_label.label) &&
            oct_get_32(&label->user_label.region.lowerLeft.x) &&
	    oct_get_32(&label->user_label.region.lowerLeft.y) &&
	    oct_get_props((generic *) label) &&
	    oct_get_contents((generic *) label);
    
    if (!val) {
	return OCT_ERROR;
    }

    label->user_label.region.upperRight.x =
				    label->user_label.region.lowerLeft.x + 100;
    label->user_label.region.upperRight.y =
				    label->user_label.region.lowerLeft.y + 20;
    label->user_label.textHeight = 20;
    label->user_label.vertJust = OCT_JUST_BOTTOM;
    label->user_label.horizJust = OCT_JUST_LEFT;
    label->user_label.lineJust = OCT_JUST_LEFT;

    label->flags.type = OCT_LABEL;
    label->objectId = id;
    label->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_LABEL], label);
    return OCT_OK;
}

static octStatus
layer_read(desc)
struct facet *desc;
{
    struct layer *layer;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_LAYER);
    layer = (struct layer *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_string(&layer->user_layer.name) && 
	    oct_get_props((generic *) layer) &&
	    oct_get_contents((generic *) layer);

    if (!val) {
	return 0;
    }

    layer->flags.type = OCT_LAYER;
    layer->objectId = id;
    layer->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_LAYER], layer);
    
    return 1;
}

static octStatus
net_read(desc)
struct facet *desc;
{
    struct net *net;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_NET);
    net = (struct net *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_string(&net->user_net.name) &&
	    oct_get_props((generic *) net) &&
	    oct_get_contents((generic *) net);
    
    if (!val) {
	return OCT_ERROR;
    }

    net->user_net.width = 1;
    net->flags.type = OCT_NET;
    net->objectId = id;
    net->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_NET], net);
    return OCT_OK;
}

static octStatus
path_read(desc)
struct facet *desc;
{
    struct path *path;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_PATH);
    path = (struct path *) ptr;

    ptr->contents = NIL(struct chain);
    val = oct_get_32(&path->user_path.width) &&
	    oct_get_box(&path->bbox) &&
            oct_get_points(&path->num_points, &path->points) &&
	    oct_get_props((generic *) path) &&
	    oct_get_contents((generic *) path);
    
    if (!val) {
	return OCT_ERROR;
    }

    path->flags.type = OCT_PATH;
    path->objectId = id;
    path->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_PATH], path);
    return OCT_OK;
}


static octStatus
point_read(desc)
struct facet *desc;
{
    struct point *point;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_POINT);
    point = (struct point *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_point(&point->user_point) &&
	    oct_get_props((generic *) point) &&
	    oct_get_contents((generic *) point);
    
    if (!val) {
	return OCT_ERROR;
    }

    point->flags.type = OCT_POINT;
    point->objectId = id;
    point->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_POINT], point);
    return OCT_OK;
}

static octStatus
polygon_read(desc)
struct facet *desc;
{
    struct polygon *polygon;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_POLYGON);
    polygon = (struct polygon *) ptr;

    ptr->contents = NIL(struct chain);
    val = oct_get_box(&polygon->bbox) &&
	    oct_get_points(&polygon->num_points, &polygon->points) &&
	    oct_get_props((generic *) polygon) &&
	    oct_get_contents((generic *) polygon);
    
    if (!val) {
	return OCT_ERROR;
    }

    polygon->flags.type = OCT_POLYGON;
    polygon->objectId = id;
    polygon->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_POLYGON], polygon);
    return OCT_OK;
}

static octStatus
prop_read(desc)
struct facet *desc;
{
    int val, type;
    generic *ptr;
    int32 id;
    struct prop *prop;
    
    OLD_GET_ID_AND_PTR(id, ptr, OCT_PROP);
    
    prop = (struct prop *) ptr;
    
    val = oct_get_string(&prop->user_prop.name) &&
	    oct_get_byte(&type);
    
    if (!val) {
	return OCT_ERROR;
    }
    
    prop->user_prop.type = (octPropType) type;
    
    switch ((octId) type) {
    case OCT_INTEGER:
	if (!oct_get_32(&prop->user_prop.value.integer)) {
	    return OCT_ERROR;
	}
	break;
    case OCT_REAL:
	if (!oct_old_get_double(&prop->user_prop.value.real)) {
	    return OCT_ERROR;
	}
	break;
    case OCT_STRING:
	if (!oct_get_string(&prop->user_prop.value.string)) {
	    return OCT_ERROR;
	}
	break;
    case OCT_ID:
	{
	    int32 id;
	    if (!oct_get_32(&id)) {
		return OCT_ERROR;
	    }
	    prop->user_prop.value.id = id + oct_id_offset;
	}
    case OCT_STRANGER:
	if (!oct_get_bytes(&prop->user_prop.value.stranger.length,
			   &prop->user_prop.value.stranger.contents)) {
			       return OCT_ERROR;
			   }
	break;
    case OCT_REAL_ARRAY:
	{
	    int32 length;
	    double *ptr;
	    
	    if (!oct_get_32(&length)) {
		return OCT_ERROR;
	    }
	    prop->user_prop.value.real_array.length = length;
	    if (length == 0) {
		prop->user_prop.value.real_array.array = NIL(double);
	    } else {
		ptr = ALLOC(double, length);
		prop->user_prop.value.real_array.array = ptr;
		for(;length > 0; length--, ptr++) {
		    if (!oct_old_get_double(ptr)) {
			return OCT_ERROR;
		    }
		}
	    }
	}
	break;
    case OCT_INTEGER_ARRAY:
	{
	    int32 length;
	    int32 *ptr;
	    
	    if (!oct_get_32(&length)) {
		return OCT_ERROR;
	    }
	    prop->user_prop.value.integer_array.length = length;
	    if (length == 0) {
		prop->user_prop.value.integer_array.array = NIL(int32);
	    } else {
		ptr = ALLOC(int32, length);
		prop->user_prop.value.integer_array.array = ptr;
		for(;length > 0;length--,ptr++) {
		    if (!oct_get_32(ptr)) {
			return OCT_ERROR;
		    }
		}
	    }
	}
	break;
    case OCT_NULL :
	break;
    default :
	oct_error("Corrupted property type");
	return OCT_ERROR;
    }
    
    ptr->contents = NIL(struct chain);
    if (!oct_get_props((generic *) prop)) {
	return OCT_ERROR;
    }

    prop->flags.type = OCT_PROP;
    prop->objectId = id;
    prop->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_PROP], prop);
    return OCT_OK;
}

static octStatus
term_read(desc)
struct facet *desc;
{
    struct term *term;
    generic *ptr;
    int32 id;
    int val;
    
    OLD_GET_ID_AND_PTR(id, ptr, OCT_TERM);
    term = (struct term *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_string(&term->user_term.name) &&
	    oct_get_32(&term->user_term.instanceId) &&
	    oct_get_props((generic *) term) &&
	    oct_get_contents((generic *) term);
    
    if (!val) {
	return OCT_ERROR;
    }
    
    term->objectId = id;
    term->facet = desc;
    term->user_term.formalExternalId = 0;
    term->user_term.width = 1;
    
    if (term->user_term.instanceId == oct_null_id) {
	term->flags.type = OCT_FORMAL;
	oct_attach_generic_list(desc->object_lists[OCT_FORMAL], term);
	desc->num_formals++;
    } else {
	term->flags.type = OCT_TERM;
	term->user_term.instanceId += oct_id_offset;
	oct_attach_generic_list(desc->object_lists[OCT_TERM], term);
    }
    return OCT_OK;
}

static octStatus
bag_read(desc)
struct facet *desc;
{
    struct bag *bag;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_BAG);
    bag = (struct bag *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_string(&bag->user_bag.name) && 
	    oct_get_props((generic *) bag) &&
	    oct_get_contents((generic *) bag);

    if (!val) {
	return 0;
    }

    bag->flags.type = OCT_BAG;
    bag->objectId = id;
    bag->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_BAG], bag);
    
    return 1;
}


static octStatus
box_read(desc)
struct facet *desc;
{
    struct box *box;
    generic *ptr;
    int val;
    int32 id;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_BOX);
    box = (struct box *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_box(&box->user_box) &&
	    oct_get_props((generic *) box) &&
	    oct_get_contents((generic *) box);
    
    if (!val) {
	return OCT_ERROR;
    }

    box->flags.type = OCT_BOX;
    box->facet = desc;
    box->objectId = id;
    oct_attach_generic_list(desc->object_lists[OCT_BOX], box);
    return OCT_OK;
}

static octStatus
circle_read(desc)
struct facet *desc;
{
    struct circle *circle;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_CIRCLE);
    circle = (struct circle *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_32(&circle->user_circle.startingAngle) &&
	    oct_get_32(&circle->user_circle.endingAngle) &&
	    oct_get_32(&circle->user_circle.innerRadius) &&
	    oct_get_32(&circle->user_circle.outerRadius) &&
	    oct_get_32(&circle->user_circle.center.x) &&
	    oct_get_32(&circle->user_circle.center.y) &&
	    oct_get_box(&circle->bbox) &&
	    oct_get_props((generic *) circle) &&
	    oct_get_contents((generic *) circle);
    
    if (!val) {
	return OCT_ERROR;
    }

    circle->flags.type = OCT_CIRCLE;
    circle->objectId = id;
    circle->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_CIRCLE], circle);
    return OCT_OK;
}


static octStatus
edge_read(desc)
struct facet *desc;
{
    struct edge *edge;
    generic *ptr;
    int32 id;
    int val;

    OLD_GET_ID_AND_PTR(id, ptr, OCT_EDGE);
    edge = (struct edge *) ptr;
    
    ptr->contents = NIL(struct chain);
    val = oct_get_point(&edge->user_edge.start) &&
            oct_get_point(&edge->user_edge.end) &&
	    oct_get_props((generic *) edge) &&
	    oct_get_contents((generic *) edge);
    
    if (!val) {
	return OCT_ERROR;
    }

    edge->flags.type = OCT_EDGE;
    edge->objectId = id;
    edge->facet = desc;
    oct_attach_generic_list(desc->object_lists[OCT_EDGE], edge);
    return OCT_OK;
}

