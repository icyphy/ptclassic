defstar {
  name { StereoOut }
  domain { CGC }
  derivedFrom { AudioBase }
  descriptor {
Writes Compact Disc audio format from a file given by "fileName". The file
can be the audio port /dev/audio, if supported by the workstation.  
The data written is linear 16 bit encoded and stereo(2 channel) format.
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
    portType.setInitValue("line_out");

  }

  codeblock (convert) {
    /* Take data from Input and put it in buffer */
    {
      int j;
      j = 2*$starSymbol(counter);
      $starSymbol(buffer)[j] = (short)($ref(left)*32767.0);
      $starSymbol(buffer)[j+1] = (short)($ref(right)*32767.0);
      $starSymbol(counter)++;
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
    /* Open file for writing data */
    addCode(openFileForWriting);	
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
			  << "0);\n";
	addCode(controlParameters);
      }
    addCode("$starSymbol(counter) = 0;\n");

  }

  go {
    addCode(convert);
    addCode("if($starSymbol(counter) == ($val(blockSize)/4)) {\n");
    addCode(setbufptr);
    addCode(write);
    addCode("$starSymbol(counter) = 0;\n");
    addCode("}\n");
  }

  wrapup {
    CGCAudioBase::wrapup();
  }
  
  exectime {
    return int(blockSize)*28;
  }
}
