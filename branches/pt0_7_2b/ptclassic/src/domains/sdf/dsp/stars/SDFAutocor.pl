ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/20/90

 Estimates a certain number of samples of the autocorrelation of the input
 by averaging a certain number of input samples.  The number of outputs
 is twice the number of lags requested.  This makes the output almost
 symmetric (discard the last sample to get a perfectly symmetric output).

**************************************************************************/
}

defstar {
	name { Autocor }
	domain { SDF }
	desc { "Estimates an autocorrelation." }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name {noInputsToAvg}
		type {int}
		default {"256"}
		desc {"Number of input samples to average"}
	}
	defstate {
		name {noLags}
		type {int}
		default {"64"}
		desc {"Number of autocorrelation lags to output"}
	}
	start {
	    if ( int(noInputsToAvg) <= int(noLags)) {
		Error::abortRun(*this,
			": noLags is larger than noInputsToAvg.");
		return;
	    }
	    input.setSDFParams(int(noInputsToAvg), int(noInputsToAvg)-1);
	    output.setSDFParams(2*int(noLags), 2*int(noLags)-1);
	}
	go {
	    for(int i = int(noLags); i>=0; i--) {
		float sum = 0.0;
		for(int j = 0; j < (int(noInputsToAvg)-i); j++)
		   sum += double(input%j) * double(input%(j+i));
		output%(i+int(noLags)) << sum/(int(noInputsToAvg) -i);
	    }
	    // Now output the second half, which by symmetry is just
	    // identical to what was just output.
	    for(i = int(noLags)-1; i >= 0; i--)
		output%i = output%(2*int(noLags) - i);
	}
}
