defstar {
	name { IntToBits }
	domain { SDF }
	desc {
Read the least significant nBits bits from an integer input,
and output the bits as integers serially on the output,
most significant bit first.
	}
	version { $Id$ }
	author { J. T Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
