defcore {
    name { RamDelay }
    domain { ACS }
    coreCategory { FPCGC }
    corona { RamDelay } 
    desc { Provides explicit pipeline delay (default 1). }
    version { @(#)ACSRamDelayFPCGC.pl	1.4 07/30/01}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    codeblock(decl) {
	/* static so that buffer will be initialized to zero */
	double $starSymbol(buffer)[$val(delays)];
	int $starSymbol(index);
    }

    codeblock (init) {
	$starSymbol(index) = 0;
    {
	int i;
	for (i = 0 ; i < $val(delays) ; i++)
	    $starSymbol(buffer)[i] = 0;
    }
    }

    codeblock (main) {
	$ref(output) = $starSymbol(buffer)[$starSymbol(index)];
	$starSymbol(buffer)[$starSymbol(index)] = $ref(input);
	if ( ++$starSymbol(index) >= $val(delays) )
	    $starSymbol(index) -= $val(delays);
    }

    setup {
	if (!(int) corona.delays) forkInit(corona.input, corona.output);
    }

    initCode {
	if (!(int) corona.delays) return;
	addDeclaration(decl);
	addCode(init);
    }

    go {
	if (!(int) corona.delays) return;
	addCode(main);
    }
}
