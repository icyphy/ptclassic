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
#ifndef OCT_INTERNAL_H
#define OCT_INTERNAL_H

#include "port.h"
#include "errtrap.h"
#include "oct.h"
#include "macros.h"
#include "str.h"
#include "xid.h"
typedef st_table master_table;
/*#include "master.h"*/

/*
 * Every internal data type having these fields, must
 * have them in this order
 */

#define CLEAR_FLAGS(obj) \
{\
    obj->flags.is_temporary = 0;\
    obj->flags.bb_valid = 0;\
    obj->flags.bb_init = 0;\
    obj->flags.is_record = 0;\
    obj->flags.is_marked = 0;\
}

    
struct obj_flags {
    /* unsigned int ref_count : 2 ;*/ /* not used any more */
    unsigned int is_temporary : 1;
    unsigned int is_record : 1;
    unsigned int is_marked : 1;	/*  generic mark bit */
    unsigned int bb_valid : 1;	/* is the bb valid, only used by instances */
    unsigned int bb_init : 1;	/* is the bb initialized, only used by instances */
    unsigned int : 3;
    unsigned int type : 8;
};

#define GENERIC_FIELDS \
    struct obj_flags flags;\
    octId objectId;\
    int32 externalId;\
    struct chain *containers;\
    struct facet *facet;\
    struct chain *contents;\
    generic *next, *last

typedef struct generic generic;

struct generic {
    GENERIC_FIELDS;
};

struct chain {
    struct chain *next;		/* next object attached to `container' */
    struct chain *last;		/* previous object attached to `container' */
    struct chain *next_chain;	/* next object containing `object' */
    generic *object;		/* the object being contained by `container'*/
    generic *container;		/* the object containing `object'*/
};

struct generator {
    struct generator *next_gen, *last_gen;
    struct chain *next;
    int (*fix_func)();		/* function to move next one link forward, see ref.c */
    octObjectMask mask;
    generic *object;
};

struct net {
    GENERIC_FIELDS;
    struct octNet user_net;
};

struct term {
    GENERIC_FIELDS;
    struct octTerm user_term;
};

struct bag {
    GENERIC_FIELDS;
    struct octBag user_bag;
};

struct prop {
    GENERIC_FIELDS;
    struct octProp user_prop;
};

struct layer {
    GENERIC_FIELDS;
    struct octLayer user_layer;
};

struct change_list {
    GENERIC_FIELDS;
    struct octChangeList user_change_list;
};

struct master {
    char *cell;
    char *view;
    char *facet;
    char *version;
    struct facet *master;
    struct instance *instances;
    int32 master_formal_date;
    int32 master_bb_date;
    struct master *next, *last;
};
    
struct instance {
    GENERIC_FIELDS;
    char *name;
    struct octTransform transform;
    struct octBox bbox;
    struct master *master;
    struct instance *next_inst, *last_inst;
};

#define CLEAR_FACET_FLAGS(desc)\
{\
    desc->facet_flags.info_loaded = 0;\
    desc->facet_flags.all_loaded = 0;\
    desc->facet_flags.locked = 0;\
    desc->facet_flags.bb_init = 0;\
    desc->facet_flags.bb_valid = 0;\
    desc->facet_flags.bb_invalid_instances = 0;\
    desc->facet_flags.old_facet = 0;\
}


struct facet {
    GENERIC_FIELDS;
    struct ff {
	unsigned int info_loaded : 1;	/* Is the facet-info header in memory */
	unsigned int all_loaded : 1;	/* Is the entire facet in memory */
	unsigned int locked : 1;		/* Is this facet locked (not used) */
	unsigned int bb_init : 1;	/* Has the bounding box been initialized yet */
	unsigned int bb_valid : 1;	/* Is the bb valid with respect to the internal geometry*/
	unsigned int bb_invalid_instances : 1; /* Is the bb invalid because of un-updated instance bbs */
	unsigned int old_facet : 1; /* is the facet an oct 1.0 facet */
    } facet_flags;
    master_table *masters;	/* table of all distinct masters instantiated in the facet */
    struct master *instantiations; /* a list of all masters in memory that are instantiations of the facet */
    struct facet *interfaces;	/* if a contents facet, a list of all associated interface facets in memory*/
    struct facet *contents_facet;/* if not a contents facet, a pointer to the contents facet */
    struct facet *next_interface;/* link pointers for the above interfaces list */
    struct facet *last_interface;
    struct generator *generators; /* all active generators in the facet */
    char **search_path;		/* not used */
    int mode;
    int object_count;
    int temp_count;		/* number of temporary objects */
    int32 next_xid;
    xid_table *xid_table;	/* the mapping between external ids and object pointers */
    struct octBox bbox;
    int ref_count;		/* number of times the facet has been opened */
    char *file_desc;		/* the descriptor for the facet from the fsys package */
    struct octFacet user_facet;
    octFunctionMask function_mask; /* the change list functions that are being watched */
    octObjectMask object_mask;	/* the change list objects that are being watched */
    int num_formals;		/* number of formal terminals in the facet */
    int32 time_stamp;
    int32 bb_date;
    int32 formal_date;
    int32 contents_date;
    int32 create_date;
    int32 delete_date;
    int32 modify_date;
    int32 attach_date;
    int32 detach_date;
    generic *object_lists[OCT_MAX_TYPE+1];
};

