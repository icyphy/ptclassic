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
    codeblock(globalDecl){
      union $sharedSymbol(CGCVISStereoIn,regoverlay) {
	vis_d64 regvaluedbl;
	vis_s16 regvaluesh[4];
      };
    }
    codeblock(mainDecl){
      union $sharedSymbol(CGCVISStereoIn,regoverlay) $starSymbol(packit);
    }
    codeblock (convert) {
      /* Convert data in buffer to Output format */

      for ($starSymbol(counter)=0; $starSymbol(counter) <($val(blockSize)/8);
	   $starSymbol(counter)++) {
	$starSymbol(packit).regvaluesh[0] =
	  $starSymbol(buf)[2*$starSymbol(counter)];
	$starSymbol(packit).regvaluesh[1] =
	  $starSymbol(buf)[2*$starSymbol(counter)+1];
	$starSymbol(packit).regvaluesh[2] =
	  $starSymbol(buf)[2*$starSymbol(counter)+2];
	$starSymbol(packit).regvaluesh[3] =
	  $starSymbol(buf)[2*$starSymbol(counter)+3];

	$ref(leftright,$starSymbol(counter)) = $starSymbol(packit).regvaluedbl;	
      }
    }

    setup {
      CGCStereoBase::setup();
      leftright.setSDFParams(int(blockSize/8), int(blockSize/8)-1);
    }

    initCode {
      CGCStereoBase::initCode();
      addGlobal(globalDecl,"CGCVISStereoIn_globalDecl");
      addDeclaration(mainDecl);
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
		       << "\"" << encodingType << "\", "
		       << "\"" << inputPort << "\", "
		       <<  volume << ", " 
		       <<  balance << ", " 
		       << "1);\n";
      addCode(updateParameters);
    }

    go {
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
