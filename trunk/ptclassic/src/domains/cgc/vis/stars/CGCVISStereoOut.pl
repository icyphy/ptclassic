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
	htmldoc {
This code is based on the description of the audio driver which can be
obtained by looking at the man page of audio.
    }
    version { $Id$ }
    author { Sunil Bhave and Bill Chen}
    copyright {
Copyright (c) 1996- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
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

    constructor {
      encodingType.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      encodingType.setInitValue("linear16");
      channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      channels.setInitValue(2);
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
      int $starSymbol(numwrites),$starSymbol(numbytes);
      vis_s16 *$starSymbol(dataptr);
    }

    codeblock (setbufptr) {
      $starSymbol(bufferptr) = $starSymbol(buffer);
      $starSymbol(dataptr) = $starSymbol(bufferptr);
    }
    
    initCode {
      CGCAudioBase::initCode();
      addInclude("<vis_types.h>"); 
      addGlobal(globalDecl,"CGCVISStereoOut_regoverlay");
      addDeclaration(mainDecl);
      /* Declare "buffer" to be of type short and blockSize/2 bytes */
      addDeclaration(declarations("short", int(blockSize)*2));
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
			<<  volume << "," 
			<<  balance << ", "
			<< "0);\n";
      addCode(controlParameters);
      addCode("$starSymbol(numbytes) = 0;"); 
      addCode("$starSymbol(numwrites) = 1;"); 
      addCode(setbufptr);
    }

    codeblock (convert_separate) {
      /* Convert data in buffer to Output format */
      {
	$starSymbol(unpackleft).regvaluedbl = $ref(left);
	$starSymbol(unpackright).regvaluedbl = $ref(right);
	
	*$starSymbol(dataptr)++ = $starSymbol(unpackleft).regvaluesh[0];
	*$starSymbol(dataptr)++ = $starSymbol(unpackright).regvaluesh[0];
	*$starSymbol(dataptr)++ = $starSymbol(unpackleft).regvaluesh[1];
	*$starSymbol(dataptr)++ = $starSymbol(unpackright).regvaluesh[1];
	*$starSymbol(dataptr)++ = $starSymbol(unpackleft).regvaluesh[2];
	*$starSymbol(dataptr)++ = $starSymbol(unpackright).regvaluesh[2];
	*$starSymbol(dataptr)++ = $starSymbol(unpackleft).regvaluesh[3];
	*$starSymbol(dataptr)++ = $starSymbol(unpackright).regvaluesh[3];
	$starSymbol(numbytes) += 16;
      }
    }

    codeblock(updatebufptr){
      $starsymbol(bufferptr) = $starSymbol(bufferptr)+$val(blockSize)/2;
    }

    go {
      addCode(convert_separate);
      addCode("if ($starSymbol(numbytes) >= 8180){");
      addCode(write);
      addCode("$starSymbol(numbytes) -= 8180;");
      addCode("if ($starSymbol(numwrites) > 3) {");
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
