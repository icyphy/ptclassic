/* 
    topFmt.c :: TOP Library
    Extension library for formating "double" floating point values.

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"

#include <stdio.h>

#include <tcl.h>

#include "topStd.h"
#include "topMisc.h"

#define TOPFmtF_PreParsed	TOP_BIT(0)
#define TOPFmtF_TotDigs		TOP_BIT(1)
#define TOPFmtF_TotSig		TOP_BIT(2)
#define TOPFmtF_FracDigs	TOP_BIT(3)
#define TOPFmtF_FracSig		TOP_BIT(4)
#define TOPFmtF_AltForm		TOP_BIT(5)

typedef struct _TOPFmtInfo TOPFmtInfo;
struct _TOPFmtInfo {
    int		CvtType;	/* char */
    int		ScaleType;	/* char */
    int		Flags;
    int		totDigs;
    int		fracDigs;
    char	*realFmt;
    char	*remainFmt;
    char	*typeFmt;
};

static char 
_topGetEngSuffix( int e) {
    switch ( e ) {
    case -15:		return('f');
    case -12:		return('p');
    case -9:		return('n');
    case -6:		return('u');
    case -3:		return('m');
    case 0:		return(' ');
    case 3:		return('K');
    case 6:		return('M');
    case 9:		return('G');
    }
    return( '?' );
}

static int 
_topCvtEngSuffixToPow( char suf) {
    switch ( suf ) {
    case 'f':		return -15;
    case 'p':		return -12;
    case 'n':		return -9;
    case 'u':		return -6;
    case 'm':		return -3;
    case ' ':		return 0;
    case 'K':		return 3;
    case 'M':		return 6;
    case 'G':		return 12;
    }
    return -1;
}

static void 
_topCvtDblToEng( char *buf, TOPFmtInfo *pInfo, double f) {
    char	magbuf[50];
    double	eval;
    int		ebase, emod;
    double	v;

    if ( f == 0.0 ) {
	ebase = 0;
	v = 0;
	emod = 0;
    } else {
        eval = log10( fabs(f)) + .0001;
	ebase = (int) TOP_FLOOR(eval,3.0);
	emod = ((int)(floor(eval))) - ebase;
        v = f * exp10( (double) -ebase);
    }
    magbuf[0] = _topGetEngSuffix( ebase);
    if ( magbuf[0] == '?' ) {
	sprintf( magbuf, "e%d", ebase);
    } else {
	magbuf[1] = '\0';
   }
    if ( TOP_IsOn(pInfo->Flags, TOPFmtF_FracSig) ) {
        /* number of digs left of decimal place is (emod + 1) */
	/* fracDigs is really sigDigs */
	int fd = pInfo->fracDigs - (emod+1);
	if ( fd < 0 )	fd = 0;
        sprintf( buf, pInfo->realFmt, fd, v, magbuf);
    } else {
        sprintf( buf, pInfo->realFmt, v, magbuf);
    }
}


#define TOP_FMT_PreParseChar	'!'
#define TOP_FMT_ScaleChar	'@'
#define TOP_FMT_SigChar		'^'

