defstar {
	name { Unwrap }
	domain { SDF }
	desc {
A very crude phase unwrapper.  It basically assumes that the phase
never changes by more than PI in one iteration, and that we "catch"
all phase transitions.  It also assumes that the input is in the
[-PI,PI]  range.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
.Id "phase unwrapping"
	}
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	state {
		name { outPhase }
		type { float }
		default { "0.0" }
		desc {  Current phase of the star. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	state {
		name { prevPhase }
		type { float }
		default { "0.0" }
		desc { Previous phase input value. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
		double newPhase = input%0;
		// compute the phase change and check for wraparound
		double phaseChange = newPhase - double(prevPhase);
		if (phaseChange < -M_PI) phaseChange += 2*M_PI;
		if (phaseChange > M_PI) phaseChange -= 2*M_PI;
		outPhase = double(outPhase) + phaseChange;
		output%0 << double(outPhase);
		prevPhase = newPhase;
	}
}
