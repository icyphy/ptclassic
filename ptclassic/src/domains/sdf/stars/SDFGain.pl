defstar {
	name { FloatGain }
	domain { SDF }
	desc { Amplifier: output is input times "gain" (default 1.0). }
	version {$Revision$ $Date$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { gain }
		type { float }
		default { "1.0" }
		desc { Gain of the star. }
	}
	go {
		output%0 << double(gain) * (float)(input%0);
	}
}

