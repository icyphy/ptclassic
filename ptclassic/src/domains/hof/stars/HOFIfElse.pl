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
	explanation {
This star is derived from the
.c Map
star.
It is also a higher-order function mechanism.
.IE "higher-order functions"
See the
.c Map
documentation for background information.
The only difference is that instead of one named replacement block,
there are two.
The choice between them is determined by the \fIcondition\fR parameter.
.pp
An application for this block is in recursion.
A galaxy \fIX\fR can be created that has somewhere within it an
.c IfElse
block with \fIX\fR given as the replacement block.
To ensure that the recursion terminates, the parameter \fIuse_alt\fR
is computed from galaxy parameters, for example by subtracting one
from an initial integer value.
When this parameter reaches zero, an alternative block is used instead
of \fIX\fR, terminating the recursion.
	}
	constructor {
	    // Change the datatype of the output to be derived from the input
	    output.setPort("output",this,ANYTYPE);
	    output.inheritTypeFrom(input);
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
		desc {The  output mapping for the false block}
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
		desc {The  output mapping for the false block}
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
	setup {
	    if ((int)condition) {
		blockname.setCurrentValue((const char*) true_block);
		where_defined.setCurrentValue((const char*) where_true_defined);
		StringList temp;
		temp = true_input_map.currentValue();
		input_map.setCurrentValue((const char*) temp);
		temp = true_output_map.currentValue();
		output_map.setCurrentValue((const char*) temp);
		temp = true_parameter_map.currentValue();
		parameter_map.setCurrentValue((const char*) temp);
	    } else {
		blockname.setCurrentValue((const char*) false_block);
		where_defined.setCurrentValue((const char*) where_false_defined);
		StringList temp;
		temp = false_input_map.currentValue();
		input_map.setCurrentValue((const char*) temp);
		temp = false_output_map.currentValue();
		output_map.setCurrentValue((const char*) temp);
		temp = false_parameter_map.currentValue();
		parameter_map.setCurrentValue((const char*) temp);
	    }
	    HOFMap::setup();
	}
}
