defstar {
	name { Quant }
	domain { VHDL }
	desc {
Quantizes input to one of N+1 possible output levels using N thresholds.
For an input less than or equal to the n-th threshold, but larger than all
previous thresholds, the output will be the n-th level.  If the input is
greater than all thresholds, the output is the N+1-th level.  If level is
specified, there must be one more level than thresholds.
	}
	version { @(#)VHDLQuant.pl	1.3 08/27/97 }
	author { Michael C. Williamson, E. A. Lee and J. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { thresholds }
		type { floatarray }
		default { 0.0 }
		desc { Quantization thresholds, in increasing order }
	}
	defstate {
		name { levels }
		type { floatarray }
		default { "-1.0 1.0" }
		desc { Output levels }
	}
	setup {
		int n = thresholds.size();
		if (levels.size() != n+1) {
		    Error::abortRun (*this,
				     "must have one more level than thresholds");
		}
	}
	go {
	    StringList out;
	    int nthresh = thresholds.size();
	    int i=0;

	    out << "if ($ref(input) <= $ref(thresholds, " << i << ")) then\n";
	    out << "  $ref(output) $assign(output) $ref(levels, " << i << ");\n";
	    i++;

	    if (nthresh > 1) {
		while (i < nthresh) {
		    out << "else if ($ref(input) <= $ref(thresholds, " << i << ")) then\n";
		    out << "  $ref(output) $assign(output) $ref(levels, " << i << ");\n";
		    i++;
		}
	    }

	    out << "else\n";
	    out << "  $ref(output) $assign(output) $ref(levels, " << i << ");\n";
	    out << "end if;\n";

	    addCode (out);
	    out.initialize();
	}
}
