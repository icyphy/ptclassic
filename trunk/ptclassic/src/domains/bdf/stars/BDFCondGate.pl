defstar {
	name { True }
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
If the value on the "control" input is nonzero, the input particle
is copied to output. Otherwise, no input is consumed (except the control
particle) and no output is produced.  This is effectively one half
of a Select.
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
		name { output }
		type { =input }
	}
	constructor {
		input.setBDFParams(1,control,BDF_TRUE);
		output.setBDFParams(1,control,BDF_TRUE, 0);
	}
	go {
		if (int(control%0)) {
			output%0 = input%0;
		}
	}
}
