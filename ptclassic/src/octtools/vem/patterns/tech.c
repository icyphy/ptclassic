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
/*LINTLIBRARY*/
/*
 * Technology Maintainence Package
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989
 *
 * This file contains the implementation of a package for
 * managing technology (pattern) information in VEM.  The
 * underlying technology information is obtained from the
 * Technology Access Package (TAP) written by Tom Laidig.
 * This module simply provides an interface to this package
 * which better suits VEM needs.
 */

#include "tech.h"		/* Self declaration        */
#include "st.h"			/* Hash table package      */
#include "attributes.h"		/* Attribute manager       */
#include "message.h"		/* Message subsystem       */
#include "list.h"		/* List handling           */
#include "tap.h"		/* Technology access pack. */
#include "errtrap.h"		/* Error handling          */
#include "defaults.h"		/* Default handling        */

/*
 * Hash table of all technologies keyed on the tap facet id
 */
static st_table *alltechs;

/*
 * Definitions for res_attr
 */
#define DO_ALT	0x01	/* Find alternate    */
#define DO_REPL	0x02	/* Replace attribute */

/*
 * Forward declarations
 */

static atrHandle res_attr();
static atrHandle user_recovery();
static int zeroAlt();
static int zapAlts();
static int res_taplyr();

/* 
 * Internal version of technology information.  tkHandle is
 * a hidden pointer to this structure.
 */

typedef struct tk_intern_defn {
    octId tapFacetId;		/* Oct Id of the technology facet */
    int ref_count;		/* Reference count                */
    st_table *layers;		/* Hash table of layers           */
} tk_intern;

typedef tk_intern tkIntTech;

/*
 * Internal form of the layer information for a given technology.
 * tkLayer is a hidden pointer to this structure (the first
 * field is shared).
 */

#define MAXCOLS	2

#define FILL_STIPPLE	0x01	/* Stipple or no                 */
#define FILL_SOLID	0x02	/* If not stippled, is it solid? */
#define BOR_STIPPLE	0x04	/* Border stipple or no          */
#define BOR_SOLID	0x08	/* Border not stippled           */
#define OFF_ALTERNATE	0x10	/* DON'T attempt to allocate secondaries */

typedef struct tk_geninfo_defn {
    int bits;			/* Status bits for layer         */
    int num_colors;		/* Number of colors (0, 1, or 2) */
    XColor cols[MAXCOLS];	/* Color information itself      */
    flPattern fpat;		/* Pattern itself (if any)       */
    flPattern bpat;		/* Border pattern itself (if any)*/
} tk_geninfo;

typedef struct tk_intlayer_defn {
    int priority;		/* Layer priority              */
    tkIntTech *parentTech;	/* What technology the layer belongs to */
    octId tapLyrId;		/* Layer id in tap facet       */
    STR tapLyrName;		/* Current layer name          */
    tk_geninfo *gen;	/* Info to generate attributes */
    atrHandle mainAtr;		/* Main attribute handle       */
    atrHandle altAtr;		/* Alternate attribute handle  */
} tk_intlayer;

typedef tk_intlayer tkIntLayer;



tkHandle tkGet(theFacet)
octObject *theFacet;		/* Cell to look up technology on */
/*
 * This routine finds the technology associated with the facet
 * `theFacet' and returns an external handle to this technology.
 * If the technology cannot be loaded,  a diagnostic message
 * is produced and a _dummy_ technology is returned.  This dummy
 * technology will always produce something so VEM can draw
 * the cell.  Technically,  the dummy technology is the one
 * with the tap facet id of zero.
 */
{
    tkIntTech *theTech;
    octId tapFacetId;
    STR pkgName, message;
    int code;
    

    if (!alltechs) {
	/* Make hash table */
	alltechs = st_init_table(st_numcmp, st_numhash);
    }
    ERR_IGNORE(tapFacetId = tapGetFacetIdFromObj(theFacet));
    if (errStatus(&pkgName, &code, &message)) {
	tapFacetId = oct_null_id;
	vemMsg(MSG_A, "Error detected in %s:\nCannot find technology for cell %s:%s:%s:\n  %s\n",
	       pkgName,
	       theFacet->contents.facet.cell,
	       theFacet->contents.facet.view,
	       theFacet->contents.facet.facet,
	       message);
    }
    if (!st_lookup(alltechs, (char *) tapFacetId, (char **) &theTech)) {
	/* Make a new record for it and insert it */
	theTech = VEMALLOC(tkIntTech);
	theTech->tapFacetId = tapFacetId;
	theTech->ref_count = 1;
	theTech->layers = st_init_table(strcmp, st_strhash);
	(void) st_insert(alltechs, (char *) tapFacetId, (char *) theTech);
    } else {
	/* Bump up reference count */
	theTech->ref_count += 1;
    }
    return (tkHandle) theTech;
}



