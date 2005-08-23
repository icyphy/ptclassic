defcore {
        name { Accum }
	domain { ACS }
	coreCategory { FPSim }
	corona { Accum }
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
	version { @(#)ACSIntegratorFPSim.pl	1.2 09/08/99 }
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
	go {
	    double t;
	    t = double(corona.data%0) + double(corona.feedbackGain) * double(corona.state);
	    if (spread > 0.0) {
		    // Limiting is in effect

		    // Take care of the top
		if (t > double(corona.top))
		    if (int(corona.saturate)) t = double(corona.top);
		    else do t -= spread; while (t > double(corona.top));

		    // Take care of the bottom
		if (t < double(corona.bottom))
		    if (int(corona.saturate)) t = double(corona.bottom);
		    else do t += spread; while (t < double(corona.bottom));
	    }
	    corona.output%0 << t;
	    corona.state = t;
	}
}


