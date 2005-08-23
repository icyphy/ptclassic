defstar {
	name { ALU }
	domain {VHDLB}
	desc { A simple ALU. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
This is a simple ALU that has several arithmetic (addition, subtraction) and
bitwise logical (shift, logic) operations.  The 4-bit wide "sel" input deter-
mines the function performed by the ALU.  Inputs are sampled and outputs are
calculated at the rising edges of the clock after an user-specified internal
delay.  Note that the input and output buses should have the same bus width
or error messages are printed.

The following is a table listing the functions provided by this ALU:

sel = "0000"  :   Output = A + B
      "0001"  :   Output = A - B

      "0010"  :   Output = A &lt;&lt; B,  zeros are used to pad
      "0011"  :   Output = A &gt;&gt; B,  zeros are used to pad
      "0100"  :   Output = A &gt;&gt; B,  sign bit is used to pad

      "0101"  :   Output = A and B
      "0110"  :   Output = A nand B
      "0111"  :   Output = A or B
      "1000"  :   Output = A nor B
      "1001"  :   Output = A xor B
      "1010"  :   Output = not A
	}
	state {
		name { delay }
		type { int }
		default { 10 }
		desc { internal delay in nanoseconds }
	}
	input {
		name { clock }
		type { int }
	}
	inmulti {
		name { inputa }
		type { int }
	}
	inmulti {
		name { inputb }
		type { int }
	}
	inmulti {
		name { sel }
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
                        if(!strcmp(pp->name(),"inputa")) {
			  return((const char*) "A_WIDTH");
			}
			else if(!strcmp(pp->name(),"inputb")) {
			  return((const char*) "B_WIDTH");
			}
			else if(!strcmp(pp->name(),"output")) {
			  return((const char*) "O_WIDTH");
			}
			else if(!strcmp(pp->name(),"sel")) {
			  return((const char*) "S_WIDTH");
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
		int o_width = 0;  // Output's bus width
		int s_width = 0;  // Selector's bus width

		CBlockMPHIter starMPHIter(*this);
		const MultiVHDLBPort* mph;

		while((mph = (const MultiVHDLBPort*) starMPHIter++) != 0) {
		  if( !strcmp(mph->name(),"inputa") )
		    a_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"inputb") )
                    b_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"output") )
                    o_width = mph->numberPorts();
		  else if( !strcmp(mph->name(),"sel") )
                    s_width = mph->numberPorts();
		}

		if( a_width != b_width )
                  Error::abortRun(*this, "Mismatched input bus widths in ALU");

		if( s_width != 4 )
		  Error::abortRun(*this, "Selector bus width must be 4");

		if( o_width != a_width )
		  Error::abortRun(*this, "Wrong output bus width in ALU");
	}
	go {
	}
}
