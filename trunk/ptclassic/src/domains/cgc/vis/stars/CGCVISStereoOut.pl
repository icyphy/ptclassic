defstar {
    name { VISStereoOut }
    domain { CGC }
    derivedFrom { AudioBase }
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
    author { Sunil Bhave and Bill Chen}
    copyright {
Copyright (c) 1990- The Regents of the University 
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
      name { interleave }
      type { int }
      default { FALSE }
      desc {
TRUE inputs interleaved audio samples through the left input;
FALSE inputs separate audio samples through both the left and right inputs.}
      attributes { A_CONSTANT|A_SETTABLE }
	}

    constructor {
      encodingType.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      encodingType.setInitValue("linear16");
      channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      channels.setInitValue(2);
    }

    setup {
      if (interleave) {
	left.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
      }
      else {
	left.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
	right.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
      }
    }

    codeblock(globalDecl){
      union $sharedSymbol(CGCVISStereoOut,regoverlay) {
	vis_d64 regvaluedbl;
	vis_s16 regvaluesh[4];
      };
    }

    codeblock(mainDecl){
      union $sharedSymbol(CGCVISStereoOut,regoverlay) $starSymbol(unpackright);
      union $sharedSymbol(CGCVISStereoOut,regoverlay) $starSymbol(unpackleft);
    } 
    
    initCode {
      CGCAudioBase::initCode();
      addInclude("<vis_types.h>"); 
      addGlobal(globalDecl,"CGCVISStereoOut_regoverlay");
      addDeclaration(mainDecl);
      if (interleave) {
	addDeclaration(declarations("short", int(blockSize)));
      }
      else{
	/* Declare "buffer" to be of type short and blockSize/2 bytes */
	addDeclaration(declarations("short", int(blockSize*2)));
      }
      /* Open file for writing data */
      addCode(openFileForWriting);	
      /* audio_setup : to set encodingType, sampleRate and channels */
      StringList setupParameters = "$sharedSymbol(CGCAudioBase,audio_setup)";
      setupParameters  << "($starSymbol(file), "
		       << "\"" << encodingType << "\", "
		       <<  sampleRate << ", " 
		       <<  channels << ");\n";
      addCode(setupParameters);
      /* audio_control : to set portType, volume and balance */
      StringList controlParameters = "$sharedSymbol(CGCAudioBase,audio_control)";
      controlParameters << "($starSymbol(file), "
			<< "\"" << portType << "\", "
			<<  volume << ",\" " 
			<<  balance << ",\" "
			<< "0);\n";
      addCode(controlParameters);
    }

    codeblock (setbufptr) {
      $starSymbol(bufferptr) = $starSymbol(buffer);
    }

    codeblock(updatebufptr){
      $starSymbol(bufferptr) = $starSymbol(bufferptr)+$val(blockSize)/2;
    }

    codeblock (convert_separate) {
      /* Convert data in buffer to Output format */
      {
	int i, j;

	for (i=0; i <($val(blockSize)/4); i++) {
	  j = 8*i;
	  $starSymbol(unpackleft).regvaluedbl = $ref(left,$val(blockSize)/4-1-i);
	  $starSymbol(unpackright).regvaluedbl = $ref(right,$val(blockSize)/4-1-i);

	  $starSymbol(buffer)[j]   = $starSymbol(unpackleft).regvaluesh[0];
	  $starSymbol(buffer)[j+1] = $starSymbol(unpackright).regvaluesh[0];
	  $starSymbol(buffer)[j+2] = $starSymbol(unpackleft).regvaluesh[1];
	  $starSymbol(buffer)[j+3] = $starSymbol(unpackright).regvaluesh[1];
	  $starSymbol(buffer)[j+4] = $starSymbol(unpackleft).regvaluesh[2];
	  $starSymbol(buffer)[j+5] = $starSymbol(unpackright).regvaluesh[2];
	  $starSymbol(buffer)[j+6] = $starSymbol(unpackleft).regvaluesh[3];
	  $starSymbol(buffer)[j+7] = $starSymbol(unpackright).regvaluesh[3];
	}
      }
    }

    codeblock (convert_interleave) {
      /* Convert data in buffer to Output format */
      {
	int i, j;
	for (i=0; i <($val(blockSize)/4); i++) {
	  j = 4*i;
	  $starSymbol(unpackleft).regvaluedbl = $ref(left,$val(blockSize)/4-1-i);

	  $starSymbol(buffer)[j]   = $starSymbol(unpackleft).regvaluesh[0];
	  $starSymbol(buffer)[j+1] = $starSymbol(unpackleft).regvaluesh[1];
	  $starSymbol(buffer)[j+2] = $starSymbol(unpackleft).regvaluesh[2];
	  $starSymbol(buffer)[j+3] = $starSymbol(unpackleft).regvaluesh[3];
	}
      }
    }

    go {
      addCode(setbufptr);
      if (interleave) {
	addCode(convert_interleave);
      }
      else {
	addCode(convert_separate);
	addCode(write);
	addCode(updatebufptr);
	addCode(write);
	addCode(updatebufptr);
      }
      addCode(write);
      addCode(updatebufptr);
      addCode(write);
    }
    
    wrapup {
      CGCAudioBase::wrapup();
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
