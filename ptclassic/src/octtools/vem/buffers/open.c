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
 * VEM Buffer Management
 * Opening Buffers
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This file contains routines for opening new buffers.  Buffers are
 * essentially a vessel for storing extra information with each open
 * facet.  Instead of opening new facets using octOpenFacet() and 
 * octOpenMaster(),  the routines bufOpen() and bufOpenMaster() are
 * used.  This file contains the implementation for bufOpen() and
 * bufOpenMaster().
 */

#include "bufinternal.h"	/* Internal view of buffer module */
#include "message.h"		/* Message handling               */
#include "vemDM.h"		/* High-level dialog interface    */
#include "defaults.h"		/* Default handling               */
#include "drs.h"		/* Undo stuff                     */

/* Forward declarations for lint */
static intern_buffer *alloc_intern();
static int gen_props();
static STR get_sprop();
static int ask_prop();
static int set_sprop();
static vemStatus find_binding();
static void fill_values();
static int ckStyle();
static void recover_message();



vemStatus bufOpen(newFacet, options)
octObject *newFacet;		/* OCT facet for buffer        */
int options;			/* BUFMUSTEXIST or BUFSILENT   */
/*
 * This routine attempts to open the given facet and establish a
 * new buffer record for the facet.  If successful,  the routine
 * will return VEM_OK.  A technology and binding table will be
 * associated with the facet.  This technology and binding table
 * will be determined using the TECHNOLOGY, VIEWTYPE, and EDITSTYLE
 * properties on the cell.  If these properties are not available,
 * the routine will use the dialog manager to prompt for them.
 * The underlying facet is always opened for read/write unless
 * the option BUFREADONLY is specified.  All destructive editing operations
 * should use bufWritable to check for writability of the facet.
 * Unless the BUFSILENT is specified as an option,  the routine 
 * produces its own diagnostics using vemMessage.  If the option 
 * BUFMUSTEXIST is specified and the cell does not exist,  a new 
 * buffer will not be created.  The option TRY_READ_ONLY is used 
 * internally and should not be specified by outside users.  For 
 * convenience sake,  the facet object will be returned properly filled.
 */
{
    intern_buffer *newIntern, *genIntern;
    vemStatus retCode;
    char *versions[BUF_MAX_ALTERNATES];
    int num;

    /* May not have defined table of buffers yet */
    if (_bufTable == (st_table *) 0) {
	_bufTable = st_init_table(st_numcmp, st_numhash);
    }

    if (options & BUFREADONLY) options |= TRY_READ_ONLY;
    if (options & TRY_READ_ONLY) newFacet->contents.facet.mode = "r";
    else newFacet->contents.facet.mode = "a";
    switch (octOpenFacet(newFacet)) {
    case OCT_NEW_FACET:
	if (options & BUFMUSTEXIST) {
	    octFreeFacet(newFacet);
	    return VEM_CANTFIND;
	}
	if (!(options & BUFSILENT)) {
	    vemMsg(MSG_C, "Buffer does not exist.  New buffer created.\n");
	}
	/* This is supposed to fall through */
    case OCT_OLD_FACET:
	if ( (num = bufCheckRecover(newFacet->objectId, BUF_MAX_ALTERNATES,
				    versions)) ) {
	    recover_message(newFacet, num, versions);
	}
	newIntern = alloc_intern(newFacet);
	/* Handle the properties on the buffer */
	if (!gen_props(newFacet)) {
	    vemMsg(MSG_C, "Can't open cell for lack of properties\n");
	    VEMFREE(newIntern);
	    return VEM_CORRUPT;
	}
	/* Add to table of buffers */
	st_insert(_bufTable, (Pointer) newFacet->objectId, (Pointer) newIntern);
	/* Set fields in new buffer */
	if ((retCode = _bufSetValues(newFacet, options, newIntern)) != VEM_OK) {
	    return retCode;
	}
	break;
    case OCT_ALREADY_OPEN:
	/* Find the buffer in the hash table of buffers */
	genIntern = NIL(intern_buffer);
	st_lookup(_bufTable, (Pointer) newFacet->objectId,(Pointer *) &genIntern);
	if (genIntern != NIL(intern_buffer)) {
	    /* Immediately close the facet so we have only one copy */
	    VEM_OCTCKRVAL(octFreeFacet(newFacet), VEM_CORRUPT);
	    newIntern = genIntern;
	    /* May have a mode bit upgrade */
	    if (STRCOMP(newFacet->contents.facet.mode, "a") == 0) {
		if ((newIntern->bufBits & READ_ONLY) ||
		    (newIntern->bufBits & WRITE_PROTECT)) {
		    newIntern->bufBits &= ~(READ_ONLY|WRITE_PROTECT);
		}
	    }
	    /* Make sure open mode corresponds to generated mode */
	    if (newIntern->bufBits & READ_ONLY) {
		newFacet->contents.facet.mode = "r";
	    }
	} else {
	    /* 
	     * Someone else has opened this cell.  We need to create
	     * all the other information about the cell like technology,
	     * cell type,  etc.
	     */
	    if ( (num = bufCheckRecover(newFacet->objectId, BUF_MAX_ALTERNATES,
					versions)) ) {
		recover_message(newFacet, num, versions);
	    }
	    newIntern = alloc_intern(newFacet);
	    if (!gen_props(newFacet)) {
		vemMsg(MSG_C, "Can't open cell for lack of properties\n");
		VEMFREE(newIntern);
		return VEM_CORRUPT;
	    }
	    st_insert(_bufTable, (Pointer)newFacet->objectId, (Pointer)newIntern);
	    if ((retCode = _bufSetValues(newFacet, options, newIntern)) != VEM_OK) {
		return retCode;
	    }
	}
	break;
    case OCT_NO_PERM:
	/* Try again using the TRY_READ_ONLY flag */
	if (options & TRY_READ_ONLY) {
	    /* Die - permission failure */
	    if (!(options & BUFSILENT)) {
		vemMsg(MSG_A, "Cannot open facet:\n  %s\n", octErrorString());
	    }
	    return VEM_ACCESS;
	} else {
	    return bufOpen(newFacet, options | TRY_READ_ONLY);
	}
    case OCT_INCONSISTENT:
	if (!(options & BUFSILENT)) {
	    vemMsg(MSG_A, "Inconsistent facet `%s:%s'\n",
		   newFacet->contents.facet.cell,
		   newFacet->contents.facet.view );
	    vemMsg(MSG_A, "\
Use the Ptolemy program `masters' \n\
or the Octtools program `inconsistent' \n\
to find the problem\n\
and the program `octmvlib' to fix it.\n\
Then you can try to open the facet again.\n" );
	}
	return VEM_ACCESS;
    default:
	/* Error:  cannot open facet */
	if (!(options & BUFSILENT)) {
	    vemMsg(MSG_A, "Cannot open cell `%s':\n  %s\n",
		   newFacet->contents.facet.cell, octErrorString());
	}
	return VEM_ACCESS;
    }
    return VEM_OK;
}


