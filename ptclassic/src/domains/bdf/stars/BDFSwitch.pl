defstar {
	name { Switch }
	domain { BDF }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	author { J. T. Buck }
	location { BDF main library }
	desc {
Switches input tokens to one of two outputs, depending on
the value of the control input.  The parameter N gives the
number of tokens read.  If the token read from control is true, the
values are written to trueOutput; otherwise they are written to
falseOutput.
	}
	defstate {
		name { N }
		type { int }
		default { 1 }
		desc { Number of data tokens to move per execution }
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
			out%0 = input%0;
	}
}


