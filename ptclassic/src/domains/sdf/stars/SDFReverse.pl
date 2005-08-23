defstar {
	name { Reverse }
	domain { SDF }
	desc {
On each execution, read a block of "N" samples (default 64)
and write them out backwards.
	}
	version {@(#)SDFReverse.pl	1.6 3/2/95}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{=input}
	}
	defstate {
		name {N}
		type {int}
		default {64}
		desc {Number of particles read and written.}
	}
	setup {
		input.setSDFParams(int(N),int(N)-1);
		output.setSDFParams(int(N),int(N)-1);
	}
	go {
		int i, j;
		for (i = 0, j = int(N) - 1; j >= 0; i++, j--)
			output%i = input%j;
	}
}