vemStatus bufOpenMaster(inst, newFacet, options)
octObject *inst;		/* Instance containing spec for master */
octObject *newFacet;		/* Master object                       */
int options;			/* BUFREADONLY                         */
/*
 * This routine opens the master of the specified instance `inst'.  The
 * facet and version are specified in the template `newFacet'.  `newFacet'
 * must have the facet field filled in.  Other fields will be filled
 * by this routine.  If successful,  the routine will return the newly 
 * created buffer in `newBuffer',  `newFacet' will be filled properly,  and the
 * routine will return VEM_OK.  This routine also takes this opportunity
 * to update the list of instances for the given master.  This is
 * used to update the screen appropriately when a master is updated.
 * Note: the master must exist or it is an error.  If options has
 * the BUFREADONLY bit set,  the master will be opened for reading
 * only. 
 */
{
    intern_buffer *newIntern=(intern_buffer*)NULL, *genIntern;
    vemStatus retCode;
    int i, found_flag;

    /* May not have defined table of buffers yet */
    if (_bufTable == (st_table *) 0) {
	_bufTable = st_init_table(st_numcmp, st_numhash);
    }

    found_flag = 0;
    i = 0;
    while (!found_flag && (i < 2)) {
	/* Fill in facet fields */
	newFacet->type = OCT_FACET;
	if ((i == 0) && !(options & BUFREADONLY)) {
	    newFacet->contents.facet.mode = "a";
	} else {
	    newFacet->contents.facet.mode = "r";
	}
	newFacet->contents.facet.version = OCT_CURRENT_VERSION;
	switch (octOpenMaster(inst, newFacet)) {
	case OCT_NEW_FACET:
	    return VEM_CANTFIND;
	case OCT_OLD_FACET:
	    newIntern = alloc_intern(newFacet);
	    /* Handle the properties */
	    if (!gen_props(newFacet)) {
		vemMsg(MSG_C, "Can't open subcell for lack of properties\n");
		VEMFREE(newIntern);
		return VEM_CORRUPT;
	    }
	    /* Add to table of buffers */
	    st_insert(_bufTable, (Pointer) newFacet->objectId, (Pointer) newIntern);
	    /* Initialize the new buffer */
	    if ((retCode = _bufSetValues(newFacet,
		       (STRCOMP(newFacet->contents.facet.mode, "r") == 0 ?
			TRY_READ_ONLY : 0),
		       newIntern)) != VEM_OK) {
		return retCode;
	    }
	    found_flag = 1;
	    break;
	case OCT_ALREADY_OPEN:
	    /* Find the buffer in the list of buffers */
	    genIntern = NIL(intern_buffer);
	    st_lookup(_bufTable, (Pointer) newFacet->objectId,
		      (Pointer *) &genIntern);
	    if (genIntern != NIL(intern_buffer)) {
		/* Immediately close the facet so we have only one copy */
		VEM_OCTCKRVAL(octFreeFacet(newFacet), VEM_CORRUPT);
		newIntern = genIntern;
		/* May have a mode bit upgrade */
		if (STRCOMP(newFacet->contents.facet.mode, "a") == 0) {
		    if ((newIntern->bufBits & READ_ONLY) ||
			(newIntern->bufBits & WRITE_PROTECT)) {
			newIntern->bufBits &= ~(READ_ONLY|WRITE_PROTECT);
		    }
		}
		/* Make sure mode is set properly */
		if (newIntern->bufBits & READ_ONLY) {
		    newFacet->contents.facet.mode = "r";
		}
	    } else {
		/* 
		 * Someone else has opened this cell.  We need to create
		 * all the other information about the cell like technology,
		 * cell type,  etc.
		 */
		newIntern = alloc_intern(newFacet);
		/* Handle properties */
		if (!gen_props(newFacet)) {
		    vemMsg(MSG_C, "Can't open subcell for lack of properties\n");
		    VEMFREE(newIntern);
		    return VEM_CORRUPT;
		}
		st_insert(_bufTable, (Pointer)newFacet->objectId, (Pointer)newIntern);
		if ((retCode=_bufSetValues(newFacet,
		       (STRCOMP(newFacet->contents.facet.mode, "r") == 0 ?
			TRY_READ_ONLY : 0),
		       newIntern)) != VEM_OK) {
		    return retCode;
		}
	    }
	    found_flag = 1;
	    break;
	case OCT_NO_PERM:
	    /* This will cause it to try again read only */
	    break;
	default:
	    return VEM_CORRUPT;
	}
	i++;
    }
    if (found_flag) {
	/* Add instance to instance table for buffer */
	(void) st_insert(newIntern->inst_table, (Pointer) inst->objectId,
			 (Pointer) 0);
	return VEM_OK;
    } else {
	return VEM_CANTFIND;
    }
}


