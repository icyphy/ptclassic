defstar {
	name { Xprism3 }
	domain { MDSDF }
	desc {
Generate a plot of a 2D signal with the xprism3 program.
Due to a bug in xprism3 reading raw files from the command line,
we go throug the intermediary step of converting the raw file
to a Viff file using raw2viff.
	}
	version {$Id$}
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	explanation {
The input signal is plotted using the \fIxprism3\fR program which
is provided with the Ptolemy distribution.
The \fItitle\fR parameter specifies a title for the plot.
The \fIsaveFile\fR parameter optionally specifies a file for
storing the data in a syntax acceptable to xprism3.
A null string prevents any such storage.
The \fIoptions\fR string is passed directly to the xprism3 program
as command-line options.  See the manual section describing xprism3
for a complete explanation of the options.
.Ir "xprism program"
.Id "graph, X window"
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
	setup {
                input.setMDSDFParams(int(numRows),int(numCols));
	}
	go {
          // read data from input
	  FloatSubMatrix* inputMatrix = (FloatSubMatrix*)(input.getInput());
	  int del = 0;

	  char fileName[256]; fileName[0] = '\000';
	  if((const char*)saveFile) {
	    strcpy(fileName, (const char*)saveFile);
	    del = 0;
	  }
	  if(fileName[0] == '\000') {
	    char* nm = tempFileName();
	    strcpy(fileName, nm);
	    del = 1;
	    LOG_DEL; delete [] nm;
	  }
	  
	  FILE * fptr = fopen(fileName, "w");
	  if(fptr == (FILE*)NULL) {
	    Error::abortRun(*this, "can not create: ", fileName);
	    delete inputMatrix;
	    return;
	  }
          int size = int(numRows) * int(numCols);
          // write out as float instead of double
	  float* buffer = new float[size];
	  float* p = buffer;
	  for(int i = 0; i < size; i++)
	    *p++ = (float)inputMatrix->entry(i);
	  fwrite((char *)buffer,sizeof(float),size,fptr);
	  fclose(fptr);
	  
	  StringList cmd;
	  StringList tmpFileName;
	  tmpFileName << fileName << ".viff";
	  // first generate the command to run raw2viff
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
