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
	htmldoc {
This code is based on the description of the audio driver which can
be obtained by looking at the man page for audio.
    }
    version { $Id$ }
    author { Sunil Bhave and Bill Chen}
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved. See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
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
      int $starSymbol(numread),$starSymbol(numbytes);
      vis_s16 *$starSymbol(dataptr);
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
      addCode(setbufptr);
      addCode(read);
      addCode("$starSymbol(numbytes) = 8180;");
      addCode("$starSymbol(numread) = 1;");
    }

    codeblock (setbufptr) {
      $starSymbol(bufferptr) = $starSymbol(buffer);
      $starSymbol(dataptr) = $starSymbol(bufferptr);
    }

    codeblock(updatebufptr){
      $starSymbol(bufferptr) = $starSymbol(bufferptr)+$val(blockSize)/2;
    }

    codeblock (convert_interleave) {
      /* Convert data in buffer to Output format */
      {
	$starSymbol(packit).regvaluesh[0] = *$starSymbol(dataptr)++;
	$starSymbol(packit).regvaluesh[1] = *$starSymbol(dataptr)++;
	$starSymbol(packit).regvaluesh[2] = *$starSymbol(dataptr)++;
	$starSymbol(packit).regvaluesh[3] = *$starSymbol(dataptr)++;
	$ref(stereoOut) = $starSymbol(packit).regvaluedbl;
	$starSymbol(numbytes) -= 8;
      }
    }

    go {
      addCode(convert_interleave);
      addCode("if ($starSymbol(numbytes) < 8) {");
      addCode("$starSymbol(numbytes) += 8180;");
      addCode("if ($starSymbol(numread) > 1) {");
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
