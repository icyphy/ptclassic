defstar {
	name { SrcFix }
	domain { HOF }
	derivedFrom { Map }
	desc {
Create one or more instances of the named block to produce the
required number of output stream(s).
This is implemented by replacing the Src star with the named block
at setup time.
The replacement block(s) are connected as specified by "input_map",
using the existing connections to the Src star.
Their parameters are determined by "parameter_map".
	}
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	htmldoc {
See the explanation for the
<tt>Map</tt>
star, from which this is derived.
The only difference is that this star forces the number of inputs to zero
and output datatype to fixed-point.
It also hides the input ports and the <i>input_map</i></b> parameter.
	}
	constructor {
	  output.setPort("output", this, FIX);
	  input.setAttributes(P_HIDDEN);
	  input_map.clearAttributes(A_SETTABLE);
	  input_map.setInitValue("");
	  blockname.setInitValue("RampFix");
	  parameter_map.setInitValue("");
	}
}
