defcore {
    name { Source }
    domain { ACS }
    coreCategory { FixSim }
    corona { Source } 
    desc { Generic code generator source star. }
    version {$Id$}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 Sanders, a Lockheed Martin Company
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
      t = Fix(0.0);
      corona.output%0 << t;
    }
}
