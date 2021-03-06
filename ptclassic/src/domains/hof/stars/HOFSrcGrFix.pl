defstar {
	name { SrcGrFix }
	domain { HOF }
	derivedFrom { MapGr }
	desc {
Create one or more instances of the named block to produce the
required number of output stream(s).
This is implemented by replacing the SrcGrFix star with the named
block at setup time.
The replacement block(s) are connected as illustrated by the example
block that is connected.
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
<tt>MapGr</tt>
star, from which this is derived.
The only difference is that this star forces the number of inputs to zero
and output datatype to fix.
It also hides the input ports and the <i>input_map</i> parameter.
	}
	constructor {
	  output.setPort("output", this, FIX);
	  exin.setPort("exin", this, FIX);
	  input.setAttributes(P_HIDDEN);
	  exout.setAttributes(P_HIDDEN);
	}
}
