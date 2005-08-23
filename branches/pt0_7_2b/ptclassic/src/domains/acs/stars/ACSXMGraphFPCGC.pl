defcore {
	name { XMGraph }
	domain { ACS }
	coreCategory { FPCGC }
	corona { XMGraph }
        version {@(#)ACSXMGraphFPCGC.pl	1.4 09/08/99}
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
	defstate {
		name {numIn}
		type {int}
		default { 1 }
		attributes {A_NONCONSTANT|A_NONSETTABLE}
	}
	defstate {
		name {resources}
		type {stringarray}
		default { "STDIO" }
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
		corona.index = corona.xInit;
		numIn = corona.input.numberPorts();
	}

	initCode {
                StringList s = "FILE* $starSymbol(fp)[";
		s << corona.input.numberPorts() << "];";
                addDeclaration(s);
                addInclude("<stdio.h>");
		for (int i = 0; i < corona.input.numberPorts(); i++) {
			StringList w = "if(!($starSymbol(fp)[";
			w << i <<  "] = fopen(\"";
			w << target()->name() << "_$starSymbol(temp)";
			w << i << "\",\"w\")))";
			addCode(w);
			addCode(err);
		}
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
		// I prefer declaring this as a static function, but there
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
	  @    if (++$ref(count) >= $val(ignore)) {
	  for (int i = 1; i <= int(numIn); i++) {
	    @        fprintf($starSymbol(fp)[@(i-1)],"%g %g\n",
	    @		 $ref(index),$ref(input#@i));
	  }
	  @    }
	  @    $ref(index) += $val(xUnits);
	}

codeblock(closeFile) {
for (i = 0; i < $val(numIn); i++) fclose($starSymbol(fp)[i]);
}

	wrapup {
		int i;
		addCode("{\n");
		addCode("int i;\n");

		// close the files
		addCode(closeFile);

		StringList cmd = "( ";

		// save File
		const char* sf = corona.saveFile;
		if (sf != NULL && *sf != 0) {
			for (i = 0; i<int(numIn); i++) {
				cmd << "/bin/cat ";
				cmd << target()->name(); 
				cmd << "_$starSymbol(temp)" << i << " >> ";
				cmd << sf << "; /bin/echo \"\" >> " << sf;
				cmd << "; ";
			}
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

		// put file names
		for (i = 0; i < int(numIn); i++) {
			cmd << target()->name() << "_$starSymbol(temp)";
			cmd << i << " ";
		}

		// remove temporary files
		for (i = 0; i < int(numIn); i++) {
			cmd << "; /bin/rm -f " << target()->name();
			cmd << "_$starSymbol(temp)" << i;
		}

		cmd << ") &";
		StringList out = "system(\"";
		out << sanitizeString(cmd) << "\");\n}\n";
		addCode(out);
	}
	exectime {
		return 6 * int(numIn) + 1;
	}
}
