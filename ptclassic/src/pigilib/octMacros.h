/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/* octMacros.h  edg
Version identification:
$Id$
Useful OCT macros.  Don't use this any more; use oh.h instead.  oh.h
is the standard Oct package that does the same thing.  (It was developed
after this file was written.)
From: /usr5/octtools/src/octflat/my.h copied 8/7/88
*/

/*
 *  Macro's to call octGetByName()
 *	c = the container
 *	t = the object
 *	s = the object name
 */

#define GetByTermName(c, t, s)				\
    ((t)->type=OCT_TERM, (t)->contents.term.name=s, octGetByName(c,t))

#define GetByNetName(c, t, s)				\
    ((t)->type=OCT_NET, (t)->contents.net.name=s, octGetByName(c,t))

#define GetByInstName(c, t, s)				\
    ((t)->type=OCT_INSTANCE,(t)->contents.instance.name=s, octGetByName(c,t))

#define GetByLabelName(c, t, s)				\
    ((t)->type=OCT_LABEL, (t)->contents.label.label=s, octGetByName(c,t))

#define GetByBagName(c, t, s)				\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octGetByName(c,t))

#define GetByLayerName(c, t, s)				\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octGetByName(c,t))

#define GetByPropName(c, t, s)				\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s, octGetByName(c,t))

/*
 *  Macro's to call octCreate()
 *	c = the container
 *	t = the object
 *	s = the object name (for named objects)
 */

#define CreateTerm(c, t, s)				\
    ((t)->type=OCT_TERM, (t)->contents.term.name=s, 	\
    (t)->contents.term.instanceId = oct_null_id, octCreate(c,t))

#define CreateNet(c, t, s)				\
    ((t)->type=OCT_NET, (t)->contents.net.name=s, octCreate(c,t))

#define CreateLabel(c, t, s, zx, zy)			\
    ((t)->type=OCT_LABEL, 				\
    (t)->contents.label.label = s, 			\
    (t)->contents.label.position.x = zx,		\
    (t)->contents.label.position.y = zy,		\
    octCreate(c,t))

#define CreateBag(c, t, s)				\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octCreate(c,t))

#define CreateLayer(c, t, s)				\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octCreate(c,t))

#define CreatePropInt(c, t, s, i)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_INTEGER, 		\
    (t)->contents.prop.value.integer = i,		\
    octCreate(c,t))

#define CreatePropStr(c, t, s, s1)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_STRING, 		\
    (t)->contents.prop.value.string = s1,		\
    octCreate(c, t))

#define CreatePropReal(c, t, s, d)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_REAL, 		\
    (t)->contents.prop.value.real = d,			\
    octCreate(c, t))

#define CreateInstance(c, t, xname, xmaster, xview, tx, ty, ttype)\
    ((t)->type=OCT_INSTANCE, 				\
    (t)->contents.instance.name = xname,		\
    (t)->contents.instance.master = xmaster, 		\
    (t)->contents.instance.view = xview,		\
    (t)->contents.instance.version = OCT_CURRENT_VERSION,\
    (t)->contents.instance.transform.translation.x = tx,\
    (t)->contents.instance.transform.translation.y = ty,\
    (t)->contents.instance.transform.transformType = ttype,\
    octCreate(c, t))

#define CreateInst(c, t, xname, xmaster, xview)		\
    CreateInstance(c, t, xname, xmaster, xview, 0, 0, OCT_NO_TRANSFORM)

#define CreateBox(c, t, llx, lly, urx, ury)		\
    ((t)->type = OCT_BOX, 				\
    (t)->contents.box.lowerLeft.x = llx, 		\
    (t)->contents.box.lowerLeft.y = lly,		\
    (t)->contents.box.upperRight.x = urx, 		\
    (t)->contents.box.upperRight.y = ury,		\
    octCreate(c, t))

#define CreateCircle(c, t, startAng, endAng, inRad, outRad, cx, cy)\
    ((t)->type = OCT_CIRCLE, 				\
    (t)->contents.circle.startingAngle = startAng,	\
    (t)->contents.circle.endingAngle = endAng,		\
    (t)->contents.circle.innerRadius = inRad,		\
    (t)->contents.circle.outerRadius = outRad,		\
    (t)->contents.circle.center.x = cx,			\
    (t)->contents.circle.center.y = cy,			\
    octCreate(c, t))

#define CreateSprop	CreatePropStr
#define CreateIprop	CreatePropInt
#define CreateRprop	CreatePropReal

/*
 *  Macro's to call octGetOrCreate()
 *	c = the container
 *	t = the object
 *	s = name of the object
 */

