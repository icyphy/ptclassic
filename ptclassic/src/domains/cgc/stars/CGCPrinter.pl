defstar {
	name { Printer }
	domain { CGC }
	desc {
Prints out one sample from each input port per line
If "fileName" is not equal to "cout" (the default) or "stdout", it
specifies the filename to write to.
	}
	version { $Id$ }
	author { E. A. Lee, Kennard }
	copyright { 1992 The Regents of the University of California }
	location { CGC main library }
	explanation {
This star prints its input, which may be any supported type.
There may be multiple inputs: all inputs are printed together on
the same line, separated by tabs.
	}
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	state {
		name { fileName }
		type { string }
		default { "cout" }
		desc { Filename for output. }
	}
	state {
		name { index }
		type { int }
		default { "1" }
		desc { index for multi input trace. }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	private {
		int fileOutput;
	}
	initCode {
	    const char *fn = fileName;
	    fileOutput = ! ( fn==NULL
	      || strcmp(fn, "cout")==0 || strcmp(fn, "stdout")==0
	      || strcmp(fn, "<cout>")==0 || strcmp(fn, "<stdout>")==0);
	    if(fileOutput) {
		StringList sym = processCode("$starSymbol(fp)");
		StringList s;
		s << "FILE* " << sym << ";\n";
		addDeclaration(s, sym);
		addInclude("<stdio.h>");
		addCode(openfile);
	    }
	}
codeblock (openfile) {
    if(!($starSymbol(fp)=fopen("$val(fileName)","w"))) {
	fprintf(stderr,"ERROR: cannot open output file for Printer star.\n");
    	exit(1);
    }
}
	go {
	    for (int i = 1; i <= input.numberPorts(); i++) {
		index = i;
		if(fileOutput) {
			addCode(CodeBlock(
"\tfprintf($starSymbol(fp),\"%f\\t\", $ref(input#index));\n"));
		} else {
			addCode(CodeBlock( 
"\tprintf(\"%f\\t\", $ref(input#index));\n"));
		}
	    }
	    if (fileOutput) {
		addCode(CodeBlock("\tfprintf($starSymbol(fp),\"\\n\");\n"));
	    } else {
		addCode(CodeBlock("\tprintf(\"\\n\");\n"));
	   }
	}
	wrapup {
	    if(fileOutput)
		addCode(CodeBlock("\tfclose($starSymbol(fp));\n"));
	}
}
