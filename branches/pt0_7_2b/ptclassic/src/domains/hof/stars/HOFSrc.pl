defstar {
	name { Src }
	domain { HOF }
	derivedFrom { Map }
	version { @(#)HOFSrc.pl	1.11 11/13/97 }
	author { Edward A. Lee, Tom Lane }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	desc {
Create one or more instances of the named block to produce the
required number of output stream(s).
This is implemented by replacing the Src star with the named block
at preinitialization time.
The replacement block(s) are connected as specified by "output_map",
using the existing connections to the Src star.
Their parameters are determined by "parameter_map".
	}
	htmldoc {
See the explanation for
<tt>Map</tt>
star, from which this is derived.
The only difference is that this star forces the number of inputs to zero.
It also hides the input port and the <i>input_map</i> parameter.
	}
	constructor {
	  input.setAttributes(P_HIDDEN);
	  input_map.clearAttributes(A_SETTABLE);
	  input_map.setInitValue("");
	  blockname.setInitValue("Ramp");
	  parameter_map.setInitValue("");
	}
}
