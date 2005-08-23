/* User:      ptuser
   Date:      Fri Jan 24 16:36:28 1997
   Target:    TclTk_Target
   Universe:  diff_sine */

/* Define macro for prototyping functions on ANSI & non-ANSI compilers */
#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args) args
#else
#define ARGS(args) ()
#endif
#endif

#include <math.h>
#include "ptkPlot.h"
#include "ptkPlot_defs.h"
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

char *name = "diff_sine";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_22;
double xMax_23;
ptkPlotWin plotwin_24;
ptkPlotDataset plotdataset_25;
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
      int file_30;      
double value_16;
char* encodingType_40="linear16";
int sampleRate_41;
int channels_42;
char* portType_43="line_out";
double volume_44;
double balance_45;
	    static int
	    setValue_15(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int position;
		static char buf[20];
                if(sscanf(argv[1], "%d", &position) != 1) {
                    errorReport("Invalid value");
                    return TCL_ERROR;
                }
		value_16 = 0.0 + (2000.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_16);
		displaySliderValue(".low", "scale_14", buf);
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
      static void audio_setup_31
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
      static void audio_control_32
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
      static void audio_gain_33
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
      static void audio_balance_34
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_47(dummy, interp, argc, argv)
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
	    volume_44 = (float) (tkvolume/100.0);
	    audio_gain_33
	      (file_30, volume_44, 0);

	    sprintf(buf, "%f", volume_44);
	    displaySliderValue(".high", "scale1_46", buf);

            return TCL_OK;
        }
