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
/*LINTLIBRARY*/
/*
 * VEM Buffer Management
 * General Buffer Query/Modification Routines
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This file contains general purpose buffer query routines.
 */

#include "bufinternal.h"	/* Internal view of buffer module */

typedef struct trace_struct_defn {
    vemStatus (*func)();	/* Function to call   */
    vemStatus retCode;	/* Its return code    */
    char *data;		/* User supplied data */
} trace_struct;



/*ARGSUSED*/
static enum st_retval inst_trace(key, value, arg)
char *key, *value, *arg;
/*
 * This routine is passed to st_foreach on the instance hash
 * table for a buffer.  It calls the function provided in
 * the structure passed in `arg'.  If it returns VEM_OK,
 * the trace continues.  Otherwise,  it stops the trace.
 */
{
    octObject inst;

    inst.objectId = (octId) key;
    if (octGetById(&inst) < OCT_OK) {
	/* The instance has disappeared - delete from table */
	return ST_DELETE;
    }
    ((trace_struct *) arg)->retCode =
      (*((trace_struct *) arg)->func)
	(&inst, ((trace_struct *) arg)->data);
    if (((trace_struct *) arg)->retCode == VEM_OK) {
	return ST_CONTINUE;
    } else {
	return ST_STOP;
    }
}

vemStatus bufInstances(fctId, func, arg)
octId fctId;			/* Facet of associated buffer */
vemStatus (*func)();		/* Function to call         */
char *arg;			/* User supplied data       */
/*
 * This routine calls `func' once for each instance of `fctId'.
 * This is used when a master is updated to cause updates to
 * all other buffers where the master is instantiated.  If
 * the routine returns anything but VEM_OK,  bufInstances will
 * stop returning the code returned by `func'.  The function
 * should be declared:
 *   vemStatus func(inst, arg)
 *   octObject *inst;
 *   char *arg;
 * Where inst is the instance object and arg is the user
 * supplied data.
 */
{
    trace_struct trace_data;
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	trace_data.func = func;
	trace_data.retCode = VEM_OK;
	trace_data.data = arg;
	if (realBuf->inst_table) {
	    st_foreach(realBuf->inst_table, inst_trace, (char *) &trace_data);
	}
	return trace_data.retCode;
    } else {
	return VEM_CORRUPT;
    }
}


vemStatus bufBinding(fctId, theBinding)
octId fctId;			/* Facet of associated buffer */
bdTable *theBinding;		/* Binding table (return)    */
/*
 * Returns the main binding table associated with a buffer.
 */
{
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	*theBinding = realBuf->mainTable;
	return VEM_OK;
    } else {
	return VEM_CORRUPT;
    }
}




vemStatus bufLock(fctId)
octId fctId;			/* Facet of associated buffer */
/*
 * This routine toggles the lock flag on a previously created
 * buffer.  If the buffer is writable,  it will be marked as
 * unwritable.  If the buffer is not writable,  it will be
 * marked as writable (if the facet itself is writable).
 */
{
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	if (realBuf->bufBits & WRITE_PROTECT) {
	    /* Attempt to turn it off */
	    if (realBuf->bufBits & READ_ONLY) return VEM_ACCESS;
	    realBuf->bufBits &= ~WRITE_PROTECT;
	} else {
	    /* Turn it on */
	    realBuf->bufBits |= WRITE_PROTECT;
	}
	return VEM_OK;
    } else {
	return VEM_CORRUPT;
    }
}



vemStatus bufWritable(fctId)
octId fctId;			/* Facet of associated buffer */
/*
 * This routine returns VEM_OK if the buffer is writable and
 * VEM_FALSE if it is not writable.  Note that bufOpen will
 * attempt to initially open all buffers using the "w" option
 * of octOpenFacet.  Thus,  this is the primary means of
 * enforcing the read-only nature of a buffer.  Note this
 * routine may also return VEM_CORRUPT if the facet is
 * detectably corrupt.
 */
{
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	if (realBuf->bufBits & FACET_CORRUPT) return VEM_CORRUPT;
	if (realBuf->bufBits & WRITE_PROTECT) return VEM_FALSE;
	bufMarkActive( fctId );	/* If the question is asked, then we are about to change this buffer. Mark it as active */
	return VEM_OK;
    } else {
	return VEM_CORRUPT;
    }
}


int bufStyle(fctId)
octId fctId;			/* Facet of associated buffer */
/*
 * This routine returns the editing style of the supplied buffer.
 * If it can't access the buffer or something goes wrong,  the
 * routine returns BUF_NO_TYPE.  Other types are defined in
 * buffer.h.
 */
{
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	return realBuf->bufBits & TYPE_MASK;
    } else {
	return BUF_NO_TYPE;
    }
}


vemStatus bufModified(fctId)
octId fctId;			/* Facet of associated buffer */
/*
 * This routine returns VEM_OK if the buffer has been modified
 * and VEM_FALSE if it has not been modified.  It is not perfect --
 * it does not compensate for selection and other operations that
 * don't modify the buffer permanently.  It may also return VEM_CORRUPT
 * if this is not a VEM buffer.
 */
{
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	if (realBuf->change_count > 0) return VEM_OK;
	else return VEM_FALSE;
    } else {
	return VEM_CORRUPT;
    }
}


vemStatus bufTech(fctId, tech)
octId fctId;			/* Facet to examine           */
tkHandle *tech;			/* Returned technology handle */
/*
 * This routine returns the current technology handle for
 * the specified buffer.
 */
{
    intern_buffer *realBuf;

    if ( (realBuf = _bufFindBuffer(fctId)) ) {
	*tech = realBuf->tech;
	return VEM_OK;
    } else {
	return VEM_CANTFIND;
    }
}
