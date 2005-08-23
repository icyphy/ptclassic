/* User:      ptuser
   Date:      Fri Jan 24 16:42:25 1997
   Target:    TclTk_Target
   Universe:  dtmf */

/* Define macro for prototyping functions on ANSI & non-ANSI compilers */
#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args) args
#else
#define ARGS(args) ()
#endif
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/audioio.h>

/* Define constants TRUE and FALSE for portability */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Define a complex data type if one has not been defined */
#if !defined(COMPLEX_DATA)
#define COMPLEX_DATA 1
typedef struct complex_data { double real; double imag; } complex;
#endif

extern go ARGS((int argc, char *argv[]));

char *name = "dtmf";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

	float outs_22[3];
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
      int file_23;      
int numOutputs_21;
char* encodingType_33="linear16";
int sampleRate_34;
int channels_35;
char* portType_36="speaker";
double volume_37;
double balance_38;
	    static int
            setOutputs_20(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int i;
		if(argc != numOutputs_21+1) {
                    /* Ignore -- probably premature */
                    return TCL_OK;
		}
		for(i=0; i<numOutputs_21; i++) {
                    if(sscanf(argv[i+1], "%f", &outs_22[i]) != 1) {
                        errorReport("Invalid value");
                        return TCL_ERROR;
                    }
		}
                return TCL_OK;
            }
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
      static void audio_setup_24
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
      /*
	Set the audio device controls like gain and balance.
	These parameters can be set once the audio is already running.

	portType: for input: "line_in", "cd", "microphone"
	          for output: "line_out", "speaker"
	volume: range 0.0 - 1.0 (double)
	balance: range -1.0 to 1.0 (double)
	recordFlag: record = 1, play = 0
	*/
      static void audio_control_25
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
      /*
	Set the audio gain. Faster than audio_control if just setting
	gain.

	volume: range 0.0 - 1.0 (double)
	recordFlag: record = 1, play = 0
	*/
      static void audio_gain_26
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
      /*
	Set the audio balance. Faster than audio_control if just setting
	gain.

	balance: range -1.0 to 1.0 (double)
	*/
      static void audio_balance_27
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_40(dummy, interp, argc, argv)
            ClientData dummy;                   /* Not used. */
            Tcl_Interp *interp;                 /* Current interpreter. */
            int argc;                           /* Number of arguments. */
            char **argv;                        /* Argument strings. */
	    {
	    static char buf[20];
	    int tkvolume;
            if(sscanf(argv[1], "%d", &tkvolume) != 1) {
                errorReport("Invalid volume");
                return TCL_ERROR;
            }
	    /* conveying the value changes of volume to audio device */
	    volume_37 = (float) (tkvolume/100.0);
	    audio_gain_26
	      (file_23, volume_37, 0);

	    sprintf(buf, "%f", volume_37);
	    displaySliderValue(".high", "scale1_39", buf);

            return TCL_OK;
        }
