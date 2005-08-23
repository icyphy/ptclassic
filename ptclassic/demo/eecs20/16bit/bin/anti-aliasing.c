/* User:      ptuser
   Date:      Fri Jan 24 16:39:50 1997
   Target:    TclTk_Target
   Universe:  anti-aliasing */

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

char *name = "anti-aliasing";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_25;
double xMax_26;
ptkPlotWin plotwin_27;
ptkPlotDataset plotdataset_28;
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
	  ptkInitPlot(&plotwin_27);
	  xMin_25 = 0.0;
	  xMax_26 = 79.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_27,
			   &w,
			   ".win_29",
			   "Aliasing",
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
double sampleCount_24;
      /* Hack for Sun only */
      unsigned count_38 = 0;
      short buffer_39[100];
      short *bufferptr_40;
      SndSoundStruct header_41;
      int counter_37;
double output_0[32000];
int output_15;
double state_53;
double output_1;
int output_2;
double output_3;
double output_4;
double output_5;
double output_6;
int input_20;
double output_7[32000];
int output_21;
double output_8[80];
double output_9[80];
int Y_1_22;
int Y_2_23;
double taps_54[32];
int signalIn_30;
double signalOut_10[100];
int signalOut_31;
encodingType_42="linear16";
sampleRate_43=8000;
channels_44=1;
portType_45="line_out";
volume_46=0.0;
balance_47=0.0;
taps_54[0]=-0.050182;
taps_54[1]=-0.008568;
taps_54[2]=0.001884;
taps_54[3]=0.015995;
taps_54[4]=0.027029;
taps_54[5]=0.028201;
taps_54[6]=0.016333;
taps_54[7]=-0.005946;
taps_54[8]=-0.029875;
taps_54[9]=-0.043214;
taps_54[10]=-0.034888;
taps_54[11]=0.000378;
taps_54[12]=0.058655;
taps_54[13]=0.127032;
taps_54[14]=0.187736;
taps_54[15]=0.223279;
taps_54[16]=0.223279;
taps_54[17]=0.187736;
taps_54[18]=0.127032;
taps_54[19]=0.058655;
taps_54[20]=0.000378;
taps_54[21]=-0.034888;
taps_54[22]=-0.043214;
taps_54[23]=-0.029875;
taps_54[24]=-0.005946;
taps_54[25]=0.016333;
taps_54[26]=0.028201;
taps_54[27]=0.027029;
taps_54[28]=0.015995;
taps_54[29]=0.001884;
taps_54[30]=-0.008568;
taps_54[31]=-0.050182;
value_19=440.0;
state_53=0.0;
{int i; for(i=0;i<32000;i++) output_0[i] = 0.0;}
output_15 = 0;
output_1 = 0.0;
output_2 = 0;
output_3 = 0.0;
output_4 = 0.0;
output_5 = 0.0;
output_6 = 0.0;
input_20 = 0;
{int i; for(i=0;i<32000;i++) output_7[i] = 0.0;}
output_21 = 3;
{int i; for(i=0;i<80;i++) output_8[i] = 0.0;}
{int i; for(i=0;i<80;i++) output_9[i] = 0.0;}
Y_1_22 = 0;
Y_2_23 = 0;
sampleCount_24 = xMin_25;
signalIn_30 = 3;
{int i; for(i=0;i<100;i++) signalOut_10[i] = 0.0;}
signalOut_31 = 0;
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
	  header_41.dataSize = 200;
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
	int sdfIterationCounter_56;
