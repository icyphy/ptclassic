/*******************************************************************
SCCS version identification
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: Kennard White

    toctFacet.c :: Tcl OCT

    Name lookup mechanism for OCT facets.

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <tk.h>

#define OCT_1_NONCOMPAT
#include "oct.h"
#include "oh.h"
#include "errtrap.h"
#include "region.h"

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"
#include "topTcl.h"

#include "toct.h"

/**
    The OCT mechanism for managing and staging facets is not entirely
    suitable.  The primary problem is that we cant lookup an facet that
    is already in memory by name (instead we must open it again).
    Since TCL stuff will sometimes want to refer to facets by name
    rather than ID, we need an extra hash table here to map facet names
    to IDs.  The hash table is indexed by the facet name
    ``cell:view:facet'', and yeilds the octId.

    Note that octOpenMaster() has an efficincy gain in that the internal
    OCT representation of an instance has a direct pointer to its master.
    However, this is typically a pointer to its1G

    Also, there is no need for the hash tables to be entirely consistent,
    since in the worst case we will just do duplicate octOpen() calls,
    and run a little slower.
**/
static TOPLogical _ToctFacetTblInitB = TOP_FALSE;
static Tcl_HashTable _ToctFacetDescTbl;
static TOPTclUid _ToctSchematicUid;
static TOPTclUid _ToctContentsUid;
static TOPTclUid _ToctCurVerUid;

typedef struct _TOCTFacetDesc TOCTFacetDesc;
struct _TOCTFacetDesc {
    octId	pntId;
    char	*cell;
    char	*view;
    char	*facet;
    char	*version;
};

static void
_toctInitFacetTbls() {
    if ( !_ToctFacetTblInitB ) {
	Tcl_InitHashTable( &_ToctFacetDescTbl, sizeof(TOCTFacetDesc)/4);
	_ToctSchematicUid = topTclGetUid("schematic");
	_ToctContentsUid = topTclGetUid("contents");
	_ToctCurVerUid = topTclGetUid(OCT_CURRENT_VERSION);
	_ToctFacetTblInitB = TOP_TRUE;
    }
}

static void
_toctUnpackFacetDesc( char *name, TOCTFacetDesc *pDesc) {
    char	*cell, *view, *facet, *version;

    cell = name;
    if ( (view=strchr(cell,':')) == NULL ) {
	if ( cell[0]!='\0' )	pDesc->cell = topTclGetUid(cell);
    } else {
	if ( cell[0]!=':' ) {
	    *view = '\0';
	    pDesc->cell = topTclGetUid(cell);
	    *view = ':';
	}
	++view;
        if ( (facet=strchr(view,':')) == NULL ) {
	    if ( view[0]!='\0' )	pDesc->view = topTclGetUid(view);
	} else {
	    if ( view[0]!=':' ) {
		*facet = '\0';
		pDesc->view = topTclGetUid(view);
		*facet = ':';
	    }
	    ++facet;
            if ( (version=strchr(facet,':')) == NULL ) {
	        if ( facet[0]!='\0' )	pDesc->facet = topTclGetUid(facet);
	    } else {
		if ( facet[0]!=':' ) {
		    *version = '\0';
		    pDesc->facet = topTclGetUid(facet);
		    *version = ':';
		}
		++version;
		if ( version[0]!='\0' ) pDesc->version = topTclGetUid(version);
	    }
	}
    }
}

static int
_toctGetFacetByDesc( TOCTFacetDesc *pDesc, octObject *pObj) {
    Tcl_HashEntry	*facetHash;
    TOPLogical		newB;
    octObject		pntobj;
    octStatus		sts;

    facetHash = Tcl_CreateHashEntry( &_ToctFacetDescTbl, (char*)pDesc, &newB);
    if ( newB ) {
	pObj->type = OCT_FACET;
	pObj->contents.facet.cell = pDesc->cell;
	pObj->contents.facet.view = pDesc->view;
	pObj->contents.facet.facet = pDesc->facet;
	pObj->contents.facet.version = pDesc->version;
	pObj->contents.facet.mode = "r";
	if ( pDesc->pntId == oct_null_id ) {
	    sts = octOpenFacet(pObj);
	} else {
	    pntobj.objectId = pDesc->pntId;
	    sts = octOpenRelative(&pntobj, pObj, OCT_SIBLING|OCT_CHILD);
	}
	if ( sts < OCT_OK ) {
	    Tcl_DeleteHashEntry(facetHash);
	    return TCL_ERROR;
	}
	Tcl_SetHashValue(facetHash,(ClientData)pObj->objectId);
    } else {
	pObj->objectId = (octId) Tcl_GetHashValue(facetHash);
	OH_FAIL(octGetById(pObj),"facet by id",pObj);
    }
    return TCL_OK;
}

/**
    Retrieve a facet into {pObj}.  {name} be either an OCT tag id,
    or a (partial) facet spec "cell:view:facet:version".
**/
int
toctGetFacetByName( char *name, octObject *pObj) {
    TOCTFacetDesc	fdesc;

    _toctInitFacetTbls();
    if ( name[0]=='%' ) {
	char	*s = name+1;
	_toctCvtStrId( &s, &pObj->objectId);
	if ( octGetById(pObj) != OCT_OK ) {
	    return TCL_ERROR;
	}
	return TCL_OK;
    }
    fdesc.pntId = oct_null_id;
    fdesc.cell = NULL;
    fdesc.view = _ToctSchematicUid;
    fdesc.facet = _ToctContentsUid;
    fdesc.version = _ToctCurVerUid;
    _toctUnpackFacetDesc( name, &fdesc);
    return _toctGetFacetByDesc( &fdesc, pObj);
}

/**
    Get the master facet of {pInst} and return it into {pMaster}.
    The argument {facet}, if non-NULL, can override some of the
    elements of {pInst}.
**/
int
toctGetMasterByInst( octObject *pInst, char *facet, octObject *pMaster) {
    TOCTFacetDesc	fdesc;
    octObject		pntobj;

    _toctInitFacetTbls();
    if ( facet==NULL || facet[0]=='\0' ) {
	pMaster->type = OCT_FACET;
	pMaster->contents.facet.mode = "r";
	if ( octOpenMaster( pInst, pMaster) < OCT_OK )
	    return TCL_ERROR;
	return TCL_OK;
    }
    octGetFacet( pInst, &pntobj);
    fdesc.pntId = pntobj.objectId;
    fdesc.cell = topTclGetUid(pInst->contents.instance.master);
    fdesc.view = topTclGetUid(pInst->contents.instance.view);
    fdesc.facet = topTclGetUid(pInst->contents.instance.facet);
    fdesc.version = topTclGetUid(pInst->contents.instance.version);
    _toctUnpackFacetDesc( facet, &fdesc);
    return _toctGetFacetByDesc( &fdesc, pMaster);
}
