/* 
    toct.c :: Tcl OCT

    This file contains helper functions for dealing with OCT.  In particular,
    funcs for looking up OCT objects by name (esp. facets), and dealing
    with string forms of octIds.

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
#endif

#include "topFixup.h"
#include <tk.h>

#include "oct.h"
#include "oh.h"
#include "errtrap.h"
#include "region.h"

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"
#include "topTcl.h"

#include "toct.h"

void
toctBoxUnion2( octBox *pSrc, octBox *pDst) {
    if ( pDst->lowerLeft.x == pDst->upperRight.x ) {
	*pDst = *pSrc;
	return;
    }
    TOCT_SetMin(pSrc->lowerLeft.x,pDst->lowerLeft.x);
    TOCT_SetMin(pSrc->lowerLeft.y,pDst->lowerLeft.y);
    TOCT_SetMax(pSrc->upperRight.x,pDst->upperRight.x);
    TOCT_SetMax(pSrc->upperRight.y,pDst->upperRight.y);
}

/**
    Convert the string stored at {*idstr} to an octId and store the result
    in {pId}.  {*idstr} is updated to point to just after the id string.
**/
int
_toctCvtStrId( char **idstr, octId *pId) {
    int		idnum = 0, c;
    char	*s;

    for ( s=*idstr; 1; s++) {
	c = *s - '0';
	if ( c < 0 || c > 7 )
	    break;
        idnum = (idnum << 3) + c;
    }
    if ( idnum == 0 )
	return TCL_ERROR;
    *idstr = s;
    *pId = (octId) idnum;
    return TCL_OK;
}

/**
    Convert the octId {*pId} into a string stored at {*idstr}.  {*idstr} is
    updated to point to just after the id string. 
**/
int
_toctCvtOctId( octId *pId, char **idstr) {
    int		idnum_in = (int) *pId, idnum, c, len;
    char	*s;

    for ( idnum=idnum_in, len=0; idnum != 0; len++) {
	idnum >>= 3;
    }
    s = *idstr + len;
    *idstr = s;
    *s-- = '\0';
    for ( idnum=idnum_in; len != 0; len--) {
	*s-- = (idnum & 0x7) + '0';
	idnum >>= 3;
    }
    return TCL_OK;
}

/**
    Scan an OCT name within the context of toctGetByTag().  Skip leading
    spaces, and terminate on '\0' or '>'.  We use backslash as
    an escape character.  It is illegal to escape a '\0'.
**/
int
_toctScanOctName(char **idstr, char *buf, int buflen_in) {
    int		c, buflen = buflen_in;
    char	*s = *idstr, *d = buf;

    while ( *s == ' ' ) s++;
    for (;;) {
        if ( (c=*s)=='\0' || c=='>' )	break;
	s++;
	if ( c == '\\' ) {
	    if ( (c=*s)=='\0' )		return TCL_ERROR;
	    s++;
	}
	if ( --buflen <= 0 )		return TCL_ERROR;
	*d++ = c;
    }
    /*
     * DONT (this is allowed): if ( d == buf ) return TCL_ERROR;
     */
    *d = '\0';
    *idstr = s;
    return TCL_OK;
}



/**
    Parses the syntax "x y ?halo? ?afterId?", and returns one of the
    objects that intersect the square (x-halo,y-halo) to (x+halo,y+halo).
    If specified, only objects occuring after {afterId} will be returned,
    where {afterId} is one of the objects previously found.
**/
int
toctGetClosest( Tcl_Interp *ip, char **spec, octObject *pPnt, octObject *pObj) {
    char	*s = *spec, *s_next;
    int		halo;
    octPoint	pt;
    octBox	bbox;
    regObjGen	gen;
    regStatus	sts;
    octId	afterId = oct_null_id;
    octObject	nextobj;

    if ( topTclPrsInt2( ip, &s, (int*)&pt.x, (int*)&pt.y) != TCL_OK ) {
	Tcl_AppendResult(ip,"Bad x,y location ``",s,"''",NULL);
	return TCL_ERROR;
    }
    bbox.lowerLeft = bbox.upperRight = pt;
    if ( topTclPrsInt( ip, &s, &halo) == TCL_OK ) {
	bbox.lowerLeft.x -= halo;
	bbox.lowerLeft.y -= halo;
	bbox.upperRight.x += halo;
	bbox.upperRight.y += halo;
    }
    while ( isspace(*s) )	++s;
    if ( *s == '%' ) {
	++s;
	if ( _toctCvtStrId( &s, &afterId) != TCL_OK ) {
	    Tcl_AppendResult(ip,"Bad OCT tag (id) ``",s,"''",NULL);
	    return TCL_ERROR;
	}
    }
    regObjStart( pPnt, &bbox, OCT_GEO_MASK|OCT_INSTANCE_MASK, &gen, TOP_FALSE);
    if ( regObjNext( gen, pObj) == REG_NOMORE ) {
	*pObj = *pPnt;
    } else {
	if ( afterId != oct_null_id ) {
	    nextobj.objectId = pObj->objectId; 
	    for (;;) {
		if ( nextobj.objectId == afterId ) {
		    /* get one more; doesnt matter if it fails or not */
		    (void) regObjNext( gen, pObj);
		    break;
		}
		if ( regObjNext( gen, &nextobj) == REG_NOMORE )
		    break;
	    }
	}
    }
    regObjFinish( gen);
    *spec = s;
    return TCL_OK;
}



