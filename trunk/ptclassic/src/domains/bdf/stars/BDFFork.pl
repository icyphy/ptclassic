defstar {
	name {Fork}
	domain {BDF}
	desc { Copies input particles to each output. }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { BDF main library }
	explanation {
This star is functionally identical to the SDF Fork star.  However,
it indicates to the BDF scheduler that the various output signals
are logically the same as the input signal, and this is necessary
to construct logically consistent schedules when Booleans are present.
As for Fork in other domains,  It will be automatically inserted
when multiple inputs are connected to the same output using the graphical
interface, or when the "nodeconnect" command is used in the interpreter.
	}

	input {
		name{input}
		type{ANYTYPE}
	}
	outmulti {
		name{output}
		type{= input}
	}
	start {
		MPHIter nextp(output);
		BDFPortHole* p;
		while ((p = (BDFPortHole*)nextp++) != 0)
			p->setRelation(BDF_SAME,&input);
	}
	go {
		// note that explicit grabData and sendData are needed
		// since this is BDF.
		MPHIter nextp(output);
		PortHole* p;
		input.grabData();
		while ((p = nextp++) != 0) {
			(*p)%0 = input%0;
			p->sendData();
		}
	}
}

