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
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "st.h"
#include "errtrap.h"
#include "internal.h"
#include "obj.h"

extern char *tPrintEnum(), *tPrintInteger(), *tPrintString(), *tPrintCoord();
extern char *tPrintID(), *tPrintReal(), *tPrintXid();
extern char *tPrintVJust(), *tPrintHJust(), *tPrintLJust();
extern char *tPrintName(), *tPrintBB(), *tPrintLayer(), *tPrintConnWidth();
extern char *tPrintNPoints(), *tPrintNGeos(), *tPrintNObjs();

extern int tGetEnum(), tGetInteger(), tGetString(), tGetCoord();
extern int tGetID(), tGetReal();
extern int tGetVJust(), tGetHJust(), tGetLJust();

/* allow object type to be modified only on creation */
#define CREATE_CHECK(objPtr) \
    if (!octIdIsNull((objPtr)->objectId)) {				\
	errRaise(optProgName, 0,					\
		    "can't modify the type of an existing object");	\
    }

static struct fieldTemplate summaryUndefinedObjTemplate[] = {
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedUndefinedObjTemplate[] = {
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initUndefinedObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
}

static struct fieldTemplate summaryFacetTemplate[] = {
    {	"    ",		OFS(facet.cell),	tPrintName,	NILGF,	0 },
    {	" ",		0,			tPrintBB,	NILGF,	0 },
    {	" ",		OFS(facet.view),	tPrintString,	NILGF,	0 },
    {	":",		OFS(facet.facet),	tPrintString,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedFacetTemplate[] = {
    {	"\n\ncell\t\t\t", OFS(facet.cell),	tPrintString,	NILGF,	0 },
    {	"\nview\t\t\t",	OFS(facet.view),	tPrintString,	NILGF,	0 },
    {	"\nfacet\t\t\t", OFS(facet.facet),	tPrintString,	NILGF,	0 },
    {	"\nversion\t\t\t", OFS(facet.version),	tPrintString,	NILGF,	0 },
    {	"\nmode\t\t\t",	OFS(facet.mode),	tPrintString,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initFacetObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.facet.cell = "";
    objPtr->contents.facet.view = "";
    objPtr->contents.facet.facet = "";
    objPtr->contents.facet.version = OCT_CURRENT_VERSION;
    objPtr->contents.facet.mode = "";
}

static struct fieldTemplate summaryTermTemplate[] = {
    {	"     ",	OFS(term.name),		tPrintName,	NILGF,	0 },
    {	" ",		OFS(term.width),	tPrintConnWidth,NILGF,	0 },
    {	" ",		OFS(term.instanceId),	tPrintID,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedTermTemplate[] = {
    {	"\n\nname\t\t\t", OFS(term.name),	tPrintString,	tGetString, 0 },
    {	"\nwidth\t\t\t", OFS(term.width),	tPrintInteger,	tGetInteger, 0},
    {	"\ninstance\t\t", OFS(term.instanceId),	tPrintID,	NILGF,	0 },
    {	"\nformal terminal Xid\t\t",	OFS(term.formalExternalId),
						tPrintInteger,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initTermObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.term.name = "";
    objPtr->contents.term.instanceId = oct_null_id;
    objPtr->contents.term.formalExternalId = 0;
    objPtr->contents.term.width = 1;
}

static struct fieldTemplate summaryNetTemplate[] = {
    {	"      ",	OFS(net.name),		tPrintName,	NILGF,	0 },
    {	" ",		OFS(net.width),		tPrintConnWidth,NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedNetTemplate[] = {
    {	"\n\nname\t\t\t", OFS(net.name),	tPrintString,	tGetString, 0 },
    {	"\nwidth\t\t\t", OFS(net.width),	tPrintInteger,	tGetInteger, 0},
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initNetObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.net.name = "";
    objPtr->contents.net.width = 1;
}

static struct fieldTemplate xformTemplate[] = {
    {	"\n\t .generalTransform\t",
OFS(instance.transform.generalTransform[0][0]),	tPrintReal,	tGetReal, 0 },
    {	"\t",
OFS(instance.transform.generalTransform[0][1]),	tPrintReal,	tGetReal, 0 },
    {	"\n\t\t\t\t",
OFS(instance.transform.generalTransform[1][0]),	tPrintReal,	tGetReal, 0 },
    {	"\t",
OFS(instance.transform.generalTransform[1][1]),	tPrintReal,	tGetReal, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initFullXform(objPtr)
octObject *objPtr;
{
    objPtr->contents.instance.transform.generalTransform[0][0] = 1.0;
    objPtr->contents.instance.transform.generalTransform[0][1] = 0.0;
    objPtr->contents.instance.transform.generalTransform[1][0] = 0.0;
    objPtr->contents.instance.transform.generalTransform[1][1] = 1.0;
}

static struct enumTemplate xformEnum[] = {
    {	"NO_TRANSFORM",	(int) OCT_NO_TRANSFORM,	0,	0		},
    {	"MIRROR_X",	(int) OCT_MIRROR_X,	0,	0		},
    {	"MIRROR_Y",	(int) OCT_MIRROR_Y,	0,	0		},
    {	"ROT90",	(int) OCT_ROT90,	0,	0		},
    {	"ROT180",	(int) OCT_ROT180,	0,	0		},
    {	"ROT270",	(int) OCT_ROT270,	0,	0		},
    {	"MX_ROT90",	(int) OCT_MX_ROT90,	0,	0		},
    {	"MY_ROT90",	(int) OCT_MY_ROT90,	0,	0		},
    {	"FULL_TRANSFORM", (int) OCT_FULL_TRANSFORM,
					xformTemplate,	initFullXform	},
    {	NIL(char),	0,			0,	0		},
};

static struct fieldTemplate summaryInstanceTemplate[] = {
    {	" ",		OFS(instance.name),	tPrintName,	NILGF,	0 },
    {	" ",		0,			tPrintBB,	NILGF,	0 },
    {	" ",		OFS(instance.master),	tPrintString,	NILGF,	0 },
    {	":",		OFS(instance.view),	tPrintString,	NILGF,	0 },
    {	":",		OFS(instance.facet),	tPrintString,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedInstanceTemplate[] = {
    {	"\n\ntransform.translation\tx=",
	OFS(instance.transform.translation.x),	tPrintCoord,	tGetCoord, 0 },
    {	"   y=",
	OFS(instance.transform.translation.y),	tPrintCoord,	tGetCoord, 0 },
    {	"\n\t .transformType\t",	OFS(instance.transform.transformType),
				tPrintEnum,	tGetEnum,	xformEnum },
    {	"\nname\t\t\t",	OFS(instance.name),	tPrintString,	tGetString, 0 },
    {	"\nmaster\t\t\t",OFS(instance.master),	tPrintString,	tGetString, 0 },
    {	"\nview\t\t\t",	OFS(instance.view),	tPrintString,	tGetString, 0 },
    {	"\nfacet\t\t\t",OFS(instance.facet),	tPrintString,	tGetString, 0 },
    {	"\nversion\t\t\t",OFS(instance.version),tPrintString,	tGetString, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initInstanceObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.instance.name = "";
    objPtr->contents.instance.master = "";
    objPtr->contents.instance.view = "";
    objPtr->contents.instance.facet = "";
    objPtr->contents.instance.version = "";
    objPtr->contents.instance.transform.translation.x = 0;
    objPtr->contents.instance.transform.translation.y = 0;
    objPtr->contents.instance.transform.transformType = OCT_NO_TRANSFORM;
}

static struct fieldTemplate summaryPolygonTemplate[] = {
    {	"                  ",	0,		tPrintBB,	NILGF,	0 },
    {	" lyr(",	0,			tPrintLayer,	NILGF,	0 },
    {	") v=",		0,			tPrintNPoints,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedPolygonTemplate[] = {
    /* XXX -- need to add point list editing */
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initPolygonObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
}

static struct fieldTemplate summaryBoxTemplate[] = {
    {	"                      ",	0,	tPrintBB,	NILGF,	0 },
    {	" lyr(",	0,			tPrintLayer,	NILGF,	0 },
    {	")",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedBoxTemplate[] = {
    {	"\n\nlowerLeft\t\tx=",	OFS(box.lowerLeft.x),
						tPrintCoord,	tGetCoord, 0 },
    {	"   y=",	OFS(box.lowerLeft.y),	tPrintCoord,	tGetCoord, 0 },
    {	"\nupperRight\t\tx=",	OFS(box.upperRight.x),
						tPrintCoord,	tGetCoord, 0 },
    {	"   y=",	OFS(box.upperRight.y),	tPrintCoord,	tGetCoord, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initBoxObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.box.lowerLeft.x = 0;
    objPtr->contents.box.lowerLeft.y = 0;
    objPtr->contents.box.upperRight.x = 0;
    objPtr->contents.box.upperRight.y = 0;
}

static struct fieldTemplate summaryCircleTemplate[] = {
    {	"                   ",	0,		tPrintBB,	NILGF,	0 },
    {	" lyr(",	0,			tPrintLayer,	NILGF,	0 },
    {	")",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedCircleTemplate[] = {
    {	"\n\nstartingAngle\t\t",	OFS(circle.startingAngle),
						tPrintCoord,	tGetCoord, 0 },
    {	"\nendingAngle\t\t",	OFS(circle.endingAngle),
						tPrintCoord,	tGetCoord, 0 },
    {	"\ninnerRadius\t\t",	OFS(circle.innerRadius),
						tPrintCoord,	tGetCoord, 0 },
    {	"\nouterRadius\t\t",	OFS(circle.outerRadius),
						tPrintCoord,	tGetCoord, 0 },
    {	"\ncenter\t\t\tx=",	OFS(circle.center.x),
						tPrintCoord,	tGetCoord, 0 },
    {	"   y=",	OFS(circle.center.y),	tPrintCoord,	tGetCoord, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initCircleObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.circle.startingAngle = 0;
    objPtr->contents.circle.endingAngle = 0;
    objPtr->contents.circle.innerRadius = 0;
    objPtr->contents.circle.outerRadius = 0;
    objPtr->contents.circle.center.x = 0;
    objPtr->contents.circle.center.y = 0;
}

static struct fieldTemplate summaryPathTemplate[] = {
    {	"                     ",	0,	tPrintBB,	NILGF,	0 },
    {	" lyr(",	0,			tPrintLayer,	NILGF,	0 },
    {	") v=",		0,			tPrintNPoints,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedPathTemplate[] = {
    {	"\n\nwidth\t\t\t", OFS(path.width),	tPrintCoord,	tGetCoord, 0 },
    /* XXX -- need to add point list editing */
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initPathObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.path.width = 0;
}

static struct fieldTemplate nullPropTemplate[] = {
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};

static struct fieldTemplate integerPropTemplate[] = {
    {	"",		OFS(prop.value.integer), tPrintInteger,	tGetInteger, 0},
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initIntProp(objPtr)
octObject *objPtr;
{
    objPtr->contents.prop.value.integer = 0;
}

static struct fieldTemplate realPropTemplate[] = {
    {	"",		OFS(prop.value.real),	tPrintReal,	tGetReal, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initRealProp(objPtr)
octObject *objPtr;
{
    objPtr->contents.prop.value.real = 0.0;
}

static struct fieldTemplate stringPropTemplate[] = {
    {	"\"",		OFS(prop.value.string),	tPrintString,	tGetString, 0 },
    {	"\"",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initStringProp(objPtr)
octObject *objPtr;
{
    objPtr->contents.prop.value.string = "";
}

static struct fieldTemplate idPropTemplate[] = {
    {	"",		OFS(prop.value.id),	tPrintID,	tGetID,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initIdProp(objPtr)
octObject *objPtr;
{
    objPtr->contents.prop.value.id = oct_null_id;
}

static struct fieldTemplate strangerPropTemplate[] = {
    {	"stranger[",	OFS(prop.value.stranger.length),
						tPrintInteger,	NILGF,	0 },
    {	"]",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initStrangerProp(objPtr)
octObject *objPtr;
{
    objPtr->contents.prop.value.stranger.length = 0;
}

static struct fieldTemplate realArrayPropTemplate[] = {
    {	"real[",	OFS(prop.value.real_array.length),
						tPrintInteger,	tGetInteger, 0},
    {	"]",		0,			NILPF,		NILGF,	0 },
    /* XXX -- need to add array editing */
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initRAProp(objPtr)
octObject *objPtr;
{
    objPtr->contents.prop.value.real_array.length = 0;
}

static struct fieldTemplate integerArrayPropTemplate[] = {
    {	"integer[",	OFS(prop.value.integer_array.length),
						tPrintInteger,	tGetInteger, 0},
    {	"]",		0,			NILPF,		NILGF,	0 },
    /* XXX -- need to add array editing */
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initIAProp(objPtr)
octObject *objPtr;
{
    objPtr->contents.prop.value.integer_array.length = 0;
}

static struct enumTemplate propValEnum[] = {
    {	"",	(int) OCT_NULL,		nullPropTemplate,	0	},
    {	"",	(int) OCT_INTEGER,	integerPropTemplate,	0	},
    {	"",	(int) OCT_REAL,		realPropTemplate,	0	},
    {	"",	(int) OCT_STRING,	stringPropTemplate,	0	},
    {	"",	(int) OCT_ID,		idPropTemplate,		0	},
    {	"",	(int) OCT_STRANGER,	strangerPropTemplate,	0	},
    {	"",	(int) OCT_REAL_ARRAY,	realArrayPropTemplate,	0	},
    {	"",	(int) OCT_INTEGER_ARRAY, integerArrayPropTemplate, 0	},
    {	NIL(char),	0,		0,			0	},
};

static struct enumTemplate propTypeEnum[] = {
    {	"NULL",		(int) OCT_NULL,		0,	0		},
    {	"INTEGER",	(int) OCT_INTEGER,	0,	initIntProp	},
    {	"REAL",		(int) OCT_REAL,		0,	initRealProp	},
    {	"STRING",	(int) OCT_STRING,	0,	initStringProp	},
    {	"ID",		(int) OCT_ID,		0,	initIdProp	},
    {	"STRANGER",	(int) OCT_STRANGER,	0,	initStrangerProp },
    {	"REAL_ARRAY",	(int) OCT_REAL_ARRAY,	0,	initRAProp	},
    {	"INTEGER_ARRAY", (int) OCT_INTEGER_ARRAY, 0,	initIAProp	},
    {	NIL(char),	0,			0,	0		},
};

static struct fieldTemplate summaryPropTemplate[] = {
    {	"     ",	OFS(prop.name),		tPrintName,	NILGF,	0 },
    {	" ",		0,			tPrintBB,	NILGF,	0 },
    {	" ",		OFS(prop.type),		tPrintEnum,	NILGF,
							    propValEnum },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedPropTemplate[] = {
    {	"\n\nname\t\t\t", OFS(prop.name),	tPrintString,	tGetString, 0 },
    {	"\ntype\t\t\t",	OFS(prop.type),		tPrintEnum,	tGetEnum,
							    propTypeEnum },
    {	"\nvalue\t\t\t",OFS(prop.type),		tPrintEnum,	NILGF,
							    propValEnum },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initPropObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.prop.name = "";
    objPtr->contents.prop.type = OCT_STRING;
    objPtr->contents.prop.value.string = "";
}

static struct enumTemplate vJustEnum[] = {
    {	"BOTTOM",	OCT_JUST_BOTTOM,	0,	0		},
    {	"CENTER",	OCT_JUST_CENTER,	0,	0		},
    {	"TOP",		OCT_JUST_TOP,		0,	0		},
    {	NIL(char),	0,			0,	0		},
};
static struct enumTemplate hJustEnum[] = {
    {	"LEFT",		OCT_JUST_LEFT,		0,	0		},
    {	"CENTER",	OCT_JUST_CENTER,	0,	0		},
    {	"RIGHT",	OCT_JUST_RIGHT,		0,	0		},
    {	NIL(char),	0,			0,	0		},
};

static struct fieldTemplate summaryLabelTemplate[] = {
    {	"    ",		OFS(label.label),	tPrintName,	NILGF,	0 },
    {	" ",		0,			tPrintBB,	NILGF,	0 },
    {	" lyr(",	0,			tPrintLayer,	NILGF,	0 },
    {	")",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedLabelTemplate[] = {
    {	"\n\nlabel\t\t\t", OFS(label.label),	tPrintString,	tGetString, 0 },
    {	"\nlowerLeft\t\tx=",	OFS(label.region.lowerLeft.x),
			tPrintCoord,	tGetCoord,	0		},
    {	"   y=",		OFS(label.region.lowerLeft.y),
			tPrintCoord,	tGetCoord,	0		},
    {	"\nupperRight\t\tx=",	OFS(label.region.upperRight.x),
			tPrintCoord,	tGetCoord,	0		},
    {	"   y=",		OFS(label.region.upperRight.y),
			tPrintCoord,	tGetCoord,	0		},
    {	"\ntextHeight\t\t",	OFS(label.textHeight),	
			tPrintInteger,	tGetInteger,	0		},
    {	"\nvertJust\t\t",	0,	
			tPrintVJust,	tGetVJust,	vJustEnum	},
    {	"\nhorizJust\t\t",	0,	
			tPrintHJust,	tGetHJust,	hJustEnum	},
    {	"\nlineJust\t\t",	0,	
			tPrintLJust,	tGetLJust,	hJustEnum	},
    {	NIL(char), 0,	NILPF,		NILGF,		0		},
};
static void initLabelObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.label.label = "";
    objPtr->contents.label.region.lowerLeft.x = 0;
    objPtr->contents.label.region.lowerLeft.y = 0;
    objPtr->contents.label.region.upperRight.x = 0;
    objPtr->contents.label.region.upperRight.y = 0;
    objPtr->contents.label.textHeight = 0;
    objPtr->contents.label.vertJust = OCT_JUST_TOP;
    objPtr->contents.label.horizJust = OCT_JUST_LEFT;
    objPtr->contents.label.lineJust = OCT_JUST_LEFT;
}

static struct fieldTemplate summaryBagTemplate[] = {
    {	"      ",	OFS(bag.name),		tPrintName,	NILGF,	0 },
    {	" ",		0,			tPrintBB,	NILGF,	0 },
    {	" objs=",	0,			tPrintNObjs,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedBagTemplate[] = {
    {	"\n\nname\t\t\t", OFS(bag.name),	tPrintString,	tGetString, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initBagObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.bag.name = "";
}

static struct fieldTemplate summaryLayerTemplate[] = {
    {	"    ",		OFS(layer.name),	tPrintName,	NILGF,	0 },
    {	" ",		0,			tPrintBB,	NILGF,	0 },
    {	" geos=",	0,			tPrintNGeos,	NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedLayerTemplate[] = {
    {	"\n\nname\t\t\t", OFS(layer.name),	tPrintString,	tGetString, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initLayerObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.layer.name = "";
}

static struct fieldTemplate summaryPointTemplate[] = {
    {	"                    ",	0,		tPrintBB,	NILGF,	0 },
    {	" lyr(",	0,			tPrintLayer,	NILGF,	0 },
    {	")",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedPointTemplate[] = {
    {	"\n\n\t\t\tx=",	OFS(point.x),		tPrintCoord,	tGetCoord, 0 },
    {	"   y=",	OFS(point.y),		tPrintCoord,	tGetCoord, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initPointObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.point.x = 0;
    objPtr->contents.point.y = 0;
}

static struct fieldTemplate summaryEdgeTemplate[] = {
    {	"                     ",	0,	tPrintBB,	NILGF,	0 },
    {	" lyr(",	0,			tPrintLayer,	NILGF,	0 },
    {	")",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedEdgeTemplate[] = {
    {	"\n\nstart\t\t\tx=", OFS(edge.start.x),	tPrintCoord,	tGetCoord, 0 },
    {	"   y=",	OFS(edge.start.y),	tPrintCoord,	tGetCoord, 0 },
    {	"\nend\t\t\tx=", OFS(edge.end.x),	tPrintCoord,	tGetCoord, 0 },
    {	"   y=",	OFS(edge.end.y),	tPrintCoord,	tGetCoord, 0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initEdgeObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.edge.start.x = 0;
    objPtr->contents.edge.start.y = 0;
    objPtr->contents.edge.end.x = 0;
    objPtr->contents.edge.end.y = 0;
}

static struct fieldTemplate summaryChgListTemplate[] = {
    {	"              ",	0,		tPrintBB,	NILGF,	0 },
    {	" ",	OFS(changeList.functionMask),	tPrintInteger,	NILGF,	0 },
    {	"(",	OFS(changeList.objectMask),	tPrintInteger,	NILGF,	0 },
    {	")",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedChgListTemplate[] = {
    {	"\n\nobjectMask\t\t\t",
		OFS(changeList.objectMask),	tPrintInteger,	tGetInteger, 0},
    {	"\nfunctionMask\t\t\t",
		OFS(changeList.functionMask),	tPrintInteger,	tGetInteger, 0},
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initChgListObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.changeList.objectMask = 0;
    objPtr->contents.changeList.functionMask = 0;
}

static struct enumTemplate chgTypeEnum[] = {
    {	"CREATE",		OCT_CREATE,		0,	0	},
    {	"DELETE",		OCT_DELETE,		0,	0	},
    {	"MODIFY",		OCT_MODIFY,		0,	0	},
    {	"PUT_POINTS",		OCT_PUT_POINTS,		0,	0	},
    {	"ATTACH_CONTENT",	OCT_ATTACH_CONTENT,	0,	0	},
    {	"ATTACH_CONTAINER",	OCT_ATTACH_CONTAINER,	0,	0	},
    {	"DETACH_CONTENT",	OCT_DETACH_CONTENT,	0,	0	},
    {	"DETACH_CONTAINER",	OCT_DETACH_CONTAINER,	0,	0	},
    {	NIL(char),		0,			0,	0	},
};

static struct fieldTemplate summaryChgRecTemplate[] = {
    {	"            ",	0,			tPrintBB,	NILGF,	0 },
    {	" ",	OFS(changeRecord.changeType),	tPrintEnum,	NILGF,	chgTypeEnum },
    {	"(",	OFS(changeRecord.objectExternalId), tPrintInteger, NILGF, 0 },
    {	",",	OFS(changeRecord.contentsExternalId), tPrintInteger, NILGF, 0 },
    {	")",		0,			NILPF,		NILGF,	0 },
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static struct fieldTemplate detailedChgRecTemplate[] = {
    {	"\n\nchangeType\t\t\t",
	    OFS(changeRecord.changeType),	tPrintEnum,	tGetEnum, chgTypeEnum},
    {	"\nobjectExternalId\t\t\t",
	    OFS(changeRecord.objectExternalId),	tPrintInteger,	tGetInteger, 0},
    {	"\ncontentsExternalId\t\t\t",
	    OFS(changeRecord.contentsExternalId),tPrintInteger,	tGetInteger, 0},
    {	NIL(char),	0,			NILPF,		NILGF,	0 },
};
static void initChgRecObj(objPtr)
octObject *objPtr;
{
    CREATE_CHECK(objPtr);
    objPtr->contents.changeRecord.changeType = 0;
    objPtr->contents.changeRecord.objectExternalId = 0;
    objPtr->contents.changeRecord.contentsExternalId = 0;
}

static struct enumTemplate summaryEnum[] = {
    {	"undefined",	OCT_UNDEFINED_OBJECT,	summaryUndefinedObjTemplate, 0},
    {	"facet",	OCT_FACET,		summaryFacetTemplate,	0 },
    {	"term",		OCT_TERM,		summaryTermTemplate,	0 },
    {	"net",		OCT_NET,		summaryNetTemplate,	0 },
    {	"instance",	OCT_INSTANCE,		summaryInstanceTemplate, 0 },
    {	"polygon",	OCT_POLYGON,		summaryPolygonTemplate,	0 },
    {	"box",		OCT_BOX,		summaryBoxTemplate,	0 },
    {	"circle",	OCT_CIRCLE,		summaryCircleTemplate,	0 },
    {	"path",		OCT_PATH,		summaryPathTemplate,	0 },
    {	"label",	OCT_LABEL,		summaryLabelTemplate,	0 },
    {	"prop",		OCT_PROP,		summaryPropTemplate,	0 },
    {	"bag",		OCT_BAG,		summaryBagTemplate,	0 },
    {	"layer",	OCT_LAYER,		summaryLayerTemplate,	0 },
    {	"point",	OCT_POINT,		summaryPointTemplate,	0 },
    {	"edge",		OCT_EDGE,		summaryEdgeTemplate,	0 },
    {	"change list",	OCT_CHANGE_LIST,	summaryChgListTemplate,	0 },
    {	"change record",OCT_CHANGE_RECORD,	summaryChgRecTemplate,	0 },
    {	0,		OCT_UNDEFINED_OBJECT,	summaryUndefinedObjTemplate, 0},
};

struct fieldTemplate summaryTemplate[] = {
    {	"",	0,		tPrintXid,	NILGF,		0	},
    {	" ",	TYPE_OFS,	tPrintEnum,	NILGF,		summaryEnum },
    {	0,	0,		NILPF,		NILGF,		0	},
};

struct enumTemplate detailedObjEnum[] = {
    {	"UNDEFINED_OBJECT", OCT_UNDEFINED_OBJECT,
		detailedUndefinedObjTemplate,	initUndefinedObj	},
    {	"FACET",	OCT_FACET,
		detailedFacetTemplate,		initFacetObj		},
    {	"TERM",		OCT_TERM,
		detailedTermTemplate,		initTermObj		},
    {	"NET",		OCT_NET,
		detailedNetTemplate,		initNetObj		},
    {	"INSTANCE",	OCT_INSTANCE,
		detailedInstanceTemplate,	initInstanceObj		},
    {	"POLYGON",	OCT_POLYGON,
		detailedPolygonTemplate,	initPolygonObj		},
    {	"BOX",		OCT_BOX,
		detailedBoxTemplate,		initBoxObj		},
    {	"CIRCLE",	OCT_CIRCLE,
		detailedCircleTemplate,		initCircleObj		},
    {	"PATH",		OCT_PATH,
		detailedPathTemplate,		initPathObj		},
    {	"LABEL",	OCT_LABEL,
		detailedLabelTemplate,		initLabelObj		},
    {	"PROP",		OCT_PROP,
		detailedPropTemplate,		initPropObj		},
    {	"BAG",		OCT_BAG,
		detailedBagTemplate,		initBagObj		},
    {	"LAYER",	OCT_LAYER,
		detailedLayerTemplate,		initLayerObj		},
    {	"POINT",	OCT_POINT,
		detailedPointTemplate,		initPointObj		},
    {	"EDGE",		OCT_EDGE,
		detailedEdgeTemplate,		initEdgeObj		},
    {	"CHANGE_LIST",	OCT_CHANGE_LIST,
		detailedChgListTemplate,	initChgListObj		},
    {	"CHANGE_RECORD",	OCT_CHANGE_RECORD,
		detailedChgRecTemplate,		initChgRecObj		},
    {	0,		OCT_UNDEFINED_OBJECT,
		detailedUndefinedObjTemplate,	initUndefinedObj	},
};

struct fieldTemplate detailedTemplate[] = {
    {	"\n\n\ttype: ", TYPE_OFS, tPrintEnum,	tGetEnum, detailedObjEnum },
    {	0,	0,		NILPF,		NILGF,	0		},
};
