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
#include "io_internal.h"
#include "oct.h"
#include "oct_utils.h"

#define NIL(type) ((type *) 0)

extern char *oct_type_names[];

static char *trans_name[] = {
    "TR_NO_TRANSFORM",
    "TR_MIRROR_X",
    "TR_MIRROR_Y",
    "TR_ROT90",
    "TR_ROT180",
    "TR_ROT270",
    "TR_MX_ROT90",
    "TR_MY_ROT90",
    "TR_FULL_TRANSFORM"
};

static char *function_name[] = {
    "NO_FUNCTION",
    "OCT_CREATE",
    "OCT_DELETE",
    "OCT_MODIFY",
    "OCT_PUT_POINTS",
    "OCT_ATTACH_CONTENT",
    "OCT_ATTACH_CONTAINER",
    "OCT_DETACH_CONTENT",
    "OCT_DETACH_CONTAINER",
	"OCT_MARKER",
};

octStatus
octPrintObject(outfile, object, sub)
FILE *outfile;
struct octObject *object;
int sub;
{
    octGenerator gen;
    octObject sub_object;
    struct octBox bbox;
    octStatus status;
    octTransformType type;
    int32 xid;
    int retval;

    if (object->objectId != oct_null_id)
	{
		if( !sub )
		{
			(void) fprintf(outfile, "ID=%ld: ", object->objectId);
		}
		else
		{
			(void) octExternalId(object, &xid);
			(void) fprintf(outfile, "XID=%ld: ", xid);
		}
    }
	else
	{
		(void) fprintf(outfile, "<NULL ID>: ");
    }
    
    switch(object->type) {
    case OCT_BOX:
	(void) fprintf(outfile, "Box (%d,%d) (%d,%d)\n", 
	    (int)object->contents.box.lowerLeft.x,
	    (int)object->contents.box.lowerLeft.y,
	    (int)object->contents.box.upperRight.x,
	    (int)object->contents.box.upperRight.y);
	break;
    case OCT_BAG:
	(void) fprintf(outfile, "Bag with name %s\n",
		(object->contents.bag.name == NIL(char) ?
		 "<NO-NAME>" : object->contents.bag.name));
	break;
    case OCT_NET:
	(void) fprintf(outfile, "Net with name %s\n",
		(object->contents.net.name == NIL(char) ?
		 "<NO-NAME>" : object->contents.net.name));
	break;
    case OCT_LAYER:
	(void) fprintf(outfile, "Layer with name %s\n",
		(object->contents.layer.name == NIL(char) ?
		 "<NO-NAME>" : object->contents.layer.name));
	break;
    case OCT_PATH: {
	int32 num_points = 20;
	struct octPoint pt_array[20];
	int retval,i;
	
	(void) fprintf(outfile, "Path with width %d\n",
		 (int)object->contents.path.width);
	retval = octGetPoints(object, &num_points, pt_array);
	if (retval == OCT_OK) {
	    (void) fprintf(outfile, "\twith %d points: ", (int)num_points);
	    for (i = 0; i < num_points; i++) {
		(void) fprintf(outfile,"(%d,%d) ", (int)pt_array[i].x,
			       (int)pt_array[i].y);
	    }
	    (void) fprintf(outfile, "\n");
	} else if (retval == OCT_TOO_SMALL) {
	    (void) fprintf(outfile,"\twith %d points (too many to print)\n",
					     (int)num_points);
	} else {
	    oct_prepend_error("octPrintObject: Getting the points on a path");
	    return retval;
	}
	break;
    }
    case OCT_POLYGON: {
	int32 num_points = 20;
	struct octPoint pt_array[20];
	int retval,i;
	
	(void) fprintf(outfile, "Polygon\n");
	retval = octGetPoints(object, &num_points, pt_array);
	if (retval == OCT_OK) {
	    (void) fprintf(outfile, "\twith %d points: ", (int)num_points);
	    for (i = 0; i < num_points; i++) {
		(void) fprintf(outfile,"(%d,%d) ", (int)pt_array[i].x,
			       (int)pt_array[i].y);
	    }
	    (void) fprintf(outfile, "\n");
	} else if (retval == OCT_TOO_SMALL) {
	    (void) fprintf(outfile,"\twith %d points (too many to print)\n",
					     (int)num_points);
	} else {
	  oct_prepend_error("octPrintObject: Getting the points on a polygon");
	    return retval;
	}
	break;
    }
    case OCT_LABEL:
	{
	    static char *vjust[] = { "bottom",	"center",	"top" };
	    static char *hjust[] = { "left",	"center",	"right" };

	    (void) fprintf(outfile, "Label labeled %s at (%d,%d) (%d,%d)\n",
		    (object->contents.label.label == NIL(char) ?
		     "<NO-NAME>" : object->contents.label.label),
		    (int)object->contents.label.region.lowerLeft.x,
		    (int)object->contents.label.region.lowerLeft.y,
		    (int)object->contents.label.region.upperRight.x,
		    (int)object->contents.label.region.upperRight.y);
	    (void) fprintf(outfile, "\ttext is %d high, with each line %s-justified\n",
		    (int)object->contents.label.textHeight,
		    hjust[object->contents.label.lineJust]);
	    (void) fprintf(outfile, "\tin a %s-%s-justified block\n",
		    vjust[object->contents.label.vertJust],
		    hjust[object->contents.label.horizJust]);
	}
	break;
    case OCT_POINT:
	(void) fprintf(outfile, "Point at (%d,%d)\n",
		(int)object->contents.point.x,
		(int)object->contents.point.y);
	break;
    case OCT_EDGE:
	(void) fprintf(outfile, "Edge from (%d,%d) to (%d,%d)\n",
		       (int)object->contents.edge.start.x,
		       (int)object->contents.edge.start.y,
		       (int)object->contents.edge.end.x,
		       (int)object->contents.edge.end.y);
	break;
    case OCT_CIRCLE:
	(void) fprintf(outfile, "Circle centered at (%d,%d)\n",
		       (int)object->contents.circle.center.x,
		       (int)object->contents.circle.center.y);
	(void) fprintf(outfile, "\tfrom angle %d to %d, and radius %d to %d\n",
		       (int)object->contents.circle.startingAngle,
		       (int)object->contents.circle.endingAngle,
		       (int)object->contents.circle.innerRadius,
		       (int)object->contents.circle.outerRadius);
	break;
    case OCT_TERM:
	(void) fprintf(outfile, "Term with name %s on instance %d\n",
		       (object->contents.term.name == NIL(char) ?
			"<NO-NAME>" : object->contents.term.name),
		       (int)object->contents.term.instanceId);
	break;
    case OCT_INSTANCE: 
	(void) fprintf(outfile, "Instance named %s of (%s %s %s)\n",
		(object->contents.instance.name == NIL(char) ?
		 "<NO-NAME>" : object->contents.instance.name),
	       object->contents.instance.master,
		object->contents.instance.view,
		object->contents.instance.facet);
	type = object->contents.instance.transform.transformType;
	(void) fprintf(outfile, "\tat (%d,%d) with transform %s\n",
	       (int)object->contents.instance.transform.translation.x,
	       (int)object->contents.instance.transform.translation.y,
		((int) type <= (int) OCT_FULL_TRANSFORM ? trans_name[(int)type] :
		  "<NO VALID TRANSFORM>"));
	if (type == OCT_FULL_TRANSFORM) {
	    double (*matrix)[2];
	    matrix = object->contents.instance.transform.generalTransform;

	    (void) fprintf(outfile, "\twith matrix [%g,%g],[%g,%g]\n",
		   matrix[0][0], matrix[0][1],matrix[1][0],matrix[1][1]);
	}
	break;
    case OCT_CHANGE_LIST:
	(void) fprintf(outfile, "Change list with object mask %x and function mask %x\n",
		(int)object->contents.changeList.objectMask,
		(int)object->contents.changeList.functionMask);
	break;
 case OCT_CHANGE_RECORD:
	(void) fprintf(outfile,
		       "Change record with type %s, object xid %d,content xid %d\n", 
		       function_name[object->contents.changeRecord.changeType],
		       (int)object->contents.changeRecord.objectExternalId,
		       (int)object->contents.changeRecord.contentsExternalId);
	fflush(outfile);
	retval = octBB(object, &bbox);
	if (retval < OCT_OK && retval != OCT_NO_BB)
	{
		octError("Getting the bounding box of a change record"); /*XXX octError !!! */
	}
	else if (retval >= OCT_OK)
	{
	    (void) fprintf(outfile, "\t bounding box of (%d,%d) -> (%d,%d)\n",
			   (int)bbox.lowerLeft.x, (int)bbox.lowerLeft.y,
			   (int)bbox.upperRight.x, (int)bbox.upperRight.y);
	}
	retval = octGenFirstContent(object, OCT_ALL_MASK, &sub_object);
	if (retval < 0)
	{
	    octError("Getting the value of the record");
	}
	else if (retval == OCT_OK)
	{
	    (void) fprintf(outfile, "\t with old value\n");
	    (void) octPrintObject(outfile, &sub_object,0);
	}
	if (object->contents.changeRecord.changeType == OCT_PUT_POINTS)
	{
	    int32 num_points = 20, i;
	    struct octPoint pt_array[20];
	    
	    retval = octGetPoints(object, &num_points, pt_array);
	    if (retval == OCT_OK)
		{
			(void) fprintf(outfile, "\twith %d old points:",
				       (int)num_points);
			for (i = 0; i < num_points; i++)
			{
				(void) fprintf(outfile,"(%d,%d) ",
					       (int)pt_array[i].x,
					       (int)pt_array[i].y);
			}
			(void) fprintf(outfile, "\n");
	    }
		else if (retval == OCT_TOO_SMALL)
		{
			(void) fprintf(outfile,
				       "\twith %d points (too many to print)\n",
						   (int)num_points);
	    }
		else
		{
			oct_prepend_error("octPrintObject: Getting the points on a change_record");
			return retval;
	    }
	}
	break;
    case OCT_PROP:
	(void) fprintf(outfile, "Prop named %s, value ", object->contents.prop.name);
	switch (object->contents.prop.type) {
	case OCT_INTEGER :
	    (void) fprintf(outfile,"Integer %d\n",
		    (int)object->contents.prop.value.integer);
	    break;
	case OCT_REAL :
	    (void) fprintf(outfile,"Real %g\n", object->contents.prop.value.real);
	    break;
	case OCT_STRING :
	    (void) fprintf(outfile,"String %s\n", object->contents.prop.value.string);
	    break;
	case OCT_STRANGER :
	    {
		int32 length = object->contents.prop.value.stranger.length;
		char *ptr = object->contents.prop.value.stranger.contents;
	    
		(void) fprintf(outfile,"%ld bytes of stranger data:",
			       (long)length);
		while (length-- > 0) {
		    (void) fprintf(outfile,"%x ", *(ptr++));
		}
		(void) fprintf(outfile,"\n");
		break;
	    }
	case OCT_REAL_ARRAY :
	    {
		int32 length = object->contents.prop.value.real_array.length;
		double *ptr = object->contents.prop.value.real_array.array;
	    
		(void) fprintf(outfile,"%ld doubles in array:", (long)length);
		while (length-- > 0) {
		    (void) fprintf(outfile,"%g ", *(ptr++));
		}
		(void) fprintf(outfile,"\n");
		break;
	   } 
	case OCT_INTEGER_ARRAY :
	    {
		int32 length = object->contents.prop.value.integer_array.length;
		int32 *ptr = object->contents.prop.value.integer_array.array;
	    
		(void) fprintf(outfile,"%ld ints in array:", (long)length);
		while (length-- > 0) {
		    (void) fprintf(outfile,"%d ", (int)(*(ptr++)));
		}
		(void) fprintf(outfile,"\n");
		break;
	    }
	case OCT_ID :
	    (void) fprintf(outfile,"Object id %d\n", 
			   (int)object->contents.prop.value.id);
	    break;
        case OCT_NULL :
	    (void) fprintf(outfile,"Object id is OCT_NULL\n"); 
	    break;
	}
	break;
    default :
	(void) fprintf(outfile, "An object of type %s\n",
		oct_type_names[object->type]);
	break;
    }
    
    if (object->objectId != oct_null_id)
	{
		
		if( sub )
		{
			if (octInitGenContents(object, OCT_ALL_MASK, &gen) != OCT_OK)
			{
				oct_prepend_error("octPrintObject: generating contents of object");
				return OCT_ERROR;
			}
			
			status = octGenerate(&gen,&sub_object);

			if (status == OCT_OK)
			{
				int32 xid;
				
				(void) fprintf(outfile,"\tcontaining ");
				do
				{
					octExternalId(&sub_object, &xid);
					(void) fprintf(outfile, "%d ",
						       (int)xid);
				} while ((status = octGenerate(&gen,&sub_object)) == OCT_OK);
				(void) fprintf(outfile, "\n");
			}
			if (status != OCT_GEN_DONE)
			{
				oct_prepend_error("octPrintObject: generating contents of object: ");
				return OCT_ERROR;
			}
			
			if (octInitGenContainers(object, OCT_ALL_MASK, &gen) != OCT_OK)
			{
				oct_prepend_error("octPrintObject: generating container of object: ");
				return OCT_ERROR;
			}
			
			status = octGenerate(&gen,&sub_object);
			
			if (status == OCT_OK)
			{
				int32 xid;
				
				(void) fprintf(outfile,"\tattached to ");
				do
				{
					octExternalId(&sub_object, &xid);
					(void) fprintf(outfile, "%d ",
						       (int)xid);
				} while ((status = octGenerate(&gen,&sub_object)) == OCT_OK);
				(void) fprintf(outfile, "\n");
			}
			if (status != OCT_GEN_DONE)
			{
				oct_prepend_error("octPrintObject: generating containers of object: ");
				return OCT_ERROR;
			}
		}
	}
    return OCT_OK;
}

char *
octFormatId(id)
octId id;
{
    static char buffer[1024];
    (void) sprintf(buffer, "%d", (int) id);
    return buffer;
}
