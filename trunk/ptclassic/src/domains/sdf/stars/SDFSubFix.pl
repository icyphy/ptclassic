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
("neg" inputs) as they are:  YES processes them unaltered, and NO casts
them to the precision specified by the parameter "InputPrecision". }
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the "neg" input in bits.  The input particles are only
cast to this precision if the parameter "ArrivingPrecision" is set to NO.}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the output in bits.  This is the precision that will hold
the result of the difference of the inputs.}
        }
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc {
The overflow characteristic for the output.  If the result
of the difference cannot be fit into the precision of the output, overflow
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
                MPHIter nexti(neg);
                PortHole *p;
                Fix diff(out_len, out_IntBits, Fix(pos%0));
                diff.set_ovflow(OV);
                Fix fixIn;

                while ((p = nexti++) != 0) {

                   if(int(ArrivingPrecision))
                      fixIn = Fix((*p)%0);
                   else
                      fixIn = Fix(in_len, in_IntBits, Fix((*p)%0));

                   diff -= fixIn; }
                output%0 << diff;
        }
}
