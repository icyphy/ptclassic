defcore {
    name { Const }
    domain { ACS }
    coreCategory { FixSim }
    corona { Const } 
    desc { Output the constant level as a fixed-point value. }
    version { $Id$ }
    author { Eric Pauer }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    defstate {
	name { fixLevel }
	type { Fix }
	default { 0.0 }
	desc { The constant value. }
    }
    defstate {
        name { OutputPrecision }
	type { precision }
	default { 2.14 }
	desc {
Output a fixed-point constant value with specified precision.
        }
    }
	protected {
		Fix out;
	}
        setup {
		out = Fix( ((const char *) OutputPrecision) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
                out.set_ovflow( ((const char *) OverflowHandler) );
		if ( out.invalid() )
		   Error::abortRun( *this, "Invalid OverflowHandler" );
		out.set_rounding( int(RoundFix) );
		out = Fix(fixLevel);
        }
	go {
		corona.output%0 << out;
	}
}
