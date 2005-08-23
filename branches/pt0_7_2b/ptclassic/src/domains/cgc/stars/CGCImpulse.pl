defstar {
	name { Impulse }
	domain { CGC }
	desc {
Generate a single impulse or an impulse train.  The impulse(s) have
amplitude "level" (default 1.0).  If "period" (default 0) is equal to 0,
then only a single impulse is generated; otherwise, it specifies the
period of the impulse train.  The impulse or impulse train is delayed
by the amount specified by "delay".
	}
	version { @(#)CGCImpulse.pl	1.4 6/6/96 }
	author { J. Weiss }
	acknowledge { SDF version by J. T. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { level }
		type { float }
		default { 1.0 }
		desc { Height of the impulse }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc {
Non-negative period of the impulse train (0 means aperiodic)
		}
	}
	defstate {
		name { delay }
		type { int }
		default { 0 }
		desc {
Non-negative delay on the output (0 means no delay)
		}
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { An internal state }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}

	setup {
		if (int(period) < 0) {
			Error::abortRun(*this, "Period must be non-negative");
		}
		if (int(delay) < 0) {
			Error::abortRun(*this, "Delay must be non-negative");
		}
		if (int(period) == 0){
			count = - int(delay);
		}
		else {
			count = - (int(delay)%int(period));
		}
	}

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
		if (int(period) > 0) addCode(periodic);
	}
}
