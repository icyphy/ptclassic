defstar {
	name { Switch }
	domain { BDF }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	author { J. T. Buck }
	location { BDF main library }
	desc {
Switch input particles to one of two outputs, depending on
the value of the control input.  The parameter N gives the
number of particles read in one firing.  If the particle read
from the control input is TRUE, then the values are written
to "trueOutput"; otherwise they are written to "falseOutput".
	}
	defstate {
		name { N }
		type { int }
		default { 1 }
		desc { Number of data particles to move per execution }
	}
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { trueOutput }
		type { =input }
	}
	output {
		name { falseOutput }
		type { =input }
	}
	setup {
		int n = N;
		input.setBDFParams(n, 0, BDF_NONE, n-1);
		trueOutput.setBDFParams(n, control, BDF_TRUE, n-1);
		falseOutput.setBDFParams(n, control, BDF_FALSE, n-1);
	}
	go {
		// read control value, and route input
		// to output depending on it.
		PortHole& out = *(int(control%0) ? &trueOutput : &falseOutput);
		for (int i = int(N)-1; i >= 0; i--)
			out%i = input%i;
	}
}


