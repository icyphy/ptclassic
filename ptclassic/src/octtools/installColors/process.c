/*
 * This is the controlling portion of pat2tap.  It is called by
 * the generic argument parser in pat2tap.c.
 */

#include "copyright.h"
#include "port.h"
#include "oct.h"
#include "tech.h"
#include "tap.h"

#define FPRINTF	(void) fprintf

process(pat_file, tech_spec, root_dir, out_tech, out_view, out_disp, chroma)
char *pat_file;			/* Name of patterns file            */
char *tech_spec;		/* Technology spec in patterns file */
char *root_dir;			/* Tap root directory               */
char *out_tech;			/* Output technology name           */
char *out_view;			/* Output view name                 */
char *out_disp;			/* Output display type              */
int chroma;			/* Chromatism                       */
/*
 * This routine reads in the technology from the specified pattern
 * file and outputs it into tap using `out_tech' and `out_view'
 * to find the proper place for it.
 */
{
    vemTech *theTech;
    octObject tapFacet;
    
    if (loadTech(tech_spec, pat_file, &theTech) == VEM_OK) {
	/* Sucessfully loaded the technology to transfer */
	(void) tapRootDirectory(root_dir);
	tapOpenTechFacet(out_tech, out_view, &tapFacet, "a");
	if (put_str_prop(&tapFacet, "TECHNOLOGY", TAP_TECH_PROP_VAL) != OCT_OK) {
	    FPRINTF(stderr, "Cannot set technology property:\n  %s\n",
		    octErrorString());
	    exit(-1);
	}
	tapSetDisplayType(out_disp, chroma);
	/* Ready for the transfer */
	if (do_transfer(&tapFacet, theTech, chroma)) {
	    FPRINTF(stderr, "Saved results into %s:%s:%s\n",
		    tapFacet.contents.facet.cell,
		    tapFacet.contents.facet.view,
		    tapFacet.contents.facet.facet);
	    (void) octCloseFacet(&tapFacet);
	} else {
	    FPRINTF(stderr, "Errors occurred: results not saved.\n");
	}
    } else {
	FPRINTF(stderr, "Cannot read specification `%s' in `%s'\n",
		tech_spec, pat_file);
	exit(-1);
    }
}



int do_transfer(tap_facet, vem_tech, chroma)
octObject *tap_facet;		/* Technology facet               */
vemTech *vem_tech;		/* Old vem technology description */
int chroma;			/* Chromatism                     */
/*
 * This routine uses tap calls to transfer information from a
 * patterns file into a tap view.  Returns 1 if everything
 * was transferred sucessfully.
 */
{
    lsGen theGen;
    vemTechLayer *lyr;
    octObject tapLayer;
    int ret_code = 1;

    tapLayer.type = OCT_LAYER;
    theGen = lsStart(vem_tech->layerList);
    while (lsNext(theGen, (Pointer *) &lyr, LS_NH) == LS_OK) {
	/* Find this layer in the technology */
	tapLayer.contents.layer.name = lyr->layerName;
	if (octGetOrCreate(tap_facet, &tapLayer) == OCT_OK) {
	    if (!into_layer(&tapLayer, lyr, chroma)) ret_code = 0;
	} else {
	    FPRINTF(stderr, "warning: cannot create layer `%s':\n  %s\n",
		    lyr->layerName, octErrorString());
	}
    }
    return ret_code;
}



int into_layer(tap_lyr, vem_lyr, chroma)
octObject *tap_lyr;		/* Layer in tap facet      */
vemTechLayer *vem_lyr;		/* Layer in vem technology */
int chroma;			/* Chromatism              */
/*
 * This routine transfers the information from a vem technology
 * layer into a tap layer.
 */
{
    int width, height;
    char buf[1024];
    int status;

    status = flSize(vem_lyr->fillData, &width, &height);

    tapSetDisplayInfo(tap_lyr, vem_lyr->priority,
		      ((chroma == TAP_COLOR) ? 1 : 0),
		      ((vem_lyr->fillData != (flPattern) 0) ? 
			  ((width == 1) ? TAP_SOLID : TAP_STIPPLED) :
			  TAP_EMPTY),
		      ((vem_lyr->borderLength > 0) ?
			  ((vem_lyr->borderLength == 1) ? TAP_SOLID : TAP_STIPPLED) :
			  TAP_EMPTY));
    if (chroma == TAP_COLOR) {
	tapSetDisplayColor(tap_lyr, 0, vem_lyr->red*65535/1000,
			   vem_lyr->green*65535/1000,
			   vem_lyr->blue*65535/1000);
    }
    if (status == VEM_OK) {
	flOutput(vem_lyr->fillData, buf);
	tapSetDisplayPattern(tap_lyr, TAP_FILL_PATTERN, width,
			     height, buf);
    }
    if (vem_lyr->borderLength > 1) {
	tapSetDisplayPattern(tap_lyr, TAP_BORDER_PATTERN, vem_lyr->borderLength,
			     1, vem_lyr->borderData);
    }
    tapSetMinWidth(tap_lyr, vem_lyr->width);
    return 1;
}




octStatus put_str_prop(container, name, value)
octObject *container;		/* Object where property is attached */
char *name, *value;		/* Property name and value           */
/*
 * This routine places a string property with name `name' and value
 * `value' onto object `container'.
 */
{
    octObject newProp;
    octStatus ret_code;

    newProp.type = OCT_PROP;
    newProp.contents.prop.name = name;
    newProp.contents.prop.type = OCT_STRING;
    newProp.contents.prop.value.string = value;
    if (octGetByName(container, &newProp) == OCT_OK) {
	newProp.contents.prop.value.string = value;
	if ((ret_code = octModify(&newProp)) != OCT_OK)
	  return ret_code;
    } else {
	if ((ret_code = octCreate(container, &newProp)) != OCT_OK)
	  return ret_code;
    }
    return OCT_OK;
}
