/*******************************************************************
SCCS version identification
$Id$

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

 Programmer: Kennard White

    oct2ptcl.c :: oct2ptcl translator

    Program to translate an OCT facet/hierarchy into the ptcl language.

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"

#include <stdio.h>
#include <pwd.h>
#include <stdarg.h>

#include <tcl.h>

#include "oct.h"
#include "oh.h"
#include "region.h"
#include "errtrap.h"
#include "options.h"

#include "topStd.h"
#include "topMem.h"
#include "topMisc.h"

#include "oct2ptcl.h"

/*global*/ char _OtpPackage[] = "otp";
#define SPKG _OtpPackage

/****************************************************************************
 			oct2ptcl
 
    This source file is devided into the following parts:
    a) Funcs for translating parameter strings (ptolemy states).
    b) Funcs for translating nets (ptolemy geodesics or nodes).
    c) Funcs for translating instances (ptolemy stars)
    d) Code for translating and managing facets (ptolemy galaxies) and
       designs (ptolemy universes).

    In the code and comments below, the following abbreviations/terminology
    is used:
        otp = oct2ptcl (translate OCT to PTCL)
	ptcl = Ptolemy Tool Command Language
    	xlate = translate

    Note that this program is written as a one-shot translator, NOT
    as a library nor as an RPC application.  It is not suitable for use
    as a library (in its current form), because it doesnt worry about
    memory utilization, and rarely frees memory (and never unloads
    OCT facets).  It cannot be used as an RPC application because
    it semi-destructively modifies the OCT facet.

    There are three classes of errors that can occur:
    - an OCT function returns OCT_ERROR, and we dont want to deal with
      the error nicely.  We use the OH_FAIL() macro, which prints a message
      and calls errRaise() causing the program to abort.
    - we discover a condition that we dont want to deal with, and call
      errRaise() directly ourselves.  This also kills the program.
    - we discover an error condition.  We print a message to stderr,
      and continue, but return OCT_ERROR instead of OCT_OK.  This will
      produce an output file, but it will be bogus.  We exit(1) in
      this case at the end of the program.

*****************************************************************************/


/****************************************************************************
 *
 *		Parameter Translation
 *
 ****************************************************************************/



/**
    Translate the parameters of an instance, galaxy, universe or target.
    The parameters are stored in a STRING propery called "params"
    (or "targetparams") in one of two different formats:
	numParams|paramName|paramVal|...
    or
	numParams^AparamName^AparamType^AparamVal^A...
    {which} may be:
	OTP_FtStar	Parameters stored in the "params" property
			will be written out using the setstate command.
	OTP_FtGalaxy	Parameters stored in the "params" property
			will be written out using the newstate command.
	OTP_FtUniverse	Same as FtGalaxy.
	OTP_FtTarget	Parameters stored in the "targetparams" property
			will be written out using the targetparam command.
    Apparently pigilib tries to be cute and omits the paramType (is the second
    form) in some cases where the type is FLOAT.  Thus we must default this.
**/
octStatus
otpXlateParams( octObject *pInst, char *instName, OTPFacetType which,
  Tcl_DString *pStr) {
    octObject	paramProp;
    char	sep;
    char	*pstr, *pstrBase, *pstrOrig, *stateName, *stateType, *stateVal;
    int		cnt = 0;

    if ( ohGetByPropName( pInst, &paramProp,
      ((which==OTP_FtTarget) ? "targetparams" : "params")) != OCT_OK
      || (pstr=paramProp.contents.prop.value.string) == NULL
      || pstr[0] == '\0' )
	return OCT_OK;

    pstrOrig = pstr; pstrBase = memStrSave(pstr);
    if ( (pstr=strchr(pstrBase,'\1')) != NULL ) 		sep = '\1';
    else if ( (pstr=strchr(pstrBase,'|')) != NULL )		sep = '|';
    else goto error;
    for ( ++pstr; pstr && *pstr!='\0'; ) {
	stateName = pstr;
	if ( (stateType = strchr(stateName,sep)) == NULL ) {
	    goto error;
	}
	*stateType = '\0'; ++stateType;
	if ( sep == '|' ) {
	    stateVal = stateType;
	    stateType = NULL;
	} else {
	    if ( (stateVal = strchr(stateType,sep)) == NULL ) {
		goto error;
	    }
	    *stateVal = '\0'; ++stateVal;
	}
	if ( (pstr = strchr(stateVal,sep)) != NULL ) {
	    *pstr = '\0'; ++pstr;
	}
	if ( stateType == NULL || stateType[0] == '\0' )
	    stateType = "FLOAT";
	Tcl_DStringAppend(pStr,"\t",-1);
	switch ( which ) {
	case OTP_FtStar:
	    Tcl_DStringAppendEls(pStr,"trysetstate", instName, stateName,
	      stateVal, NULL);
	    break;
	case OTP_FtGalaxy:
	case OTP_FtUniverse:
	    Tcl_DStringAppendEls(pStr,"newstate", stateName,
	      stateType, stateVal, NULL);
	    break;
	case OTP_FtTarget:
	    Tcl_DStringAppendEls(pStr,"targetparam", stateName,
	      stateVal, NULL);
	    break;
	default:
	    errRaise(SPKG,-1,"Invalid which param.");
	}
	Tcl_DStringAppend(pStr,"\n",-1);
	++cnt;
    }
    memFree(pstrBase);
    return OCT_OK;

error:;
    errRaise(SPKG,-1,"Bogus params property in %s: ``%s''",
      ohFormatName(pInst), pstrOrig);
    memFree(pstrBase);
    return OCT_ERROR;
}

/****************************************************************************
 *
 *		Net Translation
 *
 ****************************************************************************/




