defstar {
	name { CxToFloat }
	domain { CGC }
	desc { type conversion from complex to float/int }
	version { $Id$ }
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { complex }
	}
	output {
		name { output }
		type { float }
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
		addInclude("<math.h>");
	}
	codeblock (body) {
	int i;
	double p,q;
	for (i = 0; i < $val(numSample); i++) {
		p = $ref(input, i).real;
		q = $ref(input, i).imag;
		$ref(output, i) = sqrt(p*p + q*q);
	}
	}
	go {
		addCode(body);
	}
	exectime {
		return int(numSample) * 200;
	}
}

