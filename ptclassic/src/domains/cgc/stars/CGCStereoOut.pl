defstar {
    name { StereoOut }
    domain { CGC }
    derivedFrom { AudioBase }
    descriptor {
Writes Compact Disc audio format to a file given by "fileName".  The file
can be the audio port /dev/audio, if supported by the workstation.  The
star writes "blockSize" 16-bit samples at each invocation.  The block
size should be a multiple of 4.
    }
    explanation {
This code is based on the description of the audio driver which can be
obtained by looking at the man page of audio.
    }
    version { $Id$ }
    author { Sunil Bhave }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University 
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
      desc { The volume of the audio output. range 0-100 }
      attributes { A_GLOBAL }
    }

    defstate {
      name { balance }
      type { int }
      default { 50 }
      desc { Balance of audio output. range 0-100 }
      attributes { A_GLOBAL }
    }

    defstate {
      name { outputPort }
      type { string }
      default { line_out }
      desc { Audio input port: line_out or speaker. }
      attributes { A_GLOBAL }    
    }

    constructor {
      blockSize = 8180;
    }

    protected {
      int standardOutput:1;
    }

    codeblock (outDeclarations) {
      short $starSymbol(buf)[$val(blockSize)/2];
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
	 int output_val;
	 AUDIO_INITINFO(&info);

	 if ($ref(encodingType) == "linear")
	   encoding_val = 3;	// Linear
	 else
	   encoding_val = 1;	// U_law
				
	 if ($ref(outputPort) == "line_out")
	   output_val = 0x04; 	// Line_Out
	 else
	   output_val = 0x01;	// Speaker

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));	 

	 info.play.encoding = encoding_val;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.play.precision =8;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.play.balance = AUDIO_MID_BALANCE*$ref(balance)/50;
	 info.play.channels = 2;
	 info.play.port = output_val;
	 info.play.sample_rate = $ref(sampleRate);
	 info.play.gain = AUDIO_MAX_GAIN*$ref(volume)/100;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.play.precision =$ref(precision);
	 info.play.sample_rate = $ref(sampleRate);

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
      /* Take data from Input and put it in buffer */
      /* Data in buffer is alternate left and right channels */
      for ($starSymbol(counter)=0; $starSymbol(counter) <($val(blockSize)/2); 
	   $starSymbol(counter) = $starSymbol(counter)+2) {

	$starSymbol(buf)[$starSymbol(counter)] = 
	  ceil($ref(left,$starSymbol(counter)/2)*32768.0);
	$starSymbol(buf)[$starSymbol(counter)+1] = 
	  ceil($ref(right,$starSymbol(counter)/2)*32768.0);

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
      addDeclaration(outDeclarations);
      addGlobal(globals);

      if (standardIO) {
	addCode(noOpen);
      }
      else {
	addInclude("<fcntl.h>");            // Define O_WRONLY
	addCode(openFileForWriting);
      }

      addCode(openCrtlfile);
      addProcedure(set_parametersDef);
      
      /* Update parameters */
      addCode("$starSymbol(set_parameters) ();\n");
    }
    
    go {
      addCode(convert);
      addCode(write);
    }
    
    wrapup {
      CGCAudioBase::wrapup();
      addCode(closeCrtlfile);
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
