defstar {
    name {Window}
    domain {SDF}
    desc {
Generates various standard window functions, including Rectangular,
Hanning, Hamming, and Blackman.
    }
    version { $Id$ }
    author { Kennard White }
    copyright { 1991 The Regents of the University of California }
    location { SDF dsp library }
    explanation {
This star produce on its output values that are samples of
a standard windowing function.  The window function to be sampled
is determined by the \fIname\fR string parameter.  Possible values
are: \fBRectangle\fR, \fBHanning\fR, \fBHamming\fR, \fBBlackman\fR, 
and \fBSteepBlackman\fR.
.lp
The parameter \fIlength\fR is the length of the window to produce.  Note
that most windows functions have zero value at the first and last
sample.
The parameter \fIperiod\fR specifies the period of the output signal:
the window will be zero-padded if required.  A \fIperiod\fR of 0
means a period equel to \fIlength\fR, and a negative period will
produce only one window then be zero for all later samples.
    }
    output {
        name {output}
        type {float}
    }
    defstate {
	name {name}
	type {string}
	default {"HANNING"}
	desc {Name of the window function to generate.}
    }
    defstate {
	name {length}
	type {int}
	default {256}
	desc {Length of the window function to produce.} 
    }
    defstate {
	name {period}
	type {int}
	default {0}
	desc {Period of the output. Period 0 implies "length" period, and
	  negative period is non-periodic (single cycle).}
	attributes { A_NONCONSTANT|A_SETTABLE }
    }
//  defstate {
//	name {iter}
//	type {int}
//	default {0}
//	desc {Which period we are in.}
//	attributes { A_NONCONSTANT|A_NONSETTABLE }
//  }
    protected {
	int winType;
	int realLen;
	int realPeriod;
	double scale0;
	double scale1;
	double freq1;
	double scale2;
	double freq2;
    }
    code {
	extern "C" {
	    extern int strcasecmp( char*, char*);
	}
#define SDFWinType_Null		(0)
#define SDFWinType_Rectangle	(1)
#define SDFWinType_Hanning	(2)
#define SDFWinType_Hamming	(3)
#define SDFWinType_Blackman	(4)
#define SDFWinType_SteepBlackman	(5)
    }
    start {
	char *wn = name;

	/*IF*/ if ( strcasecmp( wn, "Rectangle")==0 ) {
	    winType = SDFWinType_Rectangle;
	} else if ( strcasecmp( wn, "Hanning")==0 ) {
	    winType = SDFWinType_Hanning;
	} else if ( strcasecmp( wn, "Hamming")==0 ) {
	    winType = SDFWinType_Hamming;
	} else if ( strcasecmp( wn, "Blackman")==0 ) {
	    winType = SDFWinType_Blackman;
	} else if ( strcasecmp( wn, "SteepBlackman")==0 ) {
	    winType = SDFWinType_SteepBlackman;
	} else {
	    Error::abortRun(*this, ": Unknown window name");
	    return;
	}
	realLen = int(length);
	if ( realLen < 4 ) {
	    /* Dont want to risk divide by zero */
	    Error::abortRun(*this, ": Window length too small");
	    return;
	}
	realPeriod = int(period);
	if ( realPeriod <= 0 )
	    realPeriod = realLen;
	output.setSDFParams( realPeriod, realPeriod-1);

	double base_w = M_PI/(realLen-1);
	double sin_base_w = sin(base_w);
	double sin_2base_w = sin(2*base_w);
	double d = - (sin_base_w/sin_2base_w) * (sin_base_w/sin_2base_w);
	scale0 = 0; scale1 = 0; freq1 = 0; scale2 = 0; freq2 = 0;
	/* 
	 *  Window defs taken from Jackson, Digital Filters and Signal
	 *  Processing, Second Ed, chap 7.
	 */
	switch ( winType ) {
	case SDFWinType_Rectangle:
	    scale0 = 1;
	    break;
	case SDFWinType_Hanning:
	    scale0 = .5;	scale1 = -.5;	freq1 = 2*base_w;
	    break;
	case SDFWinType_Hamming:
	    scale0 = .54;	scale1 = -.46;	freq1 = 2*base_w;
	    break;
	case SDFWinType_Blackman:
	    /* This is a special case of SteepBlackman */
	    d = -.16;		/* scale0 = .42, scale1 = -.5, scale2=.08 */
	    /*FALLTHROUGH*/
	case SDFWinType_SteepBlackman:
	    /* See Jackson 2ed, eqns 7.3.6 through 7.3.10 */
	    scale0 = (d+1)/2;	scale1 = -.5;	freq1 = 2*base_w;
	    			scale2 = -d/2;	freq2 = 4*base_w;
	    break;
	default:
	    Error::abortRun(*this, ": Invalid window type");
	    return;
	}
	    
    }
    go {
	int i;
	double val;

	for ( i=0; i < realLen; i++) {
	    val = scale0 + scale1 * cos(freq1*i) + scale2 * cos(freq2*i);
	    output%(realPeriod-i-1) << val;
	}
	for ( ; i < realPeriod; i++) {
	    output%(realPeriod-i-1) << double(0.0);
	}
	if ( int(period) < 0 )
	    realLen = 0;
    }
}	