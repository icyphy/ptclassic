defstar
{
    name { VarDelay }
    domain { DE }
    derivedFrom { Delay }
    descriptor {
This star delays its input by a variable amount.
The "delay" parameter gives the initial delay,
and the delay is changed using the "newDelay" input.
    }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    input {
	name { newDelay }
	type { float }
    }

    constructor {
	// state is no longer constant
	delay.clearAttributes(A_CONSTANT);
    }

    go {
	if (newDelay.dataNew)
	    delay = newDelay.get();

	if (input.dataNew)
	    DEDelay::go();
    }
}