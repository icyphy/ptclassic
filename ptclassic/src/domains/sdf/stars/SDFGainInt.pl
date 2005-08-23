defstar {
	name { GainInt }
	domain { SDF }
	desc {
This is an amplifier; the integer output is the integer input
multiplied by the integer "gain" (default 1).
	}
	version {@(#)SDFGainInt.pl	1.4	3/2/95}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
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
		name { gain }
		type { int }
		default { "1" }
		desc { Gain of the amplifier. }
	}
	go {
		output%0 << int(gain) * int(input%0);
	}
}