/**
    Report an error regarding a net.  {fmt} is a printf()-style string.
**/
static void
_otpNetErr( OTPNetInfo *pInfo, char *fmt, ...) {
    va_list	args;
    int		i;

    fprintf( stderr, "Error:\tNet ``%s'' of facet ``%s'' ",
      pInfo->netName, pInfo->facetInfo->facetName);
    va_start( args, fmt);
    vfprintf( stderr, fmt, args);
    va_end( args);
    fprintf( stderr, "\tNet %s connects (so far): ", pInfo->netName);
    for (i=0; i < pInfo->numOut; i++) {
	OTPPortInfo *pPI = & pInfo->outPorts[i];
	fprintf( stderr, "%s.%s(%s) ", pPI->instName ? pPI->instName : "formal",
	  pPI->portName, pPI->flags & OTP_PifIn ? "in" : "out");
    }
    for (i=0; i < pInfo->numIn; i++) {
	OTPPortInfo *pPI = & pInfo->outPorts[i];
	fprintf( stderr, "%s.%s(%s) ", pPI->instName ? pPI->instName : "formal",
	  pPI->portName, pPI->flags & OTP_PifIn ? "in" : "out");
    }
    fprintf(stderr,"\n");
}


/**
    Determine if a given port {pTerm} is input or output.
    Ptolemy policy is to put a property called "input" or "output"
    on the interface formal terminal, while oct schematic policy
    is to use a direction property "DIRECTION" that contains
    "INPUT" or "OUTPUT".
    If non-NULL, pNetInfo is used only for error reporting.
**/
static TOPMask
_otpGetPortFlags( octObject *pTerm, OTPNetInfo *pNetInfo) {
    octObject	fterm, aterm, prop, pntFacet;
    TOPMask	flags = 0;
    char	*termName;

    if ( pTerm->contents.term.instanceId == oct_null_id ) {
#ifdef notdef
	OH_FAIL(octGenFirstContainer( pTerm, OCT_FACET_MASK, &facet),
	  "facet of terminal", pTerm);
	if ( ohOpenInterface( &master, &facet, "r") <= OCT_NEW_FACET )
	    errRaise(SPKG,-1,"Read interface of %s", ohFormatName(&facet));
	OH_FAIL(ohGetTerminal(&master, pTerm->contents.term.formalExternalId,
	  &fterm), "get interface terminal", pTerm);
#else
	OH_FAIL(octGenFirstContent( pTerm, OCT_TERM_MASK, &aterm),
	  "contents actual term from contents formal", pTerm);
	OH_FAIL(ohFindFormal( &fterm, &aterm),
	  "interface formal term from contents actual(1)", pTerm);
#endif
	flags |= OTP_PifFormal;
    } else {
	OH_FAIL(ohFindFormal( &fterm, pTerm),
	  "interface formal term from contents actual(2)", pTerm);
    }
    if ( ohGetByPropName( &fterm, &prop, "input") == OCT_OK )
	flags |= OTP_PifIn;
    if ( ohGetByPropName( &fterm, &prop, "output") == OCT_OK )
	flags |= OTP_PifOut;
    termName = pTerm->contents.term.name;
    if ( (flags & (OTP_PifIn|OTP_PifOut)) == 0 ) {
	OH_FAIL(octGenFirstContainer( &fterm, OCT_FACET_MASK, &pntFacet),
	  "facet of formal term", &fterm);
	fprintf( stderr, "Error: Terminal ``%s'' of facet ``%s'' is neither input nor output\n",
	  termName, ohFormatName(&pntFacet));
	if ( pNetInfo ) {
	    _otpNetErr( pNetInfo, "has bad terminal ``%s''\n", termName);
	}
    }
    return flags;
}