/*
 * Routines to help bufOpen and bufOpenMaster
 */

static intern_buffer *alloc_intern(fct)
octObject *fct;			/* Associated facet */
/*
 * Returns a newly allocated intern_buffer structure
 * with all of the fields set to default values.
 */
{
    intern_buffer *nb;

    nb = VEMALLOC(intern_buffer);
    nb->mainTable = (bdTable) 0;
    nb->tech = (tkHandle) 0;
    nb->lyr_tbl = (st_table *) 0;
    nb->hi_lyr = oct_null_id;
    nb->bufBits = 0;
    nb->inst_table = (st_table *) 0;
#ifndef OLD_DISPLAY
    _bufChInit(nb);
#else
    nb->buf_changes = _bufNewChanges(fct);
#endif
    nb->change_count = 0;
    drsInitChangeList(fct);
    return nb;
}



static int gen_props(newFacet)
octObject *newFacet;		/* Newly generated facet */
/*
 * This routine handles the disposition of properties on
 * a newly opened facet.  Three properties are affected:
 * TECHNOLOGY, VIEWTYPE, and EDITSTYLE.  If these properties
 * do not exist,  the user will be queried for them.  The
 * routine returns zero if it can't resolve the properties.
 */
{
    STR tk, vt, es;

    tk = get_sprop(newFacet, "TECHNOLOGY");
    vt = get_sprop(newFacet, "VIEWTYPE");
    es = get_sprop(newFacet, "EDITSTYLE");
    if (!tk || !vt || !es) {
	/* Ask the user for them */
	return ask_prop(newFacet, tk, vt, es);
    } else return 1;
}

