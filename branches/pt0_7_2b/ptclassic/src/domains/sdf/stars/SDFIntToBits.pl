defstar {
	name { IntToBits }
	domain { SDF }
	desc {
Reads the least significant "nBits" bits from an integer input, and
outputs the bits serially on the output, most significant bit first.
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
		output.setSDFParams(int(nBits),int(nBits)-1);
	}
	go {
		int o = int(input%0);
		for (int i = 0; i < int(nBits); i++) {
			output%i << (o & 1);
			o >>= 1;
		}
	}
}