defstar {
	name { BitsToInt }
	domain { SDF }
	desc {
The integer input sequence is interpreted as a bit stream in which any nonzero
value is interpreted as to mean a "one" bit.
This star consumes "nBits" successive bits from the input,
packs them into an integer, and outputs the resulting integer.
The first received bit becomes the most significant bit of the output.
If "nBits" is larger than the integer word size, then the first bits
received will be lost.
If "nBits" is smaller than the wordsize minus one, then the
output integer will always be non-negative.
	}
	version { $Id$ }
	author { J. T Buck }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	setup {
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
