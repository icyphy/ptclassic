defstar {
	name { Xgraph }
	domain { CGC }
	desc { Generate a plot with the xgraph program. }
	version {$Id$}
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
This star is actually equivalent to the XMgraph star with only one input.
The input signal is plotted using the \fIxgraph\fR program.
This program must be in your path, or this star will not work!
The \fItitle\fR parameter specifies a title for the plot.
The \fIsaveFile\fR parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The \fIoptions\fR string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
.Ir "xgraph program"
.Id "graph, X window"
	}
	input {
		name { input }
		type { anytype }
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
		desc {File to save the input to the xgraph program.}
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc { Command line options for xgraph.}
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
		name {index}
		type {float}
		default { 0.0 }
		attributes {A_NONSETTABLE|A_NONCONSTANT}
	}
	ccinclude { "Target.h" }

	setup {
		index = xInit;
		count = 0;
	}
	protected {
		int count;
	}
	initCode {
                addDeclaration("    FILE* $starSymbol(fp);");
                addInclude("<stdio.h>");
		StringList w = "    if(!($starSymbol(fp) = fopen(\"";
		w << targetPtr->name() << "_$starSymbol(temp)";
		w << "\",\"w\")))";
		addCode(w);
		addCode(err);
	}

codeblock (err) {
    {
        fprintf(stderr,"ERROR: cannot open output file for Xgraph star.\n");
	exit(1);
    }
}
		
	go {
		count++;
		if (count <= int(ignore)) return;
		addCode(
"\tfprintf($starSymbol(fp),\"%g %g\\n\",$ref(index),$ref(input));\n");
		addCode("\t$ref(index) += $val(xUnits);\n");
	}

codeblock(closeFile) {
    fclose($starSymbol(fp));
}

	wrapup {
		// close the files
		addCode(closeFile);

		StringList cmd;
		cmd << "( ";

		// save File
		const char* sf = saveFile;
		if (sf != NULL && *sf != 0) {
			cmd << "/bin/cat ";
			cmd << targetPtr->name(); 
			cmd << "_$starSymbol(temp)" << " >> ";
			cmd << sf << "; /bin/echo \"\" >> " << sf << "; ";
		}

		cmd << "xgraph ";

		// put title on command line

		const char* ttl = title;

		if (ttl && *ttl) {
			if (strchr(ttl,'\'')) {
				cmd << "-t \"" << ttl << "\" ";
			}
			else {
				cmd << "-t '" << ttl << "' ";
			}
		}

		const char* opt = options;

		// put options on the command line
		if (opt && *opt) {
			cmd << opt << " ";
		}

		// put file name
		cmd << targetPtr->name() << "_$starSymbol(temp)";

		// remove temporary files
		cmd << "; /bin/rm -f " << targetPtr->name();
		cmd << "_$starSymbol(temp)";

		cmd << ") &";
		StringList out = "    system(\"";
		out << cmd << "\");\n";
		addCode(out);
	}
	exectime {
		return 7;
	}
}
