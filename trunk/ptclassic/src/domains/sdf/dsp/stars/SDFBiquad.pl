defstar {
	name {BiQuad}
	domain {SDF}
	version {$Id$}
	desc {
A two-pole, two-zero IIR filter (a biquad).
The default is a Butterworth filter with a cutoff 0.1 times sample freq.
The transfer function is (n0+n1*z1+n2*z2)/(1+d1*z1+d2*z2)
where z1 = {z sup -1}, z2 = {z sup -2}.
	}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
.lp
This two-pole, two-zero IIR filter has a transfer function of
.EQ
{n sub 0 ~+~ n sub 1 z sup -1 ~+~ n sub 2 z sup -2} over
{1 ~+~ d sub 1 z sup -1 ~+~ d sub 2 z sup -2} ~.
.EN
The default is a Butterworth filter with a cutoff 0.1 times sample freq.
.lp
\fBThis star will eventually be replaced by a general IIR star.\fR
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
		desc { internal state. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {state2}
		type { float }
		default { "0.0" }
		desc {internal state. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	go {
		double t = double(input%0) - double(d1) * double (state1) -
			double(d2) * double(state2);
		double o = t * double(n0) + double(state1) * double(n1) +
			double(state2) * double(n2);
		output%0 << o;
		state2 = double(state1);
		state1 = t;
	}
}
