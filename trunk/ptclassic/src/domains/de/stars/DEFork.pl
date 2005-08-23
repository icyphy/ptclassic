defstar {
	name {Fork}
	domain {DE}
	version { @(#)DEFork.pl	2.7	3/2/95}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc { Replicate input events on the outputs with zero delay. }
	input {
		name{input}
		type{ANYTYPE}
	}
	outmulti {
		name{output}
		type{=input}
	}
	go {
		completionTime = arrivalTime;
                Particle& pp = input.get();
		OutDEMPHIter nextp(output);
		OutDEPort *oport;
		while ((oport = nextp++) != 0)
			oport->put(completionTime) = pp;
	}
}
