defstar
{
    name { Expr }
    domain { CG56 } 
    desc { General expression evaluation. }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }

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
	default { "\tmove\t$ref(in#1),a\n\tmove\ta,$ref(out)" }
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
