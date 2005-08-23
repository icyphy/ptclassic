defstar {
	name { BSplit }
	domain {VHDLB}
	desc {
Splits a bus into two parts.  Note that the total output widths must add up to
the input width.
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
Since ptolemy doesn't allow the user to easily "split" a bus (a multi-port),
this star is written to allow the user to send a portion of a bus into one
destination while the remaining portion of the same bus to another destination.
Note that if the two output bus' widths don't add up to that of the input bus,
an error message is printed.
        }
	inmulti {
		name { input }
		type { int }
	}
	outmulti {
		name { top }
		type { int }
	}
	outmulti {
		name { bottom }
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
			else if(!strcmp(pp->name(),"top")) {
			  return((const char*) "A_WIDTH");
			}
			else if(!strcmp(pp->name(),"bottom")) {
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
		  else if( !strcmp(mph->name(),"top") )
		    a_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"bottom") )
		    b_width = mph->numberPorts();
                }

                if( i_width != (a_width + b_width) )
                  Error::abortRun(*this, "Output bus widths are inconsistent!");
        }
	go {
	}
}
