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
#include <varargs.h>  
#include "uprintf.h"

#include "bag.h"
#include "box.h"
#include "change_record.h"
#include "change_list.h"
#include "circle.h" 
#include "default.h"
#include "edge.h"
#include "facet.h"
#include "fsys.h"
#include "geo.h"
#include "geo_points.h"
#include "inst.h"
#include "label.h"
#include "layer.h"
#include "master.h"
#include "net.h"
#include "oct_id.h"
#include "path.h"
#include "point.h"
#include "polygon.h"
#include "prop.h"
#include "term.h"

#include "oct_utils.h"

int oct_abort_on_error = 0;

#if defined(MCC_DMS)
/* THIS IS A HACK.  These are the functions that DMS references in OCT.  By */
/* putting a reference to them here, one no longer needs to put liboct.a */
/* between two references to libdms.a on the link command line. */
static octStatus (*a[])() = {
	octAttach,
	octCloseFacet,
	octCreate,
	octCreateOrModify,
	octDelete,
	octDetach,
	octFreeFacet,
	octFreeGenerator,
	octGenFirstContent,
	octGetById,
	octGetByName,
	octGetContainerByName,
	octGetOrCreate,
	octInitGenContainers,
	octInitGenContents,
	octModify,
	octOpenFacet
};
#endif

char *oct_type_names[] = {
    "OCT_UNDEFINED_OBJECT",
    "OCT_FACET",
    "OCT_TERM",
    "OCT_NET",
    "OCT_INSTANCE",
    "OCT_POLYGON",
    "OCT_BOX",
    "OCT_CIRCLE",
    "OCT_PATH",
    "OCT_LABEL",
    "OCT_PROP",
    "OCT_BAG",
    "OCT_LAYER",
    "OCT_POINT",
    "OCT_EDGE",
    "OCT_FORMAL_TERM",
    "OCT_MASTER",
    "OCT_CHANGE_LIST",
    "OCT_CHANGE_RECORD",
    (char *) 0
};

static char error_buffer[256] = "";
static char error_temp[256];
char *oct_error_string = error_buffer;

char *oct_facet_name(facet)
struct octFacet *facet;
{
    static char name_buf[1024];

    (void) sprintf(name_buf, "%s:%s:%s;%s",
	    (facet->cell == NIL(char) ? "" : facet->cell),
	    (facet->view == NIL(char) ? "" : facet->view),
	    (facet->facet == NIL(char) ? "" : facet->facet),
	    (facet->version == NIL(char) ? "" : facet->version));
    return name_buf;
}

    
char *
octErrorString()
{
    return oct_error_string;
}

/* oct_error(format, args...) */

#ifdef lint
/*ARGSUSED*/
/*VARARGS1*/
#undef va_alist
#undef va_dcl
#define va_alist	fmt
#define va_dcl		char *fmt;
#endif

void
oct_error(va_alist)
va_dcl
{
    va_list ap;
    char *format;

    va_start(ap);
    
    format = va_arg(ap, char *);

    (void) uprintf(error_buffer, format, &ap);

    va_end(ap);
    
    if (oct_abort_on_error) {
	(void) fprintf(stderr, "%s\n", error_buffer);
	abort();
    }
    oct_error_string = error_buffer;
}

#ifdef lint
/*ARGSUSED*/
/*VARARGS1*/
#undef va_alist
#undef va_dcl
#define va_alist	fmt
#define va_dcl		char *fmt;
#endif

void
oct_prepend_error(va_alist)
va_dcl
{
    va_list ap; 
    char *format;

    va_start(ap);

    (void) strcpy(error_temp, oct_error_string);
    format = va_arg(ap, char *);
    (void) uprintf(error_buffer, format, &ap);
    va_end(ap);
    (void) strcat(error_buffer, error_temp);
    if (oct_abort_on_error) {
	(void) fprintf(stderr, "%s\n", error_buffer);
	abort();
    }
    oct_error_string = error_buffer;
}

void
octError(string)
char *string;
{
    if (string != NIL(char)) {
	(void) fprintf(stderr, "%s: %s\n", string, oct_error_string);
    } else {
	(void) fprintf(stderr, "%s\n", oct_error_string);
    }
    return;
}

