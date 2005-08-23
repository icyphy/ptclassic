defstar {
	name { FloatToCx }
	domain { VHDL }
	desc { type conversion from float/int to complex }
	version { @(#)VHDLFloatToCx.pl	1.5 03/07/96 }
	author { Michael C. Williamson, S. Ha }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { complex }
	}
	state {
		name { numSample }
		type { int }
		default { "1" }
		desc { number of samples to be converted }
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
	go {
	  StringList out;
	  for (int i = 0; i < int(numSample); i++) {
	    out << "$refCx(output, ";
	    out << -i;
	    out << ", real) $assign(output) $ref(input, ";
	    out << -i;
	    out << ");\n";
	    out << "$refCx(output, ";
	    out << -i;
	    out << ", imag) $assign(output) 0.0;\n";
	  }
	  addCode(out);
	  out.initialize();
	}
}
