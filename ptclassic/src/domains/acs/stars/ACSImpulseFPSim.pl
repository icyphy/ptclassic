defcore {
    name { Impulse }
    domain { ACS }
    coreCategory { FPSim }
    corona { Impulse } 
    desc { 
Generate a single impulse or an impulse train.  By default, the
impulse(s) have unity (1.0) amplitude.  If "period" (default 0) is
equal to zero 0, then only a single impulse is generated; otherwise,
it specifies the period of the impulse train.  The impulse or impulse
train is delayed by the amount specified by "delay".
}
    version { @(#)ACSImpulseFPSim.pl	1.1 05/07/98 }
    author { Eric Pauer }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    defstate {
	name { level }
	type{ float }
	default { 1.0 }
	desc { The height of the impulse(s). }
    }
    go {
	double t = 0.0;
	if (int(corona.count) == 0) t = level;
	corona.count = int(corona.count) + 1;
	if (int(corona.period) > 0 && int(corona.count) >= int(corona.period)) corona.count = 0;
	corona.output%0 << t;
    }
}
