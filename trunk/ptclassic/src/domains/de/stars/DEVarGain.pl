defstar
{
    name { VarGain }
    domain { DE }
    derivedFrom { Gain }
    descriptor {
This star multiplies its input by a variable amount.
The "gain" parameter gives the initial amount,
and the gain is changed using the "newGain" input.
    }
    version { $Id$ }
    author { Bilung Lee }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    input {
	name { newGain }
	type { float }
    }

    constructor {
	// state is no longer constant
	gain.clearAttributes(A_CONSTANT);
    }

    go {
	if (newGain.dataNew)
	    gain = double(newGain.get());

	if (input.dataNew)
	    DEGain::go();
    }
}
