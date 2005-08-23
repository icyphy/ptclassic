defstar {
	name { Bus_14 }
	domain {VHDLB}
	desc { Converts a 4-bit wide bus into 4 bits }
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
to convert a 4-bit-wide bus into 4 explicit wires.  If the output bus is not
4 bits wide, an error message is printed.
        }
	inmulti {
		name { input }
		type { int }
	}
	output {
		name { out_3 }
		type { int }
		desc { Highest order bit }
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
                int i_width = 0;  // Input bus width (should be 4)

                CBlockMPHIter starMPHIter(*this);
                const MultiVHDLBPort* mph;

                while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
                  if( !strcmp(mph->name(),"input") )
                    i_width = mph->numberPorts();
                }

                if( i_width != 4 )
                  Error::abortRun(*this, "Input bus width must be 4!");
        }
	go {
	}
}
