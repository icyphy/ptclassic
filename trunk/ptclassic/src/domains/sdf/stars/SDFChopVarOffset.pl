defstar {
	name { ChopVarOffset }
	domain { SDF }
	derivedFrom { Chop }
	author { G. S. Walter, E. A. Lee }
	version { $Id$ }
	location { SDF control palette }
	copyright { 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	desc {
This star has the same functionality as the
CutAndPaste star except now the offset parameter is
determined at run time through a control input.
	}

	input { name { offsetCntrl } type { int } }

	constructor { offset.setAttributes(A_NONCONSTANT | A_NONSETTABLE); }

	go {
		offset = int( offsetCntrl%0 );
		SDFCutAndPaste :: go();
	}
}
