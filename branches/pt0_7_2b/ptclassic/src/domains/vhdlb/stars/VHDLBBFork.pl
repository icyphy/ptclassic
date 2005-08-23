defstar {
	name { BFork }
	domain {VHDLB}
	desc {
Makes two copies of a bus, since Ptolemy doesn't allow multiple connections
from a bus.
	}
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Since ptolemy doesn't allow a bus (a multi-port) to be routed to several
different destination stars, this star is written to allow the user to
create two copies of the same bus.  The user can then route the two identical
output buses to different stars.  Note that if the input and output buses have
different widths, a ptolemy error message will be printed.
        }
	inmulti {
		name { input }
		type { int }
	}
	outmulti {
		name { outputa }
		type { int }
	}
	outmulti {
		name { outputb }
		type { int }
	}
        hinclude { "ConstIters.h" }
	method {
                name { portWidthName }
                type { "const char*" }
                arglist { "(const MultiVHDLBPort* pp)" }
                access { protected }
                code {
                        if(!strcmp(pp->name(),"input")) {
                          return((const char*) "I_WIDTH");
                        }
			else if(!strcmp(pp->name(),"outputa")) {
			  return((const char*) "A_WIDTH");
			}
			else if(!strcmp(pp->name(),"outputb")) {
			  return((const char*) "B_WIDTH");
			}
                        else {
                          Error::abortRun(*this, "No MultiVHDLBPort named",
                                          pp->name(), "in star");
                          return((const char*) "ERROR");
                        }
                }
	}
        setup {
                int i_width = 0;  // Input bus width
		int a_width = 0;  // Output A width
		int b_width = 0;  // Output B width

                CBlockMPHIter starMPHIter(*this);
                const MultiVHDLBPort* mph;

                while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
                  if( !strcmp(mph->name(),"input") )
                    i_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"outputa") )
		    a_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"outputb") )
		    b_width = mph->numberPorts();
                }

		if( a_width != b_width )
		  Error::abortRun(*this, "Output bus widths are inconsistent!");

                if( i_width != a_width )
                  Error::abortRun(*this, "Output bus widths are inconsistent!");
        }
	go {
	}
}