void
oct_subclass_responsibility()
{
    errRaise(OCT_PKG_NAME, OCT_ERROR, "subfunction not implemented");
}

struct object_desc oct_object_descs[OCT_MAX_TYPE+1];

void
oct_init_descs()
{
    oct_init_default_desc();
    oct_init_geo_desc();
    oct_init_geo_points_desc();
    
    oct_box_desc_set(&oct_object_descs[OCT_BOX]);
    oct_layer_desc_set(&oct_object_descs[OCT_LAYER]);
    oct_facet_desc_set(&oct_object_descs[OCT_FACET]);
    oct_net_desc_set(&oct_object_descs[OCT_NET]);
    oct_term_desc_set(&oct_object_descs[OCT_TERM]);
    oct_instance_desc_set(&oct_object_descs[OCT_INSTANCE]);
    oct_prop_desc_set(&oct_object_descs[OCT_PROP]);
    oct_path_desc_set(&oct_object_descs[OCT_PATH]);
    oct_polygon_desc_set(&oct_object_descs[OCT_POLYGON]);
    oct_bag_desc_set(&oct_object_descs[OCT_BAG]);
    oct_label_desc_set(&oct_object_descs[OCT_LABEL]);
    oct_circle_desc_set(&oct_object_descs[OCT_CIRCLE]);
    oct_point_desc_set(&oct_object_descs[OCT_POINT]);
    oct_edge_desc_set(&oct_object_descs[OCT_EDGE]);
    oct_formal_desc_set(&oct_object_descs[OCT_FORMAL]);
    oct_net_desc_set(&oct_object_descs[OCT_NET]);
    oct_master_desc_set(&oct_object_descs[OCT_MASTER]);
    oct_change_list_desc_set(&oct_object_descs[OCT_CHANGE_LIST]);
    oct_change_record_desc_set(&oct_object_descs[OCT_CHANGE_RECORD]);
}

static int oct_initialized = 0;

char *oct_contents_facet_name;

void
octBegin()
{
    if (!oct_initialized) {
#if defined(MCC_DMS)
	dmsBegin();
#endif
	oct_str_create();	/* initialize string table */
	oct_init_facet_table();
	oct_init_facet_key_table();
	(void) oct_init_ids();
	oct_init_descs();

	oct_contents_facet_name = oct_str_intern("contents");

	oct_initialized = 1;
    }
}

void
octEnd()
{
    oct_initialized = 0;
#if defined(MCC_DMS)
    dmsEnd();
#endif
    (void) oct_free_facet_table();
    oct_str_destroy();
    return;
}


char *
oct_determine_mode(facet)
struct facet *facet;
{
    if ((facet->mode&OCT_APPEND) == OCT_APPEND)  {
	return "a";
    } else if ((facet->mode&OCT_OVER_WRITE) == OCT_OVER_WRITE) {
	return "w";
    } else {
	return "r";
    }
}


void
oct_fill_object(object_ptr, ptr)
octObject *object_ptr;
generic *ptr;
{
    int offset =  oct_object_descs[ptr->flags.type].user_offset;
    int size =  oct_object_descs[ptr->flags.type].external_size;

    if (ptr->flags.type == OCT_INSTANCE) {
	oct_fill_instance(object_ptr, ptr);
    } else {
	(void) memcpy((char *) &(object_ptr->contents),
		      (char *)ptr + offset,
		      size);
	object_ptr->type = ptr->flags.type;
	if (object_ptr->type == OCT_FORMAL) object_ptr->type = OCT_TERM;
	else if (object_ptr->type == OCT_FACET) {
	    struct facet *facet = (struct facet *)ptr;
	    object_ptr->contents.facet.mode = oct_determine_mode(facet);
	}
	object_ptr->objectId = ptr->objectId;
    }
    return; 
}

octStatus oct_illegal_op()
{
    oct_error("This operation is not supported on objects of this type");
    return OCT_ERROR;
}

int
oct_false() {
    return 0;
}

