defstar {
    name { Window }
    domain { VHDL }
    desc {
Generate standard window functions or periodic repetitions of standard
window functions.
The possible functions are: Rectangle, Bartlett, Hanning, Hamming,
Blackman, Kaiser, and SteepBlackman.
One period of samples is produced at each firing.
    }
    explanation {
.lp
This star produces on its output values that are samples of a standard
windowing function.
The windowing function is determined by the \fIname\fR string parameter.
Possible values are: \fBRectangle\fR, \fBBartlett\fR, \fBHanning\fR,
\fBHamming\fR, \fBBlackman\fR, \fBKaiser\fR and \fBSteepBlackman\fR.
Upper- and lower-case characters in the names are equivalent.
.lp
The parameter \fIlength\fR is the length of the window to produce.
Note that most window functions have a zero value at the first and last sample.
The parameter \fIperiod\fR specifies the period of the output signal:
the window will be zero-padded if required.
A \fIperiod\fR of 0 means a period equal to \fIlength\fR.
A negative period will produce only one window, and then output zero
for all later samples.
A period of less than the window length will be equivalent to a period of
the window length (i.e., \fIperiod\fR = 0).
.UH REFERENCES
.ip [1]
Leland Jackson, Digital Filters and Signal Processing, 2nd ed.,
Kluwer Academic Publishers, ISBN 0-89838-276-9, 1989.
    }
    version { $Id$ }
    author { Michael C. Williamson, Kennard White }
    acknowledge { William Chen }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { VHDL main library }

    // Include function prototypes from Cephes Math Library
    // Necessary for Kaiser window for i0 (0th-order modified Bessel function)
    ccinclude { <string.h>, <math.h>, "cephes.h" }

    output {
        name { output }
        type { float }
    }

    defstate {
	name { name }
	type { string }
	default { "Hanning" }
	desc {
Name of the window function to generate:
Rectangle, Bartlett, Hanning, Hamming, Blackman, SteepBlackman, or Kaiser.
	}
    }

    defstate {
	name { length }
	type { int }
	default { 256 }
	desc { Length of the window function to produce. } 
    }

    defstate {
	name { period }
	type { int }
	default { 0 }
	desc {
Period of the output. Period 0 implies "length" period, and
a negative period is non-periodic (single cycle).
	}
	attributes { A_NONCONSTANT|A_SETTABLE }
    }

    defstate {
        name { WindowParameters }
	type { floatarray }
	default { 0 }
	desc {
An array of numeric parameters for the window.
For the Kaiser window, the first entry in this state is taken as the
beta parameter which is proportional to the stopband attenuation of
the window.
	}
    }

    defstate {
        name { WindowValues }
	type { floatarray }
	default { 0 }
	desc {
An array of numeric values for the window.
The values to be output will be calculated and
stored in this array in preparation for output.
	}
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
	double beta;
	double alpha;
	double norm;
    }

    code {
#define SDFWinType_Null			(0)
#define SDFWinType_Rectangle		(1)
#define SDFWinType_Bartlett		(2)
#define SDFWinType_Hanning		(3)
#define SDFWinType_Hamming		(4)
#define SDFWinType_Blackman		(5)
#define SDFWinType_SteepBlackman	(6)
#define SDFWinType_Kaiser		(7)
    }

    setup {
	// Extract the string data from the string state data structure name
	const char *wn = name;

	if ( strcasecmp( wn, "Rectangle")==0 ) {
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
	} else if ( strcasecmp( wn, "Kaiser")==0 ) {
	    winType = SDFWinType_Kaiser;
	} else {
	    Error::abortRun(*this, ": Unknown window name", wn);
	    return;
	}

	// Don't want to risk divide by zero
	realLen = int(length);
	if ( realLen < 4 ) {
	    Error::abortRun(*this, ": Window length is too small",
			    " (should be greater than 3)" );
	    return;
	}
	realPeriod = int(period);
	if ( realPeriod < realLen )
	    realPeriod = realLen;
	output.setSDFParams(realPeriod, realPeriod-1);

        beta = double(WindowParameters[0]);

       	double base_w = M_PI/(realLen-1);
	double sin_base_w = sin(base_w);
	double sin_2base_w = sin(2*base_w);
	double d = - (sin_base_w/sin_2base_w) * (sin_base_w/sin_2base_w);
	scale0 = scale1 = scale2 = freq1 = freq2 = 0.0;
	// 
	//  Window defs taken from Jackson, Digital Filters and Signal
	//  Processing, Second Ed, chap 7.
	//
	switch ( winType ) {
	  case SDFWinType_Rectangle:
	    scale0 = 1.0;
	    break;
	  case SDFWinType_Bartlett:
	    scale1 = 2.0/(realLen - 1);
	    break;
	  case SDFWinType_Hanning:
	    scale0 = .5;
	    scale1 = -.5;
	    freq1 = 2*base_w;
	    break;
	  case SDFWinType_Hamming:
	    scale0 = .54;
	    scale1 = -.46;
	    freq1 = 2*base_w;
	    break;
	  case SDFWinType_Blackman:
	    // This is a special case of SteepBlackman
	    d = -.16;		// scale0 = .42, scale1 = -.5, scale2=.08
	    // FALLTHROUGH (omit break statement)
	  case SDFWinType_SteepBlackman:
	    // See Jackson 2ed, eqns 7.3.6 through 7.3.10
	    scale0 = (d+1)/2;
	    scale1 = -.5;
	    freq1 = 2*base_w;
	    scale2 = -d/2;
	    freq2 = 4*base_w;
	    break;
	  case SDFWinType_Kaiser:
            alpha = double((realLen-1.0)/2.0);
	    norm = fabs(i0(beta));
            break;
          default:
	    Error::abortRun(*this, ": Invalid window type", wn);
	    return;
	}

	// Adapted from go() method of SDFWindow
        int i;
	StringList windowVals = "";
	
	switch ( winType ) {
	  case SDFWinType_Bartlett:
	    for ( i=0; i <= (realLen - 1)/2; i++) {
		double val = scale1 * i;
//		output%(realPeriod-(i+1)) << val;
		windowVals << val << " ";
	    }
	    for ( ; i < realLen; i++) {
		double val = scale1 * (realLen-(i+1));
//		output%(realPeriod-(i+1)) << val;
		windowVals << val << " ";
	    }
	    break;
          case SDFWinType_Kaiser:
            for ( i=0; i < realLen; i++) {
                double squared = double(pow(((i - alpha)/ alpha),2));
                double entry = beta* sqrt(1-squared);
                double val = i0(entry)/norm;
//     		output%(realPeriod-(i+1)) << val;
		windowVals << val << " ";
            }
            break;
	  default:
	    for ( i=0; i < realLen; i++) {
		double val = scale0 + scale1 * cos(freq1*i) + scale2 * cos(freq2*i);
//		output%(realPeriod-(i+1)) << val;
		windowVals << val << " ";
	    }
	}
	for ( ; i < realPeriod; i++) {
//	  output%(realPeriod-(i+1)) << double(0.0);
	  windowVals << double(0.0) << " ";
	}
	if ( int(period) < 0 )
	    realLen = 0;

	WindowValues.setInitValue(windowVals);
	WindowValues.initialize();
    }

    go {
      StringList out;
      for (int i = 0; i < int(realPeriod); i++) {
	out << "$ref(output, ";
	out << -i;
	out << ") $assign(output) ";
	out << WindowValues[i];
	out << ";\n";
      }
      addCode(out);
      out.initialize();
    }
}