/*ARGSUSED*/
static enum st_retval dumpLayer(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * This routine is passed to st_foreach to destroy the
 * members of a technology's layer table before destroying
 * the technology itself.
 */
{
    tkIntLayer *realLayer = (tkIntLayer *) value;
    atrHandle noLayerAttr;

    if (realLayer->gen != (tk_geninfo *) 0) {
	/* Free the generation information */
	if (realLayer->gen->fpat != (flPattern) 0) {
	    flFreePattern(realLayer->gen->fpat);
	}
	if (realLayer->gen->bpat != (flPattern) 0) {
	    flFreePattern(realLayer->gen->bpat);
	}
	VEMFREE(realLayer->gen);
    }
    dfGetAttr("nolayer", &noLayerAttr);
    if ((realLayer->mainAtr != (atrHandle) 0) &&
	(realLayer->mainAtr != noLayerAttr))
      {
	  atrRelease(realLayer->mainAtr);
      }
    if ((realLayer->altAtr != (atrHandle) 0) &&
	(realLayer->altAtr != noLayerAttr))
      {
	  atrRelease(realLayer->altAtr);
      }
    VEMFREE(realLayer);
    return ST_CONTINUE;
}

void tkFree(tech)
tkHandle tech;			/* Technology to free */
/*
 * This routine releases the resources consumed by the
 * technology `tech'.  Note that the package does the
 * right thing if multiple references to the technology
 * have been established using tkGet().
 */
{
    tkIntTech *realTech = (tkIntTech *) tech;

    realTech->ref_count -= 1;
    if (realTech->ref_count <= 0) {
	/* Remove from overall hash table */
	if (st_delete(alltechs, (char **) &(realTech->tapFacetId),
		      (char **) &realTech))
	  {
	      /* Deallocate */
	      st_foreach(realTech->layers, dumpLayer, (char *) 0);
	      st_free_table(realTech->layers);
	      VEMFREE(realTech);
	  }
    }
}



/*ARGSUSED*/
static enum st_retval updateLayer(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * This routine is passed to st_foreach to zero out the information
 * associated with the layers of a technology.  This will force
 * the system to re-read this information from TAP.  It also
 * makes sure the tapLyrId is correct for the item.
 */
{
    tkIntLayer *realLayer = (tkIntLayer *) value;
    octObject *tapFacet = (octObject *) arg;
    atrHandle noLayerAttr;
    octObject tapLayer;

    /* First update the tapLyrId */
    if (tapFacet) {
	tapLayer.type = OCT_LAYER;
	tapLayer.contents.layer.name = realLayer->tapLyrName;
	VEM_OCTCKRVAL(octGetByName(tapFacet, &tapLayer), ST_CONTINUE);
	realLayer->tapLyrId = tapLayer.objectId;
    }
    /* Get rid of the generation info (if any) */
    if (realLayer->gen != (tk_geninfo *) 0) {
	if (realLayer->gen->fpat != (flPattern) 0) {
	    flFreePattern(realLayer->gen->fpat);
	}
	if (realLayer->gen->bpat != (flPattern) 0) {
	    flFreePattern(realLayer->gen->bpat);
	}
	VEMFREE(realLayer->gen);
	realLayer->gen = (tk_geninfo *) 0;
    }
    /* Now replace attributes as appropriate */
    dfGetAttr("nolayer", &noLayerAttr);
    if ((realLayer->mainAtr != (atrHandle) 0) &&
	(realLayer->mainAtr != noLayerAttr))
      {
	  realLayer->mainAtr = res_attr(realLayer, DO_REPL);
      }
    if ((realLayer->altAtr != (atrHandle) 0) &&
	(realLayer->altAtr != (atrHandle) noLayerAttr))
      {
	  realLayer->altAtr = res_attr(realLayer, DO_ALT|DO_REPL);
      }
    return ST_CONTINUE;
}



/*ARGSUSED*/
static enum st_retval flushtk(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * This routine is called by tkWipe to delete cached technology
 * information associated with a tkHandle.
 */
{
    tkIntTech *theTech = (tkIntTech *) value;
    octObject tapFacet;

    tapFacet.objectId = (octId) key;
    VEM_OCTCKRVAL(octGetById(&tapFacet), ST_CONTINUE);
    /* Now update each layer */
    st_foreach(theTech->layers, updateLayer,
	       ((theTech->tapFacetId != oct_null_id) ? (char *) &tapFacet :
		(char *) 0));
    return ST_CONTINUE;
}

void tkWipe()
/*
 * This function deletes all cached technology information without
 * actually asking TAP to reread it's information. 
 */
{
    st_foreach(alltechs, flushtk, (char *) 0);
}


vemStatus tkIsTapFacet(fct)
octObject *fct;			/* Facet to check */
/*
 * This routine returns VEM_OK if the specified facet is
 * a known TAP facet.  This is used by bufRevert() to handle
 * the reversion of tap facets correctly.
 */
{
    tkIntTech *theTech;

    if (st_lookup(alltechs, (char *) fct->objectId, (char **) &theTech)) {
	return VEM_OK;
    } else {
	return VEM_FALSE;
    }
}


tkLayer tkLyr(tech, lyrObj)
tkHandle tech;			/* Technology for layer */
octObject *lyrObj;		/* Layer to look up     */
/*
 * This routine looks up the layer `lyrObj' in the technology
 * `tech' and returns a handle to this layer (which includes
 * the layer priority).  If the layer is not in the technology,
 * an error diagnostic will be produced and a _dummy_ layer
 * will be returned so that VEM can continue anyway.
 */
{
    tkIntTech *realTech = (tkIntTech *) tech;
    tkIntLayer *realLayer;
    octObject theTech, tapLayer;
    
    if (realTech && (realTech->tapFacetId)) {
	/* Valid technology - look up layer */
	if (st_lookup(realTech->layers, lyrObj->contents.layer.name,
		      (char **) &realLayer))
	  {
	      /* Found existing layer -- return it */
	      return (tkLayer) realLayer;
	  } else {
	      /* Make a new layer */
	      realLayer = VEMALLOC(tkIntLayer);
	      realLayer->parentTech = realTech;
	      realLayer->tapLyrId = oct_null_id;
	      realLayer->priority = 0;
	      realLayer->gen = (tk_geninfo *) 0;
	      realLayer->mainAtr = (atrHandle) 0;
	      realLayer->altAtr = (atrHandle) 0;
	      /* Try to find it using TAP */
	      if (!res_taplyr(lyrObj, realLayer)) {
		  octObject tapFacet;

		  /* No layer associated with it */
		  realLayer->priority = VEMMININT;
		  realLayer->tapLyrName = VEMSTRCOPY(lyrObj->contents.layer.name);
		  dfGetAttr("nolayer", &(realLayer->mainAtr));
		  tapFacet.objectId = realTech->tapFacetId;
		  VEM_OCTCKRVAL(octGetById(&tapFacet), (tkLayer) realLayer);
		  vemMsg(MSG_A,
			 "Layer `%s' is not in technology %s:%s:%s:\n  %s\n",
			 lyrObj->contents.layer.name,
			 tapFacet.contents.facet.cell,
			 tapFacet.contents.facet.view,
			 tapFacet.contents.facet.facet,
			 octErrorString());
	      } else {
		  theTech.objectId = realTech->tapFacetId;
		  VEM_OCTCKRVAL(octGetById(&theTech), (tkLayer) realLayer);
		  tapLayer.type = OCT_LAYER;
		  tapLayer.contents.layer.name = lyrObj->contents.layer.name;
		  VEM_OCTCKRVAL(octGetByName(&theTech, &tapLayer),
				(tkLayer) realLayer);
		  realLayer->tapLyrId = tapLayer.objectId;
		  realLayer->tapLyrName = VEMSTRCOPY(tapLayer.contents.layer.name);
	      }
	      VEM_CKRVAL(!st_insert(realTech->layers,
				   VEMSTRCOPY(lyrObj->contents.layer.name),
				   (char *) realLayer),
			 "Duplicate layers",
			 (tkLayer) realLayer);
	      return (tkLayer) realLayer;
	  }
    } else {
	/* Technology is not valid - return a dummy layer */
	if (st_lookup(realTech->layers, lyrObj->contents.layer.name,
		      (char **) &realLayer))
	  {
	      /* Found existing dummy layer -- return it */
	      return (tkLayer) realLayer;
	  } else {
	      /* Make new dummy layer */
	      realLayer = VEMALLOC(tkIntLayer);
	      realLayer->parentTech = realTech;
	      realLayer->tapLyrId = oct_null_id;
	      realLayer->priority = VEMMININT;
	      realLayer->gen = (tk_geninfo *) 0;
	      dfGetAttr("nolayer", &(realLayer->mainAtr));
	      realLayer->altAtr = (atrHandle) 0;
	      VEM_CKRVAL(!st_insert(realTech->layers,
				    VEMSTRCOPY(lyrObj->contents.layer.name),
				    (char *) realLayer),
			 "Duplicate layers",
			 (tkLayer) realLayer);
	      return (tkLayer) realLayer;
	  }
    }
}


static int res_taplyr(lyrObj, realLayer)
octObject *lyrObj;		/* Layer to look up using TAP */
tkIntLayer *realLayer;		/* Where to drop information  */
/*
 * This routine looks up the layer `lyrObj' using TAP and
 * stores the basic information used to generate attributes
 * for the layer.  If the layer is not found,  it returns
 * 0.  Otherwise,  `realLayer' will be filled with a
 * priority and all necessary generation information.
 */
{
    int numcols, fillstat, borstat;
    int idx, width, height, code;
    STR str, pkg, message;

    VEM_OCTCKRVAL(octGetById(lyrObj), 0);
    ERR_IGNORE(tapGetDisplayInfo(lyrObj, &(realLayer->priority), &numcols,
				 &fillstat, &borstat));
    if (!errStatus(&pkg, &code, &message)) {
	/* Layer is ok - fill structure */
	realLayer->gen = VEMALLOC(tk_geninfo);
	realLayer->gen->bits = 0;
	realLayer->gen->num_colors = VEMMIN(numcols, 2);
	for (idx = 0; idx < realLayer->gen->num_colors; idx++) {
	    realLayer->gen->cols[idx].flags = DoRed | DoGreen | DoBlue;
	    tapGetDisplayColor(lyrObj, idx,
			       &(realLayer->gen->cols[idx].red),
			       &(realLayer->gen->cols[idx].green),
			       &(realLayer->gen->cols[idx].blue));
	}
	
	if (fillstat == TAP_STIPPLED) {
	    realLayer->gen->bits |= FILL_STIPPLE;
	    tapGetDisplayPattern(lyrObj, TAP_FILL_PATTERN,
				 &width, &height, &str);
	    realLayer->gen->fpat = flNewPattern(width, height, str);
	} else if (fillstat == TAP_SOLID) {
	    realLayer->gen->bits |= FILL_SOLID;
	    realLayer->gen->fpat = flNewPattern(1, 1, "1");
	} else {
	    realLayer->gen->fpat = (flPattern) 0;
	}

	if (borstat == TAP_STIPPLED) {
	    realLayer->gen->bits |= BOR_STIPPLE;
	    tapGetDisplayPattern(lyrObj, TAP_BORDER_PATTERN,
				 &width, &height, &str);
	    realLayer->gen->bpat = flNewPattern(width, height, str);
	} else if (borstat == TAP_SOLID) {
	    realLayer->gen->bits |= BOR_SOLID;
	    realLayer->gen->bpat = flNewPattern(1, 1, "1");
	} else {
	    realLayer->gen->bpat = (flPattern) 0;
	}
	return 1;
    } else {
	/* No layer info */
	vemMsg(MSG_C, "Can't get info on layer `%s':\n  %s\n",
	       lyrObj->contents.layer.name, message);
	return 0;
    }
}



atrHandle tkAttr(lyr, alt)
tkLayer lyr;			/* Layer generated by tkLyr */
int alt;			/* If set,  get alternates  */
/*
 * This routine returns the actual display attribute handle for
 * a layer.  Normally,  this includes the primary layer information.
 * However,  if `alt' is set,  the alternate layer information
 * will be produced.  If either the technology or the layer where
 * not found in the previous steps,  a dummy attribute is returned
 * so that VEM can continue.  The routine may also invoke a very
 * involved user-driven recovery algorithm if it runs into insufficient 
 * resources.
 */
{
    tkIntLayer *realLayer = (tkIntLayer *) lyr;

    if (alt) {
	if (realLayer->altAtr) {
	    return realLayer->altAtr;
	} else {
	    realLayer->altAtr = res_attr(realLayer, DO_ALT);
	    if (realLayer->altAtr) {
		return realLayer->altAtr;
	    } else {
		return tkAttr(lyr, 0);
	    }
	}
    } else {
	if (realLayer->mainAtr)
	  return realLayer->mainAtr;
	else
	  return (realLayer->mainAtr = res_attr(realLayer, 0));
    }
}



static atrHandle res_attr(realLayer, opt)
tkIntLayer *realLayer;		/* Internal rep. for layer    */
int opt;			/* DO_ALT or DO_REPL          */
/*
 * This function is called to generate an attribute from the
 * generation information stored in the layer information
 * structure.  If necessary,  the routine will read the
 * generation information from TAP.  If DO_ALT is specified,
 * the alternate attribute will be fetched.  If DO_REPL is
 * specified the attribute will be replaced rather than
 * allocated.
 */
{
    atrHandle result;
    vemStatus atrResult;

    if (!realLayer->gen) {
	octObject tapFacet, tapLayer;
	atrHandle noLayer;

	dfGetAttr("nolayer", &noLayer);
	if (realLayer->tapLyrId == oct_null_id) {
	    tapFacet.objectId = realLayer->parentTech->tapFacetId;
	    if (octGetById(&tapFacet) < OCT_OK)
	      return noLayer;
	    tapLayer.type = OCT_LAYER;
	    tapLayer.contents.layer.name = realLayer->tapLyrName;
	    if (octGetByName(&tapFacet, &tapLayer) < OCT_OK)
	      return noLayer;
	    realLayer->tapLyrId = tapLayer.objectId;
	} else {
	    tapLayer.objectId = realLayer->tapLyrId;
	}
	if (!res_taplyr(&tapLayer, realLayer))
	  return noLayer;
    }
    if (opt & DO_ALT) {
	/* Do the alternate - if any and not turned off */
	if ((realLayer->gen->num_colors > 1) &&
	    (!(realLayer->gen->bits & OFF_ALTERNATE)))
	  {
	      if (opt & DO_REPL) {
		  result = realLayer->altAtr;
		  atrResult = atrReplace(result, &(realLayer->gen->cols[1]),
					 realLayer->gen->fpat,
					 realLayer->gen->bpat,
					 1);
	      } else {
		  atrResult = atrNew(&result, &(realLayer->gen->cols[1]),
				     realLayer->gen->fpat, realLayer->gen->bpat,
				     1);
	      }
	      if (atrResult != VEM_OK) {
		  /*
		   * Resource failure - get rid of alternates and return
		   * the zero attribute.
		   */
		  (void) zeroAlt(realLayer->parentTech);
		  return (atrHandle) 0;
	      } else {
		  return result;
	      }
	  } else {
	      return (atrHandle) 0;
	  }
    } else {
	/* Do the primary */
	if (opt & DO_REPL) {
	    result = realLayer->mainAtr;
	    atrResult = atrReplace(result, &(realLayer->gen->cols[0]),
				   realLayer->gen->fpat, realLayer->gen->bpat,
				   1);
	} else {
	    atrResult = atrNew(&result, &(realLayer->gen->cols[0]),
			       realLayer->gen->fpat, realLayer->gen->bpat,
			       1);
	}
	if (atrResult != VEM_OK) {
	    /* Resource failure - try getting rid of alternates */
	    if (!zapAlts()) {
		/* Ask the user for recovery option */
		return user_recovery(realLayer, opt);
	    } else {
		/* Try again */
		return res_attr(realLayer, opt);
	    }
	} else {
	    return result;
	}
    }
}



/*ARGSUSED*/
static enum st_retval zAlt(key, value, arg)
char *key, *value, *arg;
/*
 * This routine is passed to st_foreach on the layer hash
 * table associated with a technology.  It releases any
 * alternate attributes and marks them as unallocatable.
 */
{
    tkIntLayer *realLayer = (tkIntLayer *) value;
    int *count = (int *) arg;

    if (realLayer->altAtr) {
	atrRelease(realLayer->altAtr);
	realLayer->altAtr = (atrHandle) 0;
	*count += 1;
    }
    if (realLayer->gen) {
	realLayer->gen->bits |= OFF_ALTERNATE;
    }
    return ST_CONTINUE;
}

/*ARGSUSED*/
static int zeroAlt(realTech)
tkIntTech *realTech;
/*
 * This routine frees up all of the secondary resources for a
 * technology and permanently marks these resources as 
 * unallocatable.  It returns zero if it didn't actually free
 * anything.
 */
{
    int zerocount;

    zerocount = 0;
    st_foreach(realTech->layers, zAlt, (char *) &zerocount);
    if (zerocount > 0) {
	octObject tapFacet;

	tapFacet.objectId = realTech->tapFacetId;
	VEM_OCTCKRVAL(octGetById(&tapFacet), zerocount);
	vemMsg(MSG_A,
	       "Alternate colors for technology `%s:%s' have been reclaimed\n",
	       tapFacet.contents.facet.cell,
	       tapFacet.contents.facet.view);
    }
    return zerocount;
}




static atrHandle user_recovery(realLayer, opt)
tkIntLayer *realLayer;		/* Internal rep. for layer    */
int opt;			/* Options to res_attr        */
/*
 * This function is called when the user must be consulted
 * about a very serious resource allocation failure.  There
 * are four possible recoverires:
 *  1.  Immediately re-try allocation
 *  2.  Steal from alternate resources
 *  3.  Replace the layer with zero layer
 *  4.  Exit
 */
{
    static char *choices[] = {
	"Try again",
	"Steal resources from alternate colors",
	"Replace the layer with the zero layer",
	"Terminate VEM"
      };
    static int optcount = sizeof(choices)/sizeof(char *);
    octObject tapFacet;
    atrHandle noLayer;

    tapFacet.objectId = realLayer->parentTech->tapFacetId;
    VEM_OCTCKRVAL(octGetById(&tapFacet), (atrHandle) 0);
    (void) sprintf(errMsgArea,
    "Cannot allocate primary resources for layer `%s' of technology %s:%s:%s.\n",
		   realLayer->tapLyrName,
		   tapFacet.contents.facet.cell,
		   tapFacet.contents.facet.view,
		   tapFacet.contents.facet.facet);

    switch (vemMultiFail(errMsgArea, optcount, choices, __FILE__, __LINE__)) {
    case 0:
	/* Retry allocation */
	return res_attr(realLayer, opt);
    case 1:
	/* Alternate resource stealing and try again */
	if (VEM_CHECK(zapAlts(), "No resources to steal from")) {
	    return res_attr(realLayer, opt);
	} else {
	    return user_recovery(realLayer, opt);
	}
    case 2:
	/* Return zero layer information */
	dfGetAttr("nolayer", &noLayer);
	return noLayer;
    case 3:
	/* Terminate the program */
	abort();
    }
    return (atrHandle) 0;
}



/*ARGSUSED*/
static enum st_retval zapOne(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the table of
 * technologies.  It tries to deallocate the alternate
 * resources of the technology passed as `value'.  If
 * it succeeds,  it ends the search.  Otherwise,  it
 * continues.
 */
{
    tkIntTech *realTech = (tkIntTech *) value;
    int *count = (int *) arg;

    if ( (*count = zeroAlt(realTech)) ) {
	return ST_STOP;
    } else {
	return ST_CONTINUE;
    }
}

static int zapAlts()
/*
 * This is a recovery routine which goes through all of the current
 * technologies until it finds one with alternate resources to
 * zap.  It will zap these and return.  Hopefully,  this will
 * free up some resources for the primary allocation.  It returns
 * zero if no secondaries where freed.
 */
{
    int count;

    st_foreach(alltechs, zapOne, (char *) &count);
    return count;
}



vemStatus tkMinWidth(tech, lyrName, minwidth)
tkHandle tech;			/* Technology handle */
STR lyrName;			/* Layer name        */
octCoord *minwidth;		/* Returned width    */
/*
 * This routine returns the minimum width of the layer
 * 'lyrName' in technology 'tech'.  If there is no
 * such layer in the technology,  or there is no
 * minimum width rule,  it will return VEM_FALSE.
 */
{
    tkIntTech *realTech = (tkIntTech *) tech;
    tkIntLayer *realLayer;
    octObject newLayer, tapFacet, tapLayer;

    if (realTech && (realTech->tapFacetId)) {
	/* Valid technology */
	if (st_lookup(realTech->layers, lyrName, (char **) &realLayer)) {
	    /* Found existing layer - see if its real */
	    if (realLayer->tapLyrId != oct_null_id) {
		/* Its an existing layer */
		tapLayer.objectId = realLayer->tapLyrId;
	        VEM_OCTCKRVAL(octGetById(&tapLayer), VEM_CORRUPT);
		if ((*minwidth = tapGetMinWidth(&tapLayer)) != TAP_NO_RULE) {
		    /* Got the width, return it */
		    return VEM_OK;
		} else {
		    return VEM_FALSE;
		}
	    } else {
		return VEM_CANTFIND;
	    }
	} else {
	    /* End around and use tap to find it */
	    tapFacet.objectId = realTech->tapFacetId;
	    VEM_OCTCKRVAL(octGetById(&tapFacet), VEM_CORRUPT);
	    if (tapIsLayerDefined(&tapFacet, lyrName)) {
		newLayer.type = OCT_LAYER;
		newLayer.contents.layer.name = lyrName;
		VEM_OCTCKRVAL(octGetByName(&tapFacet, &newLayer), VEM_CORRUPT);
		*minwidth = tapGetMinWidth(&newLayer);
		if (*minwidth != TAP_NO_RULE) {
		    return VEM_OK;
		} else {
		    return VEM_FALSE;
		}
	    } else {
		return VEM_CANTFIND;
	    }
	}
    } else {
	return VEM_CORRUPT;
    }
}

#ifdef PRT
/*
 * Print functions exported for use in debugging
 */

char *tdir(dir)
int dir;
{
    static char result[200];

    result[0] = '\0';
    if (dir & TAP_RIGHT) (void) strcat(result, "|TAP_RIGHT");
    if (dir & TAP_TOP) (void) strcat(result, "|TAP_TOP");
    if (dir & TAP_LEFT) (void) strcat(result, "|TAP_LEFT");
    if (dir & TAP_BOTTOM) (void) strcat(result, "|TAP_BOTTOM");
    if (dir & TAP_WEIRD_DIR) (void) strcat(result, "|TAP_WEIRD_DIR");
    if (result[0] == '|') return &(result[1]);
    else return &(result[0]);
}

int prt_tl(ptr, tl)
char *ptr;
char *tl;
/* Prints out a tap layer list structure */
{
    tapLayerListElement *elem = (tapLayerListElement *) tl;
    char *start = ptr;

    ptr += prt_str(ptr, "[(tapLayerListElement) layer = ");
    ptr += prt_oct(ptr, &(elem->layer));
    ptr += prt_str(ptr, ", width = ");
    ptr += prt_int(ptr, elem->width);
    ptr += prt_str(ptr, ", direction = ");
    ptr += prt_str(ptr, tdir(elem->direction));
    if (elem->direction & TAP_WEIRD_DIR) {
	ptr += prt_str(ptr, ", angle = ");
	ptr += prt_int(ptr, elem->angle);
    }
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}
#endif
