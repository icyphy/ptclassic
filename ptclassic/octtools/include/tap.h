/* Version Identification:
 * @(#)tap.h	1.3	09/22/98
 */
/*
Copyright (c) 1990-1998 The Regents of the University of California.
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
#ifndef TAP_H
#define TAP_H

#include "ansi.h"

extern char tapPkgName[];
#define TAP_PKG_NAME tapPkgName

    /* General Stuff */

#define TAP_TECH_PROP_VAL		"<self>"

OCT_EXTERN char *tapRootDirectory
	ARGS((char *name));
OCT_EXTERN char *tapGetDefaultRoot
	NULLARGS;
OCT_EXTERN char *tapGetDirectory
	ARGS((octObject *obj));
OCT_EXTERN int tapIsLayerDefined
	ARGS((octObject *obj, char *name));


    /* Palette and Primitive Library Stuff */

#define TAP_LAYER_PALETTE_NAME		"layer"
#define TAP_CONNECTOR_PALETTE_NAME	"connector"

OCT_EXTERN void tapListPalettes
	ARGS((octObject *obj, int *Npal, char ***names));
OCT_EXTERN int tapOpenPalette
	ARGS((octObject *obj, char *name, octObject *facet, char *mode));
OCT_EXTERN void tapLayoutPalette
	ARGS((octGenerator *gen, octCoord pad, double aspect));

OCT_EXTERN int tap_quantizeWidth
        ARGS(( int w ));
OCT_EXTERN int tap_quantizeWidthDOWN
        ARGS(( int w ));
OCT_EXTERN double tap_currentDensity
        ARGS(( octObject *layer ));
OCT_EXTERN int tap_cDensityInit
        ARGS(( octObject* facet ));
OCT_EXTERN int tap_getWidthFromPeakCurrent
        ARGS(( double peakCurrent, octObject *layer ));


typedef struct tapLayerListElement {
    octObject layer;	/* layer to connect to */
    octCoord width;	/* width of connecting path */
    int direction;	/* direction(s) from which paths approach (see below) */
    octCoord angle;	/* angle (in deci-degrees) for non-manhattan paths */
} tapLayerListElement;

    /* bit values for tapLayerListElement direction */
#define TAP_RIGHT	(1<<0)	/* path approaches from left */
#define TAP_TOP		(1<<2)	/* path approaches from top */
#define TAP_LEFT	(1<<4)	/* path approaches from right */
#define TAP_BOTTOM	(1<<6)	/* path approaches from bottom */
#define TAP_WEIRD_DIR	(1<<8)	/* path approaches from `angle' direction */
#define TAP_HORIZONTAL	(TAP_LEFT | TAP_RIGHT)
#define TAP_VERTICAL	(TAP_TOP | TAP_BOTTOM)

OCT_EXTERN int tapGetConnector
	ARGS((int Nlyr, tapLayerListElement *lyrs, octObject *inst));
OCT_EXTERN int tapGetStrangeConnector
	ARGS((int Nlyr, tapLayerListElement *lyrs, octObject *inst));

typedef int (*TGC)
	ARGS((int Nlyr, tapLayerListElement *lyrs, octObject *inst));
OCT_EXTERN void tapSetUserConnectorGenerator
	ARGS((TGC));


    /* Property Stuff */

OCT_EXTERN int tapGetProp
	ARGS((octObject *obj, struct octProp *prop));
OCT_EXTERN void tapSetProp
	ARGS((octObject *obj, struct octProp *prop));
OCT_EXTERN int tapGetLyrProp
	ARGS((int Nlyr, tapLayerListElement *lyrs, struct octProp *prop));
OCT_EXTERN void tapSetLyrProp
	ARGS((int Nlyr, tapLayerListElement *lyrs, struct octProp *prop));

#define tapGetPropInt(c, t, n) \
    ((t)->type = OCT_INTEGER,			\
     (t)->name = (n),				\
     tapGetProp((c), (t)))
#define tapGetPropReal(c, t, n) \
    ((t)->type = OCT_REAL,			\
     (t)->name = (n),				\
     tapGetProp((c), (t)))
#define tapGetPropStr(c, t, n) \
    ((t)->type = OCT_STRING,			\
     (t)->name = (n),				\
     tapGetProp((c), (t)))

#define tapGetLyrPropInt(num, lyrs, t, n) \
    ((t)->type = OCT_INTEGER,			\
     (t)->name = (n),				\
     tapGetLyrProp((num), (lyrs), (t)))
#define tapGetLyrPropReal(num, lyrs, t, n) \
    ((t)->type = OCT_REAL,			\
     (t)->name = (n),				\
     tapGetLyrProp((num), (lyrs), (t)))
