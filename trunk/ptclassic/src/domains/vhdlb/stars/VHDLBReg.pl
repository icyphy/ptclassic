defstar {
	name { Reg }
	domain {VHDLB}
	desc { A simple multiple-bit rising-edge triggered register. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
This star latches and stores a bus value at every rising edge of
the synchronization clock.  If the input and output bus widths don't match,
an error message is printed.
        }
	state {
		name { delay }
		type { int }
		default { 1 }
		desc { internal delay in nanoseconds }
	}
	input {
		name { clock }
		type { int }
	}
	inmulti {
		name { input }
		type { int }
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
                        if(!strcmp(pp->name(),"input")) {
			  return((const char*) "I_WIDTH");
			}
			else if(!strcmp(pp->name(),"output")) {
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
		int i_width = 0;  // Input's bus width
		int o_width = 0;  // Output's bus width

		CBlockMPHIter starMPHIter(*this);
		const MultiVHDLBPort* mph;

		while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
		  if( !strcmp(mph->name(),"input") )
		    i_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"output") )
                    o_width = mph->numberPorts();
		}

		if( i_width != o_width )
                  Error::abortRun(*this, "Mismatched input/output bus widths in register");
	}
	go {
	}
}
