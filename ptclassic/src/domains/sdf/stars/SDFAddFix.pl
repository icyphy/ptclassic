defstar {
	name { AddFix }	
	domain {SDF}
	desc { Output the sum of the fixed-piont inputs as a fixed-point value. }
	version { $Id$ }
        author { A. Khazeni }
        location { SDF main library }
	hinclude { "Fix.h" }
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
                default { "2.14" }
                desc { 
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc { 
Precision of the output in bits.  This is the precision that will hold
the result of the sum of the inputs.}
        }
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc { 
Overflow characteristic for the output.  If the result
of the sum cannot be fit into the precision of the output, overflow 
occurs and the overflow is taken care of by the method specified by this 
parameter.  The keywords for overflow handling methods are : 
"saturate"(default), "zero_saturate", "wrapped", "warning". }     
        }
        protected {
                const char* IP;
                const char* OP;
                const char* OV;
                int in_IntBits;
                int in_len;
                int out_IntBits;
                int out_len;
        }
        setup {
                IP = InputPrecision;
                OP = OutputPrecision;
                OV = OverflowHandler;
                in_IntBits = Fix::get_intBits (IP);
                in_len = Fix::get_length (IP);
                out_IntBits = Fix::get_intBits (OP);
                out_len = Fix::get_length (OP);
        }
        go {
                MPHIter nexti(input);
                PortHole *p;
                Fix sum(out_len, out_IntBits);
                sum.set_ovflow(OV);  //Set the overflow characteristic of sum
                Fix fixIn;

                while ((p = nexti++) != 0) {

                   if(int(ArrivingPrecision))
                      fixIn = Fix((*p)%0);
                   else
                      fixIn = Fix(in_len, in_IntBits, Fix((*p)%0));

                   sum += fixIn; }
                output%0 << sum;
        }

}
