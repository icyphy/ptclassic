defstar {
    name { VISStereoOut }
    domain { CGC }
    derivedFrom { StereoBase }
    descriptor {
Writes Compact Disc audio format to a file given by "fileName".  The file
can be the audio port /dev/audio, if supported by the workstation.  The
star writes "blockSize" 16-bit samples at each invocation.  The block
size should be a multiple of 4.
    }
    explanation {
This code is based on the description of the audio driver which can be
obtained by looking at the man page of audio.
    }
    version { @(#)CGCStereoOut.pl	1.10 07/15/96 }
    author { Sunil Bhave }
    copyright {
Copyright (c) 1990- The Regents of the University 
of California. All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty 
provisions.
    }
    location { CGC main library }
    
    input {
      name { leftright }
      type { float }
      desc { Left and Right channel input }
    }

    defstate {
      name { outputPort }
      type { string }
      default { line_out }
      desc { Audio input port: line_out or speaker. }
      attributes { A_GLOBAL }
    }

    protected {
      int standardOutput:1;
    }
    codeblock (convert) {
	/* Take data from Input and put it in buffer */
	/* Data in buffer is alternate left and right channels */
	for (i = 0; i < ($val(blockSize)/2); i ++) {
	  $starSymbol(buf)[i] = ceil($ref(leftright,i)*32768.0);	
	}
    }

    setup {
      CGCStereoBase::setup();
      leftright.setSDFParams(int(blockSize/2), int(blockSize/2)-1);
    }
      
    initCode {
      CGCStereoBase::initCode();
      if (standardIO) {
	addCode(noOpen);
      }
      else {
	addInclude("<fcntl.h>");            // Define O_WRONLY
	addCode(openFileForWriting);
      }

      /* Update parameters */
      StringList updateParameters = "$sharedSymbol(CGCStereoBase,set_parameters)";
      updateParameters << "($starSymbol(file), "
		       << "\"" << outputPort << "\", "
		       <<  volume << ", " 
		       <<  balance << ", " 
		       << "0);\n";
      addCode(updateParameters);

    }

    go {
      addDeclaration("int i;");
      addCode(convert);
      addCode(write);
    }
    
    wrapup {
      CGCStereoBase::wrapup();
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
