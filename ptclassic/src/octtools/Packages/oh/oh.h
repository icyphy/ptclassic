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
#ifndef OH_H
#define OH_H

#define OH_PKG_NAME "oh"

#ifndef OCT_LEVEL
#define OCT_LEVEL 1
#endif

#include "ansi.h"

EXTERN void ohOctFailed
	ARGS((char *file, int line, char *why, octObject *offending_object));
EXTERN void ohCreateSegment
	ARGS((octObject *c, octObject *t, octCoord p1x, octCoord p1y, octCoord p2x, octCoord p2y, octCoord width));
EXTERN octStatus ohBB
	ARGS((octObject *obj, octBox *bb));
EXTERN octStatus ohTerminalBB
	ARGS((octObject *term, octBox *bb));
EXTERN octStatus ohActualTerminalBB
	ARGS((octObject *aterm, octBox *bb));
EXTERN octStatus ohFormalTerminalBB
	ARGS((octObject *fterm, octBox *bb));
EXTERN octStatus ohGetByNameContainer
	ARGS((octObject *contents, octObject *obj, int type, char *container_name));
EXTERN char *ohGetName
	ARGS((octObject *object));
EXTERN octStatus ohPutName
	ARGS((octObject *object, char *name));
EXTERN char *ohTypeName
	ARGS((octObject *object));
EXTERN char *ohFormatName
	ARGS((octObject *object));
EXTERN void ohReplaceInstance
	ARGS((octObject *newInst, octObject *oldInst));
EXTERN void ohAttachContents
	ARGS((octObject *src, octObject *dst, octObjectMask mask));
EXTERN void ohAttachContainers
	ARGS((octObject *src, octObject *dst, octObjectMask mask));
EXTERN void ohAttachOnceContents
	ARGS((octObject *src, octObject *dst, octObjectMask mask));
EXTERN void ohAttachOnceContainers
	ARGS((octObject *src, octObject *dst, octObjectMask mask));
EXTERN void ohDeleteContents
	ARGS((octObject *container, octObjectMask mask));
EXTERN void ohRecursiveDelete
	ARGS((octObject *object, octObjectMask mask));
EXTERN octStatus ohTerminalNet
	ARGS((octObject *fterm, octObject *net));
EXTERN octStatus ohFindFormal
	ARGS((octObject *fterm, octObject *aterm));
EXTERN void ohGetPoints
	ARGS((octObject *object, int32 *num, octPoint **arr));
EXTERN octStatus ohUnpackFacetName
	ARGS((octObject *facet, char *name));
EXTERN void ohWorkFacet
	ARGS((octObject *facet, char *inputname, char* outputname));
EXTERN void ohInputFacet
	ARGS((octObject *facet, char *inputname ));
EXTERN void ohUniqNames
	ARGS((octObject *container, octObjectMask mask));
EXTERN int ohCountContents
	ARGS((octObject *object, octObjectMask mask));
EXTERN int ohCountContainers
	ARGS((octObject *object, octObjectMask mask));
EXTERN void ohContentsToArray
	ARGS((octObject *object, octObjectMask mask, octId **array, int *array_n));
EXTERN octStatus ohFastGetByNetName
	ARGS((octObject *facet, octObject *net, char *name));
EXTERN octStatus ohFastCreateNet
	ARGS((octObject *facet, octObject *net, char *name));
EXTERN octStatus ohFastGetOrCreateNet
	ARGS((octObject *facet, octObject *net, char *name));
EXTERN void ohFastReleaseStorage
	ARGS((void));

#if OCT_LEVEL == 2
EXTERN octStatus ohGetTerminal
	ARGS((octObject *container, int32 fxid, octObject *term));
EXTERN void ohPrintInconsistent
	ARGS((octObject *facet, FILE *stream));
EXTERN octStatus ohBestName
	ARGS((octObject *facet, octObject *instance, octObject *master));
#endif

extern octStatus ohLastStatus;

#define OH_ASSERT(fct) 			\
    ((ohLastStatus = fct) >= OCT_OK) ? 0 : 	\
	(ohOctFailed(__FILE__, __LINE__, (char *) 0, (octObject *) 0), 1)