#define tapGetLyrPropStr(num, lyrs, t, n) \
    ((t)->type = OCT_STRING,			\
     (t)->name = (n),				\
     tapGetLyrProp((num), (lyrs), (t)))
 
typedef struct {
    struct octProp prop;
    int nLayers;
    char *description;
} tapPropDescription;
extern tapPropDescription tapKnownProperties[];

    /* Properties not associated with layers */
#define TAP_LAMBDA		(&tapKnownProperties[0].prop)
#define TAP_COORD_SIZE		(&tapKnownProperties[1].prop)

    /* Properties associated with 1 layer */
#define TAP_LAYER_CLASS		(&tapKnownProperties[2].prop)
#define TAP_MAX_CURRENT_DENSITY	(&tapKnownProperties[3].prop)
#define TAP_SHEET_RESISTANCE	(&tapKnownProperties[4].prop)
#define TAP_CONTACT_RESISTANCE  (&tapKnownProperties[5].prop)
#define TAP_AREA_CAP_TO_GROUND	(&tapKnownProperties[6].prop)
#define TAP_PERIM_CAP_TO_GROUND	(&tapKnownProperties[7].prop)

    /* Properties associated with 2 layers */
#define TAP_AREA_CAP_TO_LAYER	(&tapKnownProperties[8].prop)
#define TAP_PERIM_CAP_TO_LAYER	(&tapKnownProperties[9].prop)

#define TAP_PROP_DESC(prp)	(((tapPropDescription *) ((char *) (prp) - (char *) ((tapPropDescription *) 0)->prop))->description)
#define TAP_PROP_NUM_LAYERS(prp)	(((tapPropDescription *) ((char *) (prp) - (char *) ((tapPropDescription *) 0)->prop))->nLayers)


    /* Design Rule Stuff */

#define TAP_RULE_ERROR		(-1)	/* XXX obsolete XXX */
#define TAP_NO_RULE		(-2)

OCT_EXTERN octCoord tapGetMinWidth
	ARGS((octObject *lyr));
OCT_EXTERN void tapSetMinWidth
	ARGS((octObject *lyr, octCoord width));
OCT_EXTERN octCoord tapGetMaxWidth
	ARGS((octObject *lyr));
OCT_EXTERN octCoord tapGetMinSpacing
	ARGS((octObject *lyr1, octObject *lyr2));
OCT_EXTERN octCoord tapGetMinOverlap
	ARGS((octObject *lyr1, octObject *lyr2));


    /* Display Stuff */

#define TAP_BW			0
#define TAP_COLOR		1

#define TAP_EMPTY		0
#define TAP_STIPPLED		1
#define TAP_SOLID		2

#define TAP_FILL_PATTERN	0
#define TAP_BORDER_PATTERN	1

#define TAP_MAX_INTENSITY	((unsigned short) 0xFFFF)

#define TAP_DEFAULT_COLOR_NAME	"GENERIC-COLOR"
#define TAP_DEFAULT_BW_NAME	"GENERIC-BW"

typedef struct tapStroke {
    octCoord angle;	/* in tenths of degrees */
    int spacing;	/* in multiples of line width */
} tapStroke;

OCT_EXTERN void tapSetDisplayType
	ARGS((char *name, int chrome));
OCT_EXTERN void tapGetDisplayInfo
	ARGS((octObject *lyr, int *prio, int *nCol, int *fill, int *bdr));
OCT_EXTERN void tapGetDisplayColor
	ARGS((octObject *lyr, int index, unsigned short int *r, unsigned short int *g, unsigned short int *b));
OCT_EXTERN void tapGetDisplayPattern
	ARGS((octObject *lyr, int type, int *w, int *h, char **bits));
OCT_EXTERN void tapGetDisplayStrokes
	ARGS((octObject *lyr, int *count, tapStroke **ptr));

OCT_EXTERN void tapSetDisplayInfo
	ARGS((octObject *lyr, int prio, int nCol, int fill, int bdr));
OCT_EXTERN void tapSetDisplayColor
#ifdef _IBMR2
	();
#else
	ARGS((octObject *lyr, int index,  int r,  int g,  int b));
#endif
OCT_EXTERN void tapSetDisplayPattern
	ARGS((octObject *lyr, int type, int w, int h, char *bits));

    /* Not-usually-used Stuff */

OCT_EXTERN octId tapGetFacetIdFromObj
	ARGS((octObject *obj));
OCT_EXTERN void tapFlushCache
	NULLARGS;
OCT_EXTERN void tapOpenTechFacet
	ARGS((char *tech, char *view, octObject *facet, char *mode));

#endif /* TAP_H */
