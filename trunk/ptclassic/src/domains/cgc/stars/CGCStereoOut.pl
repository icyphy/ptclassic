defstar {
  name { StereoOut }
  domain { CGC }
  derivedFrom { AudioBase }
  descriptor {
Writes Compact Disc audio format from a file given by "fileName". The file
can be the audio port /dev/audio, if supported by the workstation.  
The data written is linear 16 bit encoded and stereo(2 channel) format.
  }
	htmldoc {
This code is based on the description of the audio driver which can
be obtained by looking at the man page for audio.
  }
  version { $Id$ }
  author { Sunil Bhave }
  copyright {
Copyright (c) 1990-1997 The Regents of the University 
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
    name {aheadLimit}
    type{int}
    default {"-1"}
    desc {If non-negative, the number of samples ahead that can computed.}
  }

  defstate {
    name { homogeneous }
    type { int }
    default { 0 }
    desc { If set to 1, the star receives 2 samples of data per
	     firing }
  }

  constructor {
    encodingType.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
    encodingType.setInitValue("linear16");
    channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
    channels.setInitValue(2);
    portType.setInitValue("line_out");

  }

  codeblock (homogeneousConvert) {
    /* Take data from Input and put it in buffer */
    /* this code writes 2 samples per star firing to the write buffer */
    {
      int j;
      j = 2*$starSymbol(counter);
      $starSymbol(buffer)[j] = (short)($ref(left)*32767.0);
      $starSymbol(buffer)[j+1] = (short)($ref(right)*32767.0);
      $starSymbol(counter)++;
    }
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
    if(homogeneous == 1) {
      left.setSDFParams(1);
      right.setSDFParams(1);
    }
    else {
      left.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
      right.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
    }
  }  

  initCode {
    CGCAudioBase::initCode();

    /* variable for the sync codeblock below */
    if ((int)aheadLimit >= 0 ) {
    	addDeclaration(syncCounter);
	addCode("$starSymbol(count) = 0;");
       }
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


  }

  go {
    if(homogeneous == 1) {
      addCode(homogeneousConvert);
      addCode("if($starSymbol(counter) == ($val(blockSize)/4)) {\n");
      addCode(setbufptr);
      addCode(write);
      addCode("$starSymbol(counter) = 0;\n");
      addCode("}\n");
    }
    else {
      addCode(convert);
      if ((int)aheadLimit >= 0 ) addCode(syncLinear16);
      addCode(setbufptr);
      addCode(write);
    }
  }  

  wrapup {
    CGCAudioBase::wrapup();
  }
  
  exectime {
    return int(blockSize)*28;
  }
}
