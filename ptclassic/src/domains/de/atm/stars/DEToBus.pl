defstar {
	name {ToBus}
	domain {DE}
	version {$Id$}
	author { Allen Lao }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ATM demo library }

	desc { Converts from MPH to multiple single ports or vice-versa for messages }

	inmulti {
		name {input}
		type {message}
	}

	outmulti {
		name {output}
		type {message}
	}

	ccinclude { "Message.h" }

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

}


