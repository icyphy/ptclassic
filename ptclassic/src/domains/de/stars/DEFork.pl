defstar {
	name {Fork}
	domain {DE}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc { Replicates input events on the outputs. }
	input {
		name{input}
		type{ANYTYPE}
	}
	outmulti {
		name{output}
		type{ANYTYPE}
	}
	constructor {
		input.inheritTypeFrom(output);
	}
	go {
           completionTime = arrivalTime;
           if (input.dataNew) {
                Particle& pp = input.get();
		OutDEMPHIter nextp(output);
		OutDEPort *oport;
		while ((oport = nextp++) != 0)
			oport->put(completionTime) = pp;
           }
	}
}
