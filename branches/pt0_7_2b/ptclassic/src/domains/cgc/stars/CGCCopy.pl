defstar {
	name { Copy }
	domain { CGC }
	desc { copy input to output }
	version { @(#)CGCCopy.pl	1.5	1/29/96 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { anytype }
	}
	output {
		name { output }
		type { =input }
	}
	state {
		name { numSample }
		type { int }
		default { "1" }
		desc { number of samples to be copied }
	}
	constructor {
		noInternalState();
	}
	setup {
		if (int(numSample) > 1) {
			input.setSDFParams(int(numSample));
			output.setSDFParams(int(numSample));
		}
	}
	initCode {
		numSample = output.numXfer();
	}
	codeblock (floatBody) {
	int i;
	for (i = 0; i < $val(numSample); i++) {
		$ref(output, i) = $ref(input, i);
	}
	}
	codeblock (complexBody) {
	int i = 0;
	for ( ; i < $val(numSample); i++) {
		$ref(output, i).real = $ref(input, i).real;
		$ref(output, i).imag = $ref(input, i).imag;
	}
	}
	go {
		if (strcmp(input.resolvedType(), "COMPLEX") == 0) 
			addCode(complexBody);
		else
			addCode(floatBody);
	}
	exectime {
		if (strcmp(input.resolvedType(), "COMPLEX") == 0) 
			return 2 * int(numSample);
		else
			return int(numSample);
	}
}

