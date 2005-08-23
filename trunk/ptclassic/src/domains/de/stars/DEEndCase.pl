defstar {
	name {EndCase}
	domain {DE}
	desc {
Select an input event from one of N inputs,
as specified by the last received control input.
The value of the control input must be between
0 and N-1 inclusive, or an error is flagged.
	}
	version { @(#)DEEndCase.pl	1.3	06/04/96 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	inmulti {
		name {input}
		type {anytype}
	}
	input {
		name {control}
		type {int}
	}
	output {
		name {output}
		type {=input}
	}
	constructor {
		control.triggers();
		control.before(input);
	}
	go {
		InDEMPHIter nexti(input);
		InDEPort* iportp = 0;
		// the control value could be positive or negative
		for (int i = int(control%0); i >= 0; i--) {
		    iportp = nexti++;
		    if (!iportp) break;
		}
		// iportp will be zero if the control value is out of range
		if (!iportp) {
		    int maxControl = input.numberPorts() - 1;
		    StringList msg = "The control value ";
		    msg << int(control%0)
			<< " is not in the range [0,"
			<< maxControl << "]";
		    Error::abortRun(*this, msg);
		    return;
		}
		if (iportp->dataNew) {
		    completionTime = arrivalTime;
		    Particle& pp = iportp->get();
		    output.put(completionTime) = pp;
		}
	}
}
