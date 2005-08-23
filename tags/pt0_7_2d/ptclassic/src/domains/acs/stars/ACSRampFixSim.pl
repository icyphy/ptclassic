defcore {
    name { Ramp }
    domain { ACS }
    coreCategory { FixSim }
    corona { Ramp } 
    desc { Generate a ramp signal, starting at "value" (default 0) and
incrementing by step size "step" (default 1) on each firing.
    }
    version { @(#)ACSRampFixSim.pl	1.14 08/02/01 }
    author { James Lundblad }
    copyright {
Copyright (c) 1998-2001 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
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
    defstate {
	name { LockOutput }
	type {int}
	default {"NO"}
	desc { 
	    Flag that indicates that the specified output precision should be used 
		rather than modified by wordlength analysis in the FPGA domain }
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
      t = Fix(corona.value);
      corona.output%0 << t;
      t += Fix(corona.step);
      corona.value = t;
    }
}
