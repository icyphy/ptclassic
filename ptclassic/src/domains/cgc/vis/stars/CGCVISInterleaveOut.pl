defstar {
    name { VISInterleaveOut }
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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved. See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC vis library }

    input {
      name { stereoIn }
      type { float }
      desc { Left and right stereo channel input }
    }

    constructor {
      encodingType.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      encodingType.setInitValue("linear16");
      channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      channels.setInitValue(2);
    }

    setup {
      stereoIn.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
    }

    codeblock(globalDecl){
      union $sharedSymbol(CGCVISInterleaveOut,regoverlay) {
	vis_d64 regvaluedbl;
	vis_s16 regvaluesh[4];
      };
    }

    codeblock(mainDecl){
      union $sharedSymbol(CGCVISInterleaveOut,regoverlay) $starSymbol(unpackit);
      int $starSymbol(numwrites),$starSymbol(numbytes);
      vis_s16 *$starSymbol(dataptr);
    }

    initCode {
      CGCAudioBase::initCode();
      addInclude("<vis_types.h>"); 
      addGlobal(globalDecl,"CGCVISInterleaveOut_regoverlay");
      addDeclaration(mainDecl);
      addDeclaration(declarations("short", int(blockSize)));
      /* Open file for reading data */
      addCode(openFileForWriting);	
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
			<< "0);\n";
      addCode(controlParameters);
      addCode("$starSymbol(numbytes) = 0;"); 
      addCode("$starSymbol(numwrites) = 1;"); 
      addCode(setbufptr);
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
	$starSymbol(unpackit).regvaluedbl = $ref(stereoIn);

	*$starSymbol(dataptr)++ = $starSymbol(unpackit).regvaluesh[0];
	*$starSymbol(dataptr)++ = $starSymbol(unpackit).regvaluesh[1];
	*$starSymbol(dataptr)++ = $starSymbol(unpackit).regvaluesh[2];
	*$starSymbol(dataptr)++ = $starSymbol(unpackit).regvaluesh[3];
 	$starSymbol(numbytes) += 8;
     }
    }

    go {
      addCode(convert_interleave);
      addCode("if ($starSymbol(numbytes) >= 8180){");
      addCode(write);
      addCode("$starSymbol(numbytes) -= 8180;");
      addCode("if ($starSymbol(numwrites) > 1) {");
      addCode(setbufptr);
      addCode("$starSymbol(numwrites) = 1;");
      addCode("}else {");
      addCode(updatebufptr);
      addCode("$starSymbol(numwrites)++;}}");   
    }

    wrapup {
      CGCAudioBase::wrapup();
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
