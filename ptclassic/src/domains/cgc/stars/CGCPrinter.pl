defstar {
	name { Printer }
	domain { CGC }
	desc {
Prints out one sample from each input port per line
If "fileName" is not equal to "cout" (the default) or "stdout", it
specifies the filename to write to.
	}
	version { $Id$ }
	author { E. A. Lee }
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
	    fileOutput = strcmp(fileName, "cout") &&
			 strcmp(fileName, "stdout");
	    if(fileOutput) {
		StringList s =
			processCode(CodeBlock("FILE *$starSymbol(fp);\n"));
		addDeclaration(s);
		addInclude("<stdio.h>");
		gencode(openfile);
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
			gencode(CodeBlock(
"\tfprintf($starSymbol(fp),\"%f\\t\", $ref(input#index));\n"));
		} else {
			gencode(CodeBlock( 
"\tprintf(\"%f\\t\", $ref(input#index));\n"));
		}
	    }
	    if (fileOutput) {
		gencode(CodeBlock("\tfprintf($starSymbol(fp),\"\\n\");\n"));
	    } else {
		gencode(CodeBlock("\tprintf(\"\\n\");\n"));
	   }
	}
	wrapup {
	    if(fileOutput)
		gencode(CodeBlock("\tfclose($starSymbol(fp));\n"));
	}
}
