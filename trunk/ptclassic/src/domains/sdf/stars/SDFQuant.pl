defstar {
	name { Quant }
	domain { SDF }
	desc {
Quantize the input value to one of N+1 possible output levels using N thresholds.
For an input less than or equal to the n-th threshold, but larger than all
previous thresholds, the output will be the n-th level.  If the input is
greater than all thresholds, the output is the N+1-th level.  If level is
specified, there must be one more level than thresholds; the default
value for level is 0, 1, 2, ... N.  This star is much slower than LinQuantIdx,
so if possible, that one should be used instead.
	}
	version {$Id$}
	author { E. A. Lee and J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
		desc { Quantization thresholds, in increasing order }
	}
	defstate {
		name { levels }
		type { floatarray }
		default {""}
		desc { Output levels.  If empty, use 0, 1, 2, ...}
	}
	setup {
		int n = thresholds.size();
		if (levels.size() == 0) {
			// default: 0, 1, 2...

			levels.resize(n + 1);
			for (int i = 0; i <= n; i++)
				levels[i] = i;
		}
		else if (levels.size() != n+1) {
			Error::abortRun (*this,
			      "must have one more level than thresholds");
		}
	}
	go {
		double in = input%0;
		int lo = 0;
		int siz = thresholds.size();
		int hi = siz;
		int mid = (hi+lo)/2;
		do {
			if (in <= thresholds[mid]) {
				hi = mid;
			} else {
				lo = mid+1;
			}
			mid = (hi+lo)/2;
		} while (mid < siz && hi > lo);

		// now in is <= thresholds[mid] but > all smaller ones.
		// (where thresholds[siz] is infinity)

		output%0 << levels[mid];
	}
}
