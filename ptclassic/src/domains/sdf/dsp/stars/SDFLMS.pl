defstar {
	name { LMS }
	domain { SDF }
	derivedFrom { FIR }
	desc {
An adaptive filter using the Least-Mean Square (LMS) algorithm.
The initial filter coefficients are given by the "taps" parameter.
The default initial coefficients give an 8th-order, linear phase
lowpass filter.
To read initial coefficients from a file, replace the default
coefficients with "&lt; fileName", preferably specifying a complete path.
This star supports decimation, but not interpolation.
	}
	version {@(#)SDFLMS.pl	2.23 10/07/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	htmldoc {
When used correctly, this LMS adaptive filter will adapt to try to minimize
the mean-squared error of the signal at its <i>error</i> input [1].
<a name="adaptive filter"></a>
<a name="filter, adaptive"></a>
<a name="filter, LMS"></a>
<a name="LMS adaptive filter"></a>
In order for this to be possible, the output of the filter should
be compared (subtracted from) some reference signal to produce
an error signal.
That error signal should be fed back to the <i>error</i> input.
The <i>delay</i> parameter must equal the total number of delays
in the path from the output of the filter back to the error input.
This ensures correct alignment of the adaptation algorithm.
The number of delays must be greater than zero or the dataflow
graph will deadlock.
The adaptation algorithm used is the well-known LMS, or
stochastic-gradient algorithm.
<a name="stochastic gradient algorithm"></a>
<p>
If the <i>saveTapsFile</i> string is non-null, a file will
be created by the name given by that string, and the final tap values
will be stored there after the run has completed.
<a name="Haykin, S."></a>
<h3>References</h3>
<p>[1]  
S. Haykin, <i>Adaptive Filter Theory</i>,
Prentice Hall: Englewood Cliffs, NJ.  1991.  2nd ed.
	}
	seealso {FIR, adaptFilter, LMSCx, LMSPlot, LMSTkPlot, LMSPlotCx}
	input {
		name { error }
		type { float }
	}
	defstate {
		name { stepSize }
		type { float }
		default { "0.01" }
		desc { Adaptation step size. }
	}
	defstate {
		name { errorDelay }
		type { int }
		default { "1" }
		desc {  Delay in the update loop. }
	}
	defstate {
		name { saveTapsFile }
		type { string }
		default { "" }
		desc { File to save final tap values. }
	}
	ccinclude { "miscFuncs.h", <stdio.h>, "WriteASCIIFiles.h" }
	constructor {
		// remove interpolation as a settable parameter
		interpolation.clearAttributes(A_SETTABLE);
		// taps are no longer constant
		taps.clearAttributes(A_CONSTANT);
	}
	setup {
	// First check to be sure that interpolation is 1.
		interpolation = 1;

		// Next run the FIR start routine
		SDFFIR :: setup();

		// Then reset the signalIn number of samples in the past
		// to account for the error delay.
		signalIn.setSDFParams(int(decimation),
			int(decimation) + 1 + int(errorDelay) + taps.size());
	}
	go {
		// First update the taps
		int index = int(errorDelay) * int(decimation) +
			    int(decimationPhase);
		double factor = double(error%0) * double(stepSize);
		for (int i = 0; i < taps.size(); i++) {
		    taps[i] += factor * double(signalIn%(index));
		    index++;
		}

		// Then run FIR filter
		SDFFIR :: go();
	}
	wrapup {
		const char* sf = saveTapsFile;
		if ( ! doubleArrayAsASCFile(sf, "%d %g\n", TRUE,
					    (double*) taps, taps.size(), 0) ) {
		    Error::warn(*this, "Error saving taps to the file ", sf);
		}
	}
}
