defstar {
	name { FixToCx }
	domain { CGC }
	version { $Id$ }
	author { J.Weiss }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC conversion palette }
	desc { Convert a fixed-point input to a complex output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { complex }
		desc { Output complex type }
	}
	state {
		name { numSample }
		type { int }
		default { "1" }
		desc { number of samples to be copied }
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
		$ref(output, i).real = FIX_Fix2Double($ref(input, i));
		$ref(output, i).imag = 0;
	}
   }
	go {
		addCode(body);
	}
}
