defcore {
    name { Ramp }
    domain { ACS }
    coreCategory { FixSim }
    corona { Ramp } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSRampFPSim.pl	1.1 03/09/98 }
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	defstate {
		name { fstep }
		type { Fix }
		default { 1.0 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { fvalue }
		type { Fix }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits and precision of the accumulation.
When the value of the accumulation extends outside of the precision,
the OverflowHandler will be called.
                }
        }
	protected {
		Fix t;
	}
        setup {
		t = Fix( ((const char *) OutputPrecision) );
		if ( t.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
                t.set_ovflow( ((const char *) OverflowHandler) );
		if ( t.invalid() )
		   Error::abortRun( *this, "Invalid OverflowHandler" );
		t.set_rounding( int(RoundFix) );
        }
	go {
		t = Fix(fvalue);
		corona.output%0 << t;
		t += Fix(fstep);
		fvalue = t;
	}

}
