defstar {
    name { VISStereoIn }
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
      name { left }
      type { float }
      desc { Left channel output }
    }

    output {
      name { right }
      type { float }
      desc { Right channel output }
    }
	
    defstate {
      name { interleave }
      type { int }
      default { FALSE }
      desc {
TRUE outputs interleaved audio samples through the left output;
FALSE outputs separate audio samples through both the left and right outputs.}
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
      union $sharedSymbol(CGCVISStereoIn,regoverlay) {
	vis_d64 regvaluedbl;
	vis_s16 regvaluesh[4];
      };
    }

    codeblock(mainDecl){
      union $sharedSymbol(CGCVISStereoIn,regoverlay) $starSymbol(packright);
      union $sharedSymbol(CGCVISStereoIn,regoverlay) $starSymbol(packleft);
    }

    initCode {
      CGCAudioBase::initCode();
      addInclude("<vis_types.h>"); 
      addGlobal(globalDecl,"CGCVISStereoIn_regoverlay");
      addDeclaration(mainDecl);
      if (interleave) {
	addDeclaration(declarations("short", int(blockSize)));
      }
      else{
	/* Declare "buffer" to be of type short and blockSize/2 bytes */
	addDeclaration(declarations("short", int(blockSize*2)));
      }
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
      controlParameters     << "($starSymbol(file),"
		       << "\"" << encodingType << "\","
		       <<  volume << "," 
		       <<  balance << ",1);\n";
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
	  $starSymbol(packleft).regvaluesh[0] = $starSymbol(buffer)[j];
	  $starSymbol(packright).regvaluesh[0] = $starSymbol(buffer)[j+1];
	  $starSymbol(packleft).regvaluesh[1] = $starSymbol(buffer)[j+2];
	  $starSymbol(packright).regvaluesh[1] = $starSymbol(buffer)[j+3];
	  $starSymbol(packleft).regvaluesh[2] = $starSymbol(buffer)[j+4];
	  $starSymbol(packright).regvaluesh[2] = $starSymbol(buffer)[j+5];
	  $starSymbol(packleft).regvaluesh[3] = $starSymbol(buffer)[j+6];
	  $starSymbol(packright).regvaluesh[3] = $starSymbol(buffer)[j+7];
	  $ref(left,$val(blockSize)/4-1-i) = $starSymbol(packleft).regvaluedbl;
	  $ref(right,$val(blockSize)/4-1-i) = $starSymbol(packright).regvaluedbl;
	}
      }
    }

    codeblock (convert_interleave) {
      /* Convert data in buffer to Output format */
      {
	int i, j;
	for (i=0; i <($val(blockSize)/4); i++) {
	  j = 4*i;
	  $starSymbol(packleft).regvaluesh[0] = $starSymbol(buffer)[j];
	  $starSymbol(packleft).regvaluesh[1] = $starSymbol(buffer)[j+1];
	  $starSymbol(packleft).regvaluesh[2] = $starSymbol(buffer)[j+2];
	  $starSymbol(packleft).regvaluesh[3] = $starSymbol(buffer)[j+3];
	  $ref(left,$val(blockSize)/4-1-i) = $starSymbol(packleft).regvaluedbl;
	}
      }
    }

    go {
      addCode(setbufptr);
      addCode(read);
      addCode(updatebufptr);
      addCode(read);

      if (interleave) {
	addCode(convert_interleave);
      }
      else {
	addCode(updatebufptr);
	addCode(read);
	addCode(updatebufptr);
	addCode(read);
	addCode(convert_separate);
      }
    }

    wrapup {
      CGCAudioBase::wrapup();
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