#define HAS_BB_FIELD	1
#define HAS_BB_FUNC	2	/* the object has a function to compute the bb */
#define HAS_BB		3	/* the object has a cached value for the bb */
#define HAS_POINTS	4	/* the object has points associated with it */
#define HAS_NAME	8	/* the object has a name */

struct object_desc {
    unsigned int internal_size;	/* size of internal oct data structure (i.e. struct term) */
    unsigned int external_size;	/* size of the external (user) structure in octObject (i.e. struct octTerm) */
    int32 flags;		/* the HAS flags defined above */
    octObjectMask contain_mask;	/* mask of objects I am allowed to contain */
    int user_offset;		/* offset of the user structure from the start of the internal structure*/
    int bb_offset;		/* location of the bb field, if I have one */
    int name_offset;		/* location of the name field, if I have one */
    octStatus (*create_func)();
    octStatus (*uncreate_func)();
    octStatus (*undelete_func)();
    octStatus (*delete_func)();
    int (*free_func)();		/* free all memory associated with your self */
    octStatus (*attach_func)();
    octStatus (*detach_func)();
    octStatus (*undetach_func)();
    octStatus (*unattach_func)();
    octStatus (*is_attached_func)();
    octStatus (*modify_func)();
    octStatus (*unmodify_func)();
    octStatus (*bb_func)();
    octStatus (*get_by_name_func)();
    octStatus (*get_container_by_name_func)();
    octStatus (*gen_first_content_func)();
    octStatus (*gen_first_container_func)();
    octStatus (*gen_contents_func)();
    octStatus (*gen_contents_with_offset_func)();
    octStatus (*gen_containers_func)();
    octStatus (*free_fields_func)(); /* free any internal memory of the object */
    octStatus (*copy_fields_func)(); /* copy the data from a user structure to my internal structure */
    octStatus (*read_fields_func)(); /* read your internal fields from disk */
    octStatus (*write_fields_func)(); /* write your internal fields to disk */
    octStatus (*fix_fields_func)(); /* put my user structure in a canonical form */
    octStatus (*read_func)();
    octStatus (*write_func)();
};

extern int errno;
extern int sys_nerr;
extern char *sys_errlist[];

#define sys_msg() (errno < sys_nerr ? sys_errlist[errno] : "Unknown error")

extern struct object_desc oct_object_descs[];

extern char *oct_contents_facet_name;

extern int oct_false();
extern octStatus oct_illegal_op();

extern octStatus oct_do_open();
extern octStatus oct_do_open_with_desc();
extern octStatus oct_creat_facet();
extern octStatus oct_commit();
extern octStatus oct_open_facet_stream();
extern octStatus oct_free();

extern void oct_mark_facet();

extern octId oct_new_id();
extern char *oct_facet_name();
extern char *oct_type_names[];
extern generic *oct_id_to_ptr();
extern generic *oct_xid_to_ptr();
extern char *oct_malloc();
extern char *oct_error_string;

extern void oct_mark_ptr();
extern void oct_copy_instance();
extern void oct_add_xid();
extern int32 oct_next_xid();
extern char *oct_determine_mode();
extern void oct_fill_object();
extern void oct_fill_instance();
extern void oct_init_marks();
extern void oct_free_id();
extern void oct_free_xid();
extern void oct_free_xid_table();
extern void oct_free_master_table();
extern void oct_detach_all_chains();
extern void oct_init_facet_table();
extern void oct_free_facet_table();

extern void oct_free_info();

#endif /* OCT_INTERNAL_H */

