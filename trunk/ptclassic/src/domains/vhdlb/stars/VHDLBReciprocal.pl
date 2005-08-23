defstar {
	name { Reciprocal }
	domain {VHDLB}
	desc {
Output the reciprocal of the input, with an optional magnitude limit.
If the magnitude limit is greater than zero, and the input value is zero,
then the output will equal the magnitude limit.
	}
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
This star computes the reciprocal of the input value x.  If the "magLimit"
parameter is not zero, then the output is bound by +/- magLimit.  In this case
even if the input is zero, no divide-by-zero exception will be caused.
        }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	state {
		name { magLimit }
		type { float }
		default { 0.0 }
		desc { If non-zero, this limits the magnitude of the output }
	}
	go {
	}
}
