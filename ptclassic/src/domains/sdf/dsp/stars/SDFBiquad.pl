ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
			All Rights Reserved.

Programmer: J. T. Buck
Date of creation: 7/23/90

This star is a two-pole, two-zero IIR filter (a biquad).

************************************************************************/
}

defstar {
	name {BiQuad}
	domain {SDF}
	desc {
		"Second order IIR filter\n"
		"Default is Butterworth with cutoff 0.1 times sample freq.\n"
		"Transfer function is (n0+n1*z1+n2*z2)/(1+d1*z1+d2*z2)\n"
		"where z1 = {z sup -1}, z2 = {z sup -2}."
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	defstate {
		name {d1}
		type { float }
		default { "-1.1430" }
	}
	defstate {
		name {d2}
		type { float }
		default { "0.41280" }
	}
	defstate {
		name {n0}
		type { float }
		default { "0.067455" }
	}
	defstate {
		name {n1}
		type { float }
		default { "0.135" }
	}
	defstate {
		name {n2}
		type { float }
		default { "0.067455" }
	}
	defstate {
		name {state1}
		type { float }
		default { "0.0" }
		desc { "state1 (internal)"}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {state2}
		type { float }
		default { "0.0" }
		desc {"state2 (internal)"}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	go {
		double t = float(input%0) - double(d1) * double (state1) -
			double(d2) * double(state2);
		double o = t * double(n0) + double(state1) * double(n1) +
			double(state2) * double(n2);
		output%0 << o;
		state2 = double(state1);
		state1 = t;
	}
}