#define OH_ASSERT_DESCR(fct, why) 		\
    ((ohLastStatus = fct) >= OCT_OK) ? 0 : 	\
	(ohOctFailed(__FILE__, __LINE__, why, (octObject *) 0), 1)

#define OH_FAIL(fct, why, offending_object)	\
    ((ohLastStatus = fct) >= OCT_OK) ? 0 : 	\
	(ohOctFailed(__FILE__, __LINE__, why, offending_object), 1)

/*
 * set up defaults for a facet
 */

#define ohUnpackDefaults(f, m, s)			\
    ((f)->type=OCT_FACET,				\
     (f)->contents.facet.cell=0,			\
     (f)->contents.facet.view=0,			\
     (f)->contents.facet.facet=0,			\
     (f)->contents.facet.version=OCT_CURRENT_VERSION,	\
     (f)->contents.facet.mode=m,			\
     (void) ohUnpackFacetName(f, s), OCT_OK)
     

/*
 *  Macro's to call octGetByName()
 *	c = the container
 *	t = the object
 *	s = the object name
 */

#define ohGetByTermName(c, t, s)				\
    ((t)->type=OCT_TERM, (t)->contents.term.name=s, octGetByName(c,t))

#define ohGetByNetName(c, t, s)				\
    ((t)->type=OCT_NET, (t)->contents.net.name=s, octGetByName(c,t))

#define ohGetByInstName(c, t, s)				\
    ((t)->type=OCT_INSTANCE,(t)->contents.instance.name=s, octGetByName(c,t))

#define ohGetByLabelName(c, t, s)				\
    ((t)->type=OCT_LABEL, (t)->contents.label.label=s, octGetByName(c,t))

#define ohGetByBagName(c, t, s)				\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octGetByName(c,t))

#define ohGetByLayerName(c, t, s)				\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octGetByName(c,t))

#define ohGetByPropName(c, t, s)				\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s, octGetByName(c,t))

/*
 *  Macro's to call octCreate()
 *	c = the container
 *	t = the object
 *	s = the object name (for named objects)
 */

#define ohCreateTerm(c, t, s)				\
    ((t)->type=OCT_TERM, (t)->contents.term.name=s, 	\
     (t)->contents.term.width = 1, \
     (t)->contents.term.instanceId = oct_null_id, octCreate(c,t))

#define ohCreateNet(c, t, s)				\
    ((t)->type=OCT_NET, (t)->contents.net.name = s, (t)->contents.net.width = 1, octCreate(c,t))

#if OCT_LEVEL == 1
#define ohCreateLabel(c, t, s, position)		\
    ((t)->type=OCT_LABEL, 				\
    (t)->contents.label.label = s, 			\
    (t)->contents.label.position = position,            \
    octCreate(c,t))
#else
#define ohCreateLabel(c, t, s, box, th, vj, hj, lj)	\
    ((t)->type=OCT_LABEL, 				\
    (t)->contents.label.label = s, 			\
    (t)->contents.label.region = box,                   \
    (t)->contents.label.textHeight = th,                \
    (t)->contents.label.vertJust = vj,                  \
    (t)->contents.label.horizJust = hj,                 \
    (t)->contents.label.lineJust = lj,                  \
    octCreate(c,t))
#endif

#define ohCreateBag(c, t, s)				\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octCreate(c,t))

#define ohCreateLayer(c, t, s)				\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octCreate(c,t))

#define ohCreatePath(c, t, w)				\
    ((t)->type=OCT_PATH, (t)->contents.path.width=w, octCreate(c,t))

#define ohCreatePropInt(c, t, s, i)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_INTEGER, 		\
    (t)->contents.prop.value.integer = i,		\
    octCreate(c,t))

#define ohCreatePropStr(c, t, s, s1)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_STRING, 		\
    (t)->contents.prop.value.string = s1,		\
    octCreate(c, t))

#define ohCreatePropReal(c, t, s, d)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_REAL, 		\
    (t)->contents.prop.value.real = d,			\
    octCreate(c, t))

