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
 * Saving Buffers
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * The buffer module provides a layer above the Oct facet to allow
 * VEM to associate additional information with each open facet.
 * This file contains routines that replace octFlushFacet() and
 * octCloseFacet().
 */

#include "bufinternal.h"	/* Internal view of buffer module */
#include "message.h"		/* Message handling               */
#include "vemDM.h"		/* Dialog management              */
#include "tap.h"		/* Technology access package      */
#include "windows.h"		/* Window management              */

/* Forward declarations for lint */
static enum st_retval techFlsFunc();
/*static enum st_retval free_layer();*/
static void preserve();
static vemStatus revert_info();



vemStatus bufSave(fctId)
octId fctId;			/* Object id of facet */
/*
 * This routine flushes the contents of the specified buffer
 * to disk and marks the buffer as unchanged.  If the buffer
 * is read-only,  this routine returns VEM_ACCESS and the buffer
 * is left untouched.
 */
{
    intern_buffer *realBuf;
    octObject bufFacet;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	bufFacet.objectId = fctId;
	VEM_OCTCKRVAL(octGetById(&bufFacet), VEM_CORRUPT);
	if (realBuf->bufBits & WRITE_PROTECT) {
	    vemMsg(MSG_C, "Buffer is read only:\n  cell: %s, view: %s, facet: %s\n",
		   bufFacet.contents.facet.cell,
		   bufFacet.contents.facet.view,
		   bufFacet.contents.facet.facet);
	    return VEM_ACCESS;
	}
	if (octFlushFacet(&bufFacet) != OCT_OK) {
	    vemMsg(MSG_A, "Cannot save facet (cell %s, view %s, facet: %s:\n  %s\n",
		   bufFacet.contents.facet.cell,
		   bufFacet.contents.facet.view,
		   bufFacet.contents.facet.facet,
		   octErrorString());
	    return VEM_CORRUPT;
	}
	realBuf->change_count = 0;
	return VEM_OK;
    } else {
	vemMsg(MSG_A, "Buffer cannot be accessed.\n");
	return VEM_CORRUPT;
    }
}


/* Array of saved items */
#define INITSAVESIZE	16
static int savedSize = 0;
static int savedAlloc = 0;
static dmWhichItem *savedBufs = (dmWhichItem *) 0;


