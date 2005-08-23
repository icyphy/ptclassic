defcore {
    name { Delay }
    domain { ACS }
    coreCategory { FixSim }
    corona { Delay } 
    desc { Provides explicit pipeline delay (default 1). }
    version{ @(#)ACSDelayFixSim.pl	1.5 08/02/01 }
    author { Eric Pauer }
    copyright {
Copyright (c) 1999-2001 Sanders, a Lockheed Martin Company
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
Flag indicated whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". }
    }
    defstate {
      name { InputPrecision }
      type { precision }
      default { 2.14 }
      desc {
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
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
      name { OutputPrecision }
      type { precision }
      default { 2.14 }
      desc {
Precision of the output in bits.
This is the precision that will hold the result of the arithmetic operation
on the inputs.
When the value of the product extends outside of the precision,
the OverflowHandler will be called.  }
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
      Fix fixIn, out;
    }
    setup {
      ACSFixSimCore::setup();
      
      if ( ! int(ArrivingPrecision) ) {
	fixIn = Fix( ((const char *) InputPrecision) );
	if ( fixIn.invalid() )
	  Error::abortRun( *this, "Invalid InputPrecision" );
      }
      fixIn.set_rounding( int(RoundFix) );
      
      out = Fix( ((const char *) OutputPrecision) );
      if ( out.invalid() )
	Error::abortRun( *this, "Invalid OutputPrecision" );
      out.set_ovflow( ((const char *) "saturate") );
      if ( out.invalid() )
	Error::abortRun( *this, "Invalid OverflowHandler" );
      out.set_rounding( int(TRUE) );
      
      corona.input.setSDFParams(1, int(corona.delays));
      Fix zero = Fix( ((const char *) OutputPrecision) );
      for (int i = 0; i < int(corona.delays); i++)
	corona.input%i << zero;
    }
    go {
      Fix tmp = corona.input%(int(corona.delays));
      out = tmp;
      corona.output%0 << out;
    }
}
