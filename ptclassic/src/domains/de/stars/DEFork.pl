ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer: S. Ha
Date of creation: 8/28/90
Modified to use preprocessor: 9/29/90, by EAL

 A Fork simply replicates input events ont its outputs

************************************************************************/
}
defstar {
	name {Fork}
	domain {DE}
	desc { "Replicates input events on the outputs" }
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
