defstar {
	name { XMgraph }
	domain { CGC }
	desc { Generate a multi-signal plot with the xgraph program. }
	version {$Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	explanation {
The input signal is plotted using the \fIxgraph\fR program.
This program must be in your path, or this star will not work!
The \fItitle\fR parameter specifies a title for the plot.
The \fIsaveFile\fR parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The \fIoptions\fR string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
	}
	inmulti {
		name { input }
		type { float }
	}
	defstate {
		name {title}
		type {string}
		default {"X graph"}
		desc { Title for the plot. }
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc { File name for saving plottable data. }
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc {Command line options for the xgraph program.}
	}
	defstate {
		name {ignore}
		type {int}
		default { 0 }
		desc { Number of initial values to ignore.}
	}
	defstate {
		name {xUnits}
		type {float}
		default { 1.0 }
		desc { For labeling, horizontal increment between samples. }
	}
	defstate {
		name {xInit}
		type {float}
		default { 0.0 }
		desc { For labeling, horizontal value of the first sample. }
	}
	defstate {
		name {numIn}
		type {int}
		default { 1 }
		attributes {A_NONCONSTANT|A_NONSETTABLE}
	}
	defstate {
		name {index}
		type {float}
		default { 0.0 }
		attributes {A_NONCONSTANT|A_NONSETTABLE}
	}
	defstate {
		name {ix}
		type {int}
		default { 0 }
		attributes {A_NONCONSTANT|A_NONSETTABLE}
	}
	private {
		int count;
	}
	start {
		index = xInit;
		numIn = input.numberPorts();
		count = 0;
	}

	initCode {
                StringList s = "FILE (*$starSymbol(fp))[";
		s += input.numberPorts();
		s += "]\n";
		StringList ss = processCode(CodeBlock((const char*)s));
                addDeclaration(ss);
                addInclude("<stdio.h>");
		for (int i = 0; i < input.numberPorts(); i++) {
			StringList w = "    if(!($starSymbol(fp)[";
			w += i;
			w += "] = fopen(\"$starSymbol(temp)";
			w += i;
			w += "\",\"w\")))";
			gencode(CodeBlock((const char*) w));
			gencode(err);
			StringList z = "    if(!$starSymbol(fp)[";
			z += i;
			z += "]) exit(1);\n";
			gencode(CodeBlock((const char*)z));
		}
	}

codeblock (err) {
        fprintf(stderr,"ERROR: cannot open output file for Xgraph star.\n");
}
		
	go {
		count++;
		if (count <= int(ignore)) return;
		for (int i = 1; i <= int(numIn); i++) {
			ix = i;
			gencode(CodeBlock(
"\tfprintf($starSymbol(fp)[i],\"%g %g\\n\",$ref(index),$ref(input#ix));\n"));
		}
		gencode(CodeBlock("\t$ref(index) += $val(xUnits);\n"));
		
	}

codeblock(closeFile) {
	for (i = 0; i < $val(numIn); i++)
	    fclose($starSymbol(fp)[i]);
}

	wrapup {
		gencode(CodeBlock("    { int i;\n"));

		// close the files
		gencode(closeFile);

		StringList cmd;
		cmd += "(xgraph ";

		// put title on command line

		const char* ttl = title;

		if (ttl && *ttl) {
			if (strchr(ttl,'\'')) {
				cmd += "-t \""; cmd += ttl; cmd += "\" ";
			}
			else {
				cmd += "-t '"; cmd += ttl; cmd += "' ";
			}
		}

		const char* opt = options;

		// put options on the command line
		if (opt && *opt) {
			cmd += opt;
			cmd += " ";
		}

		// put file names
		for (int i = 0; i < int(numIn); i++) {
			cmd += "$starSymbol(temp) ";
			cmd += i;
			cmd += " ";
		}

		// save File
		const char* sf = saveFile;
		if (sf != NULL && *sf != 0) {
			for (i = 0; i<int(numIn); i++) {
				cmd += "; /bin/cat ";
				cmd += "$starSymbol(temp)";
				cmd += i;
				cmd += " >> ";
				cmd += sf;
				cmd += "; /bin/echo \"\" >> ";
				cmd += sf;
			}
		}

		// remove temporary files
		for (i = 0; i < int(numIn); i++) {
			cmd += "; /bin/rm -f ";
			cmd += "$starSymbol(temp)";
			cmd += i;
		}

		cmd += ") &";
		StringList out = "    system(";
		out += cmd;
		out += ");\n";
		gencode(CodeBlock((const char*)out));
	}
}
