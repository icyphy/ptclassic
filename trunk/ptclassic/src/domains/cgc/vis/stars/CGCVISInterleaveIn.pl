defstar {
    name { VISInterleaveIn }
    domain { CGC }
    derivedFrom { AudioBase }
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
    author { Sunil Bhave and Bill Chen}
    copyright {
Copyright (c) 1990-1996 The Regents of the University 
of California. All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty 
provisions.
    }
    location { CGC vis library }

    output {
      name { stereoOut }
      type { float }
      desc { Left and right stereo channel output }
    }

    constructor {
      encodingType.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      encodingType.setInitValue("linear16");
      channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      channels.setInitValue(2);
    }

    setup {
      stereoOut.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
    }

    codeblock(globalDecl){
      union $sharedSymbol(CGCVISInterleaveIn,regoverlay) {
	vis_d64 regvaluedbl;
	vis_s16 regvaluesh[4];
      };
    }

    codeblock(mainDecl){
      union $sharedSymbol(CGCVISInterleaveIn,regoverlay) $starSymbol(packit);
    }

    initCode {
      CGCAudioBase::initCode();
      addInclude("<vis_types.h>"); 
      addGlobal(globalDecl,"CGCVISInterleaveIn_regoverlay");
      addDeclaration(mainDecl);
      addDeclaration(declarations("short", int(blockSize)));
      /* Open file for reading data */
      addCode(openFileForReading);	
      /* Update audio_control */
      StringList setupParameters = "$sharedSymbol(CGCAudioBase,audio_setup)";
      setupParameters  << "($starSymbol(file),"
		       << "\"" << encodingType << "\","
		       <<  sampleRate << "," 
		       <<  channels << ");\n";
      addCode(setupParameters);
      StringList controlParameters =  "$sharedSymbol(CGCAudioBase,audio_control)";
      controlParameters << "($starSymbol(file), "
			<< "\"" << portType << "\", "
			<<  volume << ", " 
			<<  balance << ", "
			<< "1);\n";
      addCode(controlParameters);
    }

    codeblock (setbufptr) {
      $starSymbol(bufferptr) = $starSymbol(buffer);
    }

    codeblock(updatebufptr){
      $starSymbol(bufferptr) = $starSymbol(bufferptr)+$val(blockSize)/2;
    }

    codeblock (convert_interleave) {
      /* Convert data in buffer to Output format */
      {
	int i, j;
	for (i=0; i <($val(blockSize)/4); i++) {
	  j = 4*i;
	  $starSymbol(packit).regvaluesh[0] = $starSymbol(buffer)[j];
	  $starSymbol(packit).regvaluesh[1] = $starSymbol(buffer)[j+1];
	  $starSymbol(packit).regvaluesh[2] = $starSymbol(buffer)[j+2];
	  $starSymbol(packit).regvaluesh[3] = $starSymbol(buffer)[j+3];
	  $ref(stereoOut,$val(blockSize)/4-1-i) = $starSymbol(packit).regvaluedbl;
	}
      }
    }

    go {
      addCode(setbufptr);
      addCode(read);
      addCode(updatebufptr);
      addCode(read);
      addCode(convert_interleave);
    }

    wrapup {
      CGCAudioBase::wrapup();
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
