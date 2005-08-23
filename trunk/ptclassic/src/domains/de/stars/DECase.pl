defstar {
	name {Case}
	domain {DE}
	desc {
Switch input events to one of N outputs,
as determined by the last received control input.
The value of the control input must be between
0 and N-1, inclusive, or an error is flagged.
	}
	version { @(#)DECase.pl	1.3	06/04/96 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	input {
		name {input}
		type {anytype}
	}
	input {
		name {control}
		type {int}
	}
	outmulti {
		name {output}
		type {=input}
	}
	constructor {
		control.triggers();
		control.before(input);
	}
	go {
		if (input.dataNew) {
		    OutDEMPHIter nexto(output);
		    OutDEPort* oportp = 0;
		    // the control value could be positive or negative
		    for (int i = int(control%0); i >= 0; i--) {
			oportp = nexto++;
			if (!oportp) break;
		    }
		    // oportp will be zero if the control value is out of range
		    if (!oportp) {
			int maxControl = output.numberPorts() - 1;
			StringList msg = "The control value ";
			msg << int(control%0)
			    << " is not in the range [0,"
			    << maxControl << "]";
			Error::abortRun(*this, msg);
			return;
		    }
		    completionTime = arrivalTime;
		    Particle& pp = input.get();
		    oportp->put(completionTime) = pp;
		}
	}
}
