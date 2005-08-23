defcore {
	name { Integrator }
	domain { ACS }
	coreCategory { FixSim }
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
	version { @(#)ACSIntegratorFixSim.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
       defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc {
Flag indicating whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the precision specified
by the parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 4.14 }
                desc {
Precision of the input in bits.
The input particles are only cast to this precision if the parameter
"ArrivingPrecision" is set to NO.
		}
        }
	defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
        defstate {
                name { FeedbackPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the feedbackGain in bits. }
        }
        defstate {
                name { AccumulationPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the accumulation in bits. }
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the output in bits. } 
        }
	defstate {
	    name { LockOutput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
	protected {
		Fix fixIn, spread, fstate, top, bottom, fbgain, out;
	}
	setup {
		
		fixIn = Fix( ((const char *) InputPrecision) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid InputPrecision" );

		spread = Fix( ((const char *) AccumulationPrecision) );
		if ( spread.invalid() )
		  Error::abortRun( *this, "Invalid AccumulationPrecision" );

		fstate = Fix( ((const char *) AccumulationPrecision) );
		fstate = double(corona.state);
		top = Fix( ((const char *) AccumulationPrecision) );
		bottom = Fix( ((const char *) AccumulationPrecision) );
		top = double(corona.top);
		bottom = double(corona.bottom);
		spread = top - bottom;

		fbgain = Fix( ((const char *) FeedbackPrecision) );
		if ( fbgain.invalid() )
		   Error::abortRun( *this, "Invalid FeedbackPrecision" );

		out = Fix( ((const char *) OutputPrecision) );
		if ( out.invalid() )
		   Error::abortRun( *this, "Invalid FeedbackPrecision" );
		
		fixIn.set_ovflow( ((const char *) OverflowHandler) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );

		spread.set_ovflow( ((const char *) OverflowHandler) );
		fstate.set_ovflow( ((const char *) OverflowHandler) );
		top.set_ovflow( ((const char *) OverflowHandler) );
		bottom.set_ovflow( ((const char *) OverflowHandler) );
		fbgain.set_ovflow( ((const char *) OverflowHandler) );
		out.set_ovflow( ((const char *) OverflowHandler) );

		
		fixIn.set_rounding( int(RoundFix) );
		spread.set_rounding( int(RoundFix) );
		fstate.set_rounding( int(RoundFix) );
		top.set_rounding( int(RoundFix) );
		bottom.set_rounding( int(RoundFix) );
		fbgain.set_rounding( int(RoundFix) );
		out.set_rounding( int(RoundFix) );

		fstate = double(corona.state);
		top = double(corona.top);
		bottom = double(corona.bottom);
		spread = top - bottom;
		fbgain = double(corona.feedbackGain);
	}
	go {
	    Fix t( ((const char *) AccumulationPrecision) );
	    Fix tmp = corona.data%0;
	    if ( int(ArrivingPrecision) )
		t = tmp + fbgain * fstate;
	    else {
		fixIn = tmp;
		t = fixIn + fbgain * fstate;
	    }
	    if (spread > Fix(0.0)) {
		    // Limiting is in effect

		    // Take care of the top
		if (t > top)
		    if (int(corona.saturate)) t = top;
		    else do t -= spread; while (t > top);

		    // Take care of the bottom
		if (t < bottom)
		    if (int(corona.saturate)) t = bottom;
		    else do t += spread; while (t < bottom);
	    }
	    corona.output%0 << t;
	    fstate = t;
	}
}
