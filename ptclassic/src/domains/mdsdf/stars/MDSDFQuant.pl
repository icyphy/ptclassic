defstar {
	name { Quant }
	domain { MDSDF }
	desc {
Quantize the input value to one of N+1 possible output levels using N thresholds.
For an input less than or equal to the n-th threshold, but larger than all
previous thresholds, the output will be the n-th level.  If the input is
greater than all thresholds, the output is the N+1-th level.  If level is
specified, there must be one more level than thresholds; the default
value for level is 0, 1, 2, ... N.  This star is much slower than LinQuantIdx,
so if possible, that one should be used instead.
	}
	version { $Id$ }
	author { E. A. Lee and J. Buck, modified for MDSDF by Mike J. Chen }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	input {
		name {input}
		type { FLOAT_MATRIX }
	}
	output {
		name {output}
		type { FLOAT_MATRIX }
	}
        defstate {
		name { numRows }
		type { int }
		default { 4 }
		desc { Number of rows in the input block. }
	}
	defstate {
		name { numCols }
		type { int }
		default { 4 }
		desc { Number of columns in the input block. }
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
		input.setMDSDFParams(int(numRows),int(numCols));
		output.setMDSDFParams(int(numRows),int(numCols));

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
		FloatSubMatrix& matrix = *(FloatSubMatrix*)(input.getInput());
		FloatSubMatrix& out = *(FloatSubMatrix*)(output.getOutput());

		int siz = thresholds.size();
		for(int i = 0; i < int(numRows)*int(numCols); i++) {
	                int lo = 0;
	                int hi = siz;
		        int mid = (hi+lo)/2;
			double in = matrix.entry(i);
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

			out.entry(i) = levels[mid];
		}
	}
}
