defstar {
	name { GainFix }
	domain { SDF }
	desc { 
Amplifier: output is input times "gain" (default 1.0). The precision
of "gain", input, and the output can be specified in bits. } 
        author { A. Khazeni }
	version { $Id$ }
	location { SDF main library }
        explanation { 
The value of the "gain" parameter and its precision in bits can currently 
be specified using two different notations.
Specifying only a value by itself in the dialog box would create a
fixed-point number with the default precision which has a total length
of 24 bits with the number of range bits as required by the value.
For example, the default value 1.0 creates a fixed-point object with
precision 2.22, and a value like 0.5 would create one with precision
1.23 and etc.  The alternative way of specifying the value and the
precision of this parameter is to use the paranthesis notation which will be
interpreted as (value, precision).  For example, filling the dialog
box of this parameter by let's say (2.546, 3.5) would create a fixed-point
object by casting the double 2.546 to the precision 3.5. }
	input {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { fix }
	}
	defstate {
		name { gain }
		type { fix }
		default { 1.0 }
		desc { Gain of the star.} 
	}
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc {
Use the arriving particles as they are if YES.  Cast them to the
precision specified by the parameter "InputPrecision" otherwise. }
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the input in bits.  The input particles are only casted
to this precision if the parameter "ArrivingPrecision" is set to NO.}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the output in bits.  This is the precision that will hold
the result of the arithmetic operation on the inputs.}
        }
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc {
Set the overflow characteristic for the output.  If the result
of the product can not be fit into the precision of the output, overflow
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
                Fix out(out_len, out_IntBits);
                out.set_ovflow(OV);
                Fix fixIn;

                if(int(ArrivingPrecision))
                   fixIn = Fix(input%0);
                else
                   fixIn = Fix(in_len, in_IntBits, Fix(input%0));

                out = fixIn * Fix(gain);
		output%0 << out;
	}
}

