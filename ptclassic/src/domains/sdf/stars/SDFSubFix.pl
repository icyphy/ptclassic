defstar {
        name { SubFix }
        domain { SDF }
        desc {
Output as a fixed-point number the "pos" input minus all "neg" inputs.
	}
        author { A. Khazeni }
	version { $Id$ }
        location { SDF main library }
        input {
                name { pos }
                type { fix }
        }
        inmulti {
                name { neg }
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
Flag indicating whether or not to use the precision of the arriving particles
(the "pos" input and the "neg" inputs) as they are:  YES processes them
unaltered, and NO casts them to the precision specified by the
parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "4.14" }
                desc {
Precision of the "neg" input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "4.14" }
                desc {
Precision of the output in bits.
This is the precision that will hold the result of the difference
of the inputs.
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
		Fix fixIn, diff;
        }
        setup {
                if ( ! int(ArrivingPrecision) )
                  fixIn = Fix( ((const char *) InputPrecision) );

                diff = Fix( ((const char *) OutputPrecision) );
                diff.set_ovflow( ((const char *) OverflowHandler) );
        }
        go {
                MPHIter nexti(neg);
                PortHole *p;

                if ( int(ArrivingPrecision) ) {
		  diff = Fix(pos%0);
                  while ((p = nexti++) != 0)
                    diff -= Fix((*p)%0);
		}
		else {
		  fixIn = Fix(pos%0);
		  diff = fixIn;
                  while ((p = nexti++) != 0) {
		    fixIn = Fix((*p)%0);
                    diff -= fixIn;
		  }
		}
                output%0 << diff;
        }
}
