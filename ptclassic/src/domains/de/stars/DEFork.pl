defstar {
	name {Fork}
	domain {DE}
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc { Replicates input events on the outputs. }
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
