defstar {
	name { Select }
	domain { BDF }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { @(#)BDFSelect.pl	2.13	1/1/96 }
	author { J. T. Buck }
	location { BDF main library }
	desc {
If the value on the "control" line is nonzero, N particles (from the
parameter N) from "trueInput" are copied to the output; otherwise, 
N particles from "falseInput" are copied to the output.
	}
	defstate {
		name { N }
		type { int }
		default { 1 }
		desc { Number of data particles to move per execution }
	}
	input {
		name { trueInput }
		type { ANYTYPE }
	}
	input {
		name { falseInput }
		type { =trueInput }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =trueInput }
	}
	setup {
		int n = N;
		trueInput.setBDFParams(n,control,BDF_TRUE,n-1);
		falseInput.setBDFParams(n,control,BDF_FALSE,n-1);
		output.setBDFParams(n,0,BDF_NONE,n-1);
	}
	go {
		// read control value, and route input to output
		// depending on it.
		PortHole& input = *(int(control%0) ? &trueInput : &falseInput);
		for (int i = int(N)-1; i >= 0; i--)
			output%i = input%i;
	}
}


