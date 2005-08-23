defstar {
	name { LMS }
	domain { CGC }
	derivedFrom { FIR }
	desc {
Adaptive filter using LMS adaptation algorithm.
Initial coefficients are in the "taps" state variable.
Default initial coefficients give an 8th order, linear phase
lowpass filter.  To read default coefficients from a file,
replace the default coefficients with "fileName".
Supports decimation, but not interpolation.
	}
	version {@(#)CGCLMS.pl	1.15	10/08/96}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { cgc main library }
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
algorithm.
<a name="stochastic gradient algorithm"></a>
<p>
If the <i>saveTapsFile</i> string is non-null, a file will
be created by the name given by that string, and the final tap values
will be stored there after the run has completed.
	}
	seealso {FIR}
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
	constructor {
		// remove interpolation as a settable parameter
		interpolation.clearAttributes(A_SETTABLE);
		// taps are no longer constant
		taps.clearAttributes(A_CONSTANT);
	}
	setup {
	// First check to be sure that interpolation is 1.
		interpolation = 1;

		// Next run the FIR setup routine
		CGCFIR :: setup();

		// Then reset the signalIn number of samples in the past
		// to account for the error delay.
		signalIn.setSDFParams(int(decimation),
				      int(decimation) + 1 + int(errorDelay)
				      + taps.size());
	}
	initCode {
		addInclude("<stdio.h>");
	}
	go {
		addCode(bodyDecl);	// from FIR
		addCode(update);
		addCode(body);		// from FIR
	}

   codeblock(update) {
	int ix;
	/* First update the taps */
	double e = $ref(error);
	int index = $val(errorDelay)*$val(decimation) + $val(decimationPhase);

	for (ix = 0; ix < $val(tapSize); ix++) {
		$ref2(taps,ix) = $ref2(taps,ix) +
			e * $ref2(signalIn,index) * $ref(stepSize);
		index++;
	}
   }

	wrapup {
		const char* sf = saveTapsFile;
		if (sf != NULL && *sf != 0) {
			addCode("    {\n");
			addCode(save);
			addCode("    }\n");
		}
	}

   codeblock(save) {
    FILE* fp;
    int i;
    if (!(fp = fopen(saveFileName,"w"))) {
	/* File cannot be opened */
	fprintf(stderr,"ERROR: Cannot open saveTapsFile for writing:\n");
    	exit(1);
    }
    for (i = 0; i < $val(tapSize); i++)
	fprintf(fp, "%d %g\n", i, $ref2(taps,i));
    fclose(fp);
   }
}
