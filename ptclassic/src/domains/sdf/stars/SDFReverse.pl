defstar {
	name { Reverse }
	domain { SDF }
	desc {
On each execution, reads a block of "N" samples (default 64)
and writes them out backwards.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
