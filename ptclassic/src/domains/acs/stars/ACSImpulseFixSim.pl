defcore {
    name { Impulse }
    domain { ACS }
    coreCategory { FixSim }
    corona { Impulse } 
    desc { 
Generate a single impulse or an impulse train.  By default, the
impulse(s) have unity (1) amplitude.  If "period" (default 0) is
equal to zero 0, then only a single impulse is generated; otherwise,
it specifies the period of the impulse train.  The impulse or impulse
train is delayed by the amount specified by "delay".
}
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
	default { "1.0" }
	desc { The height of the impulse(s). }
    }
    defstate {
        name { OutputPrecision }
	type { precision }
	default { 2.14 }
	desc {
Impulse(s) will have height with specified precision.
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
    }
    go {
	out = 0.0;
	if (int(corona.count) == 0) out = Fix(fixLevel);
	corona.count = int(corona.count) + 1;
	if (int(corona.period) > 0 && int(corona.count) >= int(corona.period)) corona.count = 0;
	corona.output%0 << out;
    }
}