#if !defined(_EEMSILAR_H_)
#define _EEMSILAR_H_ 1
/* 
    RCS: $Header$
    eemSilar.h :: EEM Library

    Functions and data structures for
    manipulating singularities in the complex plane (poles/zeros).

    Author: Kennard White

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

						PT_COPYRIGHT_VERSION_3
						COPYRIGHTENDKEY
*/

#if !defined(_TOPVEC_H_) && __TOP_AUTOINCLUDE__
#include "topVec.h"
#endif
#if !defined(_EEMCPLX_H_) && __TOP_AUTOINCLUDE__
#include "eemCplx.h"
#endif

typedef TOPMask EEMSilarFlags;

typedef struct _EEMSilar EEMSilar;
struct _EEMSilar {
    EEMSilarFlags	flags;
    int			power;	/* neg for poles, pos for zeros */
    EEMComplex 		loc;	/* location in s- or z-plane */
    union _EEMSilarExtra {
        TOPPtr		pnt;	/* app-dependent parent */
	double		val;	/* app-dependent value */
    } extra;
};


typedef struct _EEMSilarGrp EEMSilarGrp;
struct _EEMSilarGrp {
    EEMComplex		gain;
    TOPVector		silars;	/* of EEMSilar */
};


/*
 * EEMSilar.flags
 */
#define EEMSilarF_Conj		TOP_BIT(0)	/* implement cplx conjugate */
#define EEMSilarF_Sel		TOP_BIT(1)	/* selected */
#define EEMSilarF_Avail		TOP_BIT(2)	/* available for re-use */
#define EEMSilarF_VisNorm	TOP_BIT(8)	/* the non-conj in visible */
#define EEMSilarF_VisConj	TOP_BIT(9)	/* the conj in visible */
#define EEMSilarF_Vis		(EEMSilarF_VisNorm|EEMSilarF_VisConj)
#define EEMSilarF_Tag1Norm	TOP_BIT(10)	/* arbitrary tag, non-conj */
#define EEMSilarF_Tag1Conj	TOP_BIT(11)	/* arbitrary tag, conj */
#define EEMSilarF_Tag1		(EEMSilarF_Tag1Norm|EEMSilarF_Tag1Conj)
#define EEMSilarF_Tag2		TOP_BIT(12)	/* arbitrary tag */

/*
 * EEMSilar.flags (change masks in hi-word)
 */
#define EEMSilarC_Exist		TOP_BIT(16)
#define EEMSilarC_Loc		TOP_BIT(17)
#define EEMSilarC_Power		TOP_BIT(18)
#define EEMSilarC_Sel		TOP_BIT(19)
#define EEMSilarC_Gain		TOP_BIT(20)
#define EEMSilarC_SilVal	(EEMSilarC_Loc|EEMSilarC_Power)
#define EEMSilarC_Value		(EEMSilarC_Gain|EEMSilarC_Exist|EEMSilarC_SilVal)
#define EEMSilarC_All		(EEMSilarC_Value|EEMSilarC_Sel)

#define EEM_SILAR_Gain(pGrp)	((pGrp)->gain)
#define EEM_SILAR_Num(pGrp)	TOPVEC_Num(&(pGrp)->silars)
#define EEM_SILAR_Get(pGrp,i)	TOPVEC_Get(&(pGrp)->silars,(i),EEMSilar)
#define EEM_SILAR_Array(pGrp)	TOPVEC_Array(&(pGrp)->silars,EEMSilar)
#define EEM_SILAR_Append(pGrp)	EEM_SILAR_Get(pGrp,eemSilarGrpAppendIdx(pGrp))

#define EEM_SILAR_ConjB(pSilar)	\
    TOP_IsOn((pSilar)->flags,EEMSilarF_Conj)

#define EEM_SILAR_TotPower(pSilar) \
    ( (pSilar)->power * ( EEM_SILAR_ConjB(pSilar) ? 2 : 1 ) )

#if __TOP_DEBUG__
#define EEM_SILAR_Print(pkg,pSilar,title) {	\
    if ( topMsgPkgDbgOnB(pkg) ) {		\
	eemSilarPrint(pSilar,title);		\
    }						\
}

#define EEM_SILARGRP_Print(pkg,pSG,title) {	\
    if ( topMsgPkgDbgOnB(pkg) ) {		\
	eemSilarGrpPrint(pSG,title);		\
    }						\
}
#else
#define EEM_SILAR_Print(pkg,pSilar,title) /* */
#define EEM_SILARGRP_Print(pkg,pSG,title) /* */
#endif


#define EEM_SILAR_LOOP_BEGIN(pGrp,pSilar) \
    TOPVEC_LOOP_FORW_BEGIN(&(pGrp)->silars,EEMSilar,pSilar)

#define EEM_SILAR_LOOP_END() \
    TOPVEC_LOOP_FORW_END()

#define EEM_SILAR_LOOP_FORW_BEGIN(pGrp,pSilar) \
    TOPVEC_LOOP_FORW_BEGIN(&(pGrp)->silars,EEMSilar,pSilar)

#define EEM_SILAR_LOOP_FORW_END() \
    TOPVEC_LOOP_FORW_END()

#define EEM_SILAR_IDX_LOOP_FORW_BEGIN(pGrp,idx,pSilar) \
    TOPVEC_IDX_LOOP_FORW_BEGIN(&(pGrp)->silars,idx,EEMSilar,pSilar)

#define EEM_SILAR_IDX_LOOP_FORW_END() \
    TOPVEC_IDX_LOOP_FORW_END()
	
