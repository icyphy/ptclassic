#ifndef _OCTMACROS_H
#define _OCTMACROS_H 1

/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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
/* octMacros.h  edg
Version identification:
@(#)octMacros.h	1.9	11/29/95
Useful OCT macros.  Don't use this any more; use oh.h instead.  oh.h
is the standard Oct package that does the same thing.  (It was developed
after this file was written, and its macros are preceded by an "oh" prefix.)
From: /usr5/octtools/src/octflat/my.h copied 8/7/88

The only macros from this file actually used by pigilib are GetByPropName,
GetById, CreateInstance2, and CK_OCT.  -BLE
*/

/* Define prototypes for octGetByName, octCreate, octGetById, octErrorString */
#include "oct.h"

/*
 *  Macros to call octGetByName()
 *	cc = the container
 *	tt = the object
 *	ss = the object name
 */

#define GetByTermName(cc,tt,ss)				\
    ((tt)->type = OCT_TERM,				\
     (tt)->contents.term.name = (ss),			\
     octGetByName((cc),(tt)))

#define GetByNetName(cc,tt,ss)				\
    ((tt)->type = OCT_NET,				\
     (tt)->contents.net.name = (ss),			\
     octGetByName((cc),(tt)))

#define GetByInstName(cc,tt,ss)				\
    ((tt)->type = OCT_INSTANCE,				\
     (tt)->contents.instance.name = (ss),		\
     octGetByName((cc),(tt)))

#define GetByLabelName(cc,tt,ss)			\
    ((tt)->type = OCT_LABEL,				\
     (tt)->contents.label.label = (ss),			\
     octGetByName((cc),(tt)))

#define GetByBagName(cc,tt,ss)				\
    ((tt)->type = OCT_BAG,				\
     (tt)->contents.bag.name = (ss),			\
     octGetByName((cc),(tt)))

#define GetByLayerName(cc,tt,ss)			\
    ((tt)->type = OCT_LAYER,				\
     (tt)->contents.layer.name = (ss),			\
     octGetByName((cc),(tt)))

/*  GetByPropName: used by compile.c, octIfc.c, palette.c */
#define GetByPropName(cc,tt,ss)				\
    ((tt)->type = OCT_PROP,				\
     (tt)->contents.prop.name = (ss),			\
     octGetByName((cc),(tt)))

/*
 *  Macros to call octCreate()
 *	cc = the container
 *	tt = the object
 *	ss = the object name (for named objects)
 */

#define CreateTerm(cc,tt,ss)				\
    ((tt)->type = OCT_TERM,				\
     (tt)->contents.term.name = (ss),			\
     (tt)->contents.term.instanceId = oct_null_id,	\
     octCreate((cc),(tt)))

#define CreateNet(cc,tt,ss)				\
    ((tt)->type = OCT_NET,				\
     (tt)->contents.net.name = (ss),			\
     octCreate((cc),(tt)))

#define CreateLabel(cc,tt,ss,zx,zy)			\
    ((tt)->type = OCT_LABEL,				\
     (tt)->contents.label.label = (ss),			\
     (tt)->contents.label.position.x = (zx),		\
     (tt)->contents.label.position.y = (zy),		\
     octCreate((cc),(tt)))

#define CreateBag(cc,tt,ss)				\
    ((tt)->type = OCT_BAG,				\
     (tt)->contents.bag.name = (ss),			\
     octCreate((cc),(tt)))

#define CreateLayer(cc,tt,ss)				\
    ((tt)->type = OCT_LAYER,				\
     (tt)->contents.layer.name = (ss),			\
     octCreate((cc),(tt)))

#define CreatePropInt(cc,tt,ss,ii)			\
    ((tt)->type = OCT_PROP,				\
     (tt)->contents.prop.name = (ss),			\
     (tt)->contents.prop.type = OCT_INTEGER,		\
     (tt)->contents.prop.value.integer = (ii),		\
     octCreate((cc),(tt)))

#define CreatePropStr(cc,tt,ss,s1)			\
    ((tt)->type = OCT_PROP,				\
     (tt)->contents.prop.name = (ss),			\
     (tt)->contents.prop.type = OCT_STRING,		\
     (tt)->contents.prop.value.string = (s1),		\
     octCreate((cc),(tt)))

#define CreatePropReal(cc,tt,ss,dd)			\
    ((tt)->type = OCT_PROP,				\
     (tt)->contents.prop.name = (ss),			\
     (tt)->contents.prop.type = OCT_REAL,		\
     (tt)->contents.prop.value.real = (dd),		\
     octCreate((cc),(tt)))

#define CreateInstance(cc,tt,xname,xmaster,xview,tx,ty,ttype)\
    ((tt)->type = OCT_INSTANCE,				\
     (tt)->contents.instance.name = (xname),		\
     (tt)->contents.instance.master = (xmaster),	\
     (tt)->contents.instance.view = (xview),		\
     (tt)->contents.instance.version = OCT_CURRENT_VERSION,\
     (tt)->contents.instance.transform.translation.x = (tx),\
     (tt)->contents.instance.transform.translation.y = (ty),\
     (tt)->contents.instance.transform.transformType = (ttype),\
     octCreate((cc),(tt)))

#define CreateInst(cc,tt,xname,xmaster,xview)		\
    CreateInstance(cc,tt,xname,xmaster,xview,0,0,OCT_NO_TRANSFORM)

#define CreateBox(cc,tt,llx,lly,urx,ury)		\
    ((tt)->type = OCT_BOX,				\
     (tt)->contents.box.lowerLeft.x = (llx),		\
     (tt)->contents.box.lowerLeft.y = (lly),		\
     (tt)->contents.box.upperRight.x = (urx),		\
     (tt)->contents.box.upperRight.y = (ury),		\
     octCreate((cc),(tt)))

#define CreateCircle(cc,tt,startAng,endAng,inRad,outRad,cx,cy)\
    ((tt)->type = OCT_CIRCLE,				\
     (tt)->contents.circle.startingAngle = (startAng),	\
     (tt)->contents.circle.endingAngle = (endAng),	\
     (tt)->contents.circle.innerRadius = (inRad),	\
     (tt)->contents.circle.outerRadius = (outRad),	\
     (tt)->contents.circle.center.x = (cx),		\
     (tt)->contents.circle.center.y = (cy),		\
     octCreate((cc),(tt)))

/*  CreateInstance2: used by palette.c  */
#define CreateInstance2(cc,tt,xname,xmaster,xview,xfacet,tx,ty,ttype)\
    ((tt)->type = OCT_INSTANCE,				\
     (tt)->contents.instance.name = (xname),		\
     (tt)->contents.instance.master = (xmaster),	\
     (tt)->contents.instance.view = (xview),		\
     (tt)->contents.instance.facet = (xfacet),		\
     (tt)->contents.instance.version = OCT_CURRENT_VERSION,\
     (tt)->contents.instance.transform.translation.x = (tx),\
     (tt)->contents.instance.transform.translation.y = (ty),\
     (tt)->contents.instance.transform.transformType = (ttype),\
     octCreate((cc),(tt)))

/*
 *  Miscellaneous macros
 */

/*  GetById: used by compile.c  */
#define GetById(obj,id)					\
    ((obj)->objectId = (id), octGetById(obj))

/*  CK_OCT:  used by compile.c, mkIcon.c, mkTerm.c, octIfc.c,  */
/*  palette.c, and vemInterface.c */
#define CK_OCT(cond) \
    if ((cond) < OCT_OK) { \
	ErrAdd(octErrorString()); \
	return(FALSE); \
    }

#endif  /* _OCTMACROS_H */
