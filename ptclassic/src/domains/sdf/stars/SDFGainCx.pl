defstar {
	name { GainCx }
	domain { SDF }
	desc {
Amplifier: output is input times "gain" (default 1.0).  Input, output,
and gain factor are all complex numbers.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name { input }
		type { complex }
	}
	output {
		name { output }
		type { complex }
	}
	defstate {
		name { gain }
		type { complex }
		default { "(1,0)" }
		desc { Gain of the star. }
	}
	go {
		output%0 << Complex(gain) * Complex(input%0);
	}
}

