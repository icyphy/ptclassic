defstar {
	name { Abs }
	domain { SDF } 
	desc { Output the absolute value of the input value. }
	version { @(#)SDFAbs.pl	1.3	01 Oct 1996 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
Outputs absolute value of the input.
<a name="absolute value"></a>
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	go {
		double t = input%0;
		if (t < 0.0) t = -t;
		output%0 << t;
	}
}
