defstar {
	name { LMSCx }
	domain { SDF }
	derivedFrom { FIRCx }
	desc {
Complex adaptive filter using LMS adaptation algorithm.
Initial coefficients are in the "taps" state variable.
Default initial coefficients give an 8th order, linear phase
lowpass filter.  To read initial coefficients from a file,
replace the default coefficients with "<fileName".
Supports decimation, but not interpolation.
	}
	version { $Id$ }
	author { J. T Buck, E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
When correctly used, this filter will adapt to try to minimize
the mean-squared error of the signal at its \fIerror\fR input.
In order for this to be possible, the output of the filter should
be compared (subtracted from) some reference signal to produce
an error signal.
That error signal should be fed back to the \fIerror\fR input.
The \fIdelay\fR parameter must equal the total number of delays
in the path from the output of the filter back to the error input.
This ensures correct alignment of the adaptation algorithm.
The number of delays must be greater than zero or the dataflow
graph will deadlock.
The adaptation algorithm used is the well-known LMS, or stochastic-gradient
algorithm, generalized to use complex signals and filter taps.
.Id "adaptive filter, complex"
.Id "filter, adaptive, complex"
.Id "filter, LMS, complex"
.Id "LMS adaptive filter, complex"
.Ir "stochastic gradient algorithm"
.lp
If the \fIsaveTapsFile\fR string is non-null, a file will
be created by the name given by that string, and the final tap values
will be stored there after the run has completed.
	}
	seealso {FIRCx, LMS, adaptFilter}
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
	ccinclude { "miscFuncs.h" }
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
		// First update the taps
		Complex e = Complex(error%0) * double(stepSize);
		int index = int(errorDelay)*int(decimation) + int(decimationPhase);
		// adjustment is e times the conjugate of the output
		for (int i = 0; i < taps.size(); i++) {
			taps[i] = taps[i] + e * conj(Complex(signalIn%index));
			index++;
		}
		
		// Then run FIR filter
		SDFFIRCx :: go();
	}
	wrapup {
		const char* sf = saveTapsFile;
		if (sf != NULL && *sf != 0) {
			const char* saveFileName = expandPathName(sf);
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
		}
	}
}
