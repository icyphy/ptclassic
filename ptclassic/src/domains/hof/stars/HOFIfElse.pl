defstar {
	name {IfElse}
	domain {HOF}
	derivedFrom {Map}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	desc {
This star is just like Map, except that it creates one of two named blocks
to replace itself, instead of just one.
If the "condition" parameter is TRUE, then the "true_block" is used.
Otherwise, the "false_block" is used.
This can be used to parameterize the use of a given block, or, more
interestingly, for statically evaluated recursion.
	}
	htmldoc {
This star is derived from the
<tt>Map</tt>
star.
It is also a higher-order function mechanism.
<a name="higher-order functions"></a>
See the
<tt>Map</tt>
documentation for background information.
The only difference is that instead of one named replacement block,
there are two.
The choice between them is determined by the <i>condition</i> parameter.
<p>
An application for this block is in recursion.
A galaxy <i>X</i> can be created that has somewhere within it an
<tt>IfElse</tt>
block with <i>X</i> given as the replacement block.
To ensure that the recursion terminates, the parameter <i>use_alt</i>
is computed from galaxy parameters, for example by subtracting one
from an initial integer value.
When this parameter reaches zero, an alternative block is used instead
of <i>X</i>, terminating the recursion.
	}
	defstate {
		name {condition}
		type {int}
		default {"TRUE"}
		desc {Specifies whether to use the true or false block}
	}
	defstate {
		name {true_block}
		type {string}
		default {"Gain"}
		desc {The name of the replacement block used if condition is non-zero}
	}
	defstate {
		name {where_true_defined}
		type {string}
		default {""}
		desc {
The full path and facet name for the definition of true_block.
		}
	}
	defstate {
		name {true_input_map}
		type {stringarray}
		default {""}
		desc {The input mapping for the false block}
	}
	defstate {
		name {true_output_map}
		type {stringarray}
		default {""}
		desc {The output mapping for the false block}
	}
	defstate {
		name {true_parameter_map}
		type {stringarray}
		default {""}
		desc {The parameter mapping for the false block}
	}
	defstate {
		name {false_block}
		type {string}
		default {""}
		desc {The name of the replacement block used if condition is zero}
	}
	defstate {
		name {where_false_defined}
		type {string}
		default {""}
		desc {
The full path and facet name for the definition of false_block.
		}
	}
	defstate {
		name {false_input_map}
		type {stringarray}
		default {""}
		desc {The input mapping for the false block}
	}
	defstate {
		name {false_output_map}
		type {stringarray}
		default {""}
		desc {The output mapping for the false block}
	}
	defstate {
		name {false_parameter_map}
		type {stringarray}
		default {""}
		desc {The parameter mapping for the false block}
	}
	constructor {
	  // Hide all the parameters that are controlled internally
	  blockname.clearAttributes(A_SETTABLE);
	  where_defined.clearAttributes(A_SETTABLE);
	  input_map.clearAttributes(A_SETTABLE);
	  output_map.clearAttributes(A_SETTABLE);
	  parameter_map.clearAttributes(A_SETTABLE);
	}
	method {
	  name { preinitialize }
	  access { public }
	  code {
	    // Call low-level preinit to make state values valid
	    HOFBaseHiOrdFn::preinitialize();
	    // Copy appropriate set of parameters
	    // Note we must set the init values, not the current values,
	    // because HOFMap::preinitialize will reinit the states.
	    // Use of hashstring here is a small memory leak.
	    if ((int) condition) {
		blockname.setInitValue(hashstring((const char*) true_block));
		where_defined.setInitValue(hashstring((const char*) where_true_defined));
		StringList temp;
		temp = true_input_map.currentValue();
		input_map.setInitValue(hashstring((const char*) temp));
		temp = true_output_map.currentValue();
		output_map.setInitValue(hashstring((const char*) temp));
		temp = true_parameter_map.currentValue();
		parameter_map.setInitValue(hashstring((const char*) temp));
	    } else {
		blockname.setInitValue(hashstring((const char*) false_block));
		where_defined.setInitValue(hashstring((const char*) where_false_defined));
		StringList temp;
		temp = false_input_map.currentValue();
		input_map.setInitValue(hashstring((const char*) temp));
		temp = false_output_map.currentValue();
		output_map.setInitValue(hashstring((const char*) temp));
		temp = false_parameter_map.currentValue();
		parameter_map.setInitValue(hashstring((const char*) temp));
	    }
	    // Now let HOFMap do its thing
	    HOFMap::preinitialize();
	  }
	}
}
