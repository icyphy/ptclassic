defstar {
	name		{ LMSLeak }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ E. A. Lee, Paul Haskell }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF dsp library }
	derivedFrom { LMS }
	desc {
An LMS-based adaptive filter in which the step size is input (to the
"step" input) every iteration. The "stepSize" state is unused.

Values in the "taps" array change as the filter adapts to minimize the
energy at the "error" input.

The "mu" state is used as a leak factor in the formula that updates
the filter coefficients.
	}

	explanation {
If two identical "LMSLeak" filters are used as an adaptive predictive
coder and decoder, then with "mu" nearly equal to but greater than 0.0,
the effects of channel errors between the coder and decoder will decay
away rather than accumulate. As "mu" increases, the effects of channel
errors decay away more quickly, but the size of the "error" input
increases also.

See pg 54 of _Adaptive_Filters_, Honig and Messerschmitt.
	}

	seealso { LMS }

	input {
		name { step }
		type { float }
		desc { Step-size for LMS algorithm. }
	}
	defstate {
		name { mu }
		type { float }
		default { 0.0 }
		desc { Leak factor for coefficient update. }
	}

	constructor { stepSize.clearAttributes(A_SETTABLE); }

	go {
// Read a new step-size for the coefficient update formula.
		stepSize = double(step%0);

// Update the taps.
		int index = int(errorDelay) * int(decimation) +
				int(decimationPhase);

		for(int i = 0; i < taps.size(); i++) {
			taps[i] *= 1.0 - double(stepSize) * double(mu);
			taps[i] += double(error%0) * double(signalIn%index) *
					double(stepSize);
			index++;
		}

// Run the filter.
		SDFFIR :: go();
	}
} // end defstar{ LMSLeak }
