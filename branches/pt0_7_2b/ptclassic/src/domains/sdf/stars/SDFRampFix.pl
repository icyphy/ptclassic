defstar {
	name { RampFix }
	domain { SDF }
	desc {
Generates a fixed-point ramp signal, starting at "value" (default 0)
with step size "step" (default 1). The precision of these
parameters can be specified in bits.  
	}
        author { A. Khazeni }
	version { $Id$ }
        location { SDF main library }
	explanation { 
The value of the "step" and "value" parameters and their precision 
in bits can currently be specified using two different notations. 
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
	output {
		name { output }
		type { fix }
	}
	defstate {
		name { step }
		type { fix }
		default { "1.0" }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { fix }
		default { "0.0" }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc {
Set the overflow characteristic for the output.  If the result
of the sum can not be fit into the precision of the output, overflow
occurs and the overflow is taken care of by the method specified by this
parameter.  The keywords for overflow handling methods are :
"saturate"(default), "zero_saturate", "wrapped", "warning". }
        }
        protected {
                const char* OV;
        }
        setup {
                OV = OverflowHandler;
        }
	go {
                Fix t = Fix(value);
                t.set_ovflow(OV);
                output%0 << t;
                t += step;
                value = t;
	}
}
