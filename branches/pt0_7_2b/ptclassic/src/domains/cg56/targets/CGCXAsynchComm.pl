defstar {
    name { XABase }
    domain { CGC }
    desc { Base star for Asynchronous CGC <-> S56X IPC }
    ccinclude { "CGCS56XTarget.h" }
    derivedFrom { XBase }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright { 
Copyright (c) 1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    location { CG56 Target Directory }

    state {
	    name { VariableName }
	    type { string }
	    default { "aioVariable" }
	    desc {The name used to identify the asynchronous connection.}
    }

    state {
	    name { updateRate }
	    type { int }
	    default { 1 }
	    desc {Update the link at most every 'updateRate' schedule iterations.}
	}
	
	state {
		name { iterationCount }
		type { int }
		default { 1 }
		attributes { A_NONSETTABLE | A_NONCONSTANT }
		desc {Internal state to keep track of iteration number.}
	}
	
	codeblock(updateLink,"const char* code") {
		if ($ref(iterationCount)++ == $val(updateRate)) {
			$ref(iterationCount) = 1;
		        {
@code
		        }
		}
	}
    setup() {
	    CGCS56XTarget* parent = (CGCS56XTarget*)(myTarget()->parent());
	    if(parent) S56XFilePrefix.setCurrentValue(parent->s56xFilePrefix());
	    CGCXBase::setup();
    }
}
