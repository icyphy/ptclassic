ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck

 A very crude phase unwrapper.  It basically assumes that the phase
 never changes by more than PI in one iteration, and that we "catch"
 all phase transitions.  We also assume that the input is in the [-PI,PI]
 range.

**************************************************************************/
}

defstar {
	name { Unwrap }
	domain { SDF }
	desc {
	"A very crude phase unwrapper.  It basically assumes that the phase\n"
	"never changes by more than PI in one iteration, and that we 'catch'\n"
	"all phase transitions.  We also assume that the input is in the\n"
	"[-PI,PI]  range."
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
		desc { "current phase of the star" }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	state {
		name { prevPhase }
		type { float }
		default { "0.0" }
		desc { "previous phase input value" }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
		double newPhase = float(input%0);
		// compute the phase change and check for wraparound
		double phaseChange = newPhase - double(prevPhase);
		if (phaseChange < -PI) phaseChange += 2*PI;
		if (phaseChange > PI) phaseChange -= 2*PI;
		outPhase = double(outPhase) + phaseChange;
		output%0 << float(double(outPhase));
		prevPhase = newPhase;
	}
}
