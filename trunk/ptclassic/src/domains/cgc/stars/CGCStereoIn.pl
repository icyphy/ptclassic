defstar {
  name { StereoIn }
  domain { CGC }
  derivedFrom { AudioBase }
  descriptor {
Reads Compact Disc audio format from a file given by "fileName". The file
can be the audio port /dev/audio, if supported by the workstation.  
The data read is linear 16 bit encoded and stereo(2 channel) format.
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
  location { CGC main library }
  
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

  codeblock (convert) {
    /* Convert data in buffer to Output format */
    {
      int j;
      j = 2*(($val(blockSize)/4) - $starSymbol(counter));
      $ref(left) = $starSymbol(buffer)[j]/32767.0;
      $ref(right) = $starSymbol(buffer)[j+1]/32767.0;
      $starSymbol(counter)--;
    }
  }
  
  codeblock (setbufptr) {
    $starSymbol(bufferptr) = $starSymbol(buffer);
  }

  setup {
    left.setSDFParams(1);
    right.setSDFParams(1);
  }

  initCode {
    CGCAudioBase::initCode();
    /* Declare "buffer" to be of type short and blockSize/2 bytes */
    addDeclaration(declarations("short", int(blockSize)/2));
    /* Open file for reading data */
    addCode(openFileForReading);
    /* Set the audio driver if file is "/dev/audio" */
    if(strcasecmp(fileName, "/dev/audio") == 0)
      {
	/* audio_setup : to set encodingType, sampleRate and channels */
	StringList setupParameters = "$sharedSymbol(CGCAudioBase,audio_setup)";
	setupParameters  << "($starSymbol(file), "
			 << "\"" << encodingType << "\", "
			 <<  sampleRate << ", " 
			 <<  channels   << ");\n";

	addCode(setupParameters);
	/* audio_control : to set portType, volume and balance */
	StringList controlParameters = "$sharedSymbol(CGCAudioBase,audio_control)";
	controlParameters << "($starSymbol(file), "
			  << "\"" << portType << "\", "
			  <<  volume << ", " 
			  <<  balance << ", "
			  << "1);\n";
	addCode(controlParameters);
      }
    addCode("$starSymbol(counter) = 0;\n");

  }
  go {
    addCode("if ($starSymbol(counter) == 0) {\n");
    addCode(setbufptr);
    addCode(read);
    addCode("$starSymbol(counter) = ($val(blockSize)/4);\n");
    addCode("}\n");
    addCode(convert);
  }

  wrapup {
    CGCAudioBase::wrapup();
  }
  
  exectime {
    return int(blockSize)*28;
  }
}

