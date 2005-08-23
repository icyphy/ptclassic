defstar {
	name { Window }
	domain { CGC }
	derivedFrom { WaveForm }
	desc {
Generates standard window functions:
Rectangle, Bartlett, Hanning, Hamming, Kaiser, Blackman, and SteepBlackman.
}
	version { @(#)CGCWindow.pl	1.5	10/06/96 }
	author { Brian L. Evans }
	acknowledge { Jose Luis Pino and Kennard White }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
<p>
This star produces on its output values that are samples of a standard
windowing function.  The window function to be sampled is determined by
the <i>name</i> string parameter.  Possible values are: <b>Rectangle</b>,
<b>Bartlett</b>, <b>Hanning</b>, <b>Hamming</b>, <b>Blackman</b>, and
<b>SteepBlackman</b>.  Upper and lower case characters in the names are
equivalent.
<p>
The parameter <i>length</i> is the length of the window to produce.  Note
that most windows functions have zero value at the first and last sample.
The parameter <i>period</i> specifies the period of the output signal:
the window will be zero-padded if required.  A <i>period</i> of 0
means a period equal to <i>length</i>.  A negative period will
produce only one window, and then outputs zero for all later samples.
A period of less than window length will be equivalent to a period of
window length (i.e, period=0).
<p>
One period of samples are produced on every firing.
	}

	state {
		name { name }
		type { string }
		default { "Hanning" }
		desc {
Name of the window function to generate:
Rectangle, Bartlett, Hanning, Hamming, Kaiser, Blackman, or SteepBlackman.
		}
	}

	state {
		name { length }
		type { int }
		default { 256 }
		desc { Length of the window function to produce. }
		attributes { A_SETTABLE|A_CONSTANT }
	}
	
	state {
		name { WindowParameters}
		type { floatarray}
		default { 0 }
		desc { 
An array of numeric parameters for the window.
For the Kaiser window, the first entry in this state is taken as the
beta parameter which is proportional to the stopband attenuation of
the window.
		} 
	}

	protected {
		// must be persistent data or there will be an error
		// in value.setInitValue(window) because window will
		// be destroyed if it were a local variable in setup
		StringList window;
    	}

	constructor {
		value.setAttributes(A_NONSETTABLE|A_CONSTANT);
	}

	ccinclude { <string.h>, <math.h>, "ptdspWindow.h" }

	setup {
		const char* wn = name;
		int winType = Ptdsp_WindowNumber(wn);
		if (winType == PTDSP_WINDOW_TYPE_NULL) {
		    Error::abortRun(*this, "Unknown window name ", wn);
		    return;
		}

		// Don't want to risk divide by zero
		int realLen = int(length);
		if ( realLen < 4 ) {
		    Error::abortRun(*this, "Window length is too small ",
				    "(should be greater than 3)");
		    return;
		}

		double* windowTaps = new double[realLen];
		int validWindow = Ptdsp_Window(windowTaps, realLen, winType,
					      (double *) WindowParameters);
		if (! validWindow) {
		    delete [] windowTaps;
		    Error::abortRun(*this, "Could not compute the taps for a ",
				    wn, " window: Ptdsp_Window failed.");
		    return;
		}

		value.resize(int(length));
		window.initialize();		// initialize to a null string
		for (int i = 0; i < realLen; i++) {
			window << windowTaps[i] << " ";
		}
		delete [] windowTaps;

		value.setInitValue(window);
		value.initialize();
		CGCWaveForm::setup();
	}
}
