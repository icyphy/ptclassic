defstar {
	name { Counter }
	domain {VHDLB}
	desc { This is a simple up-down 8-bit counter. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
This is a simple up-down, 8-bit, resettable counter.

If the "up-down" bit is 1, this is an up-counter, otherwise this is a
down-counter.

Depending on the "up-down" bit value, "\reset" resets the output to either 0
(as an up-counter) or 0xFF (as a down-counter) at the rising edge of the
clock.  Notice that "\reset" is positive-low.  The 8-bit output value is
maintained (unchanged) if the positive-low "\enable" is disabled, otherwise
the output value will be updated after every clock cycle.

If the output bus is not 8 bits wide, an error message is printed.
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
	input {
		name { enable }
		type { int }
		desc { positive low!!! }
	}
	input {
		name { reset }
		type { int }
		desc { positive low!!! }
	}
	input {
		name { up_down }
		type { int }
		desc { 1 = up counter, 0 = down counter }
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
		int o_width = 0;  // Output's bus width (has to be 8)

		CBlockMPHIter starMPHIter(*this);
		const MultiVHDLBPort* mph;

		while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
		  if( !strcmp(mph->name(),"output") )
		    o_width = mph->numberPorts();
		}

		if( o_width != 8 )
		  Error::abortRun(*this, "Wrong output bus width in counter");
	}
	go {
	}
}