static TOPLogical
_topFmtParse( char *fmt, TOPFmtInfo *pInfo) {
    int totDigs, fracDigs;

    pInfo->Flags = 0;
    if ( *fmt != '%' ) {
	return TOP_FALSE;
    }
    ++fmt;
    if ( *fmt == TOP_FMT_PreParseChar ) {
	++fmt;
	pInfo->Flags |= TOPFmtF_PreParsed;
	pInfo->CvtType = *(fmt++) & 0377;
	pInfo->ScaleType = *(fmt++) & 0377;
	pInfo->realFmt = fmt;
	return TOP_TRUE;
    }
    if ( *fmt == TOP_FMT_ScaleChar ) {
	++fmt;
	pInfo->ScaleType = *(fmt++) & 0377;
    } else {
	pInfo->ScaleType = '-';
    }
    pInfo->remainFmt = fmt;

    if ( *fmt == '#' ) {
	++fmt;
	pInfo->Flags |= TOPFmtF_AltForm;
    }

    totDigs = 0;
    if ( *fmt == TOP_FMT_SigChar ) {
	pInfo->Flags |= TOPFmtF_TotSig;
	++fmt;
    }
    if ( isdigit(*fmt) ) {
	pInfo->Flags |= TOPFmtF_TotDigs;
	for (; ; totDigs *= 10) {
	    totDigs += (*(fmt++) & 0377) - '0';
	    if ( !isdigit(*fmt) )
		break;
	}
    }
    pInfo->totDigs = totDigs;

    fracDigs = 0;
    if ( *fmt == '.' ) {
	++fmt;
	if ( *fmt == TOP_FMT_SigChar ) {
	    pInfo->Flags |= TOPFmtF_FracSig;
	    ++fmt;
	}
        if ( isdigit(*fmt) ) {
	    pInfo->Flags |= TOPFmtF_FracDigs;
	    for ( ; ; fracDigs *= 10) {
	        fracDigs += (*(fmt++) & 0377) - '0';
		if ( !isdigit(*fmt) )
		    break;
	    }
	}
    }
    pInfo->fracDigs = fracDigs;

    if ( ! isalpha(*fmt) )
	return TOP_FALSE;
    pInfo->typeFmt = fmt;
    pInfo->CvtType = *(fmt++) & 0377;

    if ( *fmt != NULL )
	return TOP_FALSE;

    return TOP_TRUE;
}


/**
    Given the magnitudes of a range: 0 <= mlo <= mhi,
    calculate how many characters the exponent will take.
    includes the leading "e".
**/
static int
_topFmtCalcExponentLen( double mlo, double mhi) {
#ifdef OMIT_PLUS
    if ( mlo <= 1e-100 ) {
	return 5;		/* e-123 */
    } else if ( mlo <= 1e-10 || mhi >= 1e100 ) {
	return 4;		/* e-12 or e123 */
    } else if ( mhi >= 1e10 ) {
	return 3;
    } else {
	return 2;
    }
#else
    if ( mlo <= 1e-100 || mhi >= 1e100 ) {
	return 5;		/* e-123 or e+123 */
    } else {
	return 4;		/* e-12,e+12 or e-00,e+00 */
    }
#endif
}

static char*
_topFmtTotSpecToStr( TOPFmtInfo *pInfo, int delta) {
    static char buf[20];

    if ( TOP_IsOff( pInfo->Flags, TOPFmtF_TotDigs) ) {
	buf[0] = '\0';
    } else {
	int td = pInfo->totDigs + delta;
	if ( td <= 0 ) {
	    buf[0] = '\0';
	} else {
	    sprintf( buf, "%d", td);
	}
    }
    return buf;
}

static char*
_topFmtFracSpecToStr( TOPFmtInfo *pInfo, int delta) {
    static char buf[20];

    if ( TOP_IsOff( pInfo->Flags, TOPFmtF_FracDigs) ) {
	buf[0] = '\0';
    } else {
	int fd = pInfo->fracDigs + delta;
	if ( fd < 0 )	fd = 0;
	sprintf( buf, ".%d", fd);
    }
    return buf;
}

