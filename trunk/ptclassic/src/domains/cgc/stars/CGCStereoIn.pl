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
      int i, j;
      for (i=0; i <($val(blockSize)/4); i++) {
	j = 2*i;
	$ref(left,($val(blockSize)/4) - 1 - i) = 
	  $starSymbol(buffer)[j] /32768.0;
	$ref(right,($val(blockSize)/4) - 1 - i) = 
	  $starSymbol(buffer)[j+1] /32768.0;
      }
    }
  }
  
  codeblock (setbufptr) {
    $starSymbol(bufferptr) = $starSymbol(buffer);
  }

  setup {
    left.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
    right.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
  }

  initCode {
    CGCAudioBase::initCode();
    /* Declare "buffer" to be of type short and blockSize/2 bytes */
    addDeclaration(declarations("short", int(blockSize)/2));
    /* Open file for reading data */
    addCode(openFileForReading);	
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
  
  go {
    addCode(setbufptr);
    addCode(read);
    addCode(convert);
  }

  wrapup {
    CGCAudioBase::wrapup();
  }
  
  exectime {
    return int(blockSize)*28;
  }
}