#define ohCreateInstance(c, t, xname, xmaster, xview, tx, ty, ttype)\
    ((t)->type=OCT_INSTANCE, 				\
    (t)->contents.instance.name = xname,		\
    (t)->contents.instance.master = xmaster, 		\
    (t)->contents.instance.view = xview,		\
    (t)->contents.instance.facet = "contents",		\
    (t)->contents.instance.version = OCT_CURRENT_VERSION,\
    (t)->contents.instance.transform.translation.x = tx,\
    (t)->contents.instance.transform.translation.y = ty,\
    (t)->contents.instance.transform.transformType = ttype,\
    octCreate(c, t))

#define ohCreateInst(c, t, xname, xmaster, xview)		\
    ohCreateInstance(c, t, xname, xmaster, xview, 0, 0, OCT_NO_TRANSFORM)

#define ohCreateBox(c, t, llx, lly, urx, ury)		\
    ((t)->type = OCT_BOX, 				\
    (t)->contents.box.lowerLeft.x = llx, 		\
    (t)->contents.box.lowerLeft.y = lly,		\
    (t)->contents.box.upperRight.x = urx, 		\
    (t)->contents.box.upperRight.y = ury,		\
    octCreate(c, t))

#define ohCreateCircle(c, t, startAng, endAng, inRad, outRad, cx, cy)\
    ((t)->type = OCT_CIRCLE, 				\
    (t)->contents.circle.startingAngle = startAng,	\
    (t)->contents.circle.endingAngle = endAng,		\
    (t)->contents.circle.innerRadius = inRad,		\
    (t)->contents.circle.outerRadius = outRad,		\
    (t)->contents.circle.center.x = cx,			\
    (t)->contents.circle.center.y = cy,			\
    octCreate(c, t))

#define ohCreateSprop	ohCreatePropStr
#define ohCreateIprop	ohCreatePropInt
#define ohCreateRprop	ohCreatePropReal

/*
 *  Macro's to call octGetOrCreate()
 *	c = the container
 *	t = the object
 *	s = name of the object
 */

#define ohGetOrCreateTerm(c, t, s)			\
    ((t)->type=OCT_TERM, (t)->contents.term.name = s, (t)->contents.term.width = 1, octGetOrCreate(c,t))

#define ohGetOrCreateNet(c, t, s)				\
    ((t)->type=OCT_NET, (t)->contents.net.name = s, (t)->contents.net.width = 1, octGetOrCreate(c,t))

#if OCT_LEVEL == 1
#define ohGetOrCreateLabel(c, t, s, position)		\
    ((t)->type=OCT_LABEL, 				\
    (t)->contents.label.label = s, 			\
    (t)->contents.label.position = position,            \
    octGetOrCreate(c,t))
#else
#define ohGetOrCreateLabel(c, t, s, box, th, vj, hj, lj)	\
    ((t)->type=OCT_LABEL,        		\
     (t)->contents.label.label = s,		\
     (t)->contents.label.region = box,		\
     (t)->contents.label.textHeight = th,	\
     (t)->contents.label.vertJust = vj,		\
     (t)->contents.label.horizJust = hj,	\
     (t)->contents.label.lineJust = lj,		\
     octGetOrCreate(c,t))
#endif

#define ohGetOrCreateBag(c, t, s)				\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octGetOrCreate(c,t))

#define ohGetOrCreateLayer(c, t, s)			\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octGetOrCreate(c,t))

#define ohGetOrCreatePropInt(c, t, s, i)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_INTEGER, 		\
    (t)->contents.prop.value.integer = i,		\
    octGetOrCreate(c,t))

#define ohGetOrCreatePropStr(c, t, s, s1)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_STRING, 		\
    (t)->contents.prop.value.string = s1,		\
    octGetOrCreate(c, t))

#define ohGetOrCreatePropReal(c, t, s, d)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_REAL, 		\
    (t)->contents.prop.value.real = d,			\
    octGetOrCreate(c, t))

#define ohGetOrCreateSprop ohGetOrCreatePropStr
#define ohGetOrCreateIprop ohGetOrCreatePropInt
#define ohGetOrCreateRprop ohGetOrCreatePropReal

/*
 *  Macro's to call octCreateOrModify()
 *	c = the container
 *	t = the object
 *	s = name of the object
 */

