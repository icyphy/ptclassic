defstar {
  name { MonoOut }
  domain { CGC }
  derivedFrom { AudioBase }
  descriptor {
Writes mono(1 channel) data with either linear16 or ulaw8 encoding to
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
  
  input {
    name { input }
    type { float }
  }

  defstate {
    name {aheadLimit}
    type{int}
    default {"-1"}
    desc {If non-negative, the number of samples ahead that can computed.}
  }

  constructor {
    channels.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
    channels.setInitValue(1);
  }
  
  codeblock (sync) {
    {
      /* Wait for samples to drain */
      audio_info_t info;
      ioctl($starSymbol(file), AUDIO_DRAIN, &info);
    }
  }

	
  codeblock (setbufptr) {
    $starSymbol(bufferptr) = $starSymbol(buffer);
  }  

  codeblock (convert_linear16) {
    {
      int i;
      for (i=0; i <($val(blockSize)/2); i++) {
	/* Convert from floating point [-1.0,1.0] to 16-bit sample */
	$starSymbol(buffer)[i] = ceil($ref(input, i)*32768.0);
      }	
    }	
  }

  codeblock (convert_ulaw8) {
    /* Convert from floating point [-1.0,1.0] to mu-law PCM. */	
    {
      int i;
      for(i = 0; i < $val(blockSize); i++) {
	/* Convert from floating point [-1.0,1.0] to 16-bit sample */
	int sample16 = (int)($ref(input,i) * 32768.0);
	/* Convert 16-bit sample to PCM mu-law */
	$starSymbol(buffer)[$val(blockSize)-1-i] = 
	  Ptdsp_LinearToPCMMuLaw(sample16);
      }
    }	
  }

  
  setup {
    if (strcasecmp(encodingType, "ulaw8") == 0){
      input.setSDFParams(int(blockSize), int(blockSize)-1);
    }
    else {
      input.setSDFParams(int(blockSize/2), int(blockSize/2)-1);
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
    if (strcasecmp(encodingType, "linear16") == 0)
      addCode(convert_linear16);
    else
      addCode(convert_ulaw8);
    if ((int)aheadLimit >= 0 ) addCode(sync);
    addCode(setbufptr);
    addCode(write);
  }

  wrapup {
    CGCAudioBase::wrapup();
  }
  
  exectime {
    return int(blockSize)*14;
  }
}
