 defstar {
	name { MaxMin }
	domain { SDF }
	desc { Finds maximum or minimum, value or magnitude. }
	version { $Id$ }
	author { Brian L. Evans }
	acknowledge { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
If \fIcmpareMagnitude\fR is "no", the star finds from among
the \fIinput\fR inputs the one with the maximum or minimum value;
otherwise, it finds from among the \fIinput\fR inputs the one with
the maximum or minimum magnitude.
if \fIoutputMagnitude\fR is "yes", the magnitude of the result is
written to the output, else the result itself is written to the output.
Returns maximum value among N (default 10) samples.
Also, the index of the output is provided (count starts at 0).
.pp
This star is based on the MaxMin star in the CG56 domain.
	}
	seealso { Max Min }
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	output {
		name {index}
		type {int}
	}
	state {
		name {N}
		type {int}
		default {10}
		desc {default samples}
	}
	state {
		name {MAX}
		type {int}
		default {"YES"}
		desc {output maximum value else minimum is the output}
	}    
	state {
		name {compareMagnitude}
		type {int}
		default {"NO"}
		desc {default is not to compare magnitude}
	}    	
	state {
		name {outputMagnitude}
		type {int}
		default {"NO"}
		desc {default is not to output magnitude}
	}    

	code {
#define FABS(a)	( (a > 0.0) ? (a) : -(a) )
	}

	setup {
		if ( int(N) <= 0 ) {
		    Error::abortRun(*this,
				    "Number of samples, N, must be positive.");
		    return;
		}
		input.setSDFParams(int(N), int(N)-1);
	}

 	go {
		int maxflag = int(MAX);
		int cmpMagFlag = int(compareMagnitude);
		int i = int(N) - 1;
		double value = double(input%i);
		double valueCmp = cmpMagFlag ? FABS(value) : value;
		int valueIndex = i;

		// Walk through the input samples
		while ( i-- ) {
		    double current = double(input%i);
		    double currentCmp = cmpMagFlag ? FABS(current) : current;
		    int minChangeFlag = ( currentCmp > valueCmp );
		    // Logical exclusive OR between maxflag and minChangeFlag
		    // but we cannot use the bitwise xor ^
		    if ( (maxflag && !minChangeFlag) ||
			 (!maxflag && minChangeFlag) ) {
		      value = current;
		      valueCmp = currentCmp;
		      valueIndex = i;
		    }
		}

		// Output the value or the magnitude of the value
		if ( int(outputMagnitude) ) {
		    value = FABS(value);
		}

		output%0 << value;
		index%0 << valueIndex;
	}
}
