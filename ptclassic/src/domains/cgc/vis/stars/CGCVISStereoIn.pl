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
	htmldoc {
This code is based on the description of the audio driver which can
be obtained by looking at the man page for audio.
    }
    version { $Id$ }
    author { Sunil Bhave and Bill Chen}
    copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved. See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
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
	
    constructor {
      encodingType.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      encodingType.setInitValue("linear16");
      channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      channels.setInitValue(2);
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
      int $starSymbol(numread),$starSymbol(numbytes);
      vis_s16 *$starSymbol(dataptr);
    }

    initCode {
      CGCAudioBase::initCode();
      addInclude("<vis_types.h>"); 
      addGlobal(globalDecl,"CGCVISStereoIn_regoverlay");
      addDeclaration(mainDecl);
      /* Declare "buffer" to be of type short and blockSize/2 bytes */
      addDeclaration(declarations("short", int(blockSize)*2));
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
      addCode(setbufptr);
      addCode(read);
      addCode("$starSymbol(numbytes) = 8180;");
      addCode("$starSymbol(numread) = 1;");
    }

    codeblock (fireonce) {
	$starSymbol(packleft).regvaluesh[0] = *$starSymbol(dataptr)++;
	$starSymbol(packright).regvaluesh[0] = *$starSymbol(dataptr)++;
	$starSymbol(packleft).regvaluesh[1] = *$starSymbol(dataptr)++;
	$starSymbol(packright).regvaluesh[1] = *$starSymbol(dataptr)++;
	$starSymbol(packleft).regvaluesh[2] = *$starSymbol(dataptr)++;
	$starSymbol(packright).regvaluesh[2] = *$starSymbol(dataptr)++;
	$starSymbol(packleft).regvaluesh[3] = *$starSymbol(dataptr)++;
	$starSymbol(packright).regvaluesh[3] = *$starSymbol(dataptr)++;
	$ref(left) = $starSymbol(packleft).regvaluedbl;
	$ref(right) = $starSymbol(packright).regvaluedbl;
	$starSymbol(numbytes) -= 16;
      }
	
    codeblock (setbufptr) {
      $starSymbol(bufferptr) = $starSymbol(buffer);
      $starSymbol(dataptr) = $starSymbol(bufferptr);
    }

    codeblock(updatebufptr){
      $starsymbol(bufferptr) = $starSymbol(bufferptr)+$val(blockSize)/2;
    }

    go {
      addCode(fireonce);
      addCode("if ($starSymbol(numbytes) < 16) {");
      addCode("$starSymbol(numbytes) += 8180;");
      addCode("if ($starSymbol(numread) > 3) {");
      addCode(setbufptr);
      addCode("$starSymbol(numread) = 1;");
      addCode("}else {");
      addCode(updatebufptr);
      addCode("$starSymbol(numread)++;}");
      addCode(read);
      addCode("}");
    }

    wrapup {
      CGCAudioBase::wrapup();
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