/**
    Given an extended format string "fmt", calculate a precise
    format string that fits {lo} {hi} and {sameDigs}.  {lo} to {hi}
    is a range of labels to be produced.  {sameDigs} is the
    number of significant digits that are the same between adjcent labels.
    For example, .00457 and .00456 have samedigs=2.
**/
int
topFmtDblCalcRange( char *fmt, double lo, double hi, int sameDigs,
  char *realFmt) {
    TOPFmtInfo	info;
    double	mlo, mhi;
    int		tmpflags, totlen;
    int		realFmtCode;
    char	*suffixStr1 = "", *suffixStr2 = "";

    if ( ! _topFmtParse( fmt, &info) ) {
	strcpy( realFmt, "ErrorFmt");
	return 0;
    }

    totlen = 0;
    switch ( info.ScaleType ) {
    case 'P':
	lo /= M_PI;
	hi /= M_PI;
	suffixStr2 = "P";
	totlen += 1;
	break;
    case 'd':
	lo = lo <= 0.0 ? -900 : 10 * log10(lo);
	hi = hi <= 0.0 ? -800 : 10 * log10(hi);
	suffixStr2 = "d";
	totlen += 1;
	break;
    case 'D':
	lo = lo <= 0.0 ? -900 : 20 * log10(lo);
	hi = hi <= 0.0 ? -800 : 20 * log10(hi);
	suffixStr2 = "D";
	totlen += 1;
	break;
    }

    if ( (mlo = fabs(lo)) > (mhi = fabs(hi)) )
        TOP_SWAPTYPE(double, mlo, mhi);

    switch ( info.CvtType ) {
    case 'E':
	realFmtCode = 'f';
	suffixStr1 = "%s";
	if ( TOP_IsOn(info.Flags, TOPFmtF_FracSig) ) {
	    info.fracDigs += sameDigs - 1;
	    info.Flags |= TOPFmtF_FracDigs;
	}
	break;
    case 'e':
	realFmtCode = info.CvtType;
	if ( TOP_IsOn(info.Flags, TOPFmtF_FracSig) ) {
	    info.fracDigs += sameDigs - 1;	/* whole part is one digit */
	    info.Flags |= TOPFmtF_FracDigs;
	}
	break;
    case 'g':
	realFmtCode = info.CvtType;
	if ( TOP_IsOn(info.Flags, TOPFmtF_FracSig) ) {
	    info.fracDigs += sameDigs;
	    info.Flags |= TOPFmtF_FracDigs;
	}
	break;
    case 'f':
	realFmtCode = info.CvtType;
	if ( TOP_IsOn(info.Flags, TOPFmtF_FracSig) ) {
	    int wholeDigs = (int)floor(log10(mhi)) + 1; 
	    if ( wholeDigs < 0 )	wholeDigs = 0;
	    info.fracDigs += sameDigs - wholeDigs;
	    info.Flags |= TOPFmtF_FracDigs;
	}
	break;
    default:
	strcpy( realFmt, "BadFmtType1");
	return 0;
    }
    if ( info.fracDigs < 0 )
	info.fracDigs = 0;
    
    tmpflags = info.Flags;
    if ( TOP_IsOn(tmpflags, TOPFmtF_TotSig) ) {
	tmpflags &= ~TOPFmtF_TotDigs;
	info.Flags |= TOPFmtF_TotDigs;
    }
    if ( TOP_IsOn(tmpflags, TOPFmtF_TotDigs) ) {
	totlen += info.totDigs;
    } else {
	/*
	 * No fixed total digits specified.  Add to user specified pad
	 * our computed max length of the result.  We assume
	 * that the fracDigs as already been proper adjusted
	 */
	switch ( info.CvtType ) {
	case 'e':	/* standard scientific */
	case 'g':	/* uhhhg.  I give up */
	    if ( lo < 0.0 || hi < 0.0 )		/* for minus sign */
		++info.totDigs;
	    if ( info.fracDigs > 0 )		/* for .12356 */
		info.totDigs += info.fracDigs + 1;
	    info.totDigs += _topFmtCalcExponentLen( mlo, mhi);
	    info.totDigs += 1;			/* for the integer part */
	    totlen += info.totDigs;
	    break;
	case 'f':	/* standard scientific */
	    if ( lo < 0.0 || hi < 0.0 )		/* for minus sign */
		++info.totDigs;
	    if ( info.fracDigs > 0 )		/* for .12356 */
		info.totDigs += info.fracDigs + 1;
	    info.totDigs += 1;			/* one digit integer part */
	    if ( mhi >= 10.0 )
		info.totDigs += (int)floor(log10(mhi)); /* extra integer part */
	    totlen += info.totDigs;
	    break;
	case 'E':	/* engineering */
	    if ( lo < 0.0 || hi < 0.0 )		/* for minus sign */
		++info.totDigs;
	    if ( info.fracDigs > 0 )		/* for .12356 */
		info.totDigs += info.fracDigs + 1;
	    info.totDigs += 3;			/* for integer part */
	    totlen += info.totDigs;
	    if ( ((mlo>0)&&(mlo < 1e-15)) || mhi >= 1e7 ) {
	        totlen += _topFmtCalcExponentLen( mlo, mhi);
	    } else {
		totlen += 1;	/* suffix */
	    }
	    break;
	default:
	    strcpy( realFmt, "BadFmtType2");
	    return 0;
	}
    }

    sprintf( realFmt, "%%!%c%c%%%s%s%c%s%s", info.CvtType, info.ScaleType, 
      _topFmtTotSpecToStr( &info, 0), _topFmtFracSpecToStr( &info, 0),
      realFmtCode, suffixStr1, suffixStr2);

    return totlen;
}
  


