defstar {
	name { ChopVarOffset }
	domain { SDF }
	derivedFrom { Chop }
	author { G. S. Walter and E. A. Lee }
	version { $Id$ }
	location { SDF main library }
	copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	desc {
This star has the same functionality as the Chop star except that the
\fIoffset\fR parameter is determined at run time by a control input and that
the \fIuse_past_inputs\fR parameter is set to FALSE and not accessible.
	}

	input { name { offsetCntrl } type { int } }

	constructor {
		offset.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
		use_past_inputs.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
		use_past_inputs = FALSE;
	}

	go {
		offset = int(offsetCntrl%0);
		SDFChop :: go();
	}
}
