defcore {
    name { Printer }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Printer } 
    desc { Prints out one sample from each input port per line.
If "fileName" is not equal to "cout" (the default) or "stdout", it
specifies the filename to write to. }
    version { @(#)ACSPrinterFPCGC.pl	1.4 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    defstate {
      name { fileNameCGC }
      type { string }
      default { "<stdout>" }
      desc { Filename for output. }
    }
    defstate {
      name { index }
      type { int }
      default { "1" }
      desc { index for multi input trace. }
      attributes { A_NONSETTABLE|A_NONCONSTANT }
    }
    private {
      int fileOutput;
      char*	expandedFileName;
    }
    initCode {
      const char *fn = fileNameCGC;
      fileOutput = ! ( fn==NULL
		       || strcmp(fn, "cout")==0 || strcmp(fn, "stdout")==0
		       || strcmp(fn, "<cout>")==0 || strcmp(fn, "<stdout>")==0);
      if(fileOutput) {
	StringList s;
	s << "    FILE* $starSymbol(fp);";
	addDeclaration(s);
	addInclude("<stdio.h>");
	expandedFileName = expandPathName(fn);
	s = "\tif(!($starSymbol(fp)=fopen(\"";
	s << expandedFileName << "\",\"w\"))) {\n";
	s << "\t\tfprintf(stderr,\"ERROR: cannot open output file for Printer star.\n\");\n";
	s << "\t\texit(1);\n";
	s << "\t}\n";
	addCode(s);
      }
    }
    go {
      for (int i = 1; i <= corona.input.numberPorts(); i++)
	{
	  index = i;
	  if(fileOutput) {
	    addCode("\tfprintf($starSymbol(fp),\"%f\\t\", (double) ($ref(input#index)));\n");
	  } 
	  else
	    {
	    addCode("\tprintf(\"%f\\t\", (double) ($ref(input#index)));\n");
	  }
	}
      if (fileOutput) {
	addCode("\tfprintf($starSymbol(fp),\"\\n\");\n");
      } 
      else {
	addCode("\tprintf(\"\\n\");\n");
      }
    }
    wrapup {
      if(fileOutput)
	addCode("\tfclose($starSymbol(fp));\n");
    }
    exectime {
      return 6;       /* unmeasured value */
    }
}
