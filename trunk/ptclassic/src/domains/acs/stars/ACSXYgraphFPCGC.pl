defcore {
    name { XYgraph }
    domain { ACS }
    coreCategory { FPCGC }
    corona { XYgraph } 
    desc { Generates an X-Y plot with the pxgraph program.
The X data is on "xInput" and the Y data is on "input". }
    version { @(#)ACSXYgraphFPCGC.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	defstate {
		name {count}
		type {int}
		default { 0 }
		desc {Samplecounter}
		attributes {A_NONSETTABLE|A_NONCONSTANT}
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
codeblock(closeFile) {
    fclose($starSymbol(fp));
}

	wrapup {
		// close the files
		addCode(closeFile);

		StringList cmd;
		cmd << "( ";

		// save File
		const char* sf = corona.saveFile;
		if (sf != NULL && *sf != 0) {
			cmd << "/bin/cat ";
			cmd << target()->name(); 
			cmd << "_$starSymbol(temp)" << " >> ";
			cmd << sf << "; /bin/echo \"\" >> " << sf << "; ";
		}

		cmd << "pxgraph ";

		// put title on command line

		const char* ttl = corona.title;

		if (ttl && *ttl) {
			if (strchr(ttl,'\'')) {
				cmd << "-t \"" << ttl << "\" ";
			}
			else {
				cmd << "-t '" << ttl << "' ";
			}
		}

		const char* opt = corona.options;

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
	go {
@	if (++$ref(count) >= $val(ignore)) 
@		fprintf($starSymbol(fp),"%g %g\n",$ref(xInput),$ref(input));
	}
	exectime {
		return 6;
	}


}