#define GetOrCreateTerm(c, t, s)			\
    ((t)->type=OCT_TERM, (t)->contents.term.name=s, octGetOrCreate(c,t))

#define GetOrCreateNet(c, t, s)				\
    ((t)->type=OCT_NET, (t)->contents.net.name=s, octGetOrCreate(c,t))

#define GetOrCreateLabel(c, t, s)			\
    ((t)->type=OCT_LABEL, (t)->contents.label.label=s, octGetOrCreate(c,t))

#define GetOrCreateBag(c, t, s)				\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octGetOrCreate(c,t))

#define GetOrCreateLayer(c, t, s)			\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octGetOrCreate(c,t))

#define GetOrCreatePropInt(c, t, s, i)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_INTEGER, 		\
    (t)->contents.prop.value.integer = i,		\
    octGetOrCreate(c,t))

#define GetOrCreatePropStr(c, t, s, s1)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_STRING, 		\
    (t)->contents.prop.value.string = s1,		\
    octGetOrCreate(c, t))

#define GetOrCreatePropReal(c, t, s, d)			\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_REAL, 		\
    (t)->contents.prop.value.real = d,			\
    octGetOrCreate(c, t))

#define GetOrCreateSprop GetOrCreatePropStr
#define GetOrCreateIprop GetOrCreatePropInt
#define GetOrCreateRprop GetOrCreatePropReal

/*
 *  Macro's to call octCreateOrModify()
 *	c = the container
 *	t = the object
 *	s = name of the object
 */

#define CreateOrModifyTerm(c, t, s)			\
    ((t)->type=OCT_TERM, (t)->contents.term.name=s, octCreateOrModify(c,t))

#define CreateOrModifyNet(c, t, s)			\
    ((t)->type=OCT_NET, (t)->contents.net.name=s, octCreateOrModify(c,t))

#define CreateOrModifyLabel(c, t, s)			\
    ((t)->type=OCT_LABEL, (t)->contents.label.label=s, octCreateOrModify(c,t))

#define CreateOrModifyBag(c, t, s)			\
    ((t)->type=OCT_BAG, (t)->contents.bag.name=s, octCreateOrModify(c,t))

#define CreateOrModifyLayer(c, t, s)			\
    ((t)->type=OCT_LAYER, (t)->contents.layer.name=s, octCreateOrModify(c,t))

#define CreateOrModifyPropInt(c, t, s, i)		\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_INTEGER, 		\
    (t)->contents.prop.value.integer = i,		\
    octCreateOrModify(c,t))

#define CreateOrModifyPropStr(c, t, s, s1)		\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_STRING,		\
    (t)->contents.prop.value.string = s1,		\
    octCreateOrModify(c, t))

#define CreateOrModifyPropReal(c, t, s, d)		\
    ((t)->type=OCT_PROP, (t)->contents.prop.name=s,	\
    (t)->contents.prop.type = OCT_REAL,			\
    (t)->contents.prop.value.real = d,			\
    octCreateOrModify(c, t))

#define CreateOrModifySprop CreateOrModifyPropStr
#define CreateOrModifyIprop CreateOrModifyPropInt
#define CreateOrModifyRprop CreateOrModifyPropReal

/*
 *  Get object by id
 */
#define GetById(obj, id)				\
    ((obj)->objectId = id, octGetById(obj))

#define OpenFacet(t, c, v, f, m)			\
    ((t)->type = OCT_FACET, 				\
    (t)->contents.facet.cell = c, 			\
    (t)->contents.facet.view = v,			\
    (t)->contents.facet.facet = f, 			\
    (t)->contents.facet.version = OCT_CURRENT_VERSION,	\
    (t)->contents.facet.mode = m, 			\
    octOpenFacet(t))

#define CreateInstance2(c, t, xname, xmaster, xview, xfacet, tx, ty, ttype)\
    ((t)->type=OCT_INSTANCE, 				\
    (t)->contents.instance.name = xname,		\
    (t)->contents.instance.master = xmaster, 		\
    (t)->contents.instance.view = xview,		\
    (t)->contents.instance.facet = xfacet,		\
    (t)->contents.instance.version = OCT_CURRENT_VERSION,\
    (t)->contents.instance.transform.translation.x = tx,\
    (t)->contents.instance.transform.translation.y = ty,\
    (t)->contents.instance.transform.transformType = ttype,\
    octCreate(c, t))

#define CK_OCT(cond) \
    if ((cond) < OCT_OK) { \
	ErrAdd(octErrorString()); \
	return(FALSE); \
    }
