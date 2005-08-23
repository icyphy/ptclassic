defstar {
	name { LMSCx }
	domain { SDF }
	derivedFrom { FIRCx }
	desc {
A complex adaptive filter using the LMS adaptation algorithm.
The initial coefficients are given by the "taps" parameter.
The default initial coefficients give an 8th order, linear phase
lowpass filter.  To read initial coefficients from a file,
replace the default coefficients with "< fileName",
preferably specifying a complete path.
This star supports decimation, but not interpolation.
	}
	version { @(#)SDFLMSCx.pl	1.16	05/28/98 }
	author { J. T Buck, E. A. Lee }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	htmldoc {
When correctly used, this filter will adapt to try to minimize
the mean-squared error of the signal at its <i>error</i> input.
In order for this to be possible, the output of the filter should
be compared (subtracted from) some reference signal to produce
an error signal.
That error signal should be fed back to the <i>error</i> input.
The <i>delay</i> parameter must equal the total number of delays
in the path from the output of the filter back to the error input.
This ensures correct alignment of the adaptation algorithm.
The number of delays must be greater than zero or the dataflow
graph will deadlock.
The adaptation algorithm used is the well-known LMS, or stochastic-gradient
algorithm, generalized to use complex signals and filter taps.
<a name="adaptive filter, complex"></a>
<a name="filter, adaptive, complex"></a>
<a name="filter, LMS, complex"></a>
<a name="LMS adaptive filter, complex"></a>
<a name="stochastic gradient algorithm"></a>
<p>
If the <i>saveTapsFile</i> string is non-null, a file will
be created by the name given by that string, and the final tap values
will be stored there after the run has completed.
	}
	seealso {FIRCx, LMS, adaptFilter, LMSPlotCx, LMSTkPlotCx }
	input {
		name { error }
		type { complex }
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
	ccinclude { "miscFuncs.h", <stdio.h> }
	constructor {
		// remove interpolation as a settable parameter
		interpolation.clearAttributes(A_SETTABLE);
		// taps are no longer constant
		taps.clearAttributes(A_CONSTANT);
	}
	setup {
		// force interpolation to 1
		interpolation = 1;
		// Next run the ComplexFIR start routine
		SDFFIRCx :: setup();

		// Then reset the signalIn number of samples in the past
		// to account for the error delay.
		signalIn.setSDFParams(int(decimation),
				      int(decimation) + 1 + int(errorDelay)
				      + taps.size());
	}
	go {
		// We use a temporary variable to avoid gcc2.7.2/2.8 problems
		Complex tmpA = (error%0);
		// First update the taps
		Complex e = tmpA * double(stepSize);
		int index = int(errorDelay)*int(decimation) + int(decimationPhase);
		// adjustment is e times the conjugate of the output
		for (int i = 0; i < taps.size(); i++) {
			Complex tmpB = (signalIn%index);
			taps[i] = taps[i] + e * conj(tmpB);
			index++;
		}
		
		// Then run FIR filter
		SDFFIRCx :: go();
	}
	wrapup {
		const char* sf = saveTapsFile;
		if (sf != NULL && *sf != 0) {
			char* saveFileName = expandPathName(sf);
			// open the file for writing
			FILE* fp = fopen(saveFileName,"w");
			if (!fp) {
				Error::warn(*this, 
					"Cannot open file for writing: ",
					saveFileName,". Taps not saved.");
			} else {
				for (int i = 0; i < taps.size(); i++)
					fprintf(fp, "(%g ,%g)\n",
							taps[i].real(),
							taps[i].imag());
			}
			fclose(fp);
			delete [] saveFileName;
		}
	}
}
