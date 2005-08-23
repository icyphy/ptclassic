defstar
{
    name { Expr }
    domain { C50 } 
    desc { General expression evaluation. }
    version { @(#)C50Expr.pl	1.1  7/22/96 }
    author {  Luis Gutierrez }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 main library }

    inmulti
    {
	name { in }
	type { anytype }
    }

    output
    {
	name { out }
	type { =in }
    }

    state
    {
	name { expr }
	type { string }
	default { "\tlmmr\tar0,$addr(in#1)\n\tsmmr\tar0,$addr(out)\n" }
	desc { Expression to evaluate. }
    }

    state
    {
	name { runTime }
	type { int }
	default { "2" }
	desc { execution time }
    }

    go
    {
	StringList code;
	code << expr << '\n';
	addCode(expr);
    }

    exectime
    {
	return int(runTime);
    }
}
