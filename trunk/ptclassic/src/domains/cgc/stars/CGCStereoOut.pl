defstar {
    name { StereoOut }
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
    version { $Id$ }
    author { Sunil Bhave }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University 
of California. All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty 
provisions.
    }
    location { CGC main library }
    
    input {
      name { left }
      type { float }
      desc { Left channel input }
    }

    input {
      name { right }
      type { float }
      desc { Right channel input }
    }
	

    defstate {
      name { outputPort }
      type { string }
      default { line_out }
      desc { Audio input port: line_out or speaker. }
      attributes { A_GLOBAL }
    }

    constructor {
      blockSize.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      blockSize.setInitValue(8180);
    }

    protected {
      int standardOutput:1;
    }

    codeblock (convert) {
      /* Take data from Input and put it in buffer */
      /* Data in buffer is alternate left and right channels */
      for (i = 0; i < ($val(blockSize)/2); i += 2) {
	$starSymbol(buf)[i] = ceil($ref(left,i/2)*32768.0);
	$starSymbol(buf)[i+1] = ceil($ref(right,i/2)*32768.0);
	}
    }

    setup {
      CGCStereoBase::setup();
      left.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
      right.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
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