static STR get_sprop(newFacet, propname)
octObject *newFacet;		/* Newly opened facet */
STR propname;			/* Name of property   */
/*
 * Returns the value of the string property named `propname' which
 * is attached to `newFacet'.  If no such property exists,  it
 * returns the zero string, (STR) 0.
 */
{
    octObject theProp;

    theProp.type = OCT_PROP;
    theProp.contents.prop.name = propname;
    if (octGetByName(newFacet, &theProp) < OCT_OK) {
	return (STR) 0;
    } else {
	return (STR) theProp.contents.prop.value.string;
    }
}



/* Acceptable values for the EDITSTYLE property */

#define ES_MAX	3
static char *es_strs[ES_MAX] = {
    "PHYSICAL", "SYMBOLIC", "SCHEMATIC"
  };

static int ask_prop(newFacet, tk, vt, es)
octObject *newFacet;		/* Newly opened facet */
STR tk;				/* Technology name    */
STR vt;				/* Viewtype           */
STR es;				/* Editing style      */
/*
 * This routine asks the user for the key property values
 * for the properties TECHNOLOGY, VIEWTYPE, and EDITSTYLE.
 * The default values for these properties are given in
 * `tk', `vt', and `es' respectively.  If no default value
 * is given,  an appropriate one is chosen.  Returns zero
 * if the property values could not be resolved.
 */
{
    dmTextItem items[4];
    char outstr[2*VEMMAXSTR], cellname[2*VEMMAXSTR];

    /* Handle default values */
    fill_values(newFacet, &tk, &vt, &es);

    /* Set up dialog */
    items[0].itemPrompt = "Technology";
    items[0].rows = 1;    items[0].cols = 15;
    items[0].value = tk;  items[0].userData = (Pointer) 0;

    items[1].itemPrompt = "View Type";
    items[1].rows = 1;    items[1].cols = 15;
    items[1].value = vt;  items[1].userData = (Pointer) 0;

    items[2].itemPrompt = "Edit Style";
    items[2].rows = 1;    items[2].cols = 15;
    items[2].value = es;  items[2].userData = (Pointer) 0;

    if (STRCOMP(newFacet->contents.facet.facet, "contents") == 0) {
	(void) sprintf(cellname, "%s:%s",
		       newFacet->contents.facet.cell,
		       newFacet->contents.facet.view);
    } else {
	(void) sprintf(cellname, "%s:%s:%s",
		       newFacet->contents.facet.cell,
		       newFacet->contents.facet.view,
		       newFacet->contents.facet.facet);
    }
    (void) sprintf(outstr, "Missing properties for `%s'", cellname);
    if (dmMultiText(outstr, 3, items) == VEM_OK) {
	return
	  ckStyle(newFacet, items[0].value, items[1].value, items[2].value) &&
	    set_sprop(newFacet, "TECHNOLOGY", items[0].value) &&
	      set_sprop(newFacet, "VIEWTYPE", items[1].value) &&
		set_sprop(newFacet, "EDITSTYLE", items[2].value);
    } else {
	return 0;
    }
}


