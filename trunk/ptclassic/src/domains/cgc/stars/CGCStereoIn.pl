defstar {
    name { StereoIn }
    domain { CGC }
    derivedFrom { AudioBase }
    descriptor {
Reads Compact Disc audio format from a file given by "fileName". The file
can be the audio port /dev/audio, if supported by the workstation.  The
star reads "blockSize" 16-bit samples at each invocation.  The block
size should be a multiple of 4.
    }
    explanation {
This code is based on the description of the audio driver which can
be obtained by looking at the man page of audio.
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

    defstate {
      name { encodingType }
      type { string }
      default { "linear" }
      desc { Type of encoding : Linear or U_Law PCM. }
      attributes { A_GLOBAL }    
    }

    defstate {
      name { sampleRate }
      type { int }
      default { 44100 }
      desc { Number of samples per second. }
      attributes { A_GLOBAL }
    }

    defstate {
      name { precision }
      type { int }
      default { 16 }
      desc { Bit-width of each sample. }
      attributes { A_GLOBAL }    
    }
    
    defstate {
      name { volume }
      type { int }
      default { 100 }
      desc { The volume of the audio input. }
      attributes { A_GLOBAL }
    }

    defstate {
      name { balance }
      type { int }
      default { 50 }
      desc { Balance of audio input. range 0-100 }
      attributes { A_GLOBAL }
    }

    defstate {
      name { inputPort }
      type { string }
      default { line_in }
      desc { Audio input port: line_in or internal_cd. }
      attributes { A_GLOBAL }    
    }

    constructor {
      blockSize.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
      blockSize.setInitValue(8180);
    }

    codeblock (inDeclarations) {
	int $starSymbol(counter);
    }
    
    codeblock (globals) {
	int $starSymbol(ctlfile);    
    }

    codeblock (set_parametersDef) {
      /* The following function is used to set the */
      /* audioctl device to the proper recording   */
      /* format					   */

      static void $starSymbol(set_parameters)()
      {
	 audio_info_t info;
	 int encoding_val;
	 int input_val;
	 AUDIO_INITINFO(&info);

	 if ($ref(encodingType) == "linear")
	   encoding_val = 3;	// Linear
	 else
	   encoding_val = 1;	// U_law
				
	 if ($ref(inputPort) == "line_in")
	   input_val = 0x02; 	// Line_In
	 else
	   input_val = 0x04;	// Internal_CD_In

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));	 

	 info.record.encoding = encoding_val;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.record.precision = 8;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.record.channels = 2;
	 info.record.port = input_val;
	 info.record.sample_rate = $ref(sampleRate);
	 info.record.gain = AUDIO_MAX_GAIN*$ref(volume)/100;
	 info.record.balance = AUDIO_MID_BALANCE*$ref(balance)/50;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.record.precision = $ref(precision);
	 info.record.sample_rate = $ref(sampleRate);

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));
      
      }
    }

    codeblock (openCrtlfile) {
      /* Open control device */
      $starSymbol(ctlfile) = open("/dev/audioctl",O_RDWR, 0666);
      if ($starSymbol(ctlfile) == -1) {
	perror("Error in opening audio control device: /dev/audioctl");
	exit(1);
      }
    }
    
    codeblock (convert) {
      /* Convert data in buffer to Output format */

      for ($starSymbol(counter)=0; $starSymbol(counter) <($val(blockSize)/4);
	   $starSymbol(counter)++) {

	$ref(left,$starSymbol(counter)) = 
	  $starSymbol(buf)[2*$starSymbol(counter)] /32768.0;
	$ref(right,$starSymbol(counter)) = 
	  $starSymbol(buf)[2*$starSymbol(counter)+1] /32768.0;
	
      }
    }
    
    codeblock (closeCrtlfile) {
      /* Close control device */
      if (close($starSymbol(ctlfile)) != 0) {
	perror("Error closing audio control device: /dev/audioctl");
	exit(1);
      }
    }

    setup {
      CGCAudioBase::setup();
      left.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
      right.setSDFParams(int(blockSize/4), int(blockSize/4)-1);
    }

    initCode {
      addInclude("<stdio.h>");
      addInclude("<unistd.h>");
      addInclude("<sys/ioctl.h>");
      addInclude("<math.h>");
      addInclude("<sys/audioio.h>");
      CGCAudioBase::initCode();
      addDeclaration(declarations("short", int(blockSize)/2));
      addDeclaration(inDeclarations);
      addGlobal(globals);
      if (standardIO) {
	addCode(noOpen);
      }
      else {
	addInclude("<fcntl.h>");            // Define O_RDONLY
	addCode(openFileForReading);
      }
      addCode(openCrtlfile);
      addProcedure(set_parametersDef);
      
      /* Update parameters */
      addCode("$starSymbol(set_parameters) ();\n");
    }
    
    go {
      addCode(read);
      addCode(convert);
    }

    wrapup {
      CGCAudioBase::wrapup();
      addCode(closeCrtlfile);
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
