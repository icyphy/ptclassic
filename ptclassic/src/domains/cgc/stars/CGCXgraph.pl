defstar {
	name { Xgraph }
	domain { CGC }
	desc { Generate a plot with the pxgraph program. }
	version {$Id$}
	author { S. Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
This star is actually equivalent to the XMgraph star with only one input.
The input signal is plotted using the \fIpxgraph\fR program.
This program must be in your path, or this star will not work!
The \fItitle\fR parameter specifies a title for the plot.
The \fIsaveFile\fR parameter optionally specifies a file for
storing the data in a syntax acceptable to pxgraph.
A null string prevents any such storage.
The \fIoptions\fR string is passed directly to the pxgraph program
as command-line options.  See the manual section describing pxgraph
for a complete explanation of the options.
.Ir "pxgraph program"
.Id "graph, X window"
	}
	input {
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
		desc {File to save the input to the pxgraph program.}
	}
	defstate {
		name {options}
		type {string}
		default {"-bb -tk =800x400"}
		desc { Command line options for pxgraph.}
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
	defstate {
		name {resources}
		type {stringarray}
		default {"STDIO"}
		desc {Resources required by star.}
	}
	defstate {
		name {count}
		type {int}
		default { 0 }
		desc {Samplecounter}
		attributes {A_NONSETTABLE|A_NONCONSTANT}
	}
	ccinclude { "Target.h" }

	setup {
		index = xInit;
	}
	initCode {
                addDeclaration("    FILE* $starSymbol(fp);");
                addInclude("<stdio.h>");
		StringList w = "    if(!($starSymbol(fp) = fopen(\"";
		w << target()->name() << "_$starSymbol(temp)";
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
		
	// escape any double quotes in a string that is to be inserted into
	// a constant C string in the generated code;
	// the returned value resides in a static buffer and is whiped out
	// on each invocation
	method {
		// I'd like to declare this as a static function, but there
		// seems to be no way to achieve this with ptlang.
		name { sanitizeString }
		type { "const char*" }
		arglist { "(StringList s)" }
		access { protected }
		code {
			// quick implementation of a string buffer
			static class Buffer {
			   public:
				Buffer()  { buf = NULL; vsize = psize = 0; }
			// omitting the destructor since GCC 2.5.8 reports an internal
			// compiler error
			//	~Buffer() { if (buf)  free(buf); }

				void initialize() {
				    if (buf)  free(buf), buf = NULL;
				    vsize = psize = 0;
				}

				void append(char c) {
				    if (vsize >= psize)
					    buf = (char*) (buf ? realloc(buf, psize += 1024)
							       : malloc(psize += 1024));
				    buf[vsize++] = c;
				}

				operator const char* ()
				{
				    if (vsize == 0 || buf[vsize-1])
					append('\0');
				    return buf;
				}
			   private:
				// the string buffer
				char* buf;
				// virtual/physical buffer size
				int vsize, psize;
			} buffer;

			buffer.initialize();
				
			for (const char* sp=s; *sp; sp++) {
			    if (*sp == '\"')
				    buffer.append('\\');
			    buffer.append(*sp);
			}
			return (const char*) buffer;
		}
	}

	go {
@	if (++$ref(count) >= $val(ignore)) 
@		fprintf($starSymbol(fp),"%g %g\n",$ref(index),$ref(input));
@	$ref(index) += $val(xUnits);
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
			cmd << target()->name(); 
			cmd << "_$starSymbol(temp)" << " >> ";
			cmd << sf << "; /bin/echo \"\" >> " << sf << "; ";
		}

		cmd << "pxgraph ";

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
		cmd << target()->name() << "_$starSymbol(temp)";

		// remove temporary files
		cmd << "; /bin/rm -f " << target()->name();
		cmd << "_$starSymbol(temp)";

		cmd << ") &";
		StringList out = "    system(\"";
		out << sanitizeString(cmd) << "\");\n";
		addCode(out);
	}
	exectime {
		return 7;
	}
}
