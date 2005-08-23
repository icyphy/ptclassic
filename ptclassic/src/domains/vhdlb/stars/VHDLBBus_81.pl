defstar {
	name { Bus_81 }
	domain {VHDLB}
	desc { Convert 8 bits into an 8-bit-wide bus }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Since ptolemy doesn't allow the user to explicitly merge several wires into a
bus, this star is written to allow the user to explicitly create an 8-bit-wide
bus from 8 input wires.  If the input bus is not 8 bits wide, an error message
will be printed.
        }
	input {
		name { in_7 }
		type { int }
		desc { Highest order bit }
	}
	input {
		name { in_6 }
		type { int }
	}
	input {
		name { in_5 }
		type { int }
	}
	input {
		name { in_4 }
		type { int }
	}
	input {
		name { in_3 }
		type { int }
	}
	input {
		name { in_2 }
		type { int }
	}
	input {
		name { in_1 }
		type { int }
	}
	input {
		name { in_0 }
		type { int }
		desc { Lowest order bit }
	}
	outmulti {
		name { output }
		type { int }
	}
        hinclude { "ConstIters.h" }
	method {
                name { portWidthName }
                type { "const char*" }
                arglist { "(const MultiVHDLBPort* pp)" }
                access { protected }
                code {
                        if(!strcmp(pp->name(),"output")) {
			  return((const char*) "O_WIDTH");
			}
                        else {
			  Error::abortRun(*this, "No MultiVHDLBPort named",
					  pp->name(), "in star");
			  return((const char*) "ERROR");
			}
                }
	}
        setup {
                int o_width = 0;  // Output bus width (should be 8)

                CBlockMPHIter starMPHIter(*this);
                const MultiVHDLBPort* mph;

                while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
                  if( !strcmp(mph->name(),"output") )
                    o_width = mph->numberPorts();
                }

                if( o_width != 8 )
                  Error::abortRun(*this, "Output bus width must be 8!");
        }
	go {
	}
}
