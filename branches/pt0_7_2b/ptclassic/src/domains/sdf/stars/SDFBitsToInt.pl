defstar {
	name { BitsToInt }
	domain { SDF }
	desc {
Reads "nBits" bits from the input, packs them into an integer, and outputs
it.  The first received bit becomes the most significant bit of the output.
	}
	version { $Id$ }
	author { J. T Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { nBits }
		type { int }
		default { 4 }
		desc { number of bits read per execution }
	}
	start {
		if (int(nBits) < 0) {
			Error::abortRun(*this, "nBits must be positive");
			return;
		}
		input.setSDFParams(int(nBits),int(nBits)-1);
	}
	go {
		int o = 0;
		for (int i = int(nBits) - 1; i >= 0; i--) {
			// shift new bits into the "shift register"
			o <<= 1;
			int b = int(input%i) ? 1 : 0;
			o += b;
		}
		output%0 << o;
	}
}