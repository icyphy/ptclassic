defstar {
    name { VISStereoIn }
    domain { CGC }
    derivedFrom { StereoBase }
    descriptor {
Reads Compact Disc audio format from a file given by "fileName". The file
can be the audio port /dev/audio, if supported by the workstation.  The
star reads "blockSize" 16-bit samples at each invocation.  The block
size should be a multiple of 4.
    }
    explanation {
This code is based on the description of the audio driver which can
be obtained by looking at the man page for audio.
    }
    version { $Id$ }
    author { Sunil Bhave }
    copyright {
Copyright (c) 1990-1996 The Regents of the University 
of California. All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty 
provisions.
    }
    location { CGC vis library }

    output {
      name { leftright }
      type { float }
      desc { Left and Right channel output }
    }
	
    defstate {
       name { inputPort }
       type { string }
       default { line_in }
       desc { Audio input port: line_in or internal_cd. }
       attributes { A_GLOBAL }
    }

    codeblock (convert) {
      /* Convert data in buffer to Output format */
      for (i=0;i<($val(blockSize)/2);i++) {
	$ref(leftright,$val(blockSize)/2-1-i) = $starSymbol(buf)[i] /32768.0;
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
        addInclude("<fcntl.h>");            // Define O_RDONLY
	addCode(openFileForReading);
      }

      /* Update parameters */
      StringList updateParameters = "$sharedSymbol(CGCStereoBase,set_parameters)";
      updateParameters << "($starSymbol(file), "
		       << "\"" << inputPort << "\", "
		       <<  volume << ", " 
		       <<  balance << ", " 
		       << "1);\n";
      addCode(updateParameters);
    }

    go {
      addDeclaration("int i;");
      addCode(read);
      addCode(convert);
    }

    wrapup {
      CGCStereoBase::wrapup();
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
