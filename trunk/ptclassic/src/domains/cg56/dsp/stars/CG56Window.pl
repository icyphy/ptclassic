defstar {
	name { Window }
	domain { CG56 }
	derivedFrom { WaveForm }
	desc {
Generates standard window functions:
Rectangle, Bartlett, Hanning, Hamming, Blackman, and SteepBlackman.
}
	version { $Id$ }
	author { Jose Luis Pino, based on CG56WaveForm by Kennard White}
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	explanation {
.lp
This star produces on its output values that are samples of a standard
windowing function.  The window function to be sampled is determined by
the \fIname\fR string parameter.  Possible values are: \fBRectangle\fR,
\fBBartlett\fR, \fBHanning\fR, \fBHamming\fR, \fBBlackman\fR, and
\fBSteepBlackman\fR.  Upper and lower case characters in the names are
equivalent.
.lp
The parameter \fIlength\fR is the length of the window to produce.  Note
that most windows functions have zero value at the first and last sample.
The parameter \fIperiod\fR specifies the period of the output signal:
the window will be zero-padded if required.  A \fIperiod\fR of 0
means a period equal to \fIlength\fR.  A negative period will
produce only one window, and then outputs zero for all later samples.
A period of less than window length will be equivalent to a period of
window length (i.e, period=0).
.lp
One period of samples are produced on every firing.
	}

	state {
		name { name }
		type { string }
		default { "Hanning" }
		desc {
Name of the window function to generate:
Rectangle, Bartlett, Hanning, Hamming, Blackman, or SteepBlackman.
		}
	}
	state {
		name { length }
		type { int }
		default { 256 }
		desc { Length of the window function to produce. }
		attributes { A_SETTABLE|A_CONSTANT }
	} 

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
		    extern int strcasecmp(const char*, const char*);
		}
#define SDFWinType_Null		(0)
#define SDFWinType_Rectangle	(1)
#define SDFWinType_Bartlett	(2)
#define SDFWinType_Hanning	(3)
#define SDFWinType_Hamming	(4)
#define SDFWinType_Blackman	(5)
#define SDFWinType_SteepBlackman	(6)
	}

	constructor {
		value.setAttributes(A_NONSETTABLE|A_CONSTANT);
	}

	setup {
	const char *wn = name;

	/*IF*/ if ( strcasecmp( wn, "Rectangle")==0 ) {
	    winType = SDFWinType_Rectangle;
	} else if ( strcasecmp( wn, "Bartlett")==0 ) {
	    winType = SDFWinType_Bartlett;
	} else if ( strcasecmp( wn, "Hanning")==0 ) {
	    winType = SDFWinType_Hanning;
	} else if ( strcasecmp( wn, "Hamming")==0 ) {
	    winType = SDFWinType_Hamming;
	} else if ( strcasecmp( wn, "Blackman")==0 ) {
	    winType = SDFWinType_Blackman;
	} else if ( strcasecmp( wn, "SteepBlackman")==0 ) {
	    winType = SDFWinType_SteepBlackman;
	} else {
	    Error::abortRun(*this, ": Unknown window name ", wn);
	    return;
	}

	// Don't want to risk divide by zero
	realLen = int(length);
	if ( realLen < 4 ) {
	    Error::abortRun(*this, ": Window length too small");
	    return;
	}

	double base_w = M_PI/(realLen-1);
	double sin_base_w = sin(base_w);
	double sin_2base_w = sin(2*base_w);
	double d = - (sin_base_w/sin_2base_w) * (sin_base_w/sin_2base_w);
	scale0 = 0; scale1 = 0; freq1 = 0; scale2 = 0; freq2 = 0;

	// Window defs taken from Jackson, Digital Filters and Signal
	//  Processing, Second Ed, chap 7.
	switch ( winType ) {
	  case SDFWinType_Rectangle:
	    scale0 = 1;
	    break;
	  case SDFWinType_Bartlett:
	    scale0 = 0;		scale1 = 2.0/realLen;
	    break;
	  case SDFWinType_Hanning:
	    scale0 = .5;	scale1 = -.5;	freq1 = 2*base_w;
	    break;
	  case SDFWinType_Hamming:
	    scale0 = .54;	scale1 = -.46;	freq1 = 2*base_w;
	    break;
	  case SDFWinType_Blackman:
	    // This is a special case of SteepBlackman
	    d = -.16;		// scale0 = .42, scale1 = -.5, scale2=.08
	    // FALLTHROUGH
	case SDFWinType_SteepBlackman:
	    // See Jackson 2ed, eqns 7.3.6 through 7.3.10
	    scale0 = (d+1)/2;
	    scale1 = -.5;
	    freq1 = 2*base_w;
	    scale2 = -d/2;
	    freq2 = 4*base_w;
	    break;
	default:
	    Error::abortRun(*this, ": Invalid window type");
	    return;
	}

	value.resize(int(length));
	StringList window;
	int i;
	double val;

	switch ( winType ) {
	  case SDFWinType_Bartlett:
	    for ( i=0; i < realLen/2; i++) {
		val = scale1 * (i+1);
		window << val << " ";
	    }
	    for ( ; i < realLen; i++) {
		val = scale1 * (realLen-(i+1));
		// Outputs created at the time the star is compiled
		// Handled as a special case by Waveform::go method.
		output%(realPeriod-(i+1)) << val;
		window << val;
		if (i != realLen-1) window << " ";
	    }
	    break;
	  default:
	    for ( i=0; i < realLen; i++) {
		if (i!=0) window << " ";
		val = scale0 + scale1 * cos(freq1*i) + scale2 * cos(freq2*i);
		window << val;
	    }
	}
	value.setInitValue(window);
	value.initialize();
	CG56WaveForm::setup();
}
}
