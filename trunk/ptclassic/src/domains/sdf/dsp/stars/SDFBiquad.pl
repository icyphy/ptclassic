defstar {
	name {Biquad}
	domain {SDF}
	version {$Id$}
	desc {
A two-pole, two-zero digital IIR filter (a biquad).
The default is a Butterworth filter with a cutoff 0.1 times sampling frequency.
	}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
.lp
This two-pole, two-zero IIR filter has a transfer function of
.EQ
{n sub 0 ~+~ n sub 1 z sup -1 ~+~ n sub 2 z sup -2} over
{1 ~+~ d sub 1 z sup -1 ~+~ d sub 2 z sup -2} ~.
.EN
.Id "filter, biquad"
.Id "biquad filter"
.Id "filter, IIR"
.Id "IIR filter"
It is implemented in direct form II [1].
The default coefficients give a Butterworth filter with a
cutoff of 0.1 times sampling frequency.
.Ir "filter, Butterworth"
.Ir "Butterworth filter"
.lp
\fBThis star will eventually be replaced by a general IIR star.\fR
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
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
