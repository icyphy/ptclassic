defstar {
	name { Quant }
	domain { SDF }
	desc {
Quantize the input value to one of N+1 possible output levels using
N thresholds.  For an input less than or equal to the n-th threshold,
but larger than all previous thresholds, the output will be the n-th
level.  If the input is greater than all thresholds, the output is
the N+1-th level.  If the input is less than all thresholds, then the
output is the zeroth level.

If level is specified, there must be one more level than thresholds.
The default value for level is 0, 1, 2, ... N.  This star takes on the
order of log N steps to find the right level, whereas the linear
quantizer star LinQuantIdx takes a constant amount of time.  Therefore,
if you doing linear quantization, use the LinQuantIdx star.
	}
	version { $Id$ }
	author { E. A. Lee and J. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
Assume that the <i>thresholds</i> parameter is set to <i></i>(<i></i>8<i>.</i>1<i>, </i>9<i>.</i>2<i>, </i>10<i>.</i>3<i></i>)<i></i>,
and that the <i>levels</i> parameter is not set so that the default values
of <i></i>(<i></i>0<i>.</i>0<i>, </i>1<i>.</i>0<i>, </i>2<i>.</i>0<i>, </i>3<i>.</i>0<i></i>)<i></i> are used.  Then, an input of <i>-</i>1<i>.</i>5<i></i> would give
an output of <i></i>0<i>.</i>0<i></i>, an input of <i></i>8<i>.</i>2<i></i> would give an output of <i></i>1<i>.</i>0<i></i>, and
an input of <i></i>15<i>.</i>5<i></i> would give an output of <i></i>3<i>.</i>0<i></i>.
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
	defstate {
		name { quantizationLevel }
		type { int }
		default {0}
		desc { The quantization level }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	setup {
		int n = thresholds.size();
		quantizationLevel = 0;

		if (levels.size() == 0) {	// set to default: 0, 1, 2...
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
		double in = double(input%0);
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

		quantizationLevel = mid;
		output%0 << levels[mid];
	}
}
