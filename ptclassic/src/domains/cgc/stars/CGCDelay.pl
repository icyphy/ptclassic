defstar
{
    name { Delay }
    domain { CGC }
    desc { Bulk delay. }
    version { @(#)CGCDelay.pl	1.6 1/11/96 }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    input
    {
	name { input }
	type { float }
    }

    output
    {
	name { output }
	type { float }
    }

    state
    {
	name { delay }
	type { int }
	default { 1 }
	desc { Number of delay samples. }
    }

    codeblock (declarations) {
	/* static so that buffer will be initialized to zero */
	double $starSymbol(buffer)[$val(delay)];
	int $starSymbol(index);
    }

    codeblock (init) {
	$starSymbol(index) = 0;
    {
	int i;
	for (i = 0 ; i < $val(delay) ; i++)
	    $starSymbol(buffer)[i] = 0;
    }
    }

    codeblock (main) {
	$ref(output) = $starSymbol(buffer)[$starSymbol(index)];
	$starSymbol(buffer)[$starSymbol(index)] = $ref(input);
	if ( ++$starSymbol(index) >= $val(delay) )
	    $starSymbol(index) -= $val(delay);
    }

    setup {
	if (!(int) delay) forkInit(input,output);
    }

    initCode {
	if (!(int) delay) return;
	addDeclaration(declarations);
	addCode(init);
    }

    go {
	if (!(int) delay) return;
	addCode(main);
    }
}