sdfIterationCounter_56 = 0;
while (sdfIterationCounter_56++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_50;for (sdfLoopCounter_50 = 0; sdfLoopCounter_50 < 80; sdfLoopCounter_50++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_51;for (sdfLoopCounter_51 = 0; sdfLoopCounter_51 < 100; sdfLoopCounter_51++) {
if ( getPollFlag() ) processFlags();
            { int sdfLoopCounter_52;for (sdfLoopCounter_52 = 0; sdfLoopCounter_52 < 4; sdfLoopCounter_52++) {
if ( getPollFlag() ) processFlags();
	{  /* star anti-aliasing.oscillator1.Const1 (class CGCConst) */
	output_2 = 0.0;
	}
	{  /* star anti-aliasing.Const1 (class CGCConst) */
	output_4 = 1.0;
	}
	{  /* star anti-aliasing.TkSlider1 (class CGCTkSlider) */
	output_5 = value_19;
	}
	{  /* star anti-aliasing.Gain1 (class CGCGain) */
output_6 = 0.000196349540849362 * output_5;
	}
	{  /* star anti-aliasing.oscillator1.Integrator1 (class CGCIntegrator) */
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
	{  /* star anti-aliasing.oscillator1.Cos1 (class CGCCos) */
	output_3 = cos(output_1);
	}
	{  /* star anti-aliasing.oscillator1.Mpy.input=21 (class CGCMpy) */
	output_0[output_15] = output_3 * output_4;
	output_15 += 1;
	if (output_15 >= 32000)
		output_15 -= 32000;
	}
	{  /* star anti-aliasing.auto-fork-node1 (class CGCFork) */
	}
}} /* end repeat, depth 3*/
	{  /* star anti-aliasing.FIR1 (class CGCFIR) */
	int phase, tapsIndex, inC, i;
	int outCount = 1 - 1;
	int inPos;
	double out, tap;
	/* phase keeps track of which phase of the filter coefficients is used.
	   Starting phase depends on the decimationPhase state. */
	phase = 4 - 0 - 1;   
	
	/* Iterate once for each input consumed */
	for (inC = 1; inC <= 4 ; inC++) {

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
			 		tap = taps_54[tapsIndex];
				inPos = 4 - inC + i;
				out += tap * output_0[(signalIn_30-(inPos)+32000)%32000];
			}
			signalOut_10[(signalOut_31-(outCount))] = out;
			outCount--;;
			phase += 4;
		}
		phase -= 1;
	}
	signalIn_30 += 4;
	if (signalIn_30 >= 32000)
		signalIn_30 -= 32000;
	signalOut_31 += 1;
	if (signalOut_31 >= 100)
		signalOut_31 -= 100;
	}
	{  /* star anti-aliasing.auto-fork-node2 (class CGCFork) */
	}
	{  /* star anti-aliasing.UpSample1 (class CGCUpSample) */
	int i;
	for (i = 0; i < 4; i++) {
		output_7[(output_21-(i))] = 0.0;
	}
	output_7[(output_21-(0))] = signalOut_10[input_20];
	input_20 += 1;
	if (input_20 >= 100)
		input_20 -= 100;
	output_21 += 4;
	if (output_21 >= 32000)
		output_21 -= 32000;
	}
}} /* end repeat, depth 2*/
	{  /* star anti-aliasing.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(200/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_39[200/2-1-i] 
	= (short)(signalOut_10[(99-(i))]*32767.0);
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
		 1000);
        count_38 += 200/2;
      }
    bufferptr_40 = buffer_39;
      /* Write blockSize bytes to file */
      if (write(file_32, bufferptr_40, 200) != 200)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	}
}} /* end repeat, depth 1*/
	{  /* star anti-aliasing.Chop1 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 31920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_8[(79-(i))] = 0;
	    }
	    else {
		output_8[(79-(i))] = output_0[(31999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star anti-aliasing.Chop2 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 31920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_9[(79-(i))] = 0;
	    }
	    else {
		output_9[(79-(i))] = output_7[(31999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_55;for (sdfLoopCounter_55 = 0; sdfLoopCounter_55 < 80; sdfLoopCounter_55++) {
if ( getPollFlag() ) processFlags();
	{  /* star anti-aliasing.TkPlot1 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_27, 1,
			   sampleCount_24, output_8[Y_1_22]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_27, 2,
			   sampleCount_24, output_9[Y_2_23]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_24 += 1.0;
if (sampleCount_24 > xMax_26) {
sampleCount_24 = xMin_25;
	  ptkPlotBreak(interp, &plotwin_27, 1);
	  ptkPlotBreak(interp, &plotwin_27, 2);
}
	Y_1_22 += 1;
	if (Y_1_22 >= 80)
		Y_1_22 -= 80;
	Y_2_23 += 1;
	if (Y_2_23 >= 80)
		Y_2_23 -= 80;
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