void tkSetup() {
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*(440.0
			- 0.0)/(2000.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_14",	/* name of the scale pane */
		      "Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_15);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_14", "440.0");
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_24);
	  xMin_22 = 0.0;
	  xMax_23 = 99.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_24,
			   &w,
			   ".win_26",
			   "Input and Output",
			   "600x400+0+0",
			   "n",
			   "y",
			   0.0,99.0, -1.5,1.5,
			   2,
			   100,
			   1,
			   100) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.5*100);
        makeScale(".high",
		  "scale1_46",
		  "Volume control",
		  tkvolume,
                  setVolume_47);
	displaySliderValue(".high", "scale1_46",
			   "0.5");
      }

}
/* main function */
int go(int argc, char *argv[]) {
	    double t_18;
double sampleCount_21;
      /* Hack for Sun only */
      unsigned count_36 = 0;
      short buffer_37[500];
      short *bufferptr_38;
      SndSoundStruct header_39;
      int counter_35;
double output_0;
double output_1[4000];
int output_17;
double state_50;
double output_2;
int output_3;
double output_4;
double output_5;
double output_6;
int Y_1_19;
int Y_2_20;
double output_7[100];
double taps_51[2];
int signalIn_27;
double signalOut_8[4000];
int signalOut_28;
double output_9[100];
int input_29;
encodingType_40="linear16";
sampleRate_41=8000;
channels_42=1;
portType_43="line_out";
volume_44=0.5;
balance_45=0.0;
taps_51[0]=1.0;
taps_51[1]=-1.0;
state_50=0.0;
value_16=440.0;
output_0 = 0.0;
{int i; for(i=0;i<4000;i++) output_1[i] = 0.0;}
output_17 = 0;
output_2 = 0.0;
output_3 = 0;
output_4 = 0.0;
output_5 = 0.0;
output_6 = 0.0;
Y_1_19 = 0;
Y_2_20 = 0;
sampleCount_21 = xMin_22;
{int i; for(i=0;i<100;i++) output_7[i] = 0.0;}
signalIn_27 = 0;
{int i; for(i=0;i<4000;i++) signalOut_8[i] = 0.0;}
signalOut_28 = 0;
{int i; for(i=0;i<100;i++) output_9[i] = 0.0;}
input_29 = 499;
counter_35 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_30 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_39.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_39.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_39.dataSize = 1000;
	  if(strcasecmp("linear16", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_39.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_39.dataFormat = 1;
	  }
	  header_39.samplingRate = 8000;
	  header_39.channelCount = 1;

	  write(file_30,(&header_39), 28);
	}
audio_setup_31(file_30, encodingType_40, sampleRate_41, channels_42);
audio_control_32(file_30, portType_43, volume_44, balance_45, 0);
audio_balance_34(file_30, balance_45);
{
	int sdfIterationCounter_53;
sdfIterationCounter_53 = 0;
while (sdfIterationCounter_53++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_48;for (sdfLoopCounter_48 = 0; sdfLoopCounter_48 < 8; sdfLoopCounter_48++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_49;for (sdfLoopCounter_49 = 0; sdfLoopCounter_49 < 500; sdfLoopCounter_49++) {
if ( getPollFlag() ) processFlags();
	{  /* star diff_sine.TkSlider1 (class CGCTkSlider) */
	output_0 = value_16;
	}
	{  /* star diff_sine.Gain1 (class CGCGain) */
output_6 = 0.000785398163397447 * output_0;
	}
	{  /* star diff_sine.oscillator1.Const1 (class CGCConst) */
	output_3 = 0.0;
	}
	{  /* star diff_sine.Const1 (class CGCConst) */
	output_5 = 1.0;
	}
	{  /* star diff_sine.oscillator1.Integrator1 (class CGCIntegrator) */
	    if (output_3 != 0) {
		t_18 = output_6;
	    } else {
		t_18 = output_6 +
			1.0 * state_50;
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
	    output_2 = t_18;
	    state_50 = t_18;
	}
	{  /* star diff_sine.oscillator1.Cos1 (class CGCCos) */
	output_4 = cos(output_2);
	}
	{  /* star diff_sine.oscillator1.Mpy.input=21 (class CGCMpy) */
	output_1[output_17] = output_4 * output_5;
	output_17 += 1;
	if (output_17 >= 4000)
		output_17 -= 4000;
	}
	{  /* star diff_sine.auto-fork-node2 (class CGCFork) */
	}
	{  /* star diff_sine.FIR1 (class CGCFIR) */
	int phase, tapsIndex, inC, i;
	int outCount = 1 - 1;
	int inPos;
	double out, tap;
	/* phase keeps track of which phase of the filter coefficients is used.
	   Starting phase depends on the decimationPhase state. */
	phase = 1 - 0 - 1;   
	
	/* Iterate once for each input consumed */
	for (inC = 1; inC <= 1 ; inC++) {

		/* Produce however many outputs are required for each 
		   input consumed */
		while (phase < 1) {
			out = 0.0;

			/* Compute the inner product. */
			for (i = 0; i < 2; i++) {
				tapsIndex = i * 1 + phase;
				if (tapsIndex >= 2)
			    		tap = 0.0;
				else
			 		tap = taps_51[tapsIndex];
				inPos = 1 - inC + i;
				out += tap * output_1[(signalIn_27-(inPos)+4000)%4000];
			}
			signalOut_8[(signalOut_28-(outCount))] = out;
			outCount--;;
			phase += 1;
		}
		phase -= 1;
	}
	signalIn_27 += 1;
	if (signalIn_27 >= 4000)
		signalIn_27 -= 4000;
	signalOut_28 += 1;
	if (signalOut_28 >= 4000)
		signalOut_28 -= 4000;
	}
	{  /* star diff_sine.auto-fork-node1 (class CGCFork) */
	}
}} /* end repeat, depth 2*/
	{  /* star diff_sine.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(1000/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_37[1000/2-1-i] 
	= (short)(signalOut_8[(input_29-(i))]*32767.0);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_30, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_36 - info.play.samples) > 
		 2000);
        count_36 += 1000/2;
      }
    bufferptr_38 = buffer_37;
      /* Write blockSize bytes to file */
      if (write(file_30, bufferptr_38, 1000) != 1000)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	input_29 += 500;
	if (input_29 >= 4000)
		input_29 -= 4000;
	}
}} /* end repeat, depth 1*/
	{  /* star diff_sine.Chop1 (class CGCChop) */
	int i;
	int hiLim = 99, inputIndex = 3900, loLim = 0;
	for (i = 0; i < 100; i++) {
	    if (i > hiLim || i < loLim) {
		output_7[(99-(i))] = 0;
	    }
	    else {
		output_7[(99-(i))] = signalOut_8[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star diff_sine.Chop2 (class CGCChop) */
	int i;
	int hiLim = 99, inputIndex = 3900, loLim = 0;
	for (i = 0; i < 100; i++) {
	    if (i > hiLim || i < loLim) {
		output_9[(99-(i))] = 0;
	    }
	    else {
		output_9[(99-(i))] = output_1[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_52;for (sdfLoopCounter_52 = 0; sdfLoopCounter_52 < 100; sdfLoopCounter_52++) {
if ( getPollFlag() ) processFlags();
	{  /* star diff_sine.TkPlot1 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_24, 1,
			   sampleCount_21, output_9[Y_1_19]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_24, 2,
			   sampleCount_21, output_7[Y_2_20]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_21 += 1.0;
if (sampleCount_21 > xMax_23) {
sampleCount_21 = xMin_22;
	  ptkPlotBreak(interp, &plotwin_24, 1);
	  ptkPlotBreak(interp, &plotwin_24, 2);
}
	Y_1_19 += 1;
	if (Y_1_19 >= 100)
		Y_1_19 -= 100;
	Y_2_20 += 1;
	if (Y_2_20 >= 100)
		Y_2_20 -= 100;
	}
}} /* end repeat, depth 1*/
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_30) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
