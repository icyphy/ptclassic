defstar {
	name { Quantizer }
	domain { SDF }
	desc {
Quantizes input to one of N+1 possible output levels using N thresholds.
For an input less than or equal to the n-th threshold, but larger than all previous
thresholds, the output will be the n-th level.  If the input is greater
than all thresholds, the output is the N+1-th level.
There must be one more level than thresholds.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name { thresholds }
		type { floatarray }
		default {0.0}
		desc { Quantization thresholds. }
	}
	defstate {
		name { levels }
		type { floatarray }
		default {"-1.0 1.0"}
		desc { Output levels. }
	}
	start {
	    if (levels.size() != thresholds.size()+1) {
	Error::abortRun (*this, ": must have one more level than thresholds");
	return;
	    }
	}
	go {
	    float in = float(input%0);
	    for(int i = 0; i < thresholds.size(); i++) {
		if( in <= thresholds[i] ) {
		    output%0 << levels[i];
		    return;
		}
		output%0 << levels[thresholds.size()];
	    }
	}
}
