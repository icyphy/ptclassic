defcore {
    name { Impulse }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Impulse } 
    desc { 
Generate a single impulse or an impulse train.  By default, the
impulse(s) have unity (1.0) amplitude.  If "period" (default 0) is
equal to zero 0, then only a single impulse is generated; otherwise,
it specifies the period of the impulse train.  The impulse or impulse
train is delayed by the amount specified by "delay".
}
    version { @(#)ACSImpulseFPCGC.pl	1.6 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    codeblock(init) {
	/* If count != 0, then output 0.0 else output "level" */
	/* Increment count */
	$ref(output) = ($ref(count)++) ? 0.0 : $val(level);
    }

    codeblock(periodic) {
	/* Reset the counter to zero if one period has elapsed */
	if ($ref(count) >= $val(period)) $ref(count) = 0;
    }

    go {
	addCode(init);
	if (int(corona.period) > 0) addCode(periodic);
    }
}
