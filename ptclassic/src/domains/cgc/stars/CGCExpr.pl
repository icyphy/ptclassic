defstar
{
    name { Expr }
    domain { CGC } 
    desc { General expression evaluation. }
    version { $Id$ }
    author { T. M. Parks }
    location { CGC main library }

    inmulti
    {
	name { in }
	type { float }
    }

    output
    {
	name { out }
	type { float}
    }

    state
    {
	name { expr }
	type { string }
	default { "$ref(in#1)" }
	desc { Expression to evaulate. }
    }

    state
    {
	name { include }
	type { stringArray }
	default { "<math.h>" }
	desc { List of necessary include files. }
    }

    state
    {
	name { runTime }
	type { int }
	default { "2" }
	desc { execution time }
    }

    setup
    {
	for(int i = 0; i < include.size(); i++)
	    addInclude(include[i]);
    }

    go
    {
	StringList code;
	code << "$ref(out) = " << expr << ";\n";
	addCode(code);
    }

    exectime {
	return int(runTime);
    }
}