/**
    Given a context {pCxt} and a oct net {pNet}, find all the ports
    and other fun stuff (bus and delay markers) attached to it.
    Complications:
    1.	OCT schematic policy says that nets are attached to the formal
	terminals, not the actual terminals.  But this is not the case
	in many broken ptolemy facets.  Thus we look for both the formal
	and actual terminals of the I/O connectors.  We use the hash table
	portTbl to prevent duplicates.
    2.	The way we process delay and bus markers here depends on
	otpProcessMarker(); see that function.
    3.	The marker values (delay or buswidth) may be either integer or
	a string.  I think an older version of ptolemy only supported
	integer values, but later versions extended it to ptolemy expressions
	via strings.
**/
static octStatus
_otpGetNetInfo( OTPFacetInfo *pFInfo, octObject *pNet, OTPNetInfo *pNInfo) {
    octStatus		sts = OCT_OK;
    pNInfo->facetInfo = pFInfo;
    pNInfo->netName = pNet->contents.net.name;
    if ( pNInfo->netName==NULL || pNInfo->netName[0]=='\0' )
	pNInfo->netName = memStrSaveFmt("node%d", (pFInfo->sn)++);
    pNInfo->delayExpr = NULL;
    pNInfo->widthExpr = NULL;
    pNInfo->numFormal = pNInfo->numIn = pNInfo->numOut = 0;

    /*
     * Look at all ports attached to the net and categorize them
     */
    POH_LOOP_CONTENTS_BEGIN( pNet, OCT_TERM_MASK, theTerm) {
	octStatus	ownerId;
	char		*ownerName;
	Tcl_HashEntry	*instEntry, *portEntry;
	OTPPortInfo	*pPI;
	TOPMask		portflags = 0;
	octObject	ownerInst, fterm, aterm;
	int		newB, *cntPtr;

	ownerId = theTerm.contents.term.instanceId;
	portflags = _otpGetPortFlags( &theTerm, pNInfo);
	pPI = (portflags & OTP_PifIn) 
	  ? (cntPtr=&pNInfo->numIn, &pNInfo->inPorts[*cntPtr])
	  : (cntPtr=&pNInfo->numOut, &pNInfo->outPorts[*cntPtr]);
	pPI->flags = portflags;
	pPI->portName = theTerm.contents.term.name;
	pPI->instName = NULL;
	if ( ownerId == oct_null_id ) {
	    /* formal term */
	    OH_FAIL(octGenFirstContent( &theTerm, OCT_TERM_MASK, &aterm),
	      "contents actual term from formal", &theTerm);
	    portEntry = Tcl_CreateHashEntry(&pNInfo->portTbl,
	      (char*)aterm.objectId, &newB);
	    if ( !newB ) continue;
	    ++(pNInfo->numFormal);
	} else {
	    /* actual term */
	    portEntry = Tcl_CreateHashEntry(&pNInfo->portTbl,
	      (char*)theTerm.objectId, &newB);
	    if ( octGenFirstContainer( &theTerm, OCT_TERM_MASK, &fterm)
	      == OCT_OK ) {
	        /* it is the actual term of a formal term */
	        if ( !newB ) continue;
	        ++(pNInfo->numFormal);
		pPI->portName = fterm.contents.term.name;
	    } else {
		/* it is a "normal" actual term of a normal instance */
		instEntry = Tcl_FindHashEntry( &pFInfo->instTbl, (char*)ownerId);
		if ( instEntry == NULL ) {
		    OH_FAIL(ohGetById(&ownerInst,ownerId),
		      "get owner instance", &theTerm);
		    _otpNetErr(pNInfo,"has unexpected term ``%s'' with owner ``%s''\n",
		      pPI->portName, ohFormatName(&ownerInst));
		    ownerName = "unknown";
		} else {
		    ownerName = (char*) Tcl_GetHashValue(instEntry);
		}
		pPI->instName = ownerName;
	    }
	}
	++(*cntPtr);
	Tcl_SetHashValue(portEntry, (ClientData)pPI);
    } POH_LOOP_CONTENTS_END();

    /*
     * Look for delay and bus markers attached to the net 
     */
    POH_LOOP_CONTENTS_BEGIN( pNet, OCT_INSTANCE_MASK, theMarker) {
	octObject	prop;
	OTPFacetInfo	*pMaster = otpGetMasterInfo( pFInfo, &theMarker);
	if ( strcmp(pMaster->facetName,"%dDelay")==0 ) {
	    OH_FAIL(ohGetByPropName(&theMarker,&prop,"delay"),
	      "delay property", &theMarker);
	    if ( pNInfo->delayExpr != NULL ) {
		_otpNetErr(pNInfo,"has multiple delays (not allowed)\n");
		return OCT_ERROR;
	    }
	    pNInfo->delayExpr = otpCvtPropToStr( &prop);
	} else if ( strcmp(pMaster->facetName,"%dBus")==0 ) {
	    OH_FAIL(ohGetByPropName(&theMarker,&prop,"buswidth"),
	      "bus width property", &theMarker);
	    if ( pNInfo->widthExpr != NULL ) {
		_otpNetErr(pNInfo,"has multiple widths (not allowed)\n");
		return OCT_ERROR;
	    }
	    pNInfo->widthExpr = otpCvtPropToStr( &prop);
	}
    } POH_LOOP_CONTENTS_END();
    return sts;
}


/**
    Given info about a net (from _otpGetNetInfo() above), generate
    ptcl code for it.  This is tremendously more
    complicated than it needs to be: this is because ptcl maps
    one-to-one to ptolemy primitive function calls (and they are
    primitive).  Thus all the different input and output configurations
    need to be handled.
**/
static octStatus
_otpXlateNet( OTPNetInfo *pInfo, Tcl_DString *pStr) {
    int		i;

    if ( pInfo->numOut == 0 ) {
	_otpNetErr(pInfo,"has %d output ports (%s)\n",
	  pInfo->numOut, "each net must have at least one output port");
	return OCT_ERROR;
    }
    if ( pInfo->numIn == 0 ) {
	_otpNetErr(pInfo,"has %d input ports (%s)\n",
	  pInfo->numIn, "each net must have at least one input port");
	return OCT_ERROR;
    }
    Tcl_DStringAppend(pStr,"    ",-1);
    if ( pInfo->numIn == 1 && pInfo->numOut == 1 ) {
	if ( pInfo->numFormal == 1 ) {
	    /* 1-1, scalar net with i/o connector: use alias */
	    char	*formName;
	    OTPPortInfo	*pFar;
	    if ( pInfo->inPorts[0].instName == NULL ) {
		formName = pInfo->inPorts[0].portName;
		pFar = &pInfo->outPorts[0];
	    } else {
		formName = pInfo->outPorts[0].portName;
		pFar = &pInfo->inPorts[0];
	    }
	    if ( pInfo->widthExpr != NULL ) {
		_otpNetErr(pInfo,"is an I/O net with bus marker (%s)\n",
		  formName, "I/O nets must be scalars");
		return OCT_ERROR;
	    }
	    Tcl_DStringAppendEls(pStr,"alias",
	      formName, pFar->instName, pFar->portName, NULL);
	} else {
	    /* 1-1, no i/o connectors: use connect or busconnect */
	    if ( pInfo->widthExpr == NULL ) {
		/* 1-1, scalar net: use connect */
		Tcl_DStringAppendEls(pStr,"connect",
		  pInfo->outPorts[0].instName, pInfo->outPorts[0].portName,
		  pInfo->inPorts[0].instName, pInfo->inPorts[0].portName,
		  pInfo->delayExpr, NULL);
	    } else {
		/* 1-1, bus net: use busconnect */
		Tcl_DStringAppendEls(pStr,"busconnect",
		  pInfo->outPorts[0].instName, pInfo->outPorts[0].portName,
		  pInfo->inPorts[0].instName, pInfo->inPorts[0].portName,
		  pInfo->widthExpr, pInfo->delayExpr, NULL);
	    }
	}
    } else if ( pInfo->numIn == 1 && pInfo->numOut > 1 ) {
	/* put all scalar outputs to the single multiport input */
	if ( pInfo->delayExpr!=NULL || pInfo->widthExpr!=NULL ) {
	    _otpNetErr(pInfo,"is a multinet with delay or bus marker (%s)\n",
	      "not allowed");
	    return OCT_ERROR;
	}
	for (i=0; i < pInfo->numOut; i++) {
	    if (i!=0) Tcl_DStringAppend(pStr,"\n\t",-1);
	    Tcl_DStringAppendEls(pStr,"connect",
	      pInfo->outPorts[i].instName, pInfo->outPorts[i].portName,
	      pInfo->inPorts[0].instName, pInfo->inPorts[0].portName,
	      pInfo->delayExpr, NULL);
	}
    } else if ( pInfo->numIn > 1 && pInfo->numOut == 1 ) {
	/* auto-fork the single output to all the inputs */
	Tcl_DStringAppendEls(pStr,"node", pInfo->netName, NULL);
	for (i=0; i < pInfo->numOut; i++) {
	    Tcl_DStringAppend(pStr,"\n\t",-1);
	    Tcl_DStringAppendEls(pStr,"nodeconnect", 
	      pInfo->outPorts[i].instName, pInfo->outPorts[i].portName,
	      pInfo->netName, NULL);
	}
	for (i=0; i < pInfo->numIn; i++) {
	    Tcl_DStringAppend(pStr,"\n\t",-1);
	    Tcl_DStringAppendEls(pStr,"nodeconnect", 
	      pInfo->inPorts[i].instName, pInfo->inPorts[i].portName,
	      pInfo->netName, NULL);
	}
    } else {
	_otpNetErr(pInfo,"Cannot handle net.");
	return OCT_ERROR;
    }
    Tcl_DStringAppend(pStr,"\n",-1);
    return OCT_OK;
}

