defstar {
    name { Expr }
    domain { CGC } 
    desc { General expression evaluation. }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }

    inmulti {
	name { in }
	type { float }
    }

    output {
	name { out }
	type { float}
    }

    state {
	name { expr }
	type { string }
	default { "$ref(in#1)" }
	desc {
C expression to evaluate. A semicolon will be appended to it.
	}
    }

    state {
	name { inDataType }
	type { string }
	default { "float" }
	desc{
DataType of in portholes, one of float, int, anytype, or complex.
	}
    }

    state {
	name { outDataType }
	type { string }
	default { "float" }
	desc {
DataType of the out porthole, one of float, int, complex, or =in.
	}
    }

    state {
	name { include }
	type { stringArray }
	default { "\"<math.h>\"" }
	desc {
List of necessary include files separated by white space.
Express standard include files as "<math.h>" and user include files
as "\\"myinclude.h\\"".  The extra syntax is necessary to override
one-character directives in the state parser.
	}
    }

    state {
	name { runTime }
	type { int }
	default { "2" }
	desc { estimated execution time }
    }

    // define strcasecmp and toupper, respectively
    ccinclude { <string.h>, <ctype.h> }

    method {
	name { returnDataType }
	access { protected }
	type { DataType }
	arglist { "(const char* dataTypeString)" }
	code {
		DataType datatype = 0;
		switch (toupper(dataTypeString[0])) {
		  case 'A':
		    if ( strcasecmp(dataTypeString, "anytype") == 0 )
			datatype = ANYTYPE;
		    break;
		  case 'C':
		    if ( strcasecmp(dataTypeString, "complex") == 0 )
			datatype = COMPLEX;
		    break;
		  case 'F':
		    if ( strcasecmp(dataTypeString, "float") == 0 )
			datatype = FLOAT;
		    break;
		  case 'I':
		    if ( strcasecmp(dataTypeString, "int") == 0 )
			datatype = INT;
		    break;
		}
		return datatype;
	}
    }

    setup {
	DataType intype = returnDataType(inDataType);
	if ( intype == 0 ) {
	    Error::abortRun(*this,
			    "CGCExpr star does not support the type ",
			    (const char*) inDataType);
	    return;
	}
	in.setPort("in", this, intype);

	DataType outtype = returnDataType(outDataType);
	if ( outtype == 0 ) {
	    Error::abortRun(*this,
			    "CGCExpr star does not support the type ",
			    (const char*) outDataType);
	    return;
	}
	out.setPort("out", this, outtype);
    }

    initCode {
	for(int i = 0; i < include.size(); i++)
	    addInclude(include[i]);
    }

    go {
	StringList code = "$ref(out) = ";
	code << expr << ";\n";
	addCode(code);
    }

    exectime {
	return int(runTime);
    }
}
