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
	defstate {
		name { fileName }
		type { string }
		default { "cout" }
		desc { Filename for output. }
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
		addInclude("#include <stdio.h>\n");
		gencode(openfile);
	    }
	}
codeblock (openfile) {
    if(!($starSymbol(fp)=fopen("$val(fileName)","w")))
	fprintf(stderr,"ERROR: cannot open output file for Printer star.\n");
/* second if temporarily necessary: can't use brackets */
    if(!$starSymbol(fp)) exit(1);
}
	go {
	    for (int i = 1; i <= input.numberPorts(); i++) {
		char buf[80];
		if(fileOutput) {
		    sprintf(buf,
"\tfprintf($starSymbol(fp),\"%%f\\n\", $ref(input#%d));\n", i);
		} else {
		    sprintf(buf, "\tprintf(\"%%f\\n\", $ref(input#%d));\n", i);
		}
		gencode(CodeBlock(buf));
	    }
	}
	wrapup {
	    if(fileOutput)
		gencode(CodeBlock("    fclose($starSymbol(fp));\n"));
	}
}
