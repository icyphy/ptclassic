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

    constructor {
      blockSize.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      blockSize.setInitValue(8180);
    }

    protected {
      int standardOutput:1;
    }
    codeblock(globalDecl){
      union $sharedSymbol(CGCVISStereoOut,regoverlay) {
	vis_d64 regvaluedbl;
	vis_s16 regvaluesh[4];
      };
    }
    codeblock(mainDecl){
      union $sharedSymbol(CGCVISStereoOut,regoverlay) $starSymbol(unpackit);
    }
    codeblock (convert) {
	/* Take data from Input and put it in buffer */
	/* Data in buffer is alternate left and right channels */
	for ($starSymbol(counter) = 0; $starSymbol(counter) <
	       ($val(blockSize)/8); $starSymbol(counter) ++) {
	  $starSymbol(unpackit).regvaluedbl = $ref(leftright,$starSymbol(counter));
	  $starSymbol(buf)[4*$starSymbol(counter)] = $starSymbol(unpackit).regvaluesh[0];
	  $starSymbol(buf)[4*$starSymbol(counter)+1] = $starSymbol(unpackit).regvaluesh[1];
	  $starSymbol(buf)[4*$starSymbol(counter)+2] = $starSymbol(unpackit).regvaluesh[2];
	  $starSymbol(buf)[4*$starSymbol(counter)+3] = $starSymbol(unpackit).regvaluesh[3];
	}
    }

    setup {
      CGCStereoBase::setup();
      leftright.setSDFParams(int(blockSize/8), int(blockSize/8)-1);
    }
      
    initCode {
      CGCStereoBase::initCode();
      addGlobal(globalDecl,"CGCVISStereoOut_globalDecl");
      addDeclaration(mainDecl);
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
		       << "\"" << encodingType << "\", "
		       << "\"" << outputPort << "\", "
		       <<  volume << ", " 
		       <<  balance << ", " 
		       << "0);\n";
      addCode(updateParameters);

    }

    go {
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
