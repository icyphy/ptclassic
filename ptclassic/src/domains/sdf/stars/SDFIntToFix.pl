defstar {
	name { IntToFix }
	domain { SDF }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF conversion palette }
	desc { Convert an integer input to a fixed-point output. }
	input {
		name { input }
		type { int }
		desc { Input integer type }
	}
	output {
		name { output }
		type { fix }
		desc { Output fix type }
	}
        defstate {
                name { OutputPrecision }
                type { string }
                default { "16.0" }
                desc {
Precision of the output in bits.
The integer number is cast to a double and then converted to this precision.
If the value of the double cannot be represented by the number of bits
specified in the precision parameter, then the OverflowHandler will be called.
		}
        }
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc {
Overflow characteristic for the output.
If the result of the sum cannot be fit into the precision of the output,
then overflow occurs and the overflow is taken care of by the method
specified by this parameter.
The keywords for overflow handling methods are:
"saturate" (the default), "zero_saturate", "wrapped", and "warning".
The "warning" option will generate a warning message whenever overflow occurs.
		}
        }
        protected {
		Fix out;
        }
        setup {
                out = Fix( ((const char *) OutputPrecision) );
		out.set_ovflow( ((const char *) OverflowHandler) );
        }
	go {
		out = (double) ((int)(input%0));
                output%0 << out;
	}
}
