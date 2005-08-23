defstar {
	name { Bus_18 }
	domain {VHDLB}
	desc { Converts an 8-bit wide bus into 8 bits }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Since ptolemy doesn't allow the user to explicitly extract individual data
out of a bus (a multi-port), this star is written to allow the user
to convert an 8-bit-wide bus into 8 explicit wires.  If the output bus is not
8 bits wide, an error message is printed.
        }
	inmulti {
		name { input }
		type { int }
	}
	output {
		name { out_7 }
		type { int }
		desc { Highest order bit }
	}
	output {
		name { out_6 }
		type { int }
	}
	output {
		name { out_5 }
		type { int }
	}
	output {
		name { out_4 }
		type { int }
	}
	output {
		name { out_3 }
		type { int }
	}
	output {
		name { out_2 }
		type { int }
	}
	output {
		name { out_1 }
		type { int }
	}
	output {
		name { out_0 }
		type { int }
		desc { Lowest order bit }
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
                        else {
                          Error::abortRun(*this, "No MultiVHDLBPort named",
                                          pp->name(), "in star");
                          return((const char*) "ERROR");
                        }
                }
	}
        setup {
                int i_width = 0;  // Input bus width (should be 8)

                CBlockMPHIter starMPHIter(*this);
                const MultiVHDLBPort* mph;

                while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
                  if( !strcmp(mph->name(),"input") )
                    i_width = mph->numberPorts();
                }

                if( i_width != 8 )
                  Error::abortRun(*this, "Input bus width must be 8!");
        }
	go {
	}
}