static void fill_values(fct, tk, vt, es)
octObject *fct;			/* Facet          */
STR *tk, *vt, *es;		/* Default values */
/*
 * Determines reasonable defaults for TECHNOLOGY, VIEWTYPE,
 * and EDITSTYLE properties if they do not already exist
 * on the facet.  For TECHNOLOGY, this is "scmos".  For
 * VIEWTYPE,  the default is the view name of the facet,  
 * and the EDITSTYLE it is the same as VIEWTYPE unless VIEWTYPE is not
 * an acceptable value for EDITSTYLE,  in which case it
 * defaults to "PHYSICAL".
 * If the view is schematic, then the default technology is also 
 * schematic.
 */
{
    int i;

    if (!(*vt)) {
	*vt = VEMSTRCOPY(fct->contents.facet.view);
	STRUPCASE(*vt);
	for (i = 0;  i < ES_MAX;  i++) {
	    if (STRCOMP(*vt, es_strs[i]) == 0) break;
	}
	if (i >= ES_MAX) {
	    dfGetString("viewtype", vt);
	    *vt = STRUPCASE(VEMSTRCOPY(*vt));
	}
	if ( STRCOMP( fct->contents.facet.view, "schematic" ) == 0 ) {
	    if (!(*tk)) {
		*tk = "schematic";
	    }
	}

    }
    if (!(*es)) {
	dfGetString("editstyle", es);
	if (STRCOMP(*es, "NONE") == 0) {
	    *es = *vt;
	    for (i = 0;  i < ES_MAX;  i++) {
		if (STRCOMP(*es, es_strs[i]) == 0) break;
	    }
	    if (i >= ES_MAX) {
		*es = "SYMBOLIC";
	    }
	}
    }
    if (!(*tk)) {
	dfGetString("deftechname", tk);
    }

}



static int ckStyle(newFacet, tk, vt, es)
octObject *newFacet;		/* Newly created facet */
STR tk, vt, es;			/* Basic prop values   */
/*
 * This routine checks the edit style value against the
 * allowable edit styles and complains (using a dmConfirm
 * dialog) if it is not right.  It then recursively
 * asks the user to try again.
 */
{
    int i;
    char explanation[VEMMAXSTR];

    for (i = 0;  i < ES_MAX;  i++) {
	if (STRCOMP(es, es_strs[i]) == 0) break;
    }
    if (i >= ES_MAX) {
	(void) STRMOVE(explanation, "The EDITSTYLE property must have\n");
	(void) STRCONCAT(explanation, "one of the following values:\n");
	(void) STRCONCAT(explanation, "`PHYSICAL', `SYMBOLIC', `SCHEMATIC'\n");
	(void) dmConfirm("Illegal Value", explanation, "Continue", (STR) 0);
	/* Give him another chance */
	return ask_prop(newFacet, tk, vt, es);
    } else return 1;
}


static int set_sprop(newFacet, name, value)
octObject *newFacet;		/* Newly opened facet      */
STR name, value;		/* Property name and value */
/*
 * This routine sets the value of the property `name' to `value'.
 */
{
    octObject theProp;

    theProp.type = OCT_PROP;
    theProp.contents.prop.name = name;
    theProp.contents.prop.type = OCT_STRING;
    theProp.contents.prop.value.string = value;
    if (octCreateOrModify(newFacet, &theProp) < OCT_OK) {
	return 0;
    } else {
	return 1;
    }
}



vemStatus _bufSetValues(newFacet, lockFlag, newIntern)
octObject *newFacet;		  /* Newly opened facet                 */
int lockFlag;			  /* TRY_READ_ONLY or nothing           */
intern_buffer *newIntern;  /* Filled in internal buffer struct   */
/*
 * This routine finds the binding table for a newly opened facet and 
 * constructs an internal buffer structure containing this information
 * along with other information.  The buffer record should be 
 * preallocated and the buffer should be added to the list of buffers 
 * if this is the first creation.  Note that technology information
 * is now handled by the TAP package.
 */
{
    bdTable newBinding;
    STR tableName;
    int buf_style;
    vemStatus retCode;

    /* Find the binding table for the buffer */
    buf_style = 0;
    retCode = find_binding(newFacet, &newBinding, &tableName, &buf_style);
    if (retCode != VEM_OK) return retCode;

    /*
     * Fill other fields of the buffer.
     */
    newIntern->mainTable = newBinding;

    /*
     * Either read or update information about technology
     */
    if (!newIntern->tech) {
	newIntern->tech = tkGet(newFacet);
    }

    newIntern->bufBits = 0;
    if (lockFlag & TRY_READ_ONLY) {
	newIntern->bufBits |= (READ_ONLY|WRITE_PROTECT);
    }
    newIntern->bufBits |= buf_style;
    _bufMrkLayers(newFacet->objectId, newIntern);

    /*
     * Create instance table (if required)
     */
    if (!newIntern->inst_table) {
	newIntern->inst_table = st_init_table(st_numcmp, st_numhash);
    }

#ifndef OLD_DISPLAY
    /*
     * This may not be necessary - but just to be safe:
     */
    _bufExChange(newFacet, newIntern);
#endif
    return VEM_OK;
}



