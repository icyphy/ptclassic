/* User:      ptuser
   Date:      Fri Jan 24 16:35:25 1997
   Target:    TclTk_Target
   Universe:  aliasing */

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

char *name = "aliasing";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_27;
double xMax_28;
ptkPlotWin plotwin_29;
ptkPlotDataset plotdataset_30;
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
      int file_32;      
double value_19;
char* encodingType_42="linear16";
int sampleRate_43;
int channels_44;
char* portType_45="line_out";
double volume_46;
double balance_47;
	    static int
	    setValue_18(dummy, interp, argc, argv)
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
		value_19 = 0.0 + (8000.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_19);
		displaySliderValue(".low", "scale_17", buf);
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
      static void audio_setup_33
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
      static void audio_control_34
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
      static void audio_gain_35
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
      static void audio_balance_36
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_49(dummy, interp, argc, argv)
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
	    volume_46 = (float) (tkvolume/100.0);
	    audio_gain_35
	      (file_32, volume_46, 0);

	    sprintf(buf, "%f", volume_46);
	    displaySliderValue(".high", "scale1_48", buf);

            return TCL_OK;
        }
void tkSetup() {
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*(440.0
			- 0.0)/(8000.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_17",	/* name of the scale pane */
		      "Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_18);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_17", "440.0");
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_29);
	  xMin_27 = 0.0;
	  xMax_28 = 159.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_29,
			   &w,
			   ".win_31",
			   "Aliasing",
			   "600x400+0+0",
			   "n",
			   "y",
			   0.0,159.0, -1.5,1.5,
			   2,
			   160,
			   1,
			   160) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.0*100);
        makeScale(".high",
		  "scale1_48",
		  "Volume control",
		  tkvolume,
                  setVolume_49);
	displaySliderValue(".high", "scale1_48",
			   "0.0");
      }

}
/* main function */
int go(int argc, char *argv[]) {
	    double t_16;
double sampleCount_26;
      /* Hack for Sun only */
      unsigned count_38 = 0;
      short buffer_39[1000];
      short *bufferptr_40;
      SndSoundStruct header_41;
      int counter_37;
double output_0[64000];
int output_15;
double state_53;
double output_1;
int output_2;
double output_3;
double output_4;
double output_5;
double output_6;
int input_20;
double output_7[1000];
int output_21;
int input_22;
double output_8[64000];
int output_23;
double output_9[160];
double output_10[160];
int Y_1_24;
int Y_2_25;
encodingType_42="linear16";
sampleRate_43=8000;
channels_44=1;
portType_45="line_out";
volume_46=0.0;
balance_47=0.0;
value_19=440.0;
state_53=0.0;
{int i; for(i=0;i<64000;i++) output_0[i] = 0.0;}
output_15 = 0;
output_1 = 0.0;
output_2 = 0;
output_3 = 0.0;
output_4 = 0.0;
output_5 = 0.0;
output_6 = 0.0;
input_20 = 7;
{int i; for(i=0;i<1000;i++) output_7[i] = 0.0;}
output_21 = 0;
input_22 = 0;
{int i; for(i=0;i<64000;i++) output_8[i] = 0.0;}
output_23 = 7;
{int i; for(i=0;i<160;i++) output_9[i] = 0.0;}
{int i; for(i=0;i<160;i++) output_10[i] = 0.0;}
Y_1_24 = 0;
Y_2_25 = 0;
sampleCount_26 = xMin_27;
counter_37 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_32 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_41.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_41.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_41.dataSize = 2000;
	  if(strcasecmp("linear16", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_41.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_41.dataFormat = 1;
	  }
	  header_41.samplingRate = 8000;
	  header_41.channelCount = 1;

	  write(file_32,(&header_41), 28);
	}
audio_setup_33(file_32, encodingType_42, sampleRate_43, channels_44);
audio_control_34(file_32, portType_45, volume_46, balance_47, 0);
audio_balance_36(file_32, balance_47);
{
	int sdfIterationCounter_55;
sdfIterationCounter_55 = 0;
while (sdfIterationCounter_55++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_50;for (sdfLoopCounter_50 = 0; sdfLoopCounter_50 < 8; sdfLoopCounter_50++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_51;for (sdfLoopCounter_51 = 0; sdfLoopCounter_51 < 1000; sdfLoopCounter_51++) {
if ( getPollFlag() ) processFlags();
            { int sdfLoopCounter_52;for (sdfLoopCounter_52 = 0; sdfLoopCounter_52 < 8; sdfLoopCounter_52++) {
if ( getPollFlag() ) processFlags();
	{  /* star aliasing.oscillator1.Const1 (class CGCConst) */
	output_2 = 0.0;
	}
	{  /* star aliasing.Const1 (class CGCConst) */
	output_4 = 1.0;
	}
	{  /* star aliasing.TkSlider1 (class CGCTkSlider) */
	output_5 = value_19;
	}
	{  /* star aliasing.Gain1 (class CGCGain) */
output_6 = 9.81747704246809e-05 * output_5;
	}
	{  /* star aliasing.oscillator1.Integrator1 (class CGCIntegrator) */
	    if (output_2 != 0) {
		t_16 = output_6;
	    } else {
		t_16 = output_6 +
			1.0 * state_53;
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
	    state_53 = t_16;
	}
	{  /* star aliasing.oscillator1.Cos1 (class CGCCos) */
	output_3 = cos(output_1);
	}
	{  /* star aliasing.oscillator1.Mpy.input=21 (class CGCMpy) */
	output_0[output_15] = output_3 * output_4;
	output_15 += 1;
	if (output_15 >= 64000)
		output_15 -= 64000;
	}
	{  /* star aliasing.auto-fork-node2 (class CGCFork) */
	}
}} /* end repeat, depth 3*/
	{  /* star aliasing.DownSample1 (class CGCDownSample) */
	output_7[output_21] = output_0[(input_20-(0))];
	input_20 += 8;
	if (input_20 >= 64000)
		input_20 -= 64000;
	output_21 += 1;
	if (output_21 >= 1000)
		output_21 -= 1000;
	}
	{  /* star aliasing.auto-fork-node1 (class CGCFork) */
	}
	{  /* star aliasing.UpSample1 (class CGCUpSample) */
	int i;
	for (i = 0; i < 8; i++) {
		output_8[(output_23-(i))] = 0.0;
	}
	output_8[(output_23-(0))] = output_7[input_22];
	input_22 += 1;
	if (input_22 >= 1000)
		input_22 -= 1000;
	output_23 += 8;
	if (output_23 >= 64000)
		output_23 -= 64000;
	}
}} /* end repeat, depth 2*/
	{  /* star aliasing.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(2000/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_39[2000/2-1-i] 
	= (short)(output_7[(999-(i))]*32767.0);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_32, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_38 - info.play.samples) > 
		 4000);
        count_38 += 2000/2;
      }
    bufferptr_40 = buffer_39;
      /* Write blockSize bytes to file */
      if (write(file_32, bufferptr_40, 2000) != 2000)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	}
}} /* end repeat, depth 1*/
	{  /* star aliasing.Chop1 (class CGCChop) */
	int i;
	int hiLim = 159, inputIndex = 63840, loLim = 0;
	for (i = 0; i < 160; i++) {
	    if (i > hiLim || i < loLim) {
		output_9[(159-(i))] = 0;
	    }
	    else {
		output_9[(159-(i))] = output_0[(63999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star aliasing.Chop2 (class CGCChop) */
	int i;
	int hiLim = 159, inputIndex = 63840, loLim = 0;
	for (i = 0; i < 160; i++) {
	    if (i > hiLim || i < loLim) {
		output_10[(159-(i))] = 0;
	    }
	    else {
		output_10[(159-(i))] = output_8[(63999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_54;for (sdfLoopCounter_54 = 0; sdfLoopCounter_54 < 160; sdfLoopCounter_54++) {
if ( getPollFlag() ) processFlags();
	{  /* star aliasing.TkPlot1 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_29, 1,
			   sampleCount_26, output_9[Y_1_24]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_29, 2,
			   sampleCount_26, output_10[Y_2_25]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_26 += 1.0;
if (sampleCount_26 > xMax_28) {
sampleCount_26 = xMin_27;
	  ptkPlotBreak(interp, &plotwin_29, 1);
	  ptkPlotBreak(interp, &plotwin_29, 2);
}
	Y_1_24 += 1;
	if (Y_1_24 >= 160)
		Y_1_24 -= 160;
	Y_2_25 += 1;
	if (Y_2_25 >= 160)
		Y_2_25 -= 160;
	}
}} /* end repeat, depth 1*/
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_32) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
