defstar {
	name { LMS }
	domain { CGC }
	derivedFrom { FIR }
	desc {
Adaptive filter using LMS adaptation algorithm.
Initial coefficients are in the "taps" state variable.
Default initial coefficients give an 8th order, linear phase
lowpass filter.  To read default coefficients from a file,
replace the default coefficients with "<fileName".
Supports decimation, but not interpolation.
	}
	version {$Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { cgc main library }
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
algorithm.
.IE "stochastic gradient algorithm"
.lp
If the \fIsaveTapsFile\fR string is non-null, a file will
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
	start {
	// First check to be sure that interpolation is 1.
		interpolation = 1;

		// Next run the FIR start routine
		CGCFIR :: start();

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
		gencode(bodyDecl);
		gencode(update1);
		gencode(CodeBlock("\t{\n"));
		gencode(update2);
		gencode(CodeBlock("\t}\n"));
		filterBody();		// from FIR
	}

   codeblock(update1) {
	int ix;
	/* First update the taps */
	double e = $ref(error);
	int index = $val(errorDelay)*$val(decimation) + $val(decimationPhase);

	for (ix = 0; ix < $val(tapSize); ix++) 
   }
   codeblock(update2) {
		$ref2(taps,ix) = $ref2(taps,ix) +
			e * $ref2(signalIn,index) * $val(stepSize);
		index++;
   }

	wrapup {
		const char* sf = saveTapsFile;
		if (sf != NULL && *sf != 0) {
			gencode(CodeBlock("    {\n"));
			gencode(save);
			gencode(CodeBlock("    }\n"));
		}
	}

   codeblock(save) {
    FILE* fp;
    int i;
    if (!(fp = fopen(saveFileName,"w"))) 
	/* File cannot be opened */
	fprintf(stderr,"ERROR: Cannot open saveTapsFile for writing:\n");
    if (!fp) exit(1);
    for (i = 0; i < $val(tapSize); i++)
	fprintf(fp, "%d %g\n", i, $ref2(taps,i));
    fclose(fp);
    }
}
