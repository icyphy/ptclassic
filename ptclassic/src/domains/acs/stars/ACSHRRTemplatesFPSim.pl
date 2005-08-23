defcore {
    name { HRRTemplates }
    domain { ACS }
    coreCategory { FPSim }
    corona { HRRTemplates }
    desc { Produces a subset of the template data based on the incoming angle }
    version { @(#)ACSHRRTemplatesFPSim.pl	1.0 12/18/00 }
    author { J. Ramanathan }
    copyright {
	Copyright (c) 1998 The Regents of the University of California.
	All rights reserved.
	See the file $PTOLEMY/copyright for copyright notice,
	limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    go {
	// Meaningless
	corona.output%0 << (double)(corona.input%0);
    }
}


