defstar {
	name { MatlabCx_M }
	domain { SDF }
	derivedFrom { Matlab_M }
	desc {
Evaluate a Matlab function if inputs are given or
evaluate a Matlab command if no inputs are given.
	}
	version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1995-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF Matlab library }
	htmldoc {
See the explanation for Matlab_M star.
The only difference is that this star forces the outputs to be complex matrices.
<a name="Matlab interface"></a>
	}
	constructor {
output.setPort("output", this, COMPLEX_MATRIX_ENV);
	}
}