int
toctGetByTag( Tcl_Interp *ip, char *tag, octObject *pObj) {
    char		*s = tag;
    int			c;
    TOPStrLenType	typeLen;
    char		*typeName;
    char		namebuf[1000];
    octObject		pntobj;
    int			dir;
    octStatus		sts;

    pntobj.objectId = oct_null_id;
    pObj->objectId = oct_null_id;
    for ( ; ; ) {
	/*
	 * sequence below allows any white-space, but only one
	 * ``>'', and there must be a childspec after the ``>''.
	 */
	while ( (c=*s), isspace(c) )	s++;
	if ( c == '\0' )		break;
	/*IF*/ if ( c == '%' ) {
	    ++s;
	    _toctCvtStrId( &s, &pObj->objectId);
	    if ( octGetById(pObj) != OCT_OK ) {
		Tcl_AppendResult(ip,"Bogus OCT tag id.",NULL);
		return TCL_ERROR;
	    }
	    continue;
	} else if ( c == '>' || c=='<' ) {
	    dir = c; s++;
	    if ( pObj->objectId == oct_null_id ) {
		Tcl_AppendResult(ip,"Bad prefix spec.",NULL);
		return TCL_ERROR;
	    }
	    pntobj = *pObj;
	} else {
	    dir = '-';
	}
	while ( (c=*s), isspace(c) )	s++;
	/*IF*/ if ( c == '\0' ) {
	    Tcl_AppendResult(ip,"Missing child/parent",NULL);
	    return TCL_ERROR;
	}
	for ( typeName=s; (c=*s)!=' ' && c!='\0'; s++)
	    ;
	c = *typeName;
	typeLen = s - typeName;
	if ( c=='c' && strncmp(typeName,"closest",typeLen)==0 ) {
	    if ( dir!='>' ) {
		Tcl_AppendResult(ip,"Missing parent prefix.",NULL);
		return TCL_ERROR;
	    }
	    if ( toctGetClosest( ip, &s, &pntobj, pObj) != TCL_OK ) {
		Tcl_AppendResult(ip,"No closest in tag: ``",s,"''",NULL);
		return TCL_ERROR;
	    }
	    continue;
	}
	if ( _toctScanOctName( &s, namebuf, sizeof(namebuf)) != TCL_OK ) {
	    Tcl_AppendResult(ip,"Invalid name in tag: ``",s,"''",NULL);
	    return TCL_ERROR;
	}
	/*IF*/ if ( c=='f' && strncmp(typeName,"facet",typeLen)==0 ) {
	    if ( dir!='-' ) {
		Tcl_AppendResult(ip,"Facet tag must not have prefix tag.",NULL);
		return TCL_ERROR;
	    }
	    if ( toctGetFacetByName( namebuf, pObj) != TCL_OK ) {
		Tcl_AppendResult(ip,"Bad facet name ``",namebuf,"''",NULL);
		return TCL_ERROR;
	    }
	    continue;
	} else if ( c=='m' && strncmp(typeName,"master",typeLen)==0 ) {
	    if ( dir!='>' || pntobj.objectId==oct_null_id
	      || pntobj.type!=OCT_INSTANCE ) {
	        Tcl_AppendResult(ip,"Bad prefix for master.",NULL);
	        return TCL_ERROR;
	    }
	    if ( toctGetMasterByInst( &pntobj, namebuf, pObj) != TCL_OK ) {
		Tcl_AppendResult(ip,"Couldn't get master.",NULL);
		return TCL_ERROR;
	    }
	    continue;
	} else if ( c=='t' && strncmp(typeName,"terminal",typeLen)==0 ) {
	    pObj->type = OCT_TERM; pObj->contents.term.name = namebuf;
	} else if ( c=='n' && strncmp(typeName,"net",typeLen)==0 ) {
	    pObj->type = OCT_NET; pObj->contents.net.name = namebuf;
	} else if ( c=='i' && strncmp(typeName,"instance",typeLen)==0 ) {
	    pObj->type = OCT_INSTANCE; pObj->contents.instance.name=namebuf;
	} else if ( c=='l' && strncmp(typeName,"label",typeLen)==0 ) {
	    pObj->type = OCT_LABEL; pObj->contents.label.label = namebuf;
	} else if ( c=='b' && strncmp(typeName,"bag",typeLen)==0 ) {
	    pObj->type = OCT_BAG; pObj->contents.bag.name = namebuf;
	} else if ( c=='l' && strncmp(typeName,"layer",typeLen)==0 ) {
	    pObj->type = OCT_LAYER; pObj->contents.layer.name = namebuf;
	} else if ( c=='p' && strncmp(typeName,"property",typeLen)==0 ) {
	    pObj->type = OCT_PROP; pObj->contents.prop.name = namebuf;
	} else {
	    Tcl_AppendResult(ip,"Bad tag type ``",typeName,"''",NULL);
	    return TCL_ERROR;
	}
	if ( namebuf[0] == '\0' || strcmp(namebuf,"*any*")==0 ) {
	    if ( dir=='>' ) {
		sts = octGenFirstContent(&pntobj,(1<<pObj->type),pObj);
	    } else if ( dir=='<' ) {
		sts = octGenFirstContainer(&pntobj,(1<<pObj->type),pObj);
	    } else {
		sts = OCT_ERROR;
	    }
	} else {
	    if ( dir=='>' ) {
		sts = octGetByName(&pntobj,pObj);
	    } else if ( dir=='<' ) {
		sts = octGetContainerByName(&pntobj,pObj);
	    } else {
		sts = OCT_ERROR;
	    }
	}
	if ( sts != OCT_OK ) {
	    Tcl_AppendResult(ip,"No ``",typeName,"'' with name ``",namebuf,
		  "''",NULL);
	    return TCL_ERROR;
	}
    }
#ifdef notdef
    if ( pObj->objectId == oct_null_id ) {
	Tcl_AppendResult(ip,"Empty tag: ``",tag,"''",NULL);
	return TCL_ERROR;
    }
#endif
    return TCL_OK;
}