/*ARGSUSED*/
static enum st_retval saveFunc(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * This function is called for each buffer using st_foreach.
 * It is used by bufSaveAll.
 */
{
    intern_buffer *genIntern = (intern_buffer *) value;
    octObject bufFacet;
    int len;

    if (savedSize >= savedAlloc) {
	/* Make the array bigger */
	savedAlloc *= 2;
	savedBufs = VEMREALLOC(dmWhichItem, savedBufs, savedAlloc);
    }
    if ((genIntern->bufBits & WRITE_PROTECT) || (genIntern->change_count == 0)) {
	return ST_CONTINUE;
    }
    bufFacet.objectId = (octId) key;
    if (octGetById(&bufFacet) != OCT_OK) {
	vemMsg(MSG_L, "Unable to get facet in save all:\n  %s\n",
	       octErrorString());
	return ST_CONTINUE;
    }
    /* Construct string */
    len = STRLEN(bufFacet.contents.facet.cell) +
      STRLEN(bufFacet.contents.facet.view) +
	STRLEN(bufFacet.contents.facet.facet) +
	  STRLEN(bufFacet.contents.facet.version) + 4;
    savedBufs[savedSize].itemName = VEMARRAYALLOC(char, len);
    (void) sprintf(savedBufs[savedSize].itemName, "%s:%s:%s;%s",
		   bufFacet.contents.facet.cell,
		   bufFacet.contents.facet.view,
		   bufFacet.contents.facet.facet,
		   bufFacet.contents.facet.version);
    savedBufs[savedSize].userData = (Pointer) key;
    savedSize++;
    return ST_CONTINUE;
}
    


vemStatus bufSaveAll()
/*
 * This routine saves all buffers opened using bufOpen.  The routine
 * uses the dialog interface to bring up a list of all buffers.
 * Only those chosen by the user will be saved.  It does not
 * post the dialog if there are no modified buffers. Return codes:
 *   VEM_OK:       All outstanding buffers were saved.
 *   VEM_NOSELECT: Cancelled the save operation
 *   VEM_FALSE:    Not all outstanding buffers were saved
 */
{
    vemStatus retCode;
    char help_buf[VEMMAXSTR*10];
    int idx, save_count;

    if (_bufTable == (st_table *) 0) {
	_bufTable = st_init_table(st_numcmp, st_numhash);
    }
    if (savedAlloc == 0) {
	savedAlloc = INITSAVESIZE;
	savedBufs = VEMARRAYALLOC(dmWhichItem, savedAlloc);
    }
    savedSize = 0;
    st_foreach(_bufTable, saveFunc, (char *) 0);
    (void) sprintf(help_buf, "%s\n%s\n%s\n%s\n%s",
		   "All modified buffers associated with windows are",
		   "listed as possible choices.  Choose one",
		   "or more buffers by clicking the mouse on",
		   "them.  Those selected will be saved to",
		   "secondary storage.");
    save_count = 0;
    if (savedSize > 0) {
	retCode = dmMultiWhich("Choose buffers to save",
			       savedSize, savedBufs,
			       (int (*)()) 0, help_buf);
	if (retCode == VEM_OK) {
	    for (idx = 0;  idx < savedSize;  idx++) {
		VEMFREE(savedBufs[idx].itemName);
		if ((retCode == VEM_OK) && (savedBufs[idx].flag)) {
		    (void) bufSave((octId) savedBufs[idx].userData);
		    save_count++;
		}
	    }
	} else {
	    return VEM_NOSELECT;
	}
    }
    if (savedSize > save_count) return VEM_FALSE;
    else return VEM_OK;
}


vemStatus bufClose(fctId)
octId fctId;			/* Facet of buffer to close */
/*
 * This routine gets rid of the in-memory representation for
 * the specified buffer and frees all buffer resources associated
 * with it.  Right now,  the routine acts silently.  When change
 * lists are implemented,  the user can be asked before doing
 * this operation.  NOTE:  the windows associated with this buffer
 * are NOT destroyed.  This should be done elsewhere.  NOTE:  this
 * routine may reak havoc on RPC users.
 */
{
    intern_buffer *realBuf;
    octObject theFacet;
    
    if (!(realBuf = _bufFindBuffer(fctId))) {
	vemMsg(MSG_A, "Cannot retrieve buffer from facet.\n");
	return VEM_CORRUPT;
    }
    theFacet.objectId = fctId;
    VEM_OCTCKRVAL(octGetById(&theFacet), VEM_CORRUPT);

    /* First,  get rid of facet by closing it until it can't be closed anymore */
    while ((octGetById(&theFacet) == OCT_OK) && (octFreeFacet(&theFacet) == OCT_OK)) {
	/* null */
    }
    /* Get rid of the layer table */
    if (realBuf->lyr_tbl) _bufFreeLayers(realBuf->lyr_tbl);
    realBuf->lyr_tbl = (st_table *) 0;
    /* Get rid of changes */
#ifndef OLD_DISPLAY
    _bufRCFree(realBuf);
#else
    _bufDelChanges(realBuf->buf_changes);
#endif
    /* Remove the buffer from the list of buffers */
    st_delete(_bufTable, (char **) &fctId, (char **) &realBuf);
    /* Release its technology */
    tkFree(realBuf->tech);
    /* Remove the instance table */
    st_free_table(realBuf->inst_table);
    /* Get rid of the record itself */
    VEMFREE(realBuf);
    return VEM_OK;
}


void multi_warning(fct, init_flag)
octObject *fct;
int init_flag;
/*
 * Produces a message if one hasn't already been seen.
 */
{
    static int out_f = 1;

    if (init_flag) {
	out_f = 1;
    } else if (out_f) {
	vemMsg(MSG_A, "WARNING: Multiple closes required for revert of %s:%s\n",
	       fct->contents.facet.cell,
	       fct->contents.facet.view);
	out_f = 0;
    }
}

vemStatus bufRevert(fctId)
octId *fctId;			/* Facet to revert */
/*
 * This routine casts out the in-memory representation for the
 * specified buffer and re-reads the disk representation for
 * the facet.  The technology and binding tables for the buffer
 * are re-read.  The new facet is written into `fctId'.
 */
{
    intern_buffer *realBuf;
    octObject newFacet, oldFacet;
    int tapFlag, i, perm_flag;
    vemStatus retCode;

    if (!(realBuf = _bufFindBuffer(*fctId))) {
	vemMsg(MSG_A, "Cannot recover buffer from facet.\n");
	return VEM_CORRUPT;
    }
    oldFacet.objectId = *fctId;
    VEM_OCTCKRVAL(octGetById(&oldFacet), VEM_CORRUPT);
    if ( (tapFlag = (tkIsTapFacet(&oldFacet)  == VEM_OK))) {
	/* Must flush tap's cache before re-read XXX - argument? */
	tapFlushCache();
    }
    /* Save the information so we can re-open it */
    preserve(&newFacet, &oldFacet);
    newFacet.contents.facet.mode = "a";
    VEM_OCTCKRVAL(octFreeFacet(&oldFacet), VEM_CORRUPT);
    perm_flag = 0;
    i = 0;
    while (!perm_flag && (i < 2)) {
	/* Reopen the cell with a new id */
	switch (octOpenFacet(&newFacet)) {
	case OCT_OLD_FACET:
	case OCT_NEW_FACET:
	    /* Everything is ok */
	    perm_flag = 1;
	    break;
	case OCT_ALREADY_OPEN:
	    /* 
	     * Horrors!  Its open more than once.  Close immediately and
	     * call recursively.  This may reak havoc on RPC users.
	     */
	    /* multi_warning(&newFacet, 0); */
	    /*
	     * Commented out by Andrea (Jan9 91). Just do it without telling anybody!
	     * vemMsg(MSG_L, "Multiple closes required for revert of %s:%s\n",
	     *   newFacet.contents.facet.cell, newFacet.contents.facet.view);
	     */
	    VEM_OCTCKRVAL(octFreeFacet(&newFacet), VEM_CORRUPT);
	    return bufRevert(fctId);
	case OCT_NO_PERM:
	    /* Will try again with read-only */
	    newFacet.contents.facet.mode = "r";
	    break;
	default:
	    /*
	     * Terrible error:  mark buffer as in a terrible state
	     */
	    vemMsg(MSG_A, "Unable to reopen facet:\n  %s\n",
		   octErrorString());
	    realBuf->bufBits |= FACET_CORRUPT;
	    return VEM_CORRUPT;
	}
    }
    if (!perm_flag) return VEM_ACCESS;
    if ((retCode = revert_info(&newFacet, realBuf, tapFlag, *fctId)) != VEM_OK) {
	return retCode;
    }
    /* Return the new facet id */
    *fctId = newFacet.objectId;
    return VEM_OK;
}

static void preserve(new, old)
octObject *new, *old;		/* New and old facet objects */
/*
 * Saves the fields from `old' into `new' using static character
 * arrays.  The idea is to save the information so that after
 * `old' is closed,  we can still re-open it using `new'.
 */
{
    static char cell[VEMMAXSTR*2];
    static char view[VEMMAXSTR];
    static char facet[VEMMAXSTR];
    static char version[VEMMAXSTR];
    static char mode[VEMMAXSTR];
    char *full_name;

    octFullName(old, &full_name);
    new->type = OCT_FACET;
    new->objectId = oct_null_id;
    new->contents.facet.cell = STRMOVE(cell, full_name);
    new->contents.facet.view = STRMOVE(view, old->contents.facet.view);
    new->contents.facet.facet = STRMOVE(facet, old->contents.facet.facet);
    new->contents.facet.version = STRMOVE(version, old->contents.facet.version);
    new->contents.facet.mode = STRMOVE(mode, old->contents.facet.mode);
    VEMFREE(full_name);
}


static vemStatus revert_info(newFct, realBuf, tapFlag, oldId)
octObject *newFct;		/* New facet            */
intern_buffer *realBuf;		/* Buffer to re-read    */
int tapFlag;			/* Non-zero if tap cell */
octId oldId;			/* Old facet identifier */
/*
 * This routine resets the values in a buffer after re-read.
 */
{
    int storeBits;
    vemStatus retCode;

    /* Get rid of the layer table */
    if (realBuf->lyr_tbl) _bufFreeLayers(realBuf->lyr_tbl);
    realBuf->lyr_tbl = (st_table *) 0;
    realBuf->bufBits |= LYR_UPDATE;
    /* Remove changes */
#ifndef OLD_DISPLAY
    _bufRCFree(realBuf);
    _bufChInit(realBuf);
#else
    _bufDelChanges(realBuf->buf_changes);
    realBuf->buf_changes = oct_null_id;
#endif
    /* If reverting tap facet,  cause all technologies to be wiped */
    if (tapFlag) bufTechFlush();
    /* Re-read the necessary information */
    storeBits = realBuf->bufBits;
    if (realBuf->tech) {
	tkFree(realBuf->tech);
	realBuf->tech = (tkHandle) 0;
    }
    if (STRCOMP(newFct->contents.facet.mode, "a") == 0) {
	retCode = _bufSetValues(newFct, 0, realBuf);
    } else {
	retCode = _bufSetValues(newFct, TRY_READ_ONLY, realBuf);
    }
    if (retCode != VEM_OK) return retCode;
    realBuf->bufBits = storeBits;
    /* Delete the old id from the buffer table */
    st_delete(_bufTable, (char **) &oldId, (char **) &realBuf);
    /* Insert the new id */
    st_insert(_bufTable, (Pointer) newFct->objectId, (Pointer) realBuf);
    multi_warning(newFct, 1);
    /* Change all the facet ids in the window module */
    wnSwitchBuf(oldId, newFct->objectId);
#ifndef OLD_DISPLAY
    /* This may not be necessary -- but just in case */
    _bufExChange(newFct, realBuf);
#endif
    return VEM_OK;
}



/*ARGSUSED*/
static enum st_retval techFlsFunc(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach by bufTechFlush to flush
 * out all technology information associated with a buffer and
 * re-read it from the technology package.
 */
{
    intern_buffer *theBuf = (intern_buffer *) value;
    octObject theFacet;

    /* Destroy technology handle */
    tkFree(theBuf->tech);

    /* Get new information */
    theFacet.objectId = (octId) key;
    VEM_OCTCKRVAL(octGetById(&theFacet), ST_CONTINUE);
    theBuf->tech = tkGet(&theFacet);
    
    /* Force layer updates */
    _bufMrkLayers(theFacet.objectId, theBuf);
    return ST_CONTINUE;
}

void bufTechFlush()
/*
 * This routine goes through all buffers,  frees the technology and
 * layer information,  and gets new technology and layer information.
 * It is called only when a tap facet is re-read.  This is a massive
 * and touchy operation which should occur rarely.
 */
{
    st_foreach(_bufTable, techFlsFunc, (char *) 0);
}



static vemStatus altSave(fctId, realBuf, version)
octId fctId;			/* Oct id for facet to save */
intern_buffer *realBuf;		/* Real buffer to save     */
char *version;			/* What version to save to */
/*
 * This function flushes the contents of the specified buffer
 * to disk under a new version (specified by version).  The
 * saved facet can be recovered to the primary using bufRecover().
 * The routine will return VEM_OK if the save worked as advertised
 * and VEM_CORRUPT if it failed.
 */
{
    octObject bufFacet, saveFacet;

    bufFacet.objectId = fctId;
    if (octGetById(&bufFacet) == OCT_OK) {
	if (realBuf->bufBits & WRITE_PROTECT) {
	    vemMsg(MSG_C, "Cannot %s %s:%s:%s:\n  Buffer is read-only.",
		   version,
		   bufFacet.contents.facet.cell,
		   bufFacet.contents.facet.view,
		   bufFacet.contents.facet.facet);
	    return VEM_CORRUPT;
	}
	saveFacet = bufFacet;
	saveFacet.contents.facet.version = AUTO_SAVE_VERSION;
	if (octWriteFacet(&saveFacet, &bufFacet) != OCT_OK) {
	    vemMsg(MSG_A, "Cannot %s facet %s:%s:%s:\n  %s\n",
		   version,
		   bufFacet.contents.facet.cell,
		   bufFacet.contents.facet.view,
		   bufFacet.contents.facet.facet,
		   octErrorString());
	    return VEM_CORRUPT;
	}
    } else {
	vemMsg(MSG_A, "Cannot %s facet:\n  Facet does not exist\n",
	       version);
	return VEM_CORRUPT;
    }
    return VEM_OK;
}



vemStatus bufAutoSave(fctId)
octId fctId;			/* Object id of facet */
/*
 * This routine flushes `fctId' to disk under the version
 * AUTO_SAVE_VERSION.  The facet contents can be reclaimed
 * using bufRecover().  The routine will return VEM_CORRUPT
 * if there were problems saving the facet and will print
 * out its own error messages.
 */
{
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	return altSave(fctId, realBuf, AUTO_SAVE_VERSION);
    } else {
	vemMsg(MSG_A, "Cannot autosave facet:\n  Facet not in buffer table\n");
	return VEM_CORRUPT;
    }
    /*NOTREACHED*/
}



vemStatus bufRecover(fctId, version)
octId *fctId;			/* Object id of facet      */
char *version;			/* Version to recover from */
/*
 * This routine recovers the contents of an alternate version
 * of a facet and restores it back to the original version
 * (OCT_CURRENT_VERSION).  This is done by flushing the contents
 * of the alternate back into the original cell on disk and
 * then reverting the facet.
 */
{
    intern_buffer *realBuf;
    octObject thisFacet, reFacet, altFacet;
    int tapFlag;
    octId old_id = *fctId;

    if (!(realBuf = _bufFindBuffer(old_id))) {
	vemMsg(MSG_A, "Cannot recover facet:\n  Facet does not exist\n");
	return VEM_CORRUPT;
    }
    thisFacet.objectId = old_id;
    if (octGetById(&thisFacet) == OCT_OK) {
	if ( (tapFlag = (tkIsTapFacet(&thisFacet) == VEM_OK)) ) {
	    tapFlushCache();
	}
	altFacet = thisFacet;
	altFacet.contents.facet.version = version;
	switch (octOpenFacet(&altFacet)) {
	case OCT_OLD_FACET:
	case OCT_ALREADY_OPEN:
	    /* This is what should happen */
	    break;
	case OCT_NEW_FACET:
	case OCT_ERROR:
	case OCT_INCONSISTENT:
	    /* This is what shouldn't happen */
	    vemMsg(MSG_A, "Cannot recover facet:\n  %s\n",
		   octErrorString());
	    (void) octFreeFacet(&altFacet);
	    return VEM_CORRUPT;
	}
	/* Kill the current one */
	preserve(&reFacet, &thisFacet);
	while (octGetById(&thisFacet) == OCT_OK) {
	    (void) octFreeFacet(&thisFacet);
	}
	/* Now write the alternate to the primary */
	if (octWriteFacet(&reFacet, &altFacet) != OCT_OK) {
	    vemMsg(MSG_A, "Cannot write alternate facet:\n  %s\n", octErrorString());
	    vemMsg(MSG_A, "Attempting to restore original.\n");
	}
	(void) octFreeFacet(&altFacet);
	if (octOpenFacet(&reFacet) != OCT_OLD_FACET) {
	    vemMsg(MSG_A, "Cannot reopen facet:\n  %s\n", octErrorString());
	    return VEM_CORRUPT;
	}
	*fctId = reFacet.objectId;
	return revert_info(&reFacet, realBuf, tapFlag, old_id);
    } else {
	vemMsg(MSG_A, "Cannot recover facet:\n  %s\n  Facet doesn't exist\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
}
