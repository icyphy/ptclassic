defstar {
	name { ParSrcCx }
	domain { HOF }
	derivedFrom { Map }
	desc {
Create one or more instances of the named block to produce the
required number of output stream(s).  This is implemented by replacing
the ParSrc star with the named block at setup time.  The replacement
block(s) are connected as specified by "input_map", using the existing
connections to the ParSrc star.  Their parameters are determined
by "parameter_map".
	}
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	explanation {
See the explanation for Map star, from which this is derived.
The only difference is that this star forces the number of inputs to zero
and output datatype to complex.  It also hides the input ports and the
\fIinput_map\fR parameter.
.Sr "Map"
	}
	constructor {
	  output.setPort("output", this, COMPLEX);
	  input.setAttributes(P_HIDDEN);
	  input_map.clearAttributes(A_SETTABLE);
	  input_map.setInitValue("");
	  blockname.setInitValue("ConstCx");
	  parameter_map.setInitValue("");
	}
}
