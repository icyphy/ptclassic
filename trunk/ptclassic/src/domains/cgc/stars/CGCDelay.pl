defstar
{
    name { Delay }
    domain { CGC }
    desc { Bulk delay. }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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

    codeblock (declarations)
    {
	/* static so that buffer will be initialized to zero */
	static double $starSymbol(buffer)[$val(delay)];
	int $starSymbol(index);
    }

    codeblock (init)
    {
	$starSymbol(index) = 0;
    }

    codeblock (main)
    {
	$ref(output) = $starSymbol(buffer)[$starSymbol(index)];
	$starSymbol(buffer)[$starSymbol(index)] = $ref(input);
	if ( ++$starSymbol(index) >= $val(delay) )
	    $starSymbol(index) -= $val(delay);
    }

    initCode
    {
	addDeclaration(declarations);
	addCode(init);
    }

    go
    {
	addCode(main);
    }
}