static vemStatus find_binding(newFacet, newBinding, tableName, buftype)
octObject *newFacet;		/* Newly opened facet                 */
bdTable *newBinding;		/* Binding table for facet (returned) */
STR *tableName;			/* Name of bindings table (returned)  */
int *buftype;			/* Type of buffer (returned)          */
/*
 * This routine locates and returns the binding table for a newly
 * opened facet.  The binding table has the same name as the
 * facet type.  There are three steps to finding the type of
 * a facet:
 *   1.  If there is a property named "EDITSTYLE" (value string)
 *       it will be used to set the type to "symbolic", "physical",
 *       or "schematic".
 *   2.  If there is no viewtype property,  the view name will
 *       be interpreted as the facet type.
 *   3.  If the lookup of the view name fails,  the default
 *       type "physical" will be used.
 */
{
    octObject facetProperty;
    char *viewValue;

    facetProperty.type = OCT_PROP;
    facetProperty.objectId = oct_null_id;
    facetProperty.contents.prop.name = "EDITSTYLE";
    *buftype = BUF_NO_TYPE;
    if ((octGetByName(newFacet, &facetProperty) == OCT_OK) &&
	(facetProperty.contents.prop.type == OCT_STRING))
      {
	  viewValue = facetProperty.contents.prop.value.string;
	  if (STRCLCOMP(viewValue, "SYMBOLIC") == 0) {
	      *tableName = "symbolic";
	      *buftype = BUF_SYMBOLIC;
	  } else if (STRCLCOMP(viewValue, "PHYSICAL") == 0) {
	      *tableName = "physical";
	      *buftype = BUF_PHYSICAL;
	  } else if (STRCLCOMP(viewValue, "SCHEMATIC") == 0) {
	      *tableName = "schematic";
	      *buftype = BUF_SCHEMATIC;
	  } else {
	      vemMsg(MSG_A, "The editstyle `%s' is unknown.\n", viewValue);
	      return VEM_NOIMPL;
	  }
	  if (bdGetTable(*tableName, newBinding) != VEM_OK) {
	      vemMsg(MSG_A, "Views of type `%s' don't seem to work.\n", *tableName);
	      return VEM_NOIMPL;
	  }
      } else {
	  /* No property - check the view type */
	  if (bdGetTable(newFacet->contents.facet.view, newBinding)
	      != VEM_OK)
	    {
		/* No valid view type - assume physical */
		*tableName = "physical";
		*buftype = BUF_PHYSICAL;
		if (bdGetTable("physical", newBinding) != VEM_OK) {
		    vemMsg(MSG_A, "I don't know how to edit `%s' views.\n",
			   newFacet->contents.facet.view);
		    return VEM_NOIMPL;
		}
	    }
	  else {
	      *tableName = newFacet->contents.facet.view;
	      if (bdGetTable(*tableName, newBinding) != VEM_OK) {
		  vemMsg(MSG_A, "Cannot load bindings for view `%s'\n",
			 *tableName);
		  return VEM_NOIMPL;
	      } else {
		  if (STRCOMP(*tableName, "physical") == 0)
		    *buftype = BUF_PHYSICAL;
		  else if (STRCOMP(*tableName, "symbolic") == 0)
		    *buftype = BUF_SYMBOLIC;
		  else if (STRCOMP(*tableName, "schematic") == 0)
		    *buftype = BUF_SCHEMATIC;
	      }
	  }
      }
    return VEM_OK;
}




#define MAX_MESSAGE 2048

