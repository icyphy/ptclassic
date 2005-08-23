defcore {
    name { Add }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Add } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSAddFPCGC.pl	1.5 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	constructor {
		noInternalState();
	}
	go {
		StringList out = "\t$ref(output) = ";
		for (int i = 1; i <= corona.input.numberPorts(); i++) {
			out << "$ref(input#" << i << ")";
			if (i < corona.input.numberPorts())
			  out << " + ";
			else
			  out << ";\n";
		}
		addCode(out);
	}
	exectime {
		return corona.input.numberPorts();
	}
}