/**
    Walk through all nets and xlate them.
**/
octStatus
otpXlateNets( OTPFacetInfo *pCxt, octObject *pFacet, Tcl_DString *pStr) {
    OTPNetInfo	netinfo;
    octStatus	sts = OCT_OK;

    POH_LOOP_CONTENTS_BEGIN( pFacet, OCT_NET_MASK, theNet) {
	Tcl_InitHashTable( &netinfo.portTbl, TCL_ONE_WORD_KEYS);
	if ( _otpGetNetInfo( pCxt, &theNet, &netinfo) != OCT_OK ) {
	    sts = OCT_ERROR;
	} else {
	    if ( _otpXlateNet( &netinfo, pStr) != OCT_OK ) {
	        sts = OCT_ERROR;
	    }
	}
	Tcl_DeleteHashTable( &netinfo.portTbl);
    } POH_LOOP_CONTENTS_END();
    return sts;
}


/****************************************************************************
 *
 *		Instance Translation
 *
 ****************************************************************************/


/**
    Return the ptolemy name of the instance {pInst}.
    Typically this is {pInst->contents.instance.name}, but modified
    to be unique.  Returns a pointer to a dynmically allocated string.
    The argument {masterName} should be the ptolemy name of the instance's
    master.
**/
static char*
_otpXlateInstName( OTPFacetInfo *pCxt, octObject *pInst, char *masterName) {
    char	*instName, *dotPtr;

    instName = pInst->contents.instance.name;
    if ( instName==NULL || instName[0]=='\0' || strcmp(instName,"*")==0 ) {
	char	buf[OTP_NAMELEN];
	TOPStrLenType	len;
	if ( masterName[0]=='%' )	masterName += 2;
	len = (dotPtr = strchr(masterName,'.')) == NULL ? strlen(masterName)
	  : dotPtr - masterName;
	strncpy(buf,masterName,len);
	buf[len] = '\0';
	instName = memStrSaveFmt( "%s%d", buf, (pCxt->sn)++);
    } else {
	instName = memStrSave( instName);
    }
    return instName;
}

/**
    The ptolemy policy for markers (delay & bus) is totally bogus.
    The marker is pure geometry without an terminals.  Thus is is
    not associated with its net in any way except by location.
    This procedure fixes this by directly attaching the instance
    as a child of the net.  This violates policy, but works
    (i.e., fixes one hack with another!).
**/
octStatus
_otpProcessMarker( OTPFacetInfo *facetInfo, octObject *pFacet,
  octObject *pMarker) {
    octBox		bBox;
    octObject		path, dummyPath, net;
    regObjGen		rGen;
    OH_FAIL(octBB( pMarker, &bBox),"marker bounding box", pMarker);
    (void)regObjStart( pFacet, &bBox, OCT_PATH_MASK, &rGen, 0);
    if ( regObjNext(rGen, &path) != REG_OK ) {
	fprintf(stderr,"Dangling delay or bus marker.");
	regObjFinish(&rGen);
	return OCT_ERROR;
    }
    if ( regObjNext(rGen, &dummyPath) == REG_OK ) {
	fprintf(stderr,"Delay or bus marker intersects more than one path.");
	regObjFinish(&rGen);
	return OCT_ERROR;
    }
    regObjFinish(rGen);
    OH_FAIL(octGenFirstContainer(&path, OCT_NET_MASK, &net),
      "net from path", &path);
    OH_FAIL(octAttach(&net,pMarker),"attach marker to net",pMarker);
    return OCT_OK;
}

