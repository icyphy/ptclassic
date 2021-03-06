/*******************************************************************
SCCS version identification
$Id$

@Copyright (c) 1994-%Q% The Regents of the University of California.
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

 Programmer: Kennard White

 tkhFont.c :: Tk Helper
********************************************************************/
#ifndef lint
static char SccsId[] = "$Id$";
#endif /* not lint */

#include "topFixup.h"

#include <tk.h>

#include "topStd.h"
#include "topMsg.h"
#include "topMem.h"
#include "topVec.h"
#include "topTcl.h"

#include "tkh.h"


#define TKH_MAXFONTS	500

static Tcl_HashTable _TkhFontListTbl;
static TOPLogical _TkhFontListTblInitB = TOP_FALSE;

static int
_tkhFontSizeCmpCB( const void *a, const void *b) {
    XFontStruct *pFA = *(XFontStruct**)a;
    XFontStruct *pFB = *(XFontStruct**)b;

    return (pFA->ascent + pFA->descent) - (pFB->ascent + pFB->descent);
}

static int
_tkhAddToFontList( Tcl_Interp *ip, TKHFontList *pFL, char *specs) {
    Tk_Window	tkwin = Tk_MainWindow(ip);
    Display	*dpy = Tk_Display(tkwin);
    int		i, j, sC, numFonts;
    char	**sV, **fnlist;
    XFontStruct	*pFS;

    if ( Tcl_SplitList( ip, specs, &sC, &sV) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nBad font list spec ``",specs,"'' (list)",NULL);
	return TCL_ERROR;
    }
    for (i=0; i < sC; i++) {
	fnlist = XListFonts( dpy, sV[i], TKH_MAXFONTS, &numFonts);
	for (j=0; j < numFonts; j++) {
	    if ( (pFS = Tk_GetFontStruct( ip, tkwin, fnlist[j])) == NULL ) {
		Tcl_AppendResult(ip,"\nCouldn't query font ``",fnlist[j],
		  "'' from font list spec ``",specs,"''",NULL);
    		free( (char*) sV);
		return TCL_ERROR;
	    }
	    TOPVEC_Append( &pFL->infos, XFontStruct*) = pFS;
	}
	/*
	 * DONT do this: XFreeFontNames(fnlist);  We have to core leak
	 * the font names because Tk_GetFontStruct() squirles them
	 * away.
	 */
    }
    free( (char*) sV);
    if ( TOPVEC_Num(&pFL->infos) == 0 ) {
	Tcl_AppendResult(ip,"No valid fonts in font list spec ``",
	  specs,"''",NULL);
	return TCL_ERROR;
    }
    topVecSort( &pFL->infos, _tkhFontSizeCmpCB);
    return TCL_OK;
}

TKHFontList*
tkhGetFontList(Tcl_Interp *ip, char *specs) {
    Tcl_HashEntry	*flEnt;
    TKHFontList		*pFL;
    TOPLogical		newB;

    if ( ! _TkhFontListTblInitB ) {
	Tcl_InitHashTable( &_TkhFontListTbl, TCL_STRING_KEYS);
	_TkhFontListTblInitB = TOP_TRUE;
    }
    if ( (flEnt = Tcl_FindHashEntry( &_TkhFontListTbl, specs)) == NULL ) {
	pFL = MEM_ALLOC(TKHFontList);
	pFL->specs = memStrSave(specs);
	pFL->refcnt = 0;
	topPtrVecConstructor( &pFL->infos);
	
	if ( _tkhAddToFontList( ip, pFL, specs) != TCL_OK ) {
	    topPtrVecDestructor( &pFL->infos);
	    MEM_FREE(pFL);
	    return NULL;
	}

	flEnt = Tcl_CreateHashEntry( &_TkhFontListTbl, pFL->specs, &newB);
	Tcl_SetHashValue( flEnt, (ClientData)pFL);
    } else {
	pFL = (TKHFontList*) Tcl_GetHashValue( flEnt);
    }
 
    ++(pFL->refcnt);
    return pFL;
}

void
tkhFreeFontList( TKHFontList *pFL) {
    if ( --(pFL->refcnt) == 0 ) {
	/* XXX: should free fonts */
    }
}

XFontStruct*
tkhGetFontByHeight( TKHFontList *pFL, int height) {
    XFontStruct	*pPrevFS = NULL;

    TOPPTRVEC_LOOP_FORW_BEGIN( &pFL->infos, XFontStruct*, pFS) {
	if ( (pFS->ascent + pFS->descent) >= height ) {
	    return pPrevFS ? pPrevFS : pFS;
	}
	pPrevFS = pFS;
    } TOPPTRVEC_LOOP_FORW_END();
    return pPrevFS;	/* at this point, return anything */
}
