defstar {
  name { MonoOut }
  domain { CGC }
  derivedFrom { AudioBase }
  descriptor {
Writes mono(1 channel) data with either linear16 or ulaw8 encoding to
a file given by "fileName". The file can be the audio port /dev/audio,
if supported by the workstation.  
  }
	htmldoc {
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
    balance.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
    balance.setInitValue(0.0);
    encodingType.setInitValue("ulaw8");
    portType.setInitValue("speaker");
    sampleRate.setInitValue(8000);

  }
  	
  codeblock (setbufptr) {
    $starSymbol(bufferptr) = $starSymbol(buffer);
  }  

  codeblock (convert) {
    {
      /* select convert procedure depending on encodingType */
      int i;
      if (strcasecmp($ref(encodingType), "linear16") == 0)
	{
	  for (i=0; i <($val(blockSize)/2); i++) {
	    /* Convert from floating point [-1.0,1.0] to 16-bit sample */
	    $starSymbol(buffer)[i] = ceil($ref(input, i)*32768.0);
	  }
	}
      else
	{
	  for(i = 0; i < $val(blockSize); i++) {
	    /* Convert from floating point [-1.0,1.0] to 16-bit sample */
	    int sample16 = (int)($ref(input,i) * 32768.0);
	    /* Convert 16-bit sample to PCM mu-law */
	    $starSymbol(buffer)[$val(blockSize)-1-i] = 
	      Ptdsp_LinearToPCMMuLaw(sample16);
	  }
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
    /* Hack: variable for the sync codeblock below */
    addDeclaration(syncCounter);
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
    /* Setting the audio driver if the output file is /dev/audio */
    if(strcasecmp(fileName, "/dev/audio") == 0)
      {
	/* audio_setup : to set encodingType, sampleRate and channels */
	addCode("$sharedSymbol(CGCAudioBase,audio_setup)($starSymbol(file), $ref(encodingType), $ref(sampleRate), $ref(channels));");
	/* audio_control : to set portType, volume and balance */
	addCode("$sharedSymbol(CGCAudioBase,audio_control)($starSymbol(file), $ref(portType), $ref(volume), $ref(balance), 0);");
	addCode("$sharedSymbol(CGCAudioBase,audio_balance)($starSymbol(file), $ref(balance));");
      }
  }


  go {
    addCode(convert);
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
