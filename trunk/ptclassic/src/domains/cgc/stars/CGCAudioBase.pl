defstar {
    name { AudioBase }
    domain { CGC }
    desc {
Base star for reading and writing audio data.
    }
    version { $Id$ }
    author { Sunil Bhave }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    defstate {
      name { fileName }
      type { string }
      default { "/dev/audio" }
      desc { File for reading/writing data. If the file is 
	     something other than /dev/audio, the file should 
	     be in Sun audio format. If the file is in another
             format, use conversion program available on web eg. 
             SOX - Sound Exchange http://www.spies.com/sox }
    }	

    defstate {
      name { blockSize }
      type { int }
      default { 8180 }
      desc { Number of samples to read or write. }
    }

    defstate {
      name { encodingType }
      type { string }
      default { "linear16" }
      desc { Encoding type of data being read from port. "linear16"
	       or " ulaw8 "}
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
      name { volume }
      type { float }
      default { 0.8 }
      desc { The volume of the audio. range 0 to 10 }
      attributes { A_GLOBAL }
    }

    defstate {
       name { portType }
       type { string }
       default { "line_in" }
       desc { Audio i/o port: line_in, line_out, cd, microphone, speaker. }
       attributes { A_GLOBAL }
    }
    
    defstate {
      name { balance }
      type { float }
      default { 0.0 }
      desc { Balance of audio. range -10 to 10 }
      attributes { A_GLOBAL }
    }	    

    defstate {
      name { channels }
      type { int }
      default { 2 }
      desc { number of interleaved channels: mono = 1, stereo = 2 }
      attributes { A_GLOBAL }
    }	  

    protected {
	int standardIO:1;
    }

    codeblock(globalDec) {
      /* Struct that contains the header information */
      /* for Sun audio files */
      typedef struct sound_struct {
	int magic;               /* magic number SND_MAGIC */
	int dataLocation;        /* offset or pointer to the data */
	int dataSize;            /* number of bytes of data */
	int dataFormat;          /* the data format code */
	int samplingRate;        /* the sampling rate */
	int channelCount;        /* the number of channels */
	char info[4];            /* optional text information */
      } SndSoundStruct;
    }
    
    codeblock(globals) {
      int $starSymbol(file);      
    }

    codeblock(declarations, "const char* datatype, int size") {
      @datatype $starSymbol(buffer)[@size];
      @datatype *$starSymbol(bufferptr);
      SndSoundStruct $starSymbol(header);
      int $starSymbol(counter);
    }

    codeblock(openFileForReading) {
      /* Open file for reading */
      /* If the file is something other than /dev/audio, open the  */
      /* file and strip of the header */
      if (($starSymbol(file) = open("$val(fileName)",O_RDONLY,0666)) == -1)
	{
	  perror("$val(fileName): Error opening read-file");
	  exit(1);
	}	
      /* To remove the header from the audio file */
      if ((strcasecmp("$val(fileName)", "/dev/audio")) != 0)
	{
	  read($starSymbol(file), (&$starSymbol(header)), 28);
	  /* check whether file is in Sun audio format */
	  if($starSymbol(header).magic != 0x2e736e64)
	    {
	      perror("$val(fileName): File not in Sun audio format. Please refer to the star Profile.");
	      exit(1);
	    }
	  /* set the corresponding defstates using info in the header */
	  /* this can be useful if the user is not sure of the */
	  /* data encoding in the file */
	  if($starSymbol(header).dataFormat == 3)
	    $ref(encodingType) = "linear16";
	  else
	    $ref(encodingType) = "ulaw8";
	  $ref(sampleRate) = $starSymbol(header).samplingRate;
	  $ref(channels) = $starSymbol(header).channelCount;
	}
    }
    codeblock(openFileForWriting) {
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if (($starSymbol(file) = open("$val(fileName)",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("$val(fileName): Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("$val(fileName)", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  $starSymbol(header).magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  $starSymbol(header).dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  $starSymbol(header).dataSize = $val(blockSize);
	  if(strcasecmp("$val(encodingType)", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    $starSymbol(header).dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    $starSymbol(header).dataFormat = 1;
	  }
	  $starSymbol(header).samplingRate = $val(sampleRate);
	  $starSymbol(header).channelCount = $val(channels);

	  write($starSymbol(file),(&$starSymbol(header)), 28);
	}
    }	

    codeblock(read) {
      /* Read blockSize bytes of data from the file. If it returns
	 zero, assume end of file and break - this will break out
 	 of the execution loop in with the TclTkTarget and the TychoTarget
	*/
      if ( read($starSymbol(file), $starSymbol(bufferptr),
		$val(blockSize)) <= 0 ) {
	    break;
	}
    }

    codeblock(write) {
      /* Write blockSize bytes to file */
      if (write($starSymbol(file), $starSymbol(bufferptr), $val(blockSize)) != $val(blockSize))
	{
	  perror("$val(fileName): Error writing to file.");
	  exit(1);
	}
    }

    codeblock(closeFile) {
      /* Close file */
      if (close($starSymbol(file)) != 0) {
	perror("$val(fileName): Error closing file");
	exit(1);
      }
    }

    codeblock (audio_setupDef) {
      /*
	Set the encoding parameters of the audio device.

	encodingType and precision:linear16 or ulaw8
	sampleRate: samples per second
	channels: stereo = 2, mono = 1

	NOTE: ulaw8 will force the channels parameter to 1.

	NOTE: Both input and output must have the same values of
	these parameters. If there are two stars that call this
	routine with different parameters, then the parameters
	corresponding to the most recent call will be used!
	*/
      static void $sharedSymbol(CGCAudioBase,audio_setup)
	(int fd, char* encodingType, int sampleRate, int channels) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);

	/* Set the type of encoding and precision for record and play */
	if (strcasecmp(encodingType, "linear16") == 0) {
	  info.record.encoding = AUDIO_ENCODING_LINEAR;
	  info.record.precision = 16;
	  info.play.encoding = AUDIO_ENCODING_LINEAR;
	  info.record.precision = 16;

	} else if (strcasecmp(encodingType, "ulaw8") == 0) {
	  info.record.encoding = AUDIO_ENCODING_ULAW;
	  info.record.precision = 8;
	  info.play.encoding = AUDIO_ENCODING_ULAW;
	  info.record.precision = 8;

	  /* Force channels to 1 */
	  channels = 1;

	} else {
	  perror("Audio encoding parameter must be \"linear16\" or \"ulaw8\"");
	  exit(1);
	}

	/* Set the number of channels and sample rate */
	info.record.channels = channels;
	info.record.sample_rate = sampleRate;
	info.play.channels = channels;
	info.play.sample_rate = sampleRate;

	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));

      }
    }

    codeblock (audio_controlDef) {
      /*
	Set the audio device controls like gain and balance.
	These parameters can be set once the audio is already running.

	portType: for input: "line_in", "cd", "microphone"
	          for output: "line_out", "speaker"
	volume: range 0.0 - 1.0 (double)
	balance: range -1.0 to 1.0 (double)
	recordFlag: record = 1, play = 0
	*/
      static void $sharedSymbol(CGCAudioBase,audio_control)
	(int fd, char* portType, double volume, double balance, 
	 int recordFlag) {
	audio_info_t info;
	audio_prinfo_t *audioStatusPtr;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);

	/* Pointer to audio status data structure to be modified */
	audioStatusPtr = recordFlag ? (&info.record) : (&info.play);

	/* Set the port */
	/* FIXME: Should print a non-modal error message if bad value */
	if (recordFlag) {
	  if (strcasecmp(portType, "line_in") == 0)
	    audioStatusPtr->port = AUDIO_LINE_IN;
	  else if (strcasecmp(portType, "cd") == 0)
	    audioStatusPtr->port = AUDIO_INTERNAL_CD_IN;
	  else if (strcasecmp(portType, "microphone") == 0)
	    audioStatusPtr->port = AUDIO_MICROPHONE;
	}
	else {
	  if (strcasecmp(portType, "line_out") == 0)
	    audioStatusPtr->port = AUDIO_LINE_OUT;
	  else
	    audioStatusPtr->port = AUDIO_SPEAKER;
	}

	audioStatusPtr->gain = (int)(AUDIO_MAX_GAIN * volume);
	audioStatusPtr->balance = (int)(AUDIO_MID_BALANCE * (balance +
							    1.0));

	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
    }

    codeblock (audio_gainDef) {
      /*
	Set the audio gain. Faster than audio_control if just setting
	gain.

	volume: range 0.0 - 1.0 (double)
	recordFlag: record = 1, play = 0
	*/
      static void $sharedSymbol(CGCAudioBase,audio_gain)
	(int fd, double volume, int recordFlag) {
	audio_info_t info;
	audio_prinfo_t *audioStatusPtr;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Pointer to audio status data structure to be modified */
	audioStatusPtr = recordFlag ? (&info.record) : (&info.play);
	/* Set gain value */
	audioStatusPtr->gain = (int)(AUDIO_MAX_GAIN * volume);
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
    }

    codeblock (audio_balanceDef) {
      /*
	Set the audio balance. Faster than audio_control if just setting
	gain.

	balance: range -1.0 to 1.0 (double)
	*/
      static void $sharedSymbol(CGCAudioBase,audio_balance)
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
    }

    codeblock (syncCounter) {
      /* Hack for Sun only */
      unsigned $starSymbol(count);
    }

    codeblock (syncUlaw) {
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl($starSymbol(file), AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) ($starSymbol(count) - info.play.samples) > 
		 $val(aheadLimit));
        $starSymbol(count) += $val(blockSize);
      }
    }

    codeblock (syncLinear16) {
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl($starSymbol(file), AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) ($starSymbol(count) - info.play.samples) > 
		 $val(aheadLimit));
        $starSymbol(count) += $val(blockSize)/2;
      }
    }


    initCode {
      addInclude("<stdio.h>");
      /* Define strcmp function */
      addInclude("<string.h>");
      /* Define read and write functions */
      addInclude("<unistd.h>");
      /* Define the open function and O_RDWR constant */
      addInclude("<fcntl.h>");
      addInclude("<sys/ioctl.h>");
      /* Define the ceil function */
      addInclude("<math.h>");
      /* Define audio driver : HACK: This is Sun Specific */
      addInclude("<sys/audioio.h>");
      /* Define the SunSound Struct for audio file header*/
      addGlobal(globalDec, "globals");
      addGlobal(globals);
      addProcedure(audio_setupDef,   "CGCAudioBase_audio_setup");
      addProcedure(audio_controlDef, "CGCAudioBase_audio_control");
      addProcedure(audio_gainDef,    "CGCAudioBase_audio_gain");
      addProcedure(audio_balanceDef, "CGCAudioBase_audio_balance");
      addCode("$starSymbol(counter) = 0;\n");
    }

    wrapup {
      addCode(closeFile);
    }
}
    
