defstar {
	name { FloatToFix }	
	domain {SDF}
	desc { 
Convert from a float to a fixed-point given the precision specified 
in bits (dot notation) by the "OutputPrecision" state variable. }
        author { A. Khazeni }
	version { $Id$ }
        location { SDF main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { fix }
	}
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the output in bits.  The double inputs will be casted
to this precision.  If the value of the double can not be represented
by the number of bits specified in the precision parameter, it signals
an error message.}
        }
        defstate {
                name { Masking }
                type { string }
                default { "truncate" }
                desc { 
Masking method.  This parameter is used to specify the way the
double is masked for casting to the fixed-point notation.  The
keywords are: "truncate" (default), "round". }
        }
        protected {
                const char* OP;
                const char* MS;
                int out_IntBits;
                int out_len;
        }
        setup {
                OP = OutputPrecision;
                MS = Masking;
                out_IntBits = Fix::get_intBits (OP);
                out_len = Fix::get_length (OP);
                if (strcmp((MS), "truncate") == 0)
                  Fix::Set_MASK(Fix::mask_truncate);
                else if (strcmp((MS), "round") == 0)
                  Fix::Set_MASK(Fix::mask_truncate_round);
                else
                {
                  Error::abortRun (*this, ": not a valid function for masking");
                  return;
                }
        }
	go {   
	       Fix out(out_len, out_IntBits, double(input%0));
	       output%0 << out;
	}
}
