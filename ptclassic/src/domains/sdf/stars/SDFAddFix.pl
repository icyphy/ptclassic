defstar {
	name { AddFix }	
	domain {SDF}
	desc {
Output the sum of the fixed-piont inputs as a fixed-point value.
	}
	version { $Id$ }
        author { A. Khazeni }
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
		}
        }
        protected {
		Fix fixIn, sum;
        }
        setup {
                if ( ! int(ArrivingPrecision) ) {
                  const char* IP = InputPrecision;
                  int in_IntBits = Fix::get_intBits(IP);
                  int in_len = Fix::get_length(IP);
                  fixIn = Fix(in_len, in_IntBits);
		}

                const char* OP = OutputPrecision;
                int out_IntBits = Fix::get_intBits(OP);
                int out_len = Fix::get_length(OP);
                sum = Fix(out_len, out_IntBits);

                const char* OV = OverflowHandler;
                sum.set_ovflow(OV);  // Set the overflow characteristic of sum
        }
        go {
                MPHIter nexti(input);
                PortHole *p;

		// The current fixed-point class can always represent 0.0
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
