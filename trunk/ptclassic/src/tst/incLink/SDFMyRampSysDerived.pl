defstar {
	name { MyRampSysDerived }
	domain { SDF }
        derived {Ramp}
       	desc {
Generate a ramp signal, starting at "value" (default 0) and
incrementing by step size "step" (default 1) on each firing.
	}
	version {$Id$}
	author { Christopher Hylands }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { dynamic linking test directory }
	defstate {
		name { step }
		type { float }
		default { 4.0 }
		desc { Increment from one sample to the next. }
	}
	go {

    		SDFRamp::step = step;
		SDFRamp::go();
	}
}
