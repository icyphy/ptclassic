defstar {
    name { Pre }
    domain { SR }
    derivedFrom { SRNonStrictStar }
    desc {
Delay a present input until the instant in which the next present instant
occurs.
}
    version { $Id$ }
    author { S. A. Edwards }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    input {
	name { input }
	type { int }
    }
    output {
	name { output }
	type { int }
    }
    state {
	name { theState }
	type { int }
	default { "0" }
	desc { Initial output value, state afterwards. }
    }
    private {
	int nextState;
    }
    go {	
	if ( !output.known() ) {
	    output.emit() << int(theState);
	}
	if ( input.present() ) {
	    nextState = int(input.get());
	}
    }
    tick {
	theState = nextState;
    }

}
