defstar {
	name { RampFix }
	domain { CGC }
	derivedFrom { CGCFix }
	desc {
Generate a fixed-point ramp signal, starting at "value" (default 0.0)
with step size "step" (default 1.0).
A precision and an initial value can be specified for a parameter by using
the notation ( <initial_value>, <precision> ).
	}
        author { J.Weiss }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
        location { CGC main library }
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
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits and precision of the accumulation.
When the value of the accumulation extends outside of the precision,
the output is set to its maximum value (or minimum for negative
magnitudes).
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
The precision of this state is the precision of the accumulation.
		}
		attributes { A_SETTABLE|A_NONCONSTANT }
	}

        setup {
		CGCFix::setup();
		// if the user specified an invalid precision string, the error
		// will be automatically reported in the initialize method of
		// class PrecisionState
		output.setPrecision(OutputPrecision);
        }

	go {
		// insert code to clear overflow flag
		CGCFix::clearOverflow();

@	FIX_Assign($ref(output), $ref(value));
@	FIX_Add($ref(value), $ref(value),$ref(step));

		// insert code to test overflow flag
		CGCFix::checkOverflow();
	}

        // a wrap-up method is inherited from CGCFix
        // if you defined your own, you should call CGCFix::wrapup()
}
