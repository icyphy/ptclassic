defstar {
	name { FloatToFix }
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
	desc { Convert a floating-point input to an fixed-point output. }
	input {
		name { input }
		type { float }
		desc { Input float type }
	}
	output {
		name { output }
		type { fix }
		desc { Output fix type }
	}
        defstate {
                name { OutputPrecision }
                type { string }
                default { "4.14" }
                desc {
Precision of the output in bits.
If the value of the double cannot be represented by the number of bits
specified in the precision parameter, then a error message is given.
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
        defstate {
                name { masking }
                type { string }
                default { "truncate" }
                desc {
Masking method.
This parameter is used to specify the way the complex number converted
to a double is masked for casting to the fixed-point notation.
The keywords are: "truncate" (the default) and "round".
		}
        }
        protected {
		Fix out;
        }
        setup {
                out = Fix( ((const char *) OutputPrecision) );
		out.set_ovflow( ((const char *) OverflowHandler) );

                const char* Masking = masking;
                if ( strcasecmp(Masking, "truncate") == 0 )
                  out.Set_MASK(Fix::mask_truncate);
                else if ( strcasecmp(Masking, "round") == 0 )
                  out.Set_MASK(Fix::mask_truncate_round);
                else
                  Error::abortRun(*this, ": not a valid function for masking");
        }
	go {
		out = (double) (input%0);
                output%0 << out;
	}
}