/**
    Ptolemy allows a star or galaxy to be "subclassed" by creating
    an OCT facet that is a special case of a real star or galaxy.
    The specialization has two aspects:
	binding the number of ports in a multiport to a specific value
	binding a state (parameter) to a specific value.
    Both are called bindings; however, we currently only handle
    multiport bindings, not state bindings.
    
    Bindings really define a new star or galaxy, but the ptolemy kernel
    is too stupid to deal with this concept.  Instead, the bindings are
    associated with each instance of a facet.  That is, the bindings
    are described in a master facet, but we tell ptolemy about it
    by attributing the instance.

    Currently, bindings are expressed as ".port=value" suffixes on the
    name of the master.  When the master is defined, we strip off
    the bindings from the name as store them in {masterInfo->nameBindings}.
    This func. issues "numport" commands for {instName} based on those
    bindings.

    In the future, we should support bindings that are stored as an OCT
    property of the facet.  We need to also support state bindings.
**/
octStatus
_otpXlateInstBindings( char *instName, OTPFacetInfo *masterInfo,
  Tcl_DString *pStr) {
    char	*bindPtr, *src, *tgt;
    char	nameBuf[OTP_NAMELEN];
    char	valBuf[OTP_NAMELEN];

    if ( (bindPtr=masterInfo->nameBindings) == NULL )
	return OCT_OK;
    for (src=bindPtr; *src; ) {
	if ( *src == '.' )	++src;
	for (tgt=nameBuf; *src != '='; src++) {
	    if ( *src == '\\' )		++src;
	    if ( *src == '\0' ) {
		errRaise(SPKG,-1,"Malformed binding string ``%s'' in %s", 
		  bindPtr, instName);
	    }
	    *tgt++ = *src;
	}
	tgt[0] = '\0';
	for (++src, tgt=valBuf; *src != '\0' && *src != '.'; src++) {
	    if ( *src == '\\' && src[1] != '\0' )		++src;
	    *tgt++ = *src;
	}
	tgt[0] = '\0';
	Tcl_DStringAppend(pStr,"\t",-1);
	Tcl_DStringAppendEls(pStr,"numports",instName,nameBuf,valBuf,NULL);
	Tcl_DStringAppend(pStr,"\n",-1);
    }
    return OCT_OK;
}


/**
    Walk through all instances and xlate them using the "star" command.
    Delay and bus instances are passed onto _otpProcessMarker(), and
    are later used when processing nets.
**/
octStatus
otpXlateInsts( OTPFacetInfo *pFInfo, octObject *pFacet, Tcl_DString *pStr) {
    octObject	instBag;
    octStatus	sts = OCT_OK;

    if ( ohGetByBagName( pFacet, &instBag, "INSTANCES") != OCT_OK )
	return OCT_OK;
    POH_LOOP_CONTENTS_BEGIN( &instBag, OCT_INSTANCE_MASK, theInst) {
	OTPFacetInfo	*mInfo = otpGetMasterInfo(pFInfo,&theInst);
	char		*instName;
	Tcl_HashEntry	*instEntry;
	int		newB;

	if ( mInfo->facetName[0]=='%' ) {
	    if ( strcmp(mInfo->facetName,"%dDelay")==0
	      || strcmp(mInfo->facetName,"%dBus")==0 ) {
		/* these are pure-geometry, no terminals.  must x,y search */
		_otpProcessMarker( pFInfo, pFacet, &theInst);
	    }
	    continue;
	}

	instName = _otpXlateInstName(pFInfo,&theInst,mInfo->facetName);
	instEntry = Tcl_CreateHashEntry( &pFInfo->instTbl,
	  (char*)theInst.objectId, &newB);
	Tcl_SetHashValue( instEntry, instName);
	Tcl_DStringAppend(pStr,"    ",-1);
	Tcl_DStringAppendEls(pStr,"star", instName, mInfo->starName,
	  NULL);
	Tcl_DStringAppend(pStr,"\n",-1);
	if ( _otpXlateInstBindings( instName, mInfo, pStr) != OCT_OK )
	    return OCT_ERROR;
	if ( otpXlateParams( &theInst, instName, OTP_FtStar, pStr) != OCT_OK )
	    return OCT_ERROR;
    } POH_LOOP_CONTENTS_END();
    return sts;
}


/**
    Recursively translate the children of {pFInfo}.  OCT policy is not obeyed
    in ptolemy palettes, and there is typically no INSTANCES bag
    (or it is empty).  Thus must directly get all instances.

    This should be extended to support universes with passed-in parameters
    (just like galaxies can have passed-in params).
**/
octStatus
otpXlateKids( OTPFacetInfo *pFInfo, octObject *pFacet, Tcl_DString *pStr) {
    octStatus	sts = OCT_OK;

    POH_LOOP_CONTENTS_BEGIN( pFacet, OCT_INSTANCE_MASK, theInst) {
	OTPFacetInfo	*mInfo = otpGetMasterInfo(pFInfo,&theInst);

	if ( (pFInfo->designInfo->flags & OTP_DsgnF_Recur)!=0
	  && mInfo->state == OTP_FcsInit
	  && (mInfo->type == OTP_FtGalaxy || mInfo->type == OTP_FtUniverse
	   || mInfo->type == OTP_FtPalette) ) {
	    Tcl_DString	galStr;
	    if ( otpXlateFacet( mInfo, &galStr) != OCT_OK )
		sts = OCT_ERROR;
	    Tcl_DStringAppend( pStr, Tcl_DStringValue(&galStr), -1);
	    Tcl_DStringFree( &galStr);
	}
	if ( pFInfo->type != OTP_FtPalette && mInfo->tcltkB ) {
	    pFInfo->tcltkB = TOP_TRUE;
	}
    } POH_LOOP_CONTENTS_END();
    return sts;
}

/****************************************************************************
 *
 *		Facet & Design Translation
 *
 *  We make an attempt to manage the open'ing and close'ing of facets.
 *  In _otpAddFacet(), we always open the interface and contents,
 *  and then free them as they are processed.
 *
 ****************************************************************************/

/**
    Return a textual (string) equiv. of {type}.  For status messages.
**/
static char*
_otpFacetTypeToStr( OTPFacetType type) {
    switch ( type ) {
    case OTP_FtStar:		return "star";
    case OTP_FtGalaxy:		return "galaxy";
    case OTP_FtUniverse:	return "universe";
    case OTP_FtPalette:		return "palette";
    case OTP_FtTarget:		return "target";
    case OTP_FtMarker:		return "marker";
    case OTP_FtContact:		return "contact";
    default:;
    }
    return "Unknown facet type";
}

