defstar {
	name { WF4 }
	domain { SDF }
	desc {

	}
	version {$Id$}
	author { Eric Pauer }
	copyright {
	}
	location { SDF Wildforce library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { step }
		type { float }
		default { 1.0 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { float }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
#ifndef PT_WILDFORCE
            Error::abortRun("Sorry, PT_WILDFORCE was not defined at compile."
                    "Perhaps you do not have an Annapolis Wildforce board?");
#else
		double t = value;
		output%0 << t;
		t += step;
		value = t;
#endif
	}
}
