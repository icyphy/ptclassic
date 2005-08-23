defstar {
	name { Xprism3 }
	domain { MDSDF }
	desc {
Generate a plot of a two-dimensional signal with the xprism3 program.  Due
to a bug in xprism3 reading raw files from the command line, we go through the
intermediate step of converting the raw file to a Viff file using raw2viff.
	}
	version {@(#)MDSDFXprism3.pl	1.11 05/28/98}
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	htmldoc {
The input signal is plotted using the <i>xprism3</i> program which is
provided with the Ptolemy distribution.  The <i>title</i> parameter specifies
a title for the plot.  The <i>saveFile</i> parameter optionally specifies a
file for storing the data in a syntax acceptable to xprism3.  A null string
prevents any such storage.  The <i>options</i> string is passed directly to
the xprism3 program as command-line options.  See the manual section
describing xprism3 for a complete explanation of the options.
<a name="xprism program"></a>
<a name="graph, X window"></a>
	}
	input {
		name { input }
		type { FLOAT_MATRIX }
	}
	defstate {
		name { numRows }
		type { int }
		default { 100 }
		desc { The number of rows in the input. }
	}
	defstate {
		name { numCols }
		type { int }
		default { 100}
		desc { The number of columns in the input. }
	}
	defstate {
		name {programName}
		type {string}
		default {"xprism3"}
		desc {File to save the input to the xgraph program.}
	}
	defstate {
		name {saveFile}
		type {string}
		default {"tmpFile"}
		desc {File to save the input to the xgraph program.}
	}
	ccinclude { <stdio.h> }
	setup {
                input.setMDSDFParams(int(numRows),int(numCols));
	}
	go {
          // read data from input
	  FloatSubMatrix* inputMatrix = (FloatSubMatrix*)(input.getInput());
	  int del = FALSE;

	  const char* iname = (const char*)saveFile;
	  char* nm = 0;
	  if (iname && *iname) {
	    nm = expandPathName(iname);
	    del = FALSE;
	  }
	  else {
	    nm = tempFileName();
	    del = TRUE;
	  }
	  StringList fileName = nm;
	  delete [] nm;

	  FILE *fp = fopen(fileName, "w");
	  if (fp == 0) {
	    delete inputMatrix;
	    Error::abortRun(*this, "cannot open '", fileName, "' for writing.");
	    return;
	  }

          // write out as float instead of double
          int size = int(numRows) * int(numCols);
	  float* buffer = new float[size];
	  float* p = buffer;
	  for(int i = 0; i < size; i++) {
	    *p++ = (float)inputMatrix->entry(i);
	  }
	  fwrite((char *)buffer, sizeof(float), size, fp);
	  fclose(fp);
	  delete [] buffer;

	  StringList tmpFileName = fileName;
	  tmpFileName << ".viff";

	  // first generate the command to run raw2viff
	  StringList cmd;
          cmd << "( raw2viff -i " << fileName << " -o " << tmpFileName;
	  cmd << " -t \"float\" -mt \"Local Machine\" -r ";
	  cmd << int(numRows) << " -c " << int(numCols) << ";";

          // next, add command to run xprism3
          cmd << programName << " -i11 " << tmpFileName;
	  if(del) {
            cmd << "; rm -f " << tmpFileName;
	    cmd << "; rm -f " << fileName;
	  }
          cmd << ")&";   // run command in the background
	  system(cmd);
	}
}
