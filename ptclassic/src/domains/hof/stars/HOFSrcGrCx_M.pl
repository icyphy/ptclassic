defstar {
	name { ParSrcGrCx_M }
	domain { HOF }
	derivedFrom { MapGr }
	desc {
Create one or more instances of the named block to produce the
required number of output stream(s).  This is implemented by replacing
the ParSrcGrCx_M star with the named block at setup time.  The replacement
block(s) are connected as illustrated by the example block that is connected.
Their parameters are determined by "parameter_map".
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
See the explanation for MapGr star, from which this is derived.
The only difference is that this star forces the number of inputs to zero
and output datatype to complex matrix.  It also hides the input ports and the
\fIinput_map\fR parameter.
.Sr "MapGr"
	}
	ccinclude { "Matrix.h" } 
	constructor {
	  output.setPort("output", this, COMPLEX_MATRIX_ENV);
	  exin.setPort("exin", this, COMPLEX_MATRIX_ENV);
	  input.setAttributes(P_HIDDEN);
	  exout.setAttributes(P_HIDDEN);
	}
}
