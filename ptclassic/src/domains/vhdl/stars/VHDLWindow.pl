defstar {
	name { Window }
	domain { VHDL }
	desc {
Generate standard window functions or periodic repetitions of standard
window functions.  The possible functions are: Rectangle, Bartlett,
Hanning, Hamming, Blackman, Kaiser, and SteepBlackman.  One period of
samples is produced at each firing.
	}
	htmldoc {
<p>
This star produces on its output values that are samples of a standard
windowing function.
The windowing function is determined by the <i>name</i> string parameter.
Possible values are: <b>Rectangle</b>, <b>Bartlett</b>, <b>Hanning</b>,
<b>Hamming</b>, <b>Blackman</b>, <b>Kaiser</b> and <b>SteepBlackman</b>.
Upper- and lower-case characters in the names are equivalent.
<p>
The parameter <i>length</i> is the length of the window to produce.
Note that most window functions have a zero value at the first and last sample.
The parameter <i>period</i> specifies the period of the output signal:
the window will be zero-padded if required.
A <i>period</i> of 0 means a period equal to <i>length</i>.
A negative period will produce only one window, and then output zero
for all later samples.
A period of less than the window length will be equivalent to a period of
the window length (i.e., <i>period</i> = 0).
<h3>References</h3>
<p>[1]  
Leland Jackson, Digital Filters and Signal Processing, 2nd ed.,
Kluwer Academic Publishers, ISBN 0-89838-276-9, 1989.
	}
	version { $Id$ }
	author { Michael C. Williamson and Kennard White }
	acknowledge { William Chen and Brian Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }

	ccinclude { <string.h>, <math.h>, "ptdspWindow.h" }

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

        protected {
		int realLen;
		int realPeriod;
		double* windowTaps;
	}

	constructor {
		windowTaps = 0;
	}

	setup {
		const char* wn = name;
		int winType = Ptdsp_WindowNumber(wn);
		if (winType == PTDSP_WINDOW_TYPE_NULL) {
		    Error::abortRun(*this, "Unknown window name ", wn);
		    return;
		}

		// Don't want to risk divide by zero
		realLen = int(length);
		if ( realLen < 4 ) {
		    Error::abortRun(*this, "Window length is too small ",
				    "(should be greater than 3)");
		    return;
		}

                realPeriod = int(period);
		if ( realPeriod < realLen ) realPeriod = realLen;
		output.setSDFParams(realPeriod, realPeriod-1);

		delete [] windowTaps;
		windowTaps = new double[realLen];
		int validWindow = Ptdsp_Window(windowTaps, realLen, winType,
					      (double *) WindowParameters);
		if (! validWindow) {
		    Error::abortRun(*this, "Could not compute the taps for a ",
				    wn, " window: Ptdsp_Window failed.");
		    return;
		}
	}

	go {
		StringList out;
		int i = 0;
		for (; i < realLen; i++) {
			out << "$ref(output, ";
			out << -i;
			out << ") $assign(output) ";
			out << windowTaps[i];
			out << ";\n";
		}
                for (; i < realPeriod; i++) {
			out << "$ref(output, ";
			out << -i;
			out << ") $assign(output) ";
			out << 0.0;
			out << ";\n";
		}
		if ( int(period) < 0 ) realLen = 0;

		addCode(out);
		out.initialize();
	}

	destructor {
		delete [] windowTaps;
	}
}
