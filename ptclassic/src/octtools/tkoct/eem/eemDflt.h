#if !defined(_EEMDFLT_H_)
#define _EEMDFLT_H_ 1
/* 
    RCS: $Header$
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

#if !defined(_TOPSTD_H_) && __TOP_AUTOINCLUDE__
#include "topStd.h"
#endif


typedef struct _EEMParamsStruct EEMParamsStruct;
struct _EEMParamsStruct {
    double	Smallno;	/* smallest pos number (ieee: min_normal) */
    double	Ulp;		/* smallest pos s.t. "1+ulp > 1" */

    int		DefAddErr;	/* default number of Smallno for Abstol */
    int		DefMulErr;	/* default number of ulp for reltol */

    int		CurAddErr;	/* default number of Smallno for Abstol */
    int		CurMulErr;	/* default number of ulp for reltol */

    double	Abstol;
    double	Reltol;
    double	HalfReltol;
    double	Polytol;

    char	DfltFmtStr[50];	/* default fmt to sprintf() for doubles*/
    char	CplxFmtStr[50];	/* default fmt to sprintf() for EEMComplex*/
};


typedef struct _EEMDblPoint2 EEMDblPoint2;
struct _EEMDblPoint2 {
    double              x;
    double              y;
};

typedef struct _EEMBox EEMBox;
struct _EEMBox {
    EEMDblPoint2 lo;
    EEMDblPoint2 hi;
    EEMDblPoint2 delta;
};

extern EEMParamsStruct EEMParams;

extern double	eemParamCalcUlp();
extern double	eemParamCalcSmallno();
extern void	eemParamsInit();
extern void	eemParamsCalc();
extern void	eemParamsSet( int addErr, int mulErr);
extern void	eemParamsSetDfltFmtStr ARGS((char *fmt));
extern void	eemParamsSetCplxFmtStr ARGS((char *fmt));
extern TOPLogical eemDfltSameB( double a, double b);


extern double	eemFactorial( int n);
extern double	eemBinomial( int n, int k);

/**
    General purpose flags for all eem routines.  These tell the routine
    how to deal with its arguments.  The exact interpretation depends
    on the function.  Note that these flag bits replace an older
    system where an argument called {sign} was positive to indicate
    EEMF_Numer and negative to indicate EEMF_Denom.  A sign of zero
    meant EEMF_Gain, or not EEMF_Gain, depending on the function.
    Typically a function was either implicitly copy (replace)
    or append (add to exist function).  The exact meaning of
    append can be addition, multiplication, or something else
    depending on context.
**/

typedef TOPMask EEMFlags;
#define EEMF_Numer	TOP_BIT(0)	/* numerator or zeros */
#define EEMF_Denom	TOP_BIT(1)	/* denominator or poles */
#define EEMF_Rat	(EEMF_Numer|EEMF_Denom)
#define EEMF_Gain	TOP_BIT(2)	/* deal with gain term */
#define EEMF_Append	TOP_BIT(3)	/* append as opposed to replace */
#define EEMF_Construct	TOP_BIT(4)	/* construct the result-args first */

#define EEM_FlgSgn(flags)	( TOP_IsOn((flags),EEMF_Numer) ? 1 : -1 )
#define EEM_FlgSgnVal(flags,k)	( TOP_IsOn((flags),EEMF_Numer) ? (k) : 0-(k) )

/*
 * dfltTcl.c
 */
extern void	eemDfltRegisterCmds ARGS_TCL((Tcl_Interp *ip));

extern int	eemInt2PrsTcl ARGS_TCL((Tcl_Interp *ip, char *str,
		  int *pX, int *pY));
extern int	eemInt4PrsTcl ARGS_TCL((Tcl_Interp *ip, char *str,
		  int *pX1,int *pY1, int *pX2,int *pY2));

extern void	eemDfltFmtTcl ARGS_TCL((Tcl_Interp *ip, double val,
		  TOPLogical NestB));
extern int	eemDflt2PrsTcl ARGS_TCL((Tcl_Interp *ip, char *str,
		  double *pX, double *pY));
extern int	_eemDfltCmds ARGS_TCL((ClientData cld, Tcl_Interp *ip,
		  int aC, char **aV));
extern int	_eemParamsCmds ARGS_TCL((ClientData cld, Tcl_Interp *ip,
		  int aC, char **aV));



#endif /* _EEMDFLT_H_ */