static void
_otpSetFacetType( OTPFacetInfo *pInfo, octObject *pXface, octObject *pConts) {
    octObject	prop;
    char	*cellbase;

    cellbase = topBasename( pXface->contents.facet.cell);
    if ( cellbase[0] == '%' ) {
	pInfo->type = OTP_FtMarker;
	return;
    }

    if ( topFindSubPath( pXface->contents.facet.cell, "tcltk")!=NULL ) {
	pInfo->tcltkB = TOP_TRUE;
    }
    if ( ohGetByPropName( pXface, &prop, "CELLTYPE") == OCT_OK ) {
	char *ctVal = prop.contents.prop.value.string;
	if ( strcmp(ctVal,"CONTACT")==0 ) {
	    pInfo->type = OTP_FtContact;
	    return;
	}
    }
    /*IF*/ if ( ohGetByPropName( pXface, &prop, "star") == OCT_OK ) {
	pInfo->type = OTP_FtMarker;
    } else if ( ohGetByPropName( pXface, &prop, "galaxy") == OCT_OK ) {
	pInfo->type = OTP_FtGalaxy;
    } else if ( ohGetByPropName( pXface, &prop, "universe") == OCT_OK ) {
	pInfo->type = OTP_FtUniverse;
    } else if ( ohGetByPropName( pXface, &prop, "palette") == OCT_OK ) {
	pInfo->type = OTP_FtPalette;
    } else {
        errRaise(SPKG,-1,"Cannot determine facet type of %s\n",
          ohFormatName(pXface));
    }
}

/**
    This is the core function for managing the facets of a design: it
    adds {cell} into the list of facet to be translated for {pDInfo}.
    Currently, only the basename of the cell name is considered important;
    we should prob. use the entire name. (so that one can have two distinct
    galaxies in different directories, but with the same name).
**/
static OTPFacetInfo*
_otpAddFacet( OTPDesignInfo *pDInfo, char *cell, TOPLogical *pNewB) {
    Tcl_HashEntry	*pEntry;
    OTPFacetInfo	*pCxt;
    int			newB;
    char		*s;

    *pNewB = TOP_FALSE;
    cell = topBasename( cell);
    pEntry = Tcl_CreateHashEntry( &pDInfo->cellTbl, cell, &newB);
    if ( newB ) {
        *pNewB = TOP_TRUE;
	pCxt = MEM_ALLOC(OTPFacetInfo);
	Tcl_SetHashValue(pEntry,(ClientData)pCxt);
	pCxt->designInfo = pDInfo;
	pCxt->sn = 0;
	Tcl_InitHashTable( &pCxt->instTbl, TCL_ONE_WORD_KEYS);
	pCxt->facetName = memStrSave( cell);
	pCxt->starName = memStrSave( cell);
	pCxt->nameBindings = NULL;
	pCxt->outerDomain = NULL;
	pCxt->curDomain = NULL;
	if ( (s = strchr( pCxt->starName, '.')) != NULL ) {
	    *s = '\0';
	    pCxt->nameBindings = s+1;
	}
	pCxt->state = OTP_FcsInit;
	pCxt->interfaceId = oct_null_id;
	pCxt->contentsId = oct_null_id;
	pCxt->type = OTP_FtUniverse;	/* default assumption */
	pCxt->tcltkB = TOP_FALSE;
    } else {
	pCxt = (OTPFacetInfo*) Tcl_GetHashValue(pEntry);
    }
    return pCxt;
}

/**
    Gets the facet info for the master of {pInst}.  This also gets
    some preliminary info about the master facet for later use.
    We use a real hack to find out if a star is tcltk or not.
**/
OTPFacetInfo*
otpGetMasterInfo( OTPFacetInfo *pFInfo, octObject *pInst) {
    OTPFacetInfo	*pMasterInfo;
    octObject		mXface, mConts;
    TOPLogical		newB;

    pMasterInfo = _otpAddFacet( pFInfo->designInfo, 
      pInst->contents.instance.master, &newB);

    if ( newB ) {
	pMasterInfo->outerDomain = pFInfo->curDomain;

	/* copy cell&view over so ohFormat below wont core dump */
	mXface.contents.facet.cell = pInst->contents.instance.master;
	mXface.contents.facet.view = pInst->contents.instance.view;
	if ( ohOpenMaster(&mXface, pInst, "interface", "r") <= OCT_NEW_FACET ) {
	    errRaise(SPKG,-1,"Failed to read %s", ohFormatName(&mXface));
	}
	pMasterInfo->interfaceId = mXface.objectId;

	/* copy cell&view over so ohFormat below wont core dump */
	mConts.contents.facet.cell = pInst->contents.instance.master;
	mConts.contents.facet.view = pInst->contents.instance.view;
	if ( ohOpenMaster(&mConts, pInst, "contents", "r") <= OCT_NEW_FACET ) {
	    errRaise(SPKG,-1,"Failed to read %s", ohFormatName(&mConts));
	}
	pMasterInfo->contentsId = mConts.objectId;

	_otpSetFacetType(pMasterInfo, &mXface, &mConts);
    }
    return pMasterInfo;
}

/**
    Retrieve the facet info about {pFacet}.  Currently, {pFacet} is assumed
    to be a universe.  Will open {pFacet} if not currently opened.
**/
OTPFacetInfo*
otpGetFacetInfo( OTPDesignInfo *pDInfo, octObject *pFacet) {
    OTPFacetInfo *pFInfo;
    octObject	conts, xface;
    TOPLogical	newB;

    pFInfo = _otpAddFacet( pDInfo, pFacet->contents.facet.cell, &newB);
    if ( newB ) {
	if ( ohOpenInterface( &xface, pFacet, "r") <= OCT_NEW_FACET ) {
	    errRaise(SPKG,-1,"Failed to read %s",ohFormatName(&xface));
	}
	pFInfo->interfaceId = xface.objectId;

	if ( ohOpenContents( &conts, pFacet, "r") <= OCT_NEW_FACET ) {
	    errRaise(SPKG,-1,"Failed to read %s",ohFormatName(&conts));
	}
	pFInfo->contentsId = conts.objectId;

	_otpSetFacetType( pFInfo, &xface, &conts);
    }
    return pFInfo;
}


