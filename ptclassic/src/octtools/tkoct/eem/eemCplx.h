#if !defined(_EEMCPLX_H_)
#define _EEMCPLX_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

Copyright (c) 1990- The Regents of the University of California.
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

#if !defined(_EEMDFLT_H_) && __TOP_AUTOINCLUDE__
#include "eemDflt.h"
#endif


typedef struct _TOPIntComplex TOPIntComplex;
struct _TOPIntComplex {
    int		r;
    int		i;
};

typedef struct _EEMComplex EEMComplex;
typedef EEMComplex EEMCplx;
struct _EEMComplex {
    double      r;
    double      i;
};

#define EEM_CPLX_SET( pc, rv, iv) \
    (void)( ((pc)->r) = (rv), ((pc)->i) = (iv), 0)
#define EEM_CPLX_ZERO( pc)		EEM_CPLX_SET( pc, 0.0, 0.0)
#define EEM_CPLX_MAGSQ( pC)		( ((pC)->r*(pC)->r)+((pC)->i*(pC)->i) )

/* This protects atan2(), which doesnt like 0/0 cases */
#define EEM_CPLX_PHASE(sr,si) ( 				\
    ((sr) == 0)							\
      ? ( ((si) == 0) ? 0 : ((si) > 0 ? M_PI_2 : - M_PI_2) )	\
      : atan2( (si), (sr))					\
)


#if __TOP_DEBUG__
#define EEM_CPLX_Print(pkg,pCplx,title) {	\
    if ( topMsgPkgDbgOnB(pkg) ) {		\
	eemCplxPrint(pCplx,title);		\
    }						\
}
#else
#define EEM_CPLX_Print(pkg,pCplx,title) /* */
#endif


#if __TOP_DEBUG__
#define EEM_CPLX_Print2(pkg,pCplx,pCplx2,title) {	\
    if ( topMsgPkgDbgOnB(pkg) ) {			\
	eemCplxPrint2(pCplx,pCplx2,title);		\
    }							\
}
#else
#define EEM_CPLX_Print2(pkg,pCplx,pCplx2,title) /* */
#endif

extern char _EemCplxSubPackage[];

extern void	eemCplxZero( EEMComplex *pD);
extern void	eemCplxSet( EEMComplex *pD, double R, double I);
extern TOPLogical	eemCplxSameB( EEMComplex *a, EEMComplex *b);
extern TOPLogical	eemCplxNegSameB( EEMComplex *a, EEMComplex *b);
extern TOPLogical	eemCplxConjSameB( EEMComplex *a, EEMComplex *b);
extern TOPLogical	eemCplxZeroB( EEMComplex *a);
extern TOPLogical	eemCplxOneB( EEMComplex *a);

extern void 	eemCplxPrint(EEMComplex *pP, char* title);
extern void 	eemCplxPrint2(EEMComplex *pP, EEMComplex *pC, char* title);
extern void 	eemRealToCplx(EEMComplex *pC, double R);
extern void	eemCplxNeg1( EEMComplex *pD);
extern void	eemCplxAdd2( EEMComplex *pD, EEMComplex *pS);
extern void	eemCplxAdd3( EEMComplex *pS1, EEMComplex *pS2, EEMComplex *pD);
extern void	eemCplxSub2( EEMComplex *pD, EEMComplex *pS);
extern void	eemCplxSub3( EEMComplex *pS1, EEMComplex *pS2, EEMComplex *pD);
extern void	eemCplxMul2( EEMComplex *pD, EEMComplex *pS);
extern void	eemCplxMul3( EEMComplex *pS1, EEMComplex *pS2, EEMComplex *pD);
extern void	eemCplxDiv2( EEMComplex *pD, EEMComplex *pS);
extern void	eemCplxDiv3( EEMComplex *pS1, EEMComplex *pS2, EEMComplex *pD);
extern void	eemCplxMulDiv2( EEMComplex *pD, EEMComplex *pS, 
		  TOPLogical mulB);
extern void	eemCplxInv1( EEMComplex *pD);
extern void	eemCplxInv2( EEMComplex *pS, EEMComplex *pD);
extern void	eemCplxCopyInv2( EEMComplex *pS, EEMComplex *pD,
		  TOPLogical copyB);

extern void	eemCplxMerge( EEMComplex *pD, EEMComplex *pS,
		  TOPLogical mixB, TOPLogical mulB);
extern void	eemCplxPow2( EEMComplex *pD, int power);
extern void	eemCplxPow3( EEMComplex *pS, int power, EEMComplex *pD);

extern void	eemCplxSqr1( EEMComplex *pD);
extern void	eemCplxSqr2( EEMComplex *pD, EEMComplex *pS);
extern void	eemCplxSqrt1( EEMComplex *pS);
extern void	eemCplxSqrt2( EEMComplex *pS, EEMComplex *pD);

extern double   eemCplxMag( EEMComplex *pS);
extern double	eemCplxPhase( double rval, double ival);

extern void	eemCplxMagPhase( EEMComplex *pD, EEMComplex *pS);
extern void	eemCplxRealImag( EEMComplex *pD, EEMComplex *pS);
extern double	eemCplxDistSq( EEMComplex *pS1, EEMComplex *pS2);
extern void	eemCplxExp( double A, EEMComplex *pS, EEMComplex *pD);


extern void	eemCplxMaxRect( EEMComplex *pL, EEMComplex *pLo, 
		  EEMComplex *pHi);

extern void	eemCplxQuadraticRoots( 
		  EEMComplex *A, EEMComplex *B, EEMComplex *C,
		  EEMComplex *pR1, EEMComplex *pR2);

extern void 	eemCplxCopy(EEMComplex *pD, EEMComplex *pS);
extern void 	eemCplxCopyNeg(EEMComplex *pD, EEMComplex *pS);

extern char*	eemCplxFmt( char *buf, EEMComplex *pC, char *fmt);


/*
 * cplxTcl.c 
 */
extern int	eemCplxPrsTcl ARGS_TCL((Tcl_Interp *ip, 
		  EEMComplex *pC, int aC, char **aV));
extern int	eemCplxStrPrsTcl ARGS_TCL((Tcl_Interp *ip, 
		  char *arg, EEMComplex *pC));
extern int	eemCplxFmtTcl ARGS_TCL((Tcl_Interp *ip, 
		  EEMComplex *pC,TOPLogical NestB, char *fmt));
extern int	eemCplxRegisterCmds ARGS_TCL((Tcl_Interp *ip));

/*
 * cpoly.h
 */
extern int cpoly_( double *opr, double *opi, int *degree, 
		  double *zeror, double *zeroi, TOPLogical *pFail);


#endif /* _EEMCPLX_H_ */
