defstar
{
    name { Expr }
    domain { CGC } 
    desc { General expression evaluation. }
    version { @(#)CGCExpr.pl	1.12 02/25/97}
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
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

    state {
	name { inDataType }
	type { string }
	default { "float" }
	desc{ "DataType of `in` porthole, one of [float,int,complex,anytype]" }
    }

    state {
	name { outDataType }
	type { string }
	default { "float" }
	desc
	 {"DataType of `out` porthole, one of [float,int,complex,=in]"}
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

    method {
      name { preinitialize }
      access { public }
      code {
	// We must change the porthole types at preinitialize time
	// so that porthole type resolution works correctly.
	// That means we have to initialize the states ourselves.
	initState();
	const char* letter = inDataType;
	switch (letter[0]) {
	case 'F':
	case 'f':
	    in.setPort(in.name(),this,FLOAT);
	    break;
	case 'I':
	case 'i':
	    in.setPort(in.name(),this,INT);
	    break;
	case 'C':
	case 'c':
	    in.setPort(in.name(),this,COMPLEX);
	    break;
	case 'A':
	case 'a':
	    in.setPort(in.name(),this,ANYTYPE);
	    break;
	default:
	    Error::abortRun(*this,"CGC Expr does not support the type",
			    inDataType);
	    break;
	}
	letter = outDataType;
	switch (letter[0]) {
	case 'F':
	case 'f':
	    out.setPort(out.name(),this,FLOAT);
	    break;
	case 'I':
	case 'i':
	    out.setPort(out.name(),this,INT);
	    break;
	case 'C':
	case 'c':
	    out.setPort(out.name(),this,COMPLEX);
	    break;
	case '=':
	    out.setPort(out.name(),this,ANYTYPE);
	    out.inheritTypeFrom(in);
	    break;
	default:
	    Error::abortRun(*this,"CGC Expr does not support the type",
			    outDataType);
	    break;
	}
      }
    }

    initCode
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

    exectime
    {
	return int(runTime);
    }
}
