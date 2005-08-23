defcore {
    name { UnitDelay }
    domain { ACS }
    coreCategory { FPCGC }
    corona { UnitDelay } 
    desc { Provides explicit pipeline delay of one. }
    version { @(#)ACSUnitDelayFPCGC.pl	1.3 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    codeblock (declarations) {
	/* static so that buffer will be initialized to zero */
	double $starSymbol(buffer)[1];
	int $starSymbol(index);
    }

    codeblock (init) {
	$starSymbol(index) = 0;
    {
	int i;
	$starSymbol(buffer)[0] = 0;
    }
    }

    codeblock (main) {
	$ref(output) = $starSymbol(buffer)[$starSymbol(index)];
	$starSymbol(buffer)[$starSymbol(index)] = $ref(input);
	if ( ++$starSymbol(index) >= 1 )
	    $starSymbol(index) -= 1;
    }

    setup {
    }

    initCode {
	addDeclaration(declarations);
	addCode(init);
    }

    go {
	addCode(main);
    }
}