#define ohCreateOrModifyTerm(c, t, s)			\
    ((t)->type=OCT_TERM, (t)->contents.term.name=s, (t)->contents.term.width = 1, octCreateOrModify(c,t))

#define ohCreateOrModifyNet(c, t, s)			\
    ((t)->type=OCT_NET, (t)->contents.net.name=s, (t)->contents.net.width = 1, octCreateOrModify(c,t))

#if OCT_LEVEL == 1
#define ohCreateOrModifyLabel(c, t, s, position)		\
    ((t)->type=OCT_LABEL, 				\
    (t)->contents.label.label = s, 			\
    (t)->contents.label.position = position,            \
    octCreateOrModify(c,t))
#else
#define ohCreateOrModifyLabel(c, t, s, box, th, vj, hj, lj)	\
    ((t)->type=OCT_LABEL,			\
     (t)->contents.label.label=s,		\
     (t)->contents.label.region = box,		\
     (t)->contents.label.textHeight = th,	\
     (t)->contents.label.vertJust = vj,		\
     (t)->contents.label.horizJust = hj,	\
     (t)->contents.label.lineJust = lj,		\
     octCreateOrModify(c,t))
#endif

#define ohCreateOrModifyBag(c, t, s)			\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octCreateOrModify(c,t))

#define ohCreateOrModifyLayer(c, t, s)			\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octCreateOrModify(c,t))

#define ohCreateOrModifyPropInt(c, t, s, i)		\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_INTEGER, 		\
    (t)->contents.prop.value.integer = i,		\
    octCreateOrModify(c,t))

#define ohCreateOrModifyPropStr(c, t, s, s1)		\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_STRING,		\
    (t)->contents.prop.value.string = s1,		\
    octCreateOrModify(c, t))

#define ohCreateOrModifyPropReal(c, t, s, d)		\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_REAL,			\
    (t)->contents.prop.value.real = d,			\
    octCreateOrModify(c, t))

#define ohCreateOrModifySprop ohCreateOrModifyPropStr
#define ohCreateOrModifyIprop ohCreateOrModifyPropInt
#define ohCreateOrModifyRprop ohCreateOrModifyPropReal

/*
 *  Get object by id
 */
#define ohGetById(obj, id)				\
    ((obj)->objectId = id, octGetById(obj))

#define ohOpenFacet(t, c, v, f, m)			\
    ((t)->type = OCT_FACET, 				\
    (t)->contents.facet.cell = c, 			\
    (t)->contents.facet.view = v,			\
    (t)->contents.facet.facet = f, 			\
    (t)->contents.facet.version = OCT_CURRENT_VERSION,	\
    (t)->contents.facet.mode = m, 			\
    octOpenFacet(t))

#define ohOpenMaster(t, i, f, m)				\
    ((t)->type = OCT_FACET, 				\
    (t)->contents.facet.facet = f, 			\
    (t)->contents.facet.version = OCT_CURRENT_VERSION,	\
    (t)->contents.facet.mode = m, 			\
    octOpenMaster(i, t))


#define ohOpenInterface(t, f, m)				\
    ((t)->type = OCT_FACET, 				\
    (t)->contents.facet.cell = (f)->contents.facet.cell,\
    (t)->contents.facet.view = (f)->contents.facet.view,\
    (t)->contents.facet.facet = "interface",		\
    (t)->contents.facet.version = OCT_CURRENT_VERSION,	\
    (t)->contents.facet.mode = m, 			\
    octOpenFacet(t))


EXTERN int ohGetIntegerProp
  ARGS( (octObject *container, char *name ) );
EXTERN double ohGetRealProp
  ARGS( (octObject *container, char *name ) );
EXTERN char* ohGetStringProp
  ARGS( (octObject *container, char *name ) );


EXTERN void ohBoxCenter
  ARGS( (struct octBox *box, struct octPoint *center ));

EXTERN octCoord ohBoxWidth
  ARGS( (struct octBox *box ));
EXTERN octCoord ohBoxHeight
  ARGS( (struct octBox *box ));
EXTERN octCoord ohBoxArea
  ARGS( (struct octBox *box ));

#endif
