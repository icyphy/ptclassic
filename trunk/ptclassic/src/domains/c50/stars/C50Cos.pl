defstar {
	name { Cos }
	derived { Sin }
	domain { C50 }
	desc { 
Cosine function.
Calculation by table lookup.  Input range of (-1,1) scaled by pi.
	}
	version { $Id$ }
	acknowledge { Gabriel version by Maureen O'Reilly }
	author { Luis Gutierrez,  ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
.Id "cosine"
This star computes the cosine of the input, which must be in the range
(-1.0, 1.0).
The output equals cos($~pi~cdot~$\fIin\fR), so the input range is 
effectively (-$~pi$, $pi~$).
The output is in the range (-1.0, 1.0).
	}
	constructor {
	  // cos is just sin shifted by 90 degrees
	  C50Sin::phase.setAttributes(A_NONSETTABLE);
	  C50Sin::phase.setInitValue(90);
	}

}
