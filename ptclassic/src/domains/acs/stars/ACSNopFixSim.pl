defcore {
	name { Nop }
	domain { ACS }
	coreCategory { FixSim }
	corona { Nop }
	desc { Does nothing }
	version {@(#)ACSNopFixSim.pl	1.3 09/08/99}
        author { Eric K. Pauer }
        copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
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
the OverflowHandler will be called.
                }
        }
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc {
Flag indicated whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". }
        }
	location { ACS main library }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
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
        }
        go {
                // We use a temporary variable to avoid gcc2.7.2/2.8 problems
                Fix tmp = corona.input%0;
                // all computations should be performed with out since that
                // is the Fix variable with the desired overflow handler
                if ( int(ArrivingPrecision) ) {
                  out = tmp;
                }
                else {
                  fixIn = tmp;
                  out = fixIn;
                }
                checkOverflow(out);
                corona.output%0 << out;
        }
}



