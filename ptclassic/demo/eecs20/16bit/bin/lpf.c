/* User:      ptuser
   Date:      Fri Jan 24 16:37:34 1997
   Target:    TclTk_Target
   Universe:  lpf */

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

char *name = "lpf";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_24;
double xMax_25;
ptkPlotWin plotwin_26;
ptkPlotDataset plotdataset_27;
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
double value_17;
char* encodingType_42="linear16";
int sampleRate_43;
int channels_44;
char* portType_45="line_out";
double volume_46;
double balance_47;
	    static int
	    setValue_16(dummy, interp, argc, argv)
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
		value_17 = 0.0 + (2000.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_17);
		displaySliderValue(".low", "scale_15", buf);
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
			- 0.0)/(2000.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_15",	/* name of the scale pane */
		      "Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_16);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_15", "440.0");
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_26);
	  xMin_24 = 0.0;
	  xMax_25 = 79.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_26,
			   &w,
			   ".win_28",
			   "Input and Output",
			   "600x400+0+0",
			   "n",
			   "y",
			   0.0,79.0, -1.5,1.5,
			   2,
			   80,
			   1,
			   80) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.5*100);
        makeScale(".high",
		  "scale1_48",
		  "Volume control",
		  tkvolume,
                  setVolume_49);
	displaySliderValue(".high", "scale1_48",
			   "0.5");
      }

}
/* main function */
int go(int argc, char *argv[]) {
	    double t_20;
double sampleCount_23;
      /* Hack for Sun only */
      unsigned count_38 = 0;
      short buffer_39[1000];
      short *bufferptr_40;
      SndSoundStruct header_41;
      int counter_37;
double output_0;
double output_1[4000];
int output_18;
double state_52;
int data_19;
double output_2;
int output_3;
double output_4;
double output_5;
double output_6;
int Y_1_21;
int Y_2_22;
double output_7[80];
double taps_53[32];
int signalIn_29;
double signalOut_8[4000];
int signalOut_30;
double output_9[80];
double output_10[1000];
int input_31;
encodingType_42="linear16";
sampleRate_43=8000;
channels_44=1;
portType_45="line_out";
volume_46=0.5;
balance_47=0.0;
taps_53[0]=-0.050182;
taps_53[1]=-0.008568;
taps_53[2]=0.001884;
taps_53[3]=0.015995;
taps_53[4]=0.027029;
taps_53[5]=0.028201;
taps_53[6]=0.016333;
taps_53[7]=-0.005946;
taps_53[8]=-0.029875;
taps_53[9]=-0.043214;
taps_53[10]=-0.034888;
taps_53[11]=0.000378;
taps_53[12]=0.058655;
taps_53[13]=0.127032;
taps_53[14]=0.187736;
taps_53[15]=0.223279;
taps_53[16]=0.223279;
taps_53[17]=0.187736;
taps_53[18]=0.127032;
taps_53[19]=0.058655;
taps_53[20]=0.000378;
taps_53[21]=-0.034888;
taps_53[22]=-0.043214;
taps_53[23]=-0.029875;
taps_53[24]=-0.005946;
taps_53[25]=0.016333;
taps_53[26]=0.028201;
taps_53[27]=0.027029;
taps_53[28]=0.015995;
taps_53[29]=0.001884;
taps_53[30]=-0.008568;
taps_53[31]=-0.050182;
state_52=0.0;
value_17=440.0;
output_0 = 0.0;
{int i; for(i=0;i<4000;i++) output_1[i] = 0.0;}
output_18 = 0;
data_19 = 0;
output_2 = 0.0;
output_3 = 0;
output_4 = 0.0;
output_5 = 0.0;
output_6 = 0.0;
Y_1_21 = 0;
Y_2_22 = 0;
sampleCount_23 = xMin_24;
{int i; for(i=0;i<80;i++) output_7[i] = 0.0;}
signalIn_29 = 0;
{int i; for(i=0;i<4000;i++) signalOut_8[i] = 0.0;}
signalOut_30 = 0;
{int i; for(i=0;i<80;i++) output_9[i] = 0.0;}
{int i; for(i=0;i<1000;i++) output_10[i] = 0.0;}
input_31 = 999;
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
    { int sdfLoopCounter_50;for (sdfLoopCounter_50 = 0; sdfLoopCounter_50 < 4; sdfLoopCounter_50++) {
if ( getPollFlag() ) processFlags();
	{  /* star lpf.TkSlider1 (class CGCTkSlider) */
	output_0 = value_17;
	}
	{  /* star lpf.Gain1 (class CGCGain) */
output_6 = 0.000785398163397447 * output_0;
	}
	{  /* star lpf.Repeat1 (class CGCRepeat) */
	int i;
	for (i = 0; i < 1000; i++) {
		output_10[(999-(i))] = output_6;
	}
	}
        { int sdfLoopCounter_51;for (sdfLoopCounter_51 = 0; sdfLoopCounter_51 < 1000; sdfLoopCounter_51++) {
if ( getPollFlag() ) processFlags();
	{  /* star lpf.oscillator1.Const1 (class CGCConst) */
	output_3 = 0.0;
	}
	{  /* star lpf.Const1 (class CGCConst) */
	output_5 = 1.0;
	}
	{  /* star lpf.oscillator1.Integrator1 (class CGCIntegrator) */
	    if (output_3 != 0) {
		t_20 = output_10[data_19];
	    } else {
		t_20 = output_10[data_19] +
			1.0 * state_52;
	    }
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if (t_20 > 3.14159265358979)
		do t_20 -= (3.14159265358979 - -3.14159265358979);
		while (t_20 > 3.14159265358979);
	    /* Take care of the bottom */
	    if (t_20 < -3.14159265358979)
		do t_20 += (3.14159265358979 - -3.14159265358979);
		while (t_20 < -3.14159265358979);
	    output_2 = t_20;
	    state_52 = t_20;
	data_19 += 1;
	if (data_19 >= 1000)
		data_19 -= 1000;
	}
	{  /* star lpf.oscillator1.Cos1 (class CGCCos) */
	output_4 = cos(output_2);
	}
	{  /* star lpf.oscillator1.Mpy.input=21 (class CGCMpy) */
	output_1[output_18] = output_4 * output_5;
	output_18 += 1;
	if (output_18 >= 4000)
		output_18 -= 4000;
	}
	{  /* star lpf.auto-fork-node2 (class CGCFork) */
	}
	{  /* star lpf.FIR1 (class CGCFIR) */
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
			for (i = 0; i < 32; i++) {
				tapsIndex = i * 1 + phase;
				if (tapsIndex >= 32)
			    		tap = 0.0;
				else
			 		tap = taps_53[tapsIndex];
				inPos = 1 - inC + i;
				out += tap * output_1[(signalIn_29-(inPos)+4000)%4000];
			}
			signalOut_8[(signalOut_30-(outCount))] = out;
			outCount--;;
			phase += 1;
		}
		phase -= 1;
	}
	signalIn_29 += 1;
	if (signalIn_29 >= 4000)
		signalIn_29 -= 4000;
	signalOut_30 += 1;
	if (signalOut_30 >= 4000)
		signalOut_30 -= 4000;
	}
	{  /* star lpf.auto-fork-node1 (class CGCFork) */
	}
}} /* end repeat, depth 2*/
	{  /* star lpf.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(2000/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_39[2000/2-1-i] 
	= (short)(signalOut_8[(input_31-(i))]*32767.0);
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
	input_31 += 1000;
	if (input_31 >= 4000)
		input_31 -= 4000;
	}
}} /* end repeat, depth 1*/
	{  /* star lpf.Chop1 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_7[(79-(i))] = 0;
	    }
	    else {
		output_7[(79-(i))] = signalOut_8[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star lpf.Chop2 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_9[(79-(i))] = 0;
	    }
	    else {
		output_9[(79-(i))] = output_1[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_54;for (sdfLoopCounter_54 = 0; sdfLoopCounter_54 < 80; sdfLoopCounter_54++) {
if ( getPollFlag() ) processFlags();
	{  /* star lpf.TkPlot1 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_26, 1,
			   sampleCount_23, output_9[Y_1_21]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_26, 2,
			   sampleCount_23, output_7[Y_2_22]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_23 += 1.0;
if (sampleCount_23 > xMax_25) {
sampleCount_23 = xMin_24;
	  ptkPlotBreak(interp, &plotwin_26, 1);
	  ptkPlotBreak(interp, &plotwin_26, 2);
}
	Y_1_21 += 1;
	if (Y_1_21 >= 80)
		Y_1_21 -= 80;
	Y_2_22 += 1;
	if (Y_2_22 >= 80)
		Y_2_22 -= 80;
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
