defstar {
	name { NumToBus }
	domain {VHDLB}
	desc {
This star accepts an integer and outputs either a 32-bit, 16-bit, or 8-bit
wide array which represents the integer, with the MSB being the sign bit.
May need additional bus-splits at the output to extract individual bits.
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
Sometimes it is necessary to convert an integer value into a multi-bit repre-
sentation so that the values can be used by a digital circuit.  This star
converts a given integer number into a 32-bit, 16-bit, or 8-bit representation.  The
MSB (most significant bit) is the sign bit.  Note that the output bus is
currently  restricted to either a 8-bit, 16-bit, or 32-bit bus, even though no
restriction is necessary.  This makes for a more consistant (and more
practical) output for digital circuits.
        }
	input {
		name { input }
		type { int }
	}
	outmulti {
		name { output }
		type { int }
	}
	state {
		name { width }
		type { int }
		default { 32 }
		desc { Maybe either 32, 16, or 8 bits wide }
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
		if( width != 8 ) {
		  if( width != 16 ) {
		    if( width != 32 )
                      Error::abortRun(*this, "Output bus width in num_to_bus must be either 16 or 32");
		  }
		}

                int o_width = 0;  // Output's bus width

                CBlockMPHIter starMPHIter(*this);
                const MultiVHDLBPort* mph;

                while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
                  if( !strcmp(mph->name(),"output") )
                    o_width = mph->numberPorts();
                }

                if( o_width != width )
                  Error::abortRun(*this, "Wrong output bus width in num_to_bus conversion");
        }
	go {
	}
}