int
toctGetByTagM( Tcl_Interp *ip, char *tag, octObject *pObj, TOPMask typeMask) {
    if ( toctGetByTag( ip, tag, pObj) != TCL_OK ) {
	return TCL_ERROR;
    }
    if ( pObj->objectId==oct_null_id
      || TOP_OffB(TOP_BIT(pObj->type),typeMask) ) {
	Tcl_AppendResult(ip,"Tag ``",tag,"'' is wrong type object",NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

int
toctGetBbByTag( Tcl_Interp *ip, char *tag, octBox *pBox) {
    octObject	obj;
    octStatus	sts;

    if ( toctGetByTag( ip, tag, &obj) != TCL_OK 
      || obj.objectId==oct_null_id ) {
	Tcl_AppendResult(ip,"\nInvalid OCT tag ``",tag,"''",NULL);
	return TCL_ERROR;
    }
    if ( (sts=octBB( &obj, pBox)) == OCT_NO_BB ) {
	Tcl_AppendResult(ip,"No bounding box for OCT tag ``",tag,"''",NULL);
	return TCL_ERROR;
    }
    if ( sts != OCT_OK ) {
	Tcl_AppendResult(ip,"Can't get bounding box for OCT tag ``",tag,"''",
	  NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

static int
_toctGetCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    octObject	obj;

    if ( aC != 2 ) {
	Tcl_AppendResult(ip,"usage: ",aV[0]," oct_tag",NULL);
	return TCL_ERROR;
    }
    if ( toctGetByTag( ip, aV[1], &obj) != TCL_OK ) {
	Tcl_AppendResult(ip,"\n",aV[0],": Invalid OCT tag ``",aV[1],"''",NULL);
	return TCL_ERROR;
    }
    if ( obj.objectId != oct_null_id ) {
        TOCT_OctIdResult(ip, &obj);
    }
    return TCL_OK;
}

static int
_toctFmtCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    octObject	obj;
    char	*tn;

    if ( aC != 2 ) {
	Tcl_AppendResult(ip,"usage: ",aV[0]," oct_tag",NULL);
	return TCL_ERROR;
    }
    if ( toctGetByTag( ip, aV[1], &obj) != TCL_OK ) {
	Tcl_AppendResult(ip,"\n",aV[0],": Invalid OCT tag ``",aV[1],"''",NULL);
	return TCL_ERROR;
    }
    if ( obj.objectId == oct_null_id ) {
	topTclRet(ip,"{} NULL {} {No object}");
    } else {
	/* {id type-name object-name description} */
        TOCT_OctIdResult(ip, &obj);
	tn = ohTypeName(&obj);
	if ( strncmp(tn,"OCT_",4)==0 )	tn += 4;
	Tcl_AppendElement(ip, tn);
	Tcl_AppendElement(ip, ohGetName(&obj));
	Tcl_AppendElement(ip, ohFormatName(&obj));
	switch ( obj.type ) {
	case OCT_FACET:
	    /* {0-3 cell view facet version mode} */
	    Tcl_AppendElement(ip, obj.contents.facet.cell);
	    Tcl_AppendElement(ip, obj.contents.facet.view);
	    Tcl_AppendElement(ip, obj.contents.facet.facet);
	    Tcl_AppendElement(ip, obj.contents.facet.version);
	    Tcl_AppendElement(ip, obj.contents.facet.mode);
	    break;
	case OCT_INSTANCE:
	    /* {0-3 master view facet version} */
	    Tcl_AppendElement(ip, obj.contents.instance.master);
	    Tcl_AppendElement(ip, obj.contents.instance.view);
	    Tcl_AppendElement(ip, obj.contents.instance.facet);
	    Tcl_AppendElement(ip, obj.contents.instance.version);
	    break;
	}
    }
    return TCL_OK;
}

static int
_toctPropCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    octObject	obj;
    char	*tn;

    if ( aC != 2 ) {
	Tcl_AppendResult(ip,"usage: ",aV[0]," oct_tag",NULL);
	return TCL_ERROR;
    }
    if ( toctGetByTag( ip, aV[1], &obj) != TCL_OK 
      || obj.objectId == oct_null_id 
      || obj.type != OCT_PROP ) {
	Tcl_AppendResult(ip,"\n",aV[0],": Invalid OCT tag ``",aV[1],"''",NULL);
	return TCL_ERROR;
    }
    switch ( obj.contents.prop.type ) {
    case OCT_INTEGER:
	sprintf( ip->result, "INTEGER %ld", obj.contents.prop.value.integer);
	break;
    case OCT_REAL:
	sprintf( ip->result, "REAL %g", obj.contents.prop.value.real);
	break;
    case OCT_STRING:
	strcpy( ip->result, "STRING");
	Tcl_AppendElement(ip, obj.contents.prop.value.string);
	break;
    default:
	Tcl_AppendResult(ip,aV[0],": Don't know how to convert property type.",
	  NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/**
    Command: {x1 y1 x2 y2} = [toct_bbox oct_tag ...]
    Return the union of the bounding box of all the tags.  At least
    one tag must be specified; all objects must have bounding boxes.
    By convention, an empty bounding box has the x components
    both zero.
**/
static int
_toctBboxCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    int		i;
    octBox	resBB, objBB;

    if ( aC < 2 ) {
	Tcl_AppendResult(ip,"usage: ",aV[0]," oct_tag ...",NULL);
	return TCL_ERROR;
    }
    resBB.lowerLeft.x = resBB.upperRight.x = 0;
    for (i=1; i < aC; i++) {
	if ( toctGetBbByTag( ip, aV[i], &objBB) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0],": bad tag/bbox",NULL);
	    return TCL_ERROR;
	}
	toctBoxUnion2( &objBB, &resBB);
    }
    topTclRetFmt( ip, "%d %d %d %d", resBB.lowerLeft.x, resBB.lowerLeft.y,
      resBB.upperRight.x, resBB.upperRight.y);
    return TCL_OK;
}


int
toctInit( Tcl_Interp *ip) {
    Tcl_CreateCommand( ip, "toct_get", _toctGetCmd,
      (ClientData)NULL, NULL);
    Tcl_CreateCommand( ip, "toct_bbox", _toctBboxCmd,
      (ClientData)NULL, NULL);
    Tcl_CreateCommand( ip, "toct_fmt", _toctFmtCmd,
      (ClientData)NULL, NULL);
    Tcl_CreateCommand( ip, "toct_prop", _toctPropCmd,
      (ClientData)NULL, NULL);
    return TCL_OK;
}
