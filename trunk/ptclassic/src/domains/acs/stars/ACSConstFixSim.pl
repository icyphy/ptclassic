defcore {
    name { Const }
    domain { ACS }
    coreCategory { FixSim }
    corona { Const } 
    desc { Output the constant level as a fixed-point value. }
    version { @(#)ACSConstFixSim.pl	1.6 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
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
Output a fixed-point constant value with specified precision.
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
	out = Fix(corona.level);
    }
    go {
	corona.output%0 << out;
    }
}
