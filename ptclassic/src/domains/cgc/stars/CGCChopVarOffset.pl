defstar {
	name { ChopVarOffset }
	domain { CGC }
	derivedFrom { Chop }
	author { S. Ha }
	version { $Id$ }
	location { CGC control palette }
	copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	desc {
This star has the same functionality as the
Chop star except now the offset parameter is
determined at run time through a control input.
	}

	input { name { offsetCntrl } type { int } }

	constructor { 
		noInternalState();
		offset.setAttributes(A_NONCONSTANT | A_NONSETTABLE); 
	}

	codeblock (init) {
		$ref(offset) = $ref(offsetCntrl);
	}
	go {
		addCode(decl);	// look at CGCChop star
		addCode(init);
		addCode(out);	// look at CGCChop star
	}
	exectime {
		return int(nwrite) + 1;
	}
}
