
defstar {
        name { ToBusFl }
	domain { DE }
	desc { Converts from MPH to multiple single ports or vice-versa for floats }

	version {$Id$}
	author { Allen Lao }

	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}

	location { ATM demo library }

	inmulti {
		name {input}
		type {float}
	}

	outmulti {
		name {output}
		type {float}
	}

	go {

	   completionTime = arrivalTime;

	   InDEMPHIter nextp(input);
	   OutDEMPHIter nextq(output);

	   InDEPort* iport;
	   OutDEPort* oport;

	   while ((iport = nextp++) != 0) {
		oport = nextq++;
		if(iport->dataNew)
		    oport -> put(completionTime) = iport -> get();
	   }

	}

}   // end defstar