int bufCheckRecover(fct_id, ver_len, versions)
octId fct_id;			/* Facet to check 	    */
int ver_len;			/* Version length           */
char *versions[];		/* Version names (returned) */
/*
 * This routine checks to see if there are more recent versions
 * of `fct_id' under alternate versions.  Currently,  the
 * versions checked are AUTO_SAVE_VERSION and CRASH_SAVE_VERSION.
 * The constant BUF_MAX_ALTERNATES can be used to generate arrays
 * large enough for worst case results.
 * The routine returns the number of more recent versions found.
 * The first `ver_len' of these are written into the user supplied
 * array `versions' (which should be at least `ver_len' in length).
 * The strings in this array are static and should be copied if
 * the caller intends to modify them.
 */
{
    octObject fct;
    struct octFacetInfo info;
    int stamp, num;

    num = 0;
    fct.objectId = fct_id;
    if ((octGetById(&fct) == OCT_OK) &&
	(octGetFacetInfo(&fct, &info) == OCT_OK)) {
	fct.contents.facet.version = CRASH_SAVE_VERSION;
	stamp = info.timeStamp;
	VEMFREE(info.fullName);
	if (octGetFacetInfo(&fct, &info) == OCT_OK) {
	    /* There is a crash facet */
	    if (stamp < info.timeStamp) {
		if (num < ver_len) versions[num] = fct.contents.facet.version;
		num++;
	    }
	    VEMFREE(info.fullName);
	}
	fct.contents.facet.version = AUTO_SAVE_VERSION;
	if (octGetFacetInfo(&fct, &info) == OCT_OK) {
	    /* There is an auto-save facet */
	    if (stamp < info.timeStamp) {
		if (num < ver_len) versions[num] = fct.contents.facet.version;
		num++;
	    }
	    VEMFREE(info.fullName);
	}
    }
    return num;
}

static void recover_message(fct, ver_len, versions)
octObject *fct;			/* Facet to check               */
int ver_len;			/* Number of alternate versions */
char *versions[];		/* List of possible versions    */
/*
 * This routine builds a message that is displayed to the user
 * when there are more recent alternate versions of a cell.  The
 * message contains instructions for recovering the contents
 * of these alternate versions.
 */
{
    char version_list[MAX_MESSAGE], crash_message[MAX_MESSAGE];
    char plural_opening[MAX_MESSAGE], buf_ident[MAX_MESSAGE];
    char *ending;
    int i;

    (void) sprintf(buf_ident, "%s:%s:%s",
		   fct->contents.facet.cell,
		   fct->contents.facet.view,
		   fct->contents.facet.facet);
    if (ver_len > 1) {
	/* plural */
	(void) sprintf(plural_opening,
"There are versions of the cell %s\n\
that were saved automatically during your last vem session.  These versions\n\
are more recent than the current version of the cell.  It may be possible to\n\
recover some lost work by replacing the contents of the current cell with\n\
the contents of one of the automatically saved versions.  You can examine\n\
the alternate versions by placing the cursor in the original cell and opening\n\
:::<version> where <version> is one of the following:",
		       buf_ident);
		       
	version_list[0] = '\0';
	for (i = 0;  i < ver_len;  i++) {
	    (void) STRCONCAT(version_list, "\t");
	    (void) STRCONCAT(version_list, versions[i]);
	    (void) STRCONCAT(version_list, "\n");
	}
	ending =
"After examining the alternate cells, you can replace the contents of the\n\
original cell with one of the alternate cells using the recover-facet command.";
	(void) sprintf(crash_message, "%s\n%s%s",
		       plural_opening, version_list, ending);
    } else {
	/* singular */
	(void) sprintf(crash_message,
"There is a version of the cell %s\n\
that was saved automatically during your last vem session. \n\
This version is more recent than the current version of the cell.\n\
It may be possible to recover some lost work by replacing the contents\n\
of the current cell with the contents of this automatically saved\n\
version.\n\
You can examine the alternate version by placing the cursor\n\
in the original cell and opening :::<version> where <version>\n\
is `%s'.\n\
\n\
After examining the alternate cell, you can replace the contents of the\n\
original cell with the alternate cell using the recover-facet command.\n",
		       buf_ident, versions[0]);
    }
    (void) dmConfirm("Crash Recovery", crash_message,
		     "Dismiss", (STR) 0);
}

/*
 * Make _bufFindBuffer externally visible so that rpc/serverVem.c
 * can call it.  Note that we do not return a value.
 * The real fix would be to check out serverVem.c and see if this call
 * is really necessary, but that would be non-trivial
 */
void bufFindBuffer(octId id)
{
  _bufFindBuffer(id);
}
