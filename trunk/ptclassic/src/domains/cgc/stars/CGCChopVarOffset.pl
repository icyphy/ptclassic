defstar {
	name { ChopVarOffset }
	domain { CGC }
	derivedFrom { Chop }
	author { S. Ha }
	version { $Id$ }
	location { CGC main library }
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
		offset.setAttributes(A_NONCONSTANT|A_NONSETTABLE); 
	}

	codeblock (init) {
	$ref(offset) = $ref(offsetCntrl);
	}
        codeblock(range) {
        int hiLim = $val(nwrite) - $ref(offset) - 1;
        if (hiLim >= $val(nwrite)) hiLim = $val(nwrite) - 1;
        else if ($val(use_past_inputs)) hiLim = $val(nwrite) - 1;

        int loLim = $val(nwrite) - $val(nread) - $ref(offset);
        if (loLim < 0) loLim = 0;

        int inidx = $val(nread) - $val(nwrite) + $ref(offset);
        if (inidx < 0) inidx = 0;
        }
	go {
		addCode(decl);			// look at CGCChop star
		addCode(init);
		addCode(range);
		if (strcmp(input.resolvedType(), "COMPLEX") == 0) 
		  addCode(complexOut);		// look at CGCChop star
		else
		  addCode(nonComplexOut);	// look at CGCChop star
	}
	exectime {
		return CGCChop::myExecTime() + 8;
	}
}
