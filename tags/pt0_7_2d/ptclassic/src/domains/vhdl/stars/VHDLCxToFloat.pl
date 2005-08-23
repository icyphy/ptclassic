defstar {
	name { CxToFloat }
	domain { VHDL }
	desc { type conversion from complex to float/int }
	version { @(#)VHDLCxToFloat.pl	1.4 03/07/96 }
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
	}
	go {
	  StringList out;
	  for (int i = 0; i < int(numSample); i++) {
	    out << "$ref(output, ";
	    out << -i;
	    out << ") $assign(output) $refCx(input, ";
	    out << -i;
	    out << ", real);\n";
	  }
	  addCode(out);
	  out.initialize();
	}
}
