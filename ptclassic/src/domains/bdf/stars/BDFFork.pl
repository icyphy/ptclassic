defstar {
	name {Fork}
	domain {BDF}
	desc { Copy the input particle to each output. }
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { BDF main library }
	explanation {
This star is functionally identical to the SDF Fork star.  However,
it indicates to the BDF scheduler that the various output signals
are logically the same as the input signal, and this is necessary
to construct logically consistent schedules when Booleans are present.
As with Fork in other domains,  It will be automatically inserted
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
	setup {
		MPHIter nextp(output);
		BDFPortHole* p;
		while ((p = (BDFPortHole*)nextp++) != 0)
			p->setRelation(BDF_SAME,&input);
	}
	go {
		MPHIter nextp(output);
		PortHole* p;
		while ((p = nextp++) != 0)
			(*p)%0 = input%0;
	}
}

