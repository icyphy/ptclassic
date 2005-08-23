defstar {
	name { Comparator }
	domain {VHDLB}
        desc {
Compares two inputs and generates an 1-bit output.  hi = equal, lo = unequal.
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
This star simply compares the two floating point numbers at the input.  If the
two numbers are equal then a logical high (1) is passed to the output.  Other-
wise a logical low (0) is passed to the output.  The output is changed when-
ever the inputs change.
        }
	state {
		name { delay }
		type { int }
		desc { Propagation delay in nanoseconds }
		default { 1 }
	}
	inmulti {
		name { inputa }
		type { int }
	}
	inmulti {
		name { inputb }
		type { int }
	}
	output {
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
                        if(!strcmp(pp->name(),"inputa")) {
			  return((const char*) "A_WIDTH");
			}
			else if(!strcmp(pp->name(),"inputb")) {
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
		int a_width = 0;  // Input A's bus width
		int b_width = 0;  // Input B's bus width

		CBlockMPHIter starMPHIter(*this);
		const MultiVHDLBPort* mph;

		while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
		  if( !strcmp(mph->name(),"inputa") )
		    a_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"inputb") )
                    b_width = mph->numberPorts();
		}

		if( a_width != b_width )
                  Error::abortRun(*this, "Mismatched input bus widths in comparator");
	}
	go {
	}
}
