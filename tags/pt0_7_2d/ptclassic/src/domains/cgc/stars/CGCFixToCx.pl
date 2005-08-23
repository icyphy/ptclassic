defstar {
	name { FixToCx }
	domain { CGC }
	derivedFrom { Fix }
	version { @(#)CGCFixToCx.pl	1.6 7/10/96 }
	author { J. Weiss }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
		if (int(numSample) <= 0) {
			Error::abortRun(*this, "numSample must be positive");
			return;
		}
		input.setSDFParams(int(numSample));
		output.setSDFParams(int(numSample));
	}

	// an initCode method is inherited from CGCFix
	// if you define your own, you should call CGCFix::initCode()

	codeblock (body) {
		int i = 0;
		for (; i < $val(numSample); i++) {
			$ref(output,i).real = FIX_Fix2Double($ref(input,i));
			$ref(output,i).imag = 0;
		}
	}

	go {
		addCode(body);
	}

	// a wrap-up method is inherited from CGCFix
	// if you define your own, you should call CGCFix::wrapup()
}
