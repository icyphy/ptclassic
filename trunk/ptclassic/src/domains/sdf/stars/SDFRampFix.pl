defstar {
	name { RampFix }
	domain { SDF }
	desc {
Generate a fixed-point ramp signal, starting at "value" (default 0.0)
with step size "step" (default 1.0).
A precision and an initial value can be specified for a parameter by using
the notation ( <initial_value>, <precision> ).
	}
        author { A. Khazeni }
	version { $Id$ }
        location { SDF main library }
	explanation { 
The value of the "step" and "value" parameters and their precision 
in bits can currently be specified using two different notations. 
Specifying only a value by itself in the dialog box would create a 
fixed-point number with the default precision, which has a total length
of 24 bits with the number of range bits set as required by the value
of the parameter.
For example, the default value 1.0 creates a fixed-point object with 
precision 2.22, and a value like 0.5 would create one with precision 1.23.
An alternate way of specifying the value and the precision of this parameter
is to use the parenthesis notation which will be interpreted as
(value, precision).
For example, filling the dialog box of this parameter by (2.546, 3.5) would
create a fixed-point object by casting the double-precision floating-point
number 2.546 to a fixed-point precision of 3.5.
	} 
	output {
		name { output }
		type { fix }
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
		name { step }
		type { fix }
		default { "1.0" }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { fix }
		default { "0.0" }
		desc {
Initial value output by the ramp.
During simulation, this parameter holds the current value output by the ramp.
		}
		attributes { A_SETTABLE|A_NONCONSTANT }
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
		Fix t;
        }
        setup {
		t = Fix( ((const char *) OutputPrecision) );
                t.set_ovflow( ((const char *) OverflowHandler) );
        }
	go {
		t = Fix(value);
                output%0 << t;
                t += step;
                value = t;
	}
}
