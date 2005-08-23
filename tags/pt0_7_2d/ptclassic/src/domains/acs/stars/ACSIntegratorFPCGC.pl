defcore {
	name { Integrator }
	domain { ACS }
	coreCategory { FPCGC }
	corona { Integrator }
	desc {
This is an integrator with leakage and limits.
With the default parameters, input samples are simply accumulated,
and the running sum is the output.  

Limits are controlled by the "top" and "bottom" parameters.
If top &gt;= bottom, no limiting is performed (this is the default).
Otherwise, the output is kept between "bottom" and "top".
If "saturate" = YES, saturation is performed.  If "saturate" = NO,
wrap-around is performed (this is the default).
Limiting is performed before output.

Leakage is controlled by the "feedbackGain" state (default 1.0).
The output is the data input plus feedbackGain*state, where state
is the previous output.
	}
	version { @(#)ACSIntegratorFPCGC.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
	protected {
		double spread;
	}
	setup {
		spread = double(corona.top) - double(corona.bottom);
	}
	codeblock (declarations) {
	    double $starSymbol(t);
	}
	initCode {
	    addDeclaration(declarations);
	}
	codeblock (integrate) {
	    $starSymbol(t) = $ref(data) +
			$val(feedbackGain) * $ref(state);
	}
	codeblock (limitWithSat) {
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if ($starSymbol(t) > $val(top)) $starSymbol(t) = $val(top);
	    /* Take care of the bottom */
	    if ($starSymbol(t) < $val(bottom)) $starSymbol(t) = $val(bottom);
	}
	codeblock (limitWithoutSat) {
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if ($starSymbol(t) > $val(top))
		do $starSymbol(t) -= ($val(top) - $val(bottom));
		while ($starSymbol(t) > $val(top));
	    /* Take care of the bottom */
	    if ($starSymbol(t) < $val(bottom))
		do $starSymbol(t) += ($val(top) - $val(bottom));
		while ($starSymbol(t) < $val(bottom));
	}
	codeblock (write) {
	    $ref(output) = $starSymbol(t);
	    $ref(state) = $starSymbol(t);
	}
	go {
	    addCode(integrate);
	    if (spread > 0.0) 
		if (int(corona.saturate))
		    addCode(limitWithSat);
		else
		    addCode(limitWithoutSat);
	    addCode(write);
	}
	exectime {
		int x = 0;
		if (spread > 0.0) {
			if (int(corona.saturate)) x = 3;
			else x = 5;
		}
		return 3 + x + 2;
	}
}
