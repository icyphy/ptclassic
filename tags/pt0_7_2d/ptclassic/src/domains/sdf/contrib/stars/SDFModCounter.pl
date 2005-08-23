defstar {
    name { ModCounter }
    domain { SDF }
    desc {
Given any input, this star will output the number which tells the
    number of iterations. 
    }
    author { Biao Lu }
    acknowledge { Brian L. Evans }
    location { SDF contrib library }
    version { @(#)SDFModCounter.pl	1.1	03/24/98 }
    copyright {
Copyright (c) 1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
    input {
	name { input }
	type { ANYTYPE }
	desc { 
Whenever the tokens of any type pass through the star, the counter
    will count up. 
             }
    }
    output {
	name { output }
	type { int }
	desc {
The output is the number of iterations.
	    }
    }
    private {
	int maxvalue;
    }
    state {
	name { modulo }
	type { int }
	default { 10 }
    }
    state {
	name { countUp }
	type { int }
	default { YES }
    }
    state {
	name { initValue }
	type { int }
	default { 0 }
    }
    state {
	name { value }
	type { int }
	default { 0 }
	desc { Count the current iteration }
	attributes { A_NONCONSTANT | A_NONSETTABLE }
    }
    setup {
	if (int(modulo) <= 0) {
	    StringList msg = "The modulo";
		msg << int(modulo)
		    << " should be greater than zero.";
		Error::abortRun(*this, msg);
                return;
	}
	if (int(initValue) < 0 || int(initValue) >= int(modulo)) {
	    StringList msg = "The initial value";
		msg << int(initValue)
		    << " should be between zero and modulo";
		Error::abortRun(*this, msg);
                return;
	}
	maxvalue = int(modulo)-1;
    }
    go {
	// output the current value
	    output%0 << int(value);
	
	// update the current value
	    if ( int(countUp) ) {
		if ( int(value) == maxvalue )
		    value = 0;
		else     
		    value = int(value) + 1;
	    }
	else {
	    if ( int(value) == 0 )
		value = maxvalue;
	    else
		value = int(value) - 1;
	}       
    }
}
