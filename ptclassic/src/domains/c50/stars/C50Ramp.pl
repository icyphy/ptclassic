defstar {
	name { Ramp }
	domain { C50 }
	desc {
Generate a ramp signal, starting at "value" (default 0) and
incrementing by step size "step" (default 1) on each firing.
	}
	version { @(#)C50Ramp.pl	1.4	01 Oct 1996 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
Generates a ramp signal, starting at "value" (default 0.0)
with step size "step" (default 0.001).
	}
	output {
		name { output }
		type { fix }
	}
	state {
		name { step }
		type { fix }
		default { 0.001 }
		desc { Increment from one sample to the next. }
		attributes { A_SETTABLE|A_UMEM }
	}
	state {
		name { value }
		type { fix }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
        state {
                name {sum}
                type {FIX}
                default {0}
                desc { An internal state.}
                attributes { A_NONSETTABLE|A_UMEM|A_NONCONSTANT }
	}

        codeblock (main) {
	mar 	*,AR5
	lar	AR0,#$addr(step)		;Address step		=> AR0
	lar	AR5,#$addr(sum)			;Address sum		=> AR5
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lacc	*,15,AR7			;Accu = sum
        sach    *,1,AR0				;sum => output
	add	*,15,AR5			;Accu = sum + step
        sach    *,1				;Accu => sum
	}

	setup {
		sum=value;
	}
	go {
		addCode(main);
	}
        execTime {
		return 8;
	}
}