/**
    Examine a raw format string {fmt} that has been parsed into {pInfo}.
    If one of our extented format types, munge the format string as
    needed to produce a info.realFmt.
    We don't want to know the total length of the string.  All we want
    to do is convert our precise-significant digits form into "fraction
    digits" wanted by sprintf.
**/
TOPLogical
_topFmtMunge( TOPFmtInfo *pInfo, double f) {
    int fracdelta = 0;

    /* WARNING: TotSig is ignored */
    switch ( pInfo->CvtType ) {
    case 'E':
	/* use totlen-1 for magsymbol */
	if ( TOP_IsOn( pInfo->Flags, TOPFmtF_FracSig) ) {
	    sprintf(pInfo->realFmt, "%%%s.*f%%s",
	      _topFmtTotSpecToStr( pInfo, -1));
	} else {
	    sprintf(pInfo->realFmt, "%%%s%sf%%s",
	      _topFmtTotSpecToStr( pInfo, -1),
	      _topFmtFracSpecToStr(pInfo, 0));
	}
	break;
    case 'e':
    case 'g':
	fracdelta = -1;	/* assume one dig left of decimal */
    case 'f':
	if ( TOP_IsOn( pInfo->Flags, TOPFmtF_FracSig) ) {
	    if ( pInfo->CvtType == 'f' ) {
		double af = TOP_ABS(f);
		if ( af >= 1.0 ) {
		    int e = floor( log10( af));
		    fracdelta = -(e+1);
		}
	    }
	    sprintf(pInfo->realFmt, "%%%s%s%s%c",
	      TOP_IsOn(pInfo->Flags, TOPFmtF_AltForm) ? "#" : "",
	      _topFmtTotSpecToStr( pInfo, 0),
	      _topFmtFracSpecToStr(pInfo, fracdelta), pInfo->CvtType);
	} else {
	    pInfo->realFmt[0] = '%';
	    strcpy( pInfo->realFmt + 1, pInfo->remainFmt);
	}
	break;
    default:
	sprintf( pInfo->realFmt, "Bad type %c", pInfo->CvtType);
	return TOP_FALSE;
    }
    if ( pInfo->ScaleType != '-' ) {
	char *s = pInfo->realFmt + strlen(pInfo->realFmt);
	s[0] = pInfo->ScaleType;
	s[1] = '\0';
    }
    return TOP_TRUE;
}


TOPLogical
topFmtDbl( char *buf, char *fmt, double f) {
    TOPFmtInfo	info;
    char	tmpRealFmt[30];

    if ( ! _topFmtParse( fmt, &info) ) {
	strcpy( buf, "ErrorFmt");
	return TOP_FALSE;
    }
    if ( TOP_IsOff( info.Flags, TOPFmtF_PreParsed) ) {
	info.realFmt = tmpRealFmt;
	if ( ! _topFmtMunge( &info, f) ) {
	    strcpy( buf, "ErrorMng: ");
	    strcat( buf, info.realFmt);
	    return TOP_FALSE;
	}
    }

    switch ( info.ScaleType ) {
    case 'P':
	f /= M_PI;
	break;
    case 'd':
    case 'D':
	if ( f == 0 ) {
	    strcpy( buf, "-Inf");	return TOP_TRUE;
	} else if ( f < 0 ) {
	    strcpy( buf, "NaN");	return TOP_TRUE;
	} else {
	    f = ( info.ScaleType == 'd' ? 10.0 : 20.0 ) * log10( f);
        }
	break;
    }

    switch ( info.CvtType ) {
    case 'E':
	_topCvtDblToEng( buf, &info, f);
	break;
    default:
	sprintf( buf, info.realFmt, f);
    }
    return TOP_TRUE;
}

TOPLogical
topFmtVerifyB( char *fmt) {
    TOPFmtInfo	info;

    if ( ! _topFmtParse( fmt, &info) )
	return TOP_FALSE;
    return TOP_TRUE;
}

int
topFmtGetScale( char *fmt, double *pLinScale) {
    TOPFmtInfo	info;

    if ( ! _topFmtParse( fmt, &info) )
	return -1;
    if ( pLinScale ) {
	switch ( info.ScaleType ) {
	case 'P':	*pLinScale = M_PI;	break;
	default:	*pLinScale = 1.0;	break;
	}
    }
    return info.ScaleType;
}

