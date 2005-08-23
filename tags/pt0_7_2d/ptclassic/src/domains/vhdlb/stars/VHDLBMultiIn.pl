defstar {
	name { MultiIn }
	domain {VHDLB}
	desc {
Output the first of multiple inputs, discarding the rest.
	}
        version { @(#)VHDLBMultiIn.pl	1.3 3/2/95 }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDLB main library }
	inmulti {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	method {
                name { portWidthName }
                type { "const char*" }
                arglist { "(const MultiVHDLBPort* pp)" }
                access { protected }
                code {
                        if(!strcmp(pp->name(),"input")) {
			  return((const char*) "N");
			}
                        else {
			  Error::abortRun(*this, "No MultiVHDLBPort named",
					  pp->name(), "in star");
			  return((const char*) "ERROR");
			}
                }
	}
	go {
	}
}