#define EEM_SILAR_IDX_LOOP_BACK_BEGIN(pGrp,idx,pSilar) \
    TOPVEC_IDX_LOOP_BACK_BEGIN(&(pGrp)->silars,idx,EEMSilar,pSilar)

#define EEM_SILAR_IDX_LOOP_BACK_END() \
    TOPVEC_IDX_LOOP_BACK_END()


/*
 * eemSilar.c :: EEMSilar and EEMSilarGrp core functions
 */
extern void	eemSilarConstructor( EEMSilar *pSilar);
extern void	eemSilarDestructor( EEMSilar *pSilar);
extern void	eemSilarGrpConstructor( EEMSilarGrp *pGrp);
extern void	eemSilarGrpDestructor( EEMSilarGrp *pGrp);
extern void	eemSilarGrpClear( EEMSilarGrp *pGrp);
extern void	eemSilarGrpSetNum( EEMSilarGrp *pGrp, unsigned n);
extern void	eemSilarGrpInsureNum( EEMSilarGrp *pGrp, unsigned n);
extern int 	eemSilarGrpAppendIdx( EEMSilarGrp *pGrp);
extern EEMSilar*eemSilarGrpAddByVal( EEMSilarGrp *pGrp,
		  TOPMask f, double lr, double li, int p);
extern EEMSilar*eemSilarGrpAddByPtr( EEMSilarGrp *pGrp, EEMSilar *pSrc);
extern void	eemSilarGrpCopy2( EEMSilarGrp *pSrc, EEMSilarGrp *pDst,
		  EEMFlags flags);

extern int	eemSilarGrpGetAvail( EEMSilarGrp *pGrp);
extern void 	eemSilarGrpGC( EEMSilarGrp *pGrp);
extern void	eemSilarGrpCompact( EEMSilarGrp *pGrp);

extern void 	eemSilarPrint(EEMSilar *pS, char* title);
extern void 	eemSilarGrpPrint( EEMSilarGrp *pGrp, char *title);

extern int	eemSilarGrpGetPower( EEMSilarGrp *pGrp, 
		  int *pZeroPow, int *pPolePow);
extern int	eemSilarGrpExtractOrigin(EEMSilarGrp *pGrp);
extern void	eemSilarGrpFlattenConj( EEMSilarGrp *pSrc, EEMSilarGrp *pTgt,
		  int *pOriginCnt);
extern void	eemSilarGrpCompressConj(EEMSilarGrp *pGrp);

extern void 	eemDeleteMultSilar( EEMSilarGrp *pSing);
extern void	eemSilarGrpInvert( EEMSilarGrp *pGrp);
extern void 	eemSilarGrpEval(EEMSilarGrp *pSG, EEMComplex *pIn, 
			EEMComplex *pOut);
extern int	eemSilarGrpEvalPart(EEMSilarGrp *pGrp,
		  EEMComplex *pIn, EEMComplex *pOut, int sign);
extern void 	eemSilarGrpEvalA0(EEMSilarGrp *pSG, EEMSilar *pSilar, 
			EEMComplex *pOut);
extern double	eemSilarGrpEvalAbsPhase(EEMSilarGrp *pGrp, EEMComplex *pIn);
extern int	eemSilarGrpBB( EEMSilarGrp *pGrp, 
		  EEMComplex *pLo, EEMComplex *pHi);
extern double	 eemSilarGrpBbMag( EEMComplex *pLo, EEMComplex *pHi, double mv);

extern int	eemSilarGrpFindIdxByLoc( EEMSilarGrp *pSG, EEMComplex *pLoc);
extern int	eemCloseToNthSilarInSilarGrp( EEMSilarGrp *pSG, EEMComplex *pS2);

extern TOPLogical eemSilarSameB(EEMSilar *pS1, EEMSilar *pS2);
extern TOPLogical eemSilarSameLocB(EEMSilar *pS1, EEMSilar *pS2);

extern void	eemSilarGrpInvertVar( EEMSilarGrp *pGrp);
/* extern void	eemSilarGrpToTime(); XXX */


/*
 * from silclose.c
 */
extern void	eemSilarClosedLoop(EEMSilarGrp *pForwGrp, EEMSilarGrp *pBackGrp,
  		  double K, EEMSilarGrp *pClosedGrp);

/*
 * from eemSilarTcl.c
 */
extern void      eemSilarFmtTcl ARGS_TCL((Tcl_Interp *ip, 
		  EEMSilar *pSilar, TOPLogical nestB));
extern void	eemSilarGrpFmtTcl ARGS_TCL((Tcl_Interp *ip,
		  EEMSilarGrp *pGrp, TOPLogical nestB));
extern int	eemSilarPrsTcl ARGS_TCL((Tcl_Interp *ip, EEMSilar *pSilar, 
		  int aC, char **aV));
extern int	eemSilarStrPrsTcl ARGS_TCL((Tcl_Interp *ip, EEMSilar *pSilar,
		  char *silarStr));
extern int      eemSilarGrpPrsTcl ARGS_TCL((Tcl_Interp *ip,
		  EEMSilarGrp *pGrp, int aC, char **aV));
extern int      eemSilarGrpStrPrsTcl ARGS_TCL((Tcl_Interp *ip,
		  EEMSilarGrp *pGrp, char *grpStr));
extern int	eemSilarGrpNotifyTcl ARGS_TCL((Tcl_Interp *ip,
		  EEMSilarGrp *pGrp, char *basecmd, EEMSilarFlags mask));

#endif /* _EEMSILAR_H_ */