void tkSetup() {
	    if(Tcl_Eval(interp, "global uniqueSymbol; set uniqueSymbol tkScript_19")
		!= TCL_OK)
		errorReport("Error accessing tcl");
	    if(Tcl_Eval(interp, 
                "set tkScript_19(procId) {-1}") != TCL_OK)
		errorReport("Cannot initialize state procId");
	    if(Tcl_Eval(interp, 
                "set tkScript_19(tcl_file) {$PTOLEMY/src/domains/cgc/tcltk/stars/tkKeypad.tcl}") != TCL_OK)
		errorReport("Cannot initialize state tcl_file");
	    if(Tcl_Eval(interp, 
                "set tkScript_19(synchronous) {0}") != TCL_OK)
		errorReport("Cannot initialize state synchronous");
	    if(Tcl_Eval(interp, 
                "set tkScript_19(numOutputs) {0}") != TCL_OK)
		errorReport("Cannot initialize state numOutputs");
	    if(Tcl_Eval(interp, 
                "set tkScript_19(numInputs) {0}") != TCL_OK)
		errorReport("Cannot initialize state numInputs");
	    if(Tcl_Eval(interp, 
                "set tkScript_19(temp) {0}") != TCL_OK)
		errorReport("Cannot initialize state temp");
	    Tcl_CreateCommand(interp, "tkScript_19setOutputs",
		setOutputs_20, (ClientData) 0, NULL);
	    if(Tcl_Eval(interp, "source [expandEnvVars \\$PTOLEMY/src/domains/cgc/tcltk/stars/tkKeypad.tcl]")
		!= TCL_OK)
		errorReport("Cannot source tcl script for TclScript star");
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.5*100);
        makeScale(".high",
		  "scale1_39",
		  "Volume control",
		  tkvolume,
                  setVolume_40);
	displaySliderValue(".high", "scale1_39",
			   "0.5");
      }

}
/* main function */
int go(int argc, char *argv[]) {
	    double t_16;
	    double t_18;
      /* Hack for Sun only */
      unsigned count_29 = 0;
      short buffer_30[250];
      short *bufferptr_31;
      SndSoundStruct header_32;
      int counter_28;
double output_0;
double state_43;
double output_1;
int output_2;
double output_3;
double output_4[250];
int output_17;
double output_5;
double state_42;
double output_6;
int output_7;
double output_8;
double output_9;
double output_10;
double output_1_11;
double output_2_12;
double output_3_13;
encodingType_33="linear16";
sampleRate_34=8000;
channels_35=1;
portType_36="speaker";
volume_37=0.5;
balance_38=0.0;
numOutputs_21=3;
state_42=0.0;
state_43=0.0;
output_0 = 0.0;
output_1 = 0.0;
output_2 = 0;
output_3 = 0.0;
{int i; for(i=0;i<250;i++) output_4[i] = 0.0;}
output_17 = 0;
output_5 = 0.0;
output_6 = 0.0;
output_7 = 0;
output_8 = 0.0;
output_9 = 0.0;
output_10 = 0.0;
output_1_11 = 0.0;
output_2_12 = 0.0;
output_3_13 = 0.0;
	    if(Tcl_Eval(interp, "global uniqueSymbol; set uniqueSymbol tkScript_19")
		!= TCL_OK)
		errorReport("Error accessing tcl");
	    if(Tcl_Eval(interp, "tkScript_19callTcl") != TCL_OK)
		errorReport("Error invoking callTcl");
counter_28 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_23 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_32.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_32.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_32.dataSize = 500;
	  if(strcasecmp("linear16", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_32.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_32.dataFormat = 1;
	  }
	  header_32.samplingRate = 8000;
	  header_32.channelCount = 1;

	  write(file_23,(&header_32), 28);
	}
audio_setup_24(file_23, encodingType_33, sampleRate_34, channels_35);
audio_control_25(file_23, portType_36, volume_37, balance_38, 0);
audio_balance_27(file_23, balance_38);
{
	int sdfIterationCounter_44;
sdfIterationCounter_44 = 0;
while (sdfIterationCounter_44++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_41;for (sdfLoopCounter_41 = 0; sdfLoopCounter_41 < 250; sdfLoopCounter_41++) {
if ( getPollFlag() ) processFlags();
	{  /* star dtmf.TclScript.input=0.output=31 (class CGCTclScript) */
	output_1_11 = outs_22[0];
	output_2_12 = outs_22[1];
	output_3_13 = outs_22[2];
	}
	{  /* star dtmf.Gain2 (class CGCGain) */
output_10 = 0.000785398163397447 * output_2_12;
	}
	{  /* star dtmf.auto-fork-node1 (class CGCFork) */
	}
	{  /* star dtmf.oscillator1.Const1 (class CGCConst) */
	output_2 = 0.0;
	}
	{  /* star dtmf.oscillator2.Const1 (class CGCConst) */
	output_7 = 0.0;
	}
	{  /* star dtmf.Gain1 (class CGCGain) */
output_9 = 0.000785398163397447 * output_3_13;
	}
	{  /* star dtmf.oscillator2.Integrator1 (class CGCIntegrator) */
	    if (output_7 != 0) {
		t_18 = output_9;
	    } else {
		t_18 = output_9 +
			1.0 * state_42;
	    }
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if (t_18 > 3.14159265358979)
		do t_18 -= (3.14159265358979 - -3.14159265358979);
		while (t_18 > 3.14159265358979);
	    /* Take care of the bottom */
	    if (t_18 < -3.14159265358979)
		do t_18 += (3.14159265358979 - -3.14159265358979);
		while (t_18 < -3.14159265358979);
	    output_6 = t_18;
	    state_42 = t_18;
	}
	{  /* star dtmf.oscillator2.Cos1 (class CGCCos) */
	output_8 = cos(output_6);
	}
	{  /* star dtmf.oscillator1.Integrator1 (class CGCIntegrator) */
	    if (output_2 != 0) {
		t_16 = output_10;
	    } else {
		t_16 = output_10 +
			1.0 * state_43;
	    }
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if (t_16 > 3.14159265358979)
		do t_16 -= (3.14159265358979 - -3.14159265358979);
		while (t_16 > 3.14159265358979);
	    /* Take care of the bottom */
	    if (t_16 < -3.14159265358979)
		do t_16 += (3.14159265358979 - -3.14159265358979);
		while (t_16 < -3.14159265358979);
	    output_1 = t_16;
	    state_43 = t_16;
	}
	{  /* star dtmf.oscillator1.Cos1 (class CGCCos) */
	output_3 = cos(output_1);
	}
	{  /* star dtmf.oscillator1.Mpy.input=21 (class CGCMpy) */
	output_0 = output_3 * output_1_11;
	}
	{  /* star dtmf.oscillator2.Mpy.input=21 (class CGCMpy) */
	output_5 = output_8 * output_1_11;
	}
	{  /* star dtmf.Add.input=21 (class CGCAdd) */
	output_4[output_17] = output_0 + output_5;
	output_17 += 1;
	if (output_17 >= 250)
		output_17 -= 250;
	}
}} /* end repeat, depth 1*/
	{  /* star dtmf.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(500/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_30[500/2-1-i] 
	= (short)(output_4[(249-(i))]*32767.0);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_23, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_29 - info.play.samples) > 
		 1000);
        count_29 += 500/2;
      }
    bufferptr_31 = buffer_30;
      /* Write blockSize bytes to file */
      if (write(file_23, bufferptr_31, 500) != 500)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	}
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_23) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
