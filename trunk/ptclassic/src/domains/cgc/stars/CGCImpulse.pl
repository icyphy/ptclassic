defstar {
	name { Impulse }
	domain { CGC }
	desc {
Generate a stream of impulses of size "level" (default 1.0).
The period is given by "period" (default 0).
If period = 0 then only one impulse is generated.
	}
	version { $Id$ }
	author { J. Weiss }
	acknowledge { SDF version by J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
		desc { The height of the impulse. }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc { The period of the impulse train, 0 = aperiodic. }
	}
	state {
		name { numSample }
		type { int }
		default { "1" }
		desc { number of samples to be generated in a single run }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { An internal state. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}

	go {
@		$ref(output) = ($ref(count) == 0) ? $val(level) : 0.0;
@		++$ref(count);
	     if (int(period) > 0) {
@		if ($ref(count) >= $val(period))
@		    $ref(count) = 0;
	     }
	}
}
