defstar {
	name { FBlackHole }
	domain {VHDLB}
	desc { Discards all real inputs }
        author { Wei-Lun Tsai }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { New_Stars directory }
	htmldoc {
Simply ignores all incoming data (real-numbers) from the multi-port.
        }
	hinclude { "ConstIters.h" }
	inmulti {
		name { input }
		type { float }
	}
        method {
                name { portWidthName }
                type { "const char*" }
                arglist { "(const MultiVHDLBPort* pp)" }
                access { protected }
                code {
                        if(!strcmp(pp->name(),"input")) {
                          return((const char*) "I_WIDTH");
                        }
                        else {
                          Error::abortRun(*this, "No MultiVHDLBPort named",
                                          pp->name(), "in star");
                          return((const char*) "ERROR");
                        }
                }
        }
	go { }
}
