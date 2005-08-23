defstar {
  name { MonoIn }
  domain { CGC }
  derivedFrom { AudioBase }
  descriptor {
Reads mono(1 channel) data with either linear16 or ulaw8 encoding from
a file given by "fileName". The file can be the audio port /dev/audio,
if supported by the workstation.  
  }
  explanation {
This code is based on the description of the audio driver which can
be obtained by looking at the man page for audio.
The ulaw algorithm is based on the description of the T1 system found
in: Simon Haykin, "Communication Systems," section 8.2 (Wiley 1983).
  }
  version { $Id$ }
  author { T. M. Parks and Sunil Bhave }
  copyright {
Copyright (c) 1990-1996 The Regents of the University 
of California. All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty 
provisions.
  }
  location { CGC main library }
  
  output {
    name { output }
    type { float }
  }

  constructor {
    channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
    channels.setInitValue(1);
  }

  codeblock (convert_linear16) {
    {
      int i;
      for (i=0; i <($val(blockSize)/2); i++) {
	/* Convert the 16-bit sample to a floating point [-1.0,1.0] */
	$ref(output,i) = $starSymbol(buffer)[i] /32768.0;
      }
    }
  }	

  codeblock (convert_ulaw8) {
    {
      /* Convert from mu-law PCM to floating point [-1.0,1.0] */
      int i;
      for (i = 0; i < $val(blockSize); i++) {
	/* Read mu-law PCM sample from buffer */
	int ulawbyte = $starSymbol(buffer)[$val(blockSize)-1-i];
	/* Convert mu-law PCM sample to a 16-bit sample */
	int sample16 = Ptdsp_PCMMuLawToLinear(ulawbyte);
	/* Convert the 16-bit sample to a floating point [-1.0,1.0] */
	$ref(output,i) = ((double)sample16) / 32768.0;
      }
    }	
  }	

  codeblock (setbufptr) {
    $starSymbol(bufferptr) = $starSymbol(buffer);
  }
  
  setup {
    if (strcasecmp(encodingType, "ulaw8") == 0){
      output.setSDFParams(int(blockSize), int(blockSize)-1);
    }
    else {
      output.setSDFParams(int(blockSize/2), int(blockSize/2)-1);
    }
  }
  
  initCode {
    CGCAudioBase::initCode();
    /* Declare buffer type and size depending on the encoding */
    if (strcasecmp(encodingType, "linear16") == 0){
      addDeclaration(declarations("short", int(blockSize)/2));
    }
    else {
      addDeclaration(declarations("unsigned char", int(blockSize)));
      addModuleFromLibrary("ptdspMuLaw", "src/utils/libptdsp", "ptdsp");
    }
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
    if (strcasecmp(encodingType, "linear16") == 0)  
      addCode(convert_linear16);
    else
      addCode(convert_ulaw8);
  }

  wrapup {
    CGCAudioBase::wrapup();
  }
  
  exectime {
    return int(blockSize)*14;
  }
}