/**
    Issue the "run" and "wrapup" commands
**/
static octStatus
_otpXlateUnivGo( OTPFacetInfo *facetInfo, octObject *pConts, Tcl_DString *pStr){
    octObject	prop;
    char	*iterCntPtr;
    int		flags = facetInfo->designInfo->flags;

    if ( ohGetByPropName( pConts, &prop, "iterate") == OCT_OK ) {
	iterCntPtr = otpCvtPropToStr(&prop);
    } else {
	iterCntPtr = "10";
    }
    if ( flags & OTP_DsgnF_Verbose ) {
	Tcl_DStringAppend(pStr, "puts stdout \"Running universe...\"\n",-1);
    }
    Tcl_DStringAppendEls(pStr, "run", iterCntPtr,NULL);
    Tcl_DStringAppend(pStr,"\n",-1);
    if ( flags & OTP_DsgnF_Verbose ) {
	Tcl_DStringAppend(pStr, "puts stdout \"Wrapping up...\"\n", -1);
    }
    Tcl_DStringAppend(pStr,"wrapup\n",-1);
    if ( flags & OTP_DsgnF_Verbose ) {
	Tcl_DStringAppend(pStr, "puts stdout \"Done.\"\n", -1);
    }
    return OCT_OK;
}

/**
    Translate {facetInfo}, {pFacet} into ptcl code in {pStr}.
    This is also where we do the recursive depth-first decent of the
    facet tree (via otpXlateKids()).
    Complications:
    1.  There is difficulty with handling domain changes and "reset".
	Apparently the ptolemy kernel does weird things if you try to change
	to domain of a universe when it is not empty.  Since galaxies have
	domain changes (outer and inner domains), we must make sure
	the top-level universe is empty (via "reset") before defining
	a galaxy.
    2.	We have the option of either leaving the universes named or unnamed.
	For simulation it doesnt really matter since there wont be
	more than one universe at a time.  But in code generation ptolemy
	writes out files based on the name of the universe.  We might
	consider issueing a deluniverse command after the wrapup, but
	why bother: thats what virtual memory is for :-).
**/
octStatus
_otpXlateFacetCore( OTPFacetInfo *pFInfo, octObject *pFacet, Tcl_DString *pStr){
    octObject		paramProp;
    char		*strval;
    octStatus		sts = OCT_OK;


    /* Must get domain right before doing kids */
    pFInfo->curDomain = pFInfo->outerDomain;
    if ( ohGetByPropName( pFacet, &paramProp, "domain") == OCT_OK ) {
	pFInfo->curDomain = paramProp.contents.prop.value.string;
    }

    sts = otpXlateKids( pFInfo, pFacet, pStr);

    if ( (pFInfo->designInfo->flags & OTP_DsgnF_NoTcltk) && pFInfo->tcltkB ) {
	fprintf(stderr,"\tSkipping %s (tcltk)\n", pFInfo->facetName);
	return sts;
    }
    switch ( pFInfo->type ) {
    case OTP_FtGalaxy:
	if ( pFInfo->outerDomain ) {
	    Tcl_DStringAppend(pStr,"reset\n",-1);
            Tcl_DStringAppendEls(pStr,"domain", pFInfo->outerDomain, NULL);
            Tcl_DStringAppend(pStr,"\n",-1);
	}
        Tcl_DStringAppendEls(pStr,"defgalaxy", pFInfo->facetName, NULL);
        Tcl_DStringStartSublist(pStr);
        Tcl_DStringAppend(pStr,"\n",-1);
	break;
    case OTP_FtUniverse:
	Tcl_DStringAppend(pStr,"reset\n",-1);
        Tcl_DStringAppendEls(pStr,"newuniverse", pFInfo->facetName, NULL);
        Tcl_DStringAppend(pStr,"\n",-1);
	break;
    case OTP_FtPalette:
	return sts;
    default: ;
    }

    if ( pFInfo->curDomain ) {
	Tcl_DStringAppend(pStr,"    ",-1);
	Tcl_DStringAppendEls( pStr, "domain", pFInfo->curDomain, NULL);
	Tcl_DStringAppend(pStr,"\n",-1);
    }

    if ( ohGetByPropName( pFacet, &paramProp, "target") == OCT_OK
      && (strval=paramProp.contents.prop.value.string) != NULL
      && strcmp(strval,"<parent>")!=0 ) {
	Tcl_DStringAppend(pStr,"    ",-1);
	Tcl_DStringAppendEls( pStr, "target", strval, NULL);
	Tcl_DStringAppend(pStr,"\n",-1);
    }
    if ( otpXlateParams( pFacet, pFInfo->facetName, OTP_FtTarget, pStr)
      != OCT_OK ) {
	fprintf(stderr,"Tgt params failed (%s)\n", ohFormatName(pFacet));
	sts = OCT_ERROR;
    }
    if ( otpXlateParams( pFacet, pFInfo->facetName, OTP_FtGalaxy, pStr)
      != OCT_OK ) {
	fprintf(stderr,"Gal params failed (%s)\n", ohFormatName(pFacet));
	sts = OCT_ERROR;
    }
    if ( otpXlateInsts( pFInfo, pFacet, pStr) != OCT_OK ) {
	fprintf(stderr,"Instances failed (%s)\n", ohFormatName(pFacet));
	sts = OCT_ERROR;
    }
    if ( otpXlateNets( pFInfo, pFacet, pStr) != OCT_OK ) {
	fprintf(stderr,"Instances failed (%s)\n", ohFormatName(pFacet));
	sts = OCT_ERROR;
    }
    if ( pFInfo->type == OTP_FtGalaxy ) {
        Tcl_DStringEndSublist(pStr);
        Tcl_DStringAppend(pStr,"\n",-1);
    }
    if ( pFInfo->type == OTP_FtUniverse 
      && (pFInfo->designInfo->flags & OTP_DsgnF_Go) ) {
	if ( _otpXlateUnivGo( pFInfo, pFacet, pStr) != OCT_OK ) {
	    fprintf(stderr,"Run params failed (%s)\n", ohFormatName(pFacet));
	    sts = OCT_ERROR;
	}
    }
    return sts;
}

