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
    /* Data in buffer is alternate left and right channels */
    {
      int i, j;
      for (i = 0; i < ($val(blockSize)/4); i ++) {
	j = i*2;
	$starSymbol(buffer)[j] = 
	  (short)($ref(left,($val(blockSize)/4) -1 - i)*32768.0);
	$starSymbol(buffer)[j+1] = 
	  (short)($ref(right,($ref(blockSize)/4) - 1 - i)*32768.0);
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
    /* Open file for writing data */
    addCode(openFileForWriting);	
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
  
  go {
    addCode(convert);
    addCode(setbufptr);
    addCode(write);
  }

  wrapup {
    CGCAudioBase::wrapup();
  }
  
  exectime {
    return int(blockSize)*28;
  }
}