int
topFmtGetType( char *fmt) {
    TOPFmtInfo	info;

    if ( ! _topFmtParse( fmt, &info) )
	return -1;
    return info.CvtType;
}

TOPLogical
topFmtChangeScale( char *newFmt, char *fmt, char newScale) {
    TOPFmtInfo	info;
    char	*nf = newFmt;

    if ( ! _topFmtParse( fmt, &info) ) {
	strcpy( newFmt, "ErrorFmt");
	return TOP_FALSE;
    }
    if ( TOP_IsOn( info.Flags, TOPFmtF_PreParsed) ) {
	strcpy( newFmt, "ErrorPreParsed");
	return TOP_FALSE;
    }
    if ( newScale != '-' && !isalpha(newScale) ) {
	strcpy( newFmt, "ErrorBadScale");
	return TOP_FALSE;
    }

    nf = newFmt;
    *nf++ = '%';
    if ( newScale != '-' && newScale != '\0' ) {
        *nf++ = '@';
        *nf++ = newScale;
    }
    strcpy( nf, info.remainFmt);
    return TOP_TRUE;
}

static int
_topFmtScaleCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    TOPLogical	ret;
    char	*fmt;
    TOPFmtInfo	info;

    if ( aC != 2 && aC != 3 ) {
	Tcl_AppendResult( ip, "usage: ", aV[0], " fmt ?scale?\n", NULL);
	return TCL_ERROR;
    }
    Tcl_ResetResult( ip);
    fmt = aV[1];
    if ( aC == 2 ) {
        if ( (ret=_topFmtParse( fmt, &info)) ) {
	    ip->result[0] = info.ScaleType;
	    ip->result[1] = '\0';
	}
    } else {
        ret = topFmtChangeScale( ip->result, fmt, aV[2][0]);
    }
    if ( ! ret ) {
	Tcl_AppendResult( ip, "\n",aV[0],": error\n",NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

TOPLogical
topFmtChangeType( char *newFmt, char *fmt, char newType) {
    TOPFmtInfo	info;

    if ( ! _topFmtParse( fmt, &info) ) {
	strcpy( newFmt, "ErrorFmt");
	return TOP_FALSE;
    }
    if ( TOP_IsOn( info.Flags, TOPFmtF_PreParsed) ) {
	strcpy( newFmt, "ErrorPreParsed");
	return TOP_FALSE;
    }
    if ( !isalpha(newType) ) {
	strcpy( newFmt, "ErrorBadType");
	return TOP_FALSE;
    }
    strcpy( newFmt, fmt);
    newFmt[info.typeFmt-fmt] = newType;
    return TOP_TRUE;
}

static int
_topFmtTypeCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    char	*fmt;
    int		ret;
    TOPFmtInfo	info;

    if ( aC != 2 && aC != 3 ) {
	Tcl_AppendResult( ip, "usage: ", aV[0], " fmt ?type?\n", NULL);
	return TCL_ERROR;
    }
    fmt = aV[1];
    Tcl_ResetResult( ip);
    if ( aC == 2 ) {
        if ( (ret=_topFmtParse( fmt, &info)) ) {
	    ip->result[0] = info.CvtType;
	    ip->result[1] = '\0';
	}
    } else {
	ret = topFmtChangeType( ip->result, fmt, aV[2][0]);
    }
    if ( ! ret ) {
	Tcl_AppendResult( ip, "\n", aV[0], ": error\n", NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

TOPLogical
topPrsDbl( char *str, double *pDbl) {
    char	*pEnd = NULL;
    double	d;
    int		e;

    d = strtod( str, &pEnd);
    if ( pEnd == NULL || pEnd == str )
	return TOP_FALSE;
    if ( *pEnd != '\0' ) {
	switch ( *pEnd ) {
	case 'P':	d *= M_PI;		break;
	case 'd':	d = exp10(d/10.0);	break;
	case 'D':	d = exp10(d/20.0);	break;
	default:
	    if ( (e=_topCvtEngSuffixToPow( *pEnd)) == -1 ) 
		return TOP_FALSE;
	    d *= exp10( (double)e);
	}
	++pEnd;
    }
    if ( *pEnd != '\0' )
	return TOP_FALSE;
    if ( pDbl )		*pDbl = d;
    return TOP_TRUE;
}

int
topPrsDblTcl( Tcl_Interp *ip, char *str, double *pDbl) {
    if ( topPrsDbl( str, pDbl) == TOP_FALSE ) {
        Tcl_AppendResult( ip, "Expected double: got ``", str, "''", NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}


static int
_topPrsDblCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    double	dblVal;
    char	*dblStr = aV[1], *s, *last;

    if ( aC != 2 ) {
	Tcl_AppendResult( ip, "usage: ", aV[0], " dbl_val\n", NULL);
	return TCL_ERROR;
    }

    Tcl_ResetResult( ip);
    while ( isspace(*dblStr) )
	++dblStr;
    for ( last = dblStr + strlen(dblStr)-1; last > dblStr && isspace(*last);
      last--)
	;
    last[1] = '\0';
    if ( ! isalpha(*last) ) {
	s = ip->result;
	for ( ; *dblStr != '\0' && !isspace(*dblStr); s++, dblStr++) {
	    *s = *dblStr;
	}
	*s = '\0';
	return TCL_OK;
    }
    if ( topPrsDblTcl( ip, dblStr, &dblVal) != TCL_OK )
	return TCL_ERROR;
    sprintf( ip->result, "%g", dblVal);
    return TCL_OK;
}

static int
_topPrsDblDbgCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    double	dblVal;

    if ( aC != 2 ) {
	Tcl_AppendResult( ip, "usage: ", aV[0], " dbl_val\n", NULL);
	return TCL_ERROR;
    }
    if ( ! topPrsDbl( aV[1], &dblVal) ) {
	Tcl_AppendResult( ip, aV[0], ": invalid double\n", NULL);
	return TCL_ERROR;
    }
    Tcl_ResetResult( ip);
    sprintf( ip->result, "%g", dblVal);
    return TCL_OK;
}

static int
_topFmtDblCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    double	dblVal;

    if ( aC != 3 ) {
	Tcl_AppendResult( ip, "usage: ", aV[0], " format dbl_val\n", NULL);
	return TCL_ERROR;
    }
    if ( Tcl_GetDouble( ip, aV[2], &dblVal) != TCL_OK ) {
	Tcl_AppendResult( ip, aV[0], ": invalid double\n", NULL);
	return TCL_ERROR;
    }
    Tcl_ResetResult( ip);
    if ( ! topFmtDbl( ip->result, aV[1], dblVal) ) {
	Tcl_AppendResult( ip, "\n", aV[0], ": invalid format\n", NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

static int
_topFmtCalcCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    double	loVal, hiVal;
    int		len, sameDigs;

    if ( aC != 5 ) {
	Tcl_AppendResult( ip, "usage: ", aV[0], " format lo hi same\n", NULL);
	return TCL_ERROR;
    }
    if ( Tcl_GetDouble( ip, aV[2], &loVal) != TCL_OK ) {
	Tcl_AppendResult( ip, aV[0], ": invalid loVal\n", NULL);
	return TCL_ERROR;
    }
    if ( Tcl_GetDouble( ip, aV[3], &hiVal) != TCL_OK ) {
	Tcl_AppendResult( ip, aV[0], ": invalid hiVal\n", NULL);
	return TCL_ERROR;
    }
    if ( Tcl_GetInt( ip, aV[4], &sameDigs) != TCL_OK ) {
	Tcl_AppendResult( ip, aV[0], ": invalid hiVal\n", NULL);
	return TCL_ERROR;
    }
    Tcl_ResetResult( ip);
    len = topFmtDblCalcRange( aV[1], loVal, hiVal, sameDigs, ip->result);
    if ( len <= 0 ) {
	Tcl_AppendResult( ip, "\n", aV[0], ": invalid format\n", NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

void
topFmtRegisterCmds( Tcl_Interp *ip) {
    Tcl_CreateCommand( ip, "fmtdbl", _topFmtDblCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "fmtcalc", _topFmtCalcCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "fmtscale", _topFmtScaleCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "fmttype", _topFmtTypeCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "prsdbl", _topPrsDblCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "prsdbldbg", _topPrsDblDbgCmd, NULL, NULL);
}
