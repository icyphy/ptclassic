defstar
{
    name { StereoIn }
    domain { CGC }
    descriptor {
Reads Compact Disc audio format from a file. The file
can be the audio port /dev/audio, if supported by the
workstation.
    }
    explanation {
This code is based on the description of the audio
driver which can be obtained by looking at the man page
of audio.
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

    defstate {
      name { fileName }
      type { string }
      default { "/dev/audio" }
      desc { Input file for CD quality data. }
    }

    defstate {
      name { blockSize }
      type { int }
      default { 8180 }
      desc { Number of samples to read. }
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

    protected {
      int standardInput:1;
    }

    output {
      name { output }
      type { float }
    }

    codeblock (declarations) {
      int $starSymbol(file);
      int $starSymbol(buffer)[$val(blockSize)*2];
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
	 
	 info.record.encoding = encoding_val;
//	 info.play.encoding = encoding_val;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.record.precision = 8;
//	 info.play.precision =8;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.record.channels = 2;
	 info.record.port = input_val;
	 info.record.sample_rate = $ref(sampleRate);
	 info.record.gain = AUDIO_MAX_GAIN*$ref(volume)/100;
	 info.record.balance = AUDIO_MID_BALANCE*$ref(balance)/50;

//	 info.play.balance = AUDIO_MID_BALANCE;
//	 info.play.channels = 2;
//	 info.play.port = AUDIO_LINE_OUT;
//	 info.play.sample_rate = $ref(sampleRate);
//	 info.play.gain = AUDIO_MAX_GAIN*$ref(volume)*0.85/100;

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));

	 info.record.precision = $ref(precision);
//	 info.play.precision =$ref(precision);
	 info.record.sample_rate = $ref(sampleRate);
//	 info.play.sample_rate = $ref(sampleRate);

	 ioctl($starSymbol(ctlfile), AUDIO_SETINFO, (caddr_t)(&info));
      
      }
    }

    codeblock (openFile) {
      /* Open file for reading */
      $starSymbol(file) = open("$val(fileName)",O_RDONLY, 0666);
      if ($starSymbol(file) == -1) {
	perror("Error: cannot open input file: $val(fileName)");
	exit(1);	
      }
    }
    
    codeblock (noOpen) {
      /* Use standard input for reading */
      $starSymbol(file) = 0;
    }
    
    codeblock (openCrtlfile) {
      /* Open control device */
      $starSymbol(ctlfile) = open("/dev/audioctl",O_RDWR, 0666);
      if ($starSymbol(ctlfile) == -1) {
	perror("Error in opening audio control device: /dev/audioctl");
	exit(1);
      }
    }
    
    codeblock (read) {
      /* Read data from a file */
      read($starSymbol(file), $starSymbol(buffer), $val(blockSize));
    }
    
    codeblock (convert) {
      /* Convert data in buffer to Output format */
      /* $ref(output) = $starSymbol(buffer);     */
      for ($starSymbol(counter)=0; $starSymbol(counter) < ($val(blockSize)*2); $starSymbol(counter)++) {
	$ref(output,$starSymbol(counter)) = $starSymbol(buffer)[$starSymbol(counter)]/32768.0;
      }

    }
    
    codeblock (closeFile) {
      /* Close file */
      if (close($starSymbol(file)) != 0)
        {
	  perror("Error in closing: $val(fileName)");
	  exit(1);
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
      fileName.clearAttributes(A_SETTABLE);
      standardInput = (strcmp(fileName,"") == 0);
      output.setSDFParams(2*int(blockSize), 2*int(blockSize)-1);
    }
      
    initCode {
      addInclude("<stdio.h>");
      addInclude("<unistd.h>");
      addInclude("<sys/ioctl.h>");
      addInclude("<math.h>");
      addInclude("<sys/audioio.h>");
      addDeclaration(declarations);
      addGlobal(globals);
      
      if (standardInput) addCode(noOpen);
      else
	{
	  addInclude("<fcntl.h>");
	  addCode(openFile);
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
      if (!standardInput) addCode(closeFile);
      addCode(closeCrtlfile);
    }
    
    exectime {
      return int(blockSize)*28;
    }
}
