defstar {
	name { AddFix }	
	domain {SDF}
	desc {
Output the sum of the fixed-point inputs as a fixed-point value.
	}
	version { $Id$ }
        author { A. Khazeni }
	copyright {
Copyright (c) 1993, 1994 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
        location { SDF main library }
	inmulti {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { fix }
	}
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc { 
Flag that indicates whether or not to keep the precision of the arriving
particles as is:  YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". } 
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "4.14" }
                desc { 
Precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "4.14" }
                desc {
Precision of the output in bits and precision of the accumulation.
When the value of the accumulation extends outside of the precision,
the OverflowHandler will be called.
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
		Fix fixIn, sum;
        }
        setup {
                if ( ! int(ArrivingPrecision) )
		  fixIn = Fix( ((const char *) InputPrecision) );

		sum = Fix( ((const char *) OutputPrecision) );
		sum.set_ovflow( ((const char *) OverflowHandler) );
        }
        go {
                MPHIter nexti(input);
                PortHole *p;

		// Fixed-point class in Ptolemy 0.5 can always represent 0.0
		sum = 0.0;
                while ((p = nexti++) != 0) {
                  if ( int(ArrivingPrecision) )
                    sum += Fix((*p)%0);
		  else {
                    fixIn = Fix((*p)%0);
                    sum += fixIn;
		  }
		}
                output%0 << sum;
        }
}
