 defstar {
	name { MaxMin }
	domain { CGC }
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

	codeblock(macros) {
#define FABS(a)	( ((a) > 0.0) ? (a) : -(a) )
	}

	codeblock(decl) {
	double current, currentCmp, value, valueCmp;
	int i, minChangeFlag, valueIndex = i;
	int cmpMagFlag, maxflag;
	}

	codeblock(initData) {
	cmpMagFlag = $val(compareMagnitude);
	maxflag = $val(MAX);
	i = $val(N) - 1;
	value = $ref(input,i);
	valueCmp = $val(cmpMagFlag) ? FABS(value) : value;
	valueIndex = i;
	}

	codeblock(compareData) {
	while ( i-- >= 0 ) {
		current = $ref(input,i);
		currentCmp = cmpMagFlag ? FABS(current) : current;
		minChangeFlag = ( currentCmp < valueCmp )

		/* Logical exclusive OR between maxflag and minChangeFlag
		   but we cannot use the bitwise xor ^ */
		if ( (maxflag && !minChangeFlag) ||
		     (!maxflag && minChangeFlag) ) {
			value = current;
			valueCmp = currentCmp;
			valueIndex = i;
		}
	}
	}

	codeblock(outputValue) {
	/* Output the value or the magnitude of the value */
	if ( $val(outputMagnitude) ) {
		value = FABS(value);
	}

	/* Send the maximum/minimum value to the output port */
	$ref(output) = value;

	// Adjust the index due to the LIFO nature of input data
	$ref(index) = $val(N) - valueIndex - 1;
	}

	initCode {
		addCode(macros);
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
		addCode(decl);
		addCode(initData);
		addCode(compareData);
		addCode(outputValue);
	}
}