/**
    Translate {facetInfo} into ptcl code in {pStr}.  
    {pStr} will be initialized (thus it should not have anything in it).
    If recursive translation is enabled, several facets may be prepended
    to {pStr}.
**/
octStatus
otpXlateFacet( OTPFacetInfo *pFInfo, Tcl_DString *pStr) {
    octObject		theFacet;
    char		*ftype;
    octStatus		sts = OCT_OK;

    Tcl_DStringInit(pStr);
    pFInfo->state = OTP_FcsBegin;
    ftype = _otpFacetTypeToStr(pFInfo->type);
    if ( pFInfo->designInfo->flags & OTP_DsgnF_Verbose ) {
        fprintf(stderr,"Xlating %s %s ...\n", ftype, pFInfo->facetName);
        Tcl_DStringAppends(pStr,"puts stdout \"Defining ",ftype,
	  " ",pFInfo->facetName, "\"\n",NULL);
    }
    if ( pFInfo->contentsId == oct_null_id ) {
	errRaise(SPKG,-1,"Lost contents for facet ``%s''",
	  pFInfo->facetName);
    }
    OH_FAIL(ohGetById(&theFacet,pFInfo->contentsId),"contents facet",NULL);

    sts = _otpXlateFacetCore( pFInfo, &theFacet, pStr);

    pFInfo->state = sts==OCT_OK ? OTP_FcsDone : OTP_FcsError;
    if ( pFInfo->designInfo->flags & OTP_DsgnF_Verbose ) {
        fprintf(stderr,"... %s done.\n", pFInfo->facetName);
        Tcl_DStringAppends(pStr,"puts stdout \"",ftype," ",
	  pFInfo->facetName, " complete.\"\n",NULL);
    }
    return sts;
}



/**
    Facet {pFacet} will be translated and stored into {pStr}.  {pStr}
    will be initialized, and thus should not contain any data on entry.
**/
octStatus
otpXlateDesign( octObject *pFacet, int flags, Tcl_DString *pStr) {
    OTPDesignInfo	designInfo;
    OTPFacetInfo	*facetInfo;
    octStatus		sts;

    designInfo.flags = flags;
    Tcl_InitHashTable( &designInfo.cellTbl, TCL_STRING_KEYS);
    facetInfo = otpGetFacetInfo( &designInfo, pFacet);
    sts = otpXlateFacet( facetInfo, pStr);
    return sts;
}

static char _OtpDesignProlog[] =
   "proc trysetstate {star state value} {\n"
   "    if [catch {statevalue $star $state initial} err] then {\n"
   "	    puts stdout \"Warning: $err\"\n"
   "    } else {\n"
   "	    setstate $star $state $value\n"
   "    }\n"
   "}\n";

octStatus
otpXlateDesignByName( char *facetName, int flags) {
    octObject	theFacet;
    Tcl_DString	str;
    octStatus	sts;

    theFacet.objectId = oct_null_id;
    ohUnpackDefaults(&theFacet, "r", ":schematic:contents");
    if (ohUnpackFacetName(&theFacet, facetName) != OCT_OK ) {
	fprintf(stderr,"%s: Invalid facet name.\n", optProgName);
	optUsage();
    }
    sts = otpXlateDesign( &theFacet, flags, &str);
    printf("\n%s\n", _OtpDesignProlog);
    printf("\n%s\n", Tcl_DStringValue(&str));
    Tcl_DStringFree(&str);
    if ( sts != OCT_OK )
	fprintf(stderr,"%s: Design translation failed!\n", optProgName);
    return sts;
}

#define OTP_VERS "oct2ptcl v0.1"

/*global*/ optionStruct optionList[] = {
    { "r",	"",		"recursive translation" },
    { "g",	"",		"append run/wrapup command (go)" },
    { "t",	"",		"don't xlate facets with tcltk stars" },
    { "v",	"",		"verbose" },
    { OPT_RARG, "facet",	"facet to translate" },
    { OPT_DESC,0,OTP_VERS},
    { OPT_CONT,0," translates an OCT facet to ptcl.\n" },
    { OPT_CONT,0,"The PTOLEMY environment variable must reference the root\n"},
    { OPT_CONT,0,"of the ptolemy tree, and defaults to ~ptolemy.\n"},
    { 0, 0, 0}
};

int
main( int argc, char **argv) {
    int		designFlags = 0;
    int		option;

    while ( (option = optGetOption(argc, argv)) != EOF ) {
	switch (option) {
	case 'r':	designFlags |= OTP_DsgnF_Recur;		break;
	case 'g':	designFlags |= OTP_DsgnF_Go;		break;
	case 'v':	designFlags |= OTP_DsgnF_Verbose;	break;
	case 't':	designFlags |= OTP_DsgnF_NoTcltk;	break;
	default:	optUsage();
	}
    }
    if ( optind != argc-1 ) {
	fprintf(stderr, "%s: You must specify at least one facet.\n",
	  optProgName);
	optUsage();
    }
    if ( getenv("PTOLEMY")==NULL ) {
	struct passwd	*pPwEnt;
	if ( (pPwEnt = getpwnam("ptolemy")) == NULL ) {
	    fprintf(stderr,"%s: %s and %s.", optProgName,
	      "Environment variable PTOLEMY not set",
	      "user ptolemy does not exist");
	    optUsage();
	}
	if ( designFlags & OTP_DsgnF_Verbose ) {
	    fprintf(stderr, "Using PTOLEMY = %s\n", pPwEnt->pw_dir);
	}
	topSetEnv("PTOLEMY",pPwEnt->pw_dir);
    }
    if ( designFlags & OTP_DsgnF_Verbose ) {
	fprintf(stderr,"%s\n",OTP_VERS);
    }
    if ( otpXlateDesignByName( argv[optind], designFlags) != OCT_OK )
	exit(1);
    exit(0);
}
