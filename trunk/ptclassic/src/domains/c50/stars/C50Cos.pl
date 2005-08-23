defstar {
	name { Cos }
	derived { Sin }
	domain { C50 }
	desc { 
Cosine function.
Calculation by table lookup.  Input range of (-1,1) scaled by pi.
	}
	version { @(#)C50Cos.pl	1.6	10/08/96 }
	acknowledge { Gabriel version by Maureen O'Reilly }
	author { Luis Gutierrez,  ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="cosine"></a>
This star computes the cosine of the input, which must be in the range
(-1.0, 1.0).
The output equals cos(<i> pi &#183 </i><i>in</i>), so the input range is
effectively (-<i> pi</i>, <i>pi </i>).
The output is in the range (-1.0, 1.0).
	}
	constructor {
	  // cos is just sin shifted by 90 degrees
	  C50Sin::phase.setAttributes(A_NONSETTABLE);
	  C50Sin::phase.setInitValue(90);
	}

}
