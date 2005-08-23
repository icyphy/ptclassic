defstar {
	name { BusToNum }
	domain {VHDLB}
	desc {
This star accepts a variable-width input array of bits representing an 
integer (MSB is sign bit, but not 2's complement) and outputs this integer.
If any of the input bits is tri-state (value 3), then the previous value is
sent to the output.
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
Sometimes it is necessary to convert the data representation in a digital
circuit (a value on a bus) to a number that can be used by components at
a more abstract level.  This star allows the user to convert a bus value
(which may be received from a digital circuit) to an equivalent integer which
can be used by other processing stars which simply expects abstract values
at their input ports.  The MSB (most significant bit) is the sign bit.  Note
that this star will not understand 2`s-complement inputs.  This star is the
counter-part of the "NumToBus" star in the VHDLF domain.
        }
	inmulti {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	state {
		name { previous }
		type { int }
		default { 0 }
		desc { previous value of output }
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
                int i_width = 0;  // Input's bus width

                CBlockMPHIter starMPHIter(*this);
                const MultiVHDLBPort* mph;

                while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
                  if( !strcmp(mph->name(),"input") )
                    i_width = mph->numberPorts();
                }

                if( i_width > 32 )
                  Error::abortRun(*this, "Input exceeds proper integer word length of 32!");
        }
	go {
	}
}
