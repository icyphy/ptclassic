defstar {
	name {Case}
	domain {DE}
	desc {
Switch input events to one of N outputs,
as determined by the last received control input.
The value of the control input must be between
0 and N-1, inclusive, or an error is flagged.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
	    if (int(control%0) < 0) {
	      Error::abortRun(*this,"negative control value received");
	      return;
	    }
	    MPHIter nexto(output);
	    OutDEPort* p = 0;
	    for (int i = int(control%0); i >=0; i--) {
	      // Apologies for this cast.
	      p = (OutDEPort*)nexto++;
	      if (!p) {
	        Error::abortRun(*this,"control value out of range");
	        return;
	      }
	    }
	    completionTime = arrivalTime;
	    Particle& pp = input.get();
            p->put(completionTime) = pp;
	  }
	}
}
