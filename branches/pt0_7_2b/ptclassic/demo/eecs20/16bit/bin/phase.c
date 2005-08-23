/* User:      ptuser
   Date:      Fri Jan 24 16:25:31 1997
   Target:    TclTk_Target
   Universe:  phase */

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
#include "ptdspMuLaw.h"

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

char *name = "phase";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_38;
double xMax_39;
ptkPlotWin plotwin_40;
ptkPlotDataset plotdataset_41;
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
      int file_50;      
double value_28;
double value_47;
char* encodingType_60="ulaw8";
int sampleRate_61;
int channels_62;
char* portType_63="speaker";
double volume_64;
double balance_65;
	    static int
	    setValue_27(dummy, interp, argc, argv)
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
		value_28 = 0.0 + (360.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_28);
		displaySliderValue(".low", "scale_26", buf);
                return TCL_OK;
            }
	    static int
	    setValue_46(dummy, interp, argc, argv)
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
		value_47 = 0.0 + (4000.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_47);
		displaySliderValue(".low", "scale_45", buf);
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
      static void audio_setup_51
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
      static void audio_control_52
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
      static void audio_gain_53
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
      static void audio_balance_54
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_67(dummy, interp, argc, argv)
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
	    volume_64 = (float) (tkvolume/100.0);
	    audio_gain_53
	      (file_50, volume_64, 0);

	    sprintf(buf, "%f", volume_64);
	    displaySliderValue(".high", "scale1_66", buf);

            return TCL_OK;
        }
void tkSetup() {
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*(0.0
			- 0.0)/(360.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_26",	/* name of the scale pane */
		      "Phase",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_27);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_26", "0.0");
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_40);
	  xMin_38 = 0.0;
	  xMax_39 = 79.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_40,
			   &w,
			   ".win_42",
			   "Three Sinusoids with Different Phases",
			   "600x400+0+0",
			   "n",
			   "y",
			   0.0,79.0, -1.5,1.5,
			   3,
			   10,
			   1,
			   80) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*(440.0
			- 0.0)/(4000.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_45",	/* name of the scale pane */
		      "Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_46);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_45", "440.0");
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.0*100);
        makeScale(".high",
		  "scale1_66",
		  "Volume control",
		  tkvolume,
                  setVolume_67);
	displaySliderValue(".high", "scale1_66",
			   "0.0");
      }

}
/* main function */
int go(int argc, char *argv[]) {
double sampleCount_37;
	    double t_43;
      /* Hack for Sun only */
      unsigned count_56 = 0;
      unsigned char buffer_57[2000];
      unsigned char *bufferptr_58;
      SndSoundStruct header_59;
      int counter_55;
double output_0;
double output_1[4000];
int output_29;
double output_2[4000];
int output_30;
double output_3;
double output_4;
double output_5;
int input_1_31;
double output_6;
int input_1_32;
double output_7[4000];
int output_33;
int Y_1_34;
int Y_2_35;
int Y_3_36;
double output_8[80];
double output_9[80];
double output_10[80];
double state_71;
double output_11;
double output_12[4001];
int output_44;
int output_13;
int input_48;
double output_14;
double output_15;
int input_49;
encodingType_60="ulaw8";
sampleRate_61=8000;
channels_62=1;
portType_63="speaker";
volume_64=0.0;
balance_65=0.0;
value_47=440.0;
state_71=0.0;
value_28=0.0;
output_0 = 0.0;
{int i; for(i=0;i<4000;i++) output_1[i] = 0.0;}
output_29 = 0;
{int i; for(i=0;i<4000;i++) output_2[i] = 0.0;}
output_30 = 0;
output_3 = 0.0;
output_4 = 0.0;
output_5 = 0.0;
input_1_31 = 0;
output_6 = 0.0;
input_1_32 = 0;
{int i; for(i=0;i<4000;i++) output_7[i] = 0.0;}
output_33 = 0;
Y_1_34 = 0;
Y_2_35 = 0;
Y_3_36 = 0;
sampleCount_37 = xMin_38;
{int i; for(i=0;i<80;i++) output_8[i] = 0.0;}
{int i; for(i=0;i<80;i++) output_9[i] = 0.0;}
{int i; for(i=0;i<80;i++) output_10[i] = 0.0;}
output_11 = 0.0;
{int i; for(i=0;i<4001;i++) output_12[i] = 0.0;}
output_44 = 0;
output_13 = 0;
input_48 = 4000;
output_14 = 0.0;
output_15 = 0.0;
input_49 = 1999;
counter_55 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_50 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_59.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_59.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_59.dataSize = 2000;
	  if(strcasecmp("ulaw8", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_59.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_59.dataFormat = 1;
	  }
	  header_59.samplingRate = 8000;
	  header_59.channelCount = 1;

	  write(file_50,(&header_59), 28);
	}
audio_setup_51(file_50, encodingType_60, sampleRate_61, channels_62);
audio_control_52(file_50, portType_63, volume_64, balance_65, 0);
audio_balance_54(file_50, balance_65);
{
	int sdfIterationCounter_73;
sdfIterationCounter_73 = 0;
while (sdfIterationCounter_73++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_68;for (sdfLoopCounter_68 = 0; sdfLoopCounter_68 < 4000; sdfLoopCounter_68++) {
if ( getPollFlag() ) processFlags();
	{  /* star phase.TkSlider2 (class CGCTkSlider) */
	output_12[output_44] = value_47;
	output_44 += 1;
	if (output_44 >= 4001)
		output_44 -= 4001;
	}
}} /* end repeat, depth 1*/
    { int sdfLoopCounter_69;for (sdfLoopCounter_69 = 0; sdfLoopCounter_69 < 2; sdfLoopCounter_69++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_70;for (sdfLoopCounter_70 = 0; sdfLoopCounter_70 < 2000; sdfLoopCounter_70++) {
if ( getPollFlag() ) processFlags();
	{  /* star phase.TkSlider1 (class CGCTkSlider) */
	output_0 = value_28;
	}
	{  /* star phase.Gain2 (class CGCGain) */
output_15 = -0.0174532925199433 * output_0;
	}
	{  /* star phase.auto-fork-node5 (class CGCFork) */
	}
	{  /* star phase.Cos2 (class CGCCos) */
	output_3 = cos(output_15);
	}
	{  /* star phase.Sin2 (class CGCSin) */
	output_4 = sin(output_15);
	}
	{  /* star phase.Const1 (class CGCConst) */
	output_13 = 0.0;
	}
	{  /* star phase.Gain1 (class CGCGain) */
output_14 = 0.000785398163397447 * output_12[input_48];
	input_48 += 1;
	if (input_48 >= 4001)
		input_48 -= 4001;
	}
	{  /* star phase.Integrator1 (class CGCIntegrator) */
	    if (output_13 != 0) {
		t_43 = output_14;
	    } else {
		t_43 = output_14 +
			1.0 * state_71;
	    }
	    output_11 = t_43;
	    state_71 = t_43;
	}
	{  /* star phase.auto-fork-node4 (class CGCFork) */
	}
	{  /* star phase.Sin1 (class CGCSin) */
	output_2[output_30] = sin(output_11);
	output_30 += 1;
	if (output_30 >= 4000)
		output_30 -= 4000;
	}
	{  /* star phase.auto-fork-node2 (class CGCFork) */
	}
	{  /* star phase.Mpy.input=22 (class CGCMpy) */
	output_6 = output_2[input_1_32] * output_4;
	input_1_32 += 1;
	if (input_1_32 >= 4000)
		input_1_32 -= 4000;
	}
	{  /* star phase.Cos1 (class CGCCos) */
	output_1[output_29] = cos(output_11);
	output_29 += 1;
	if (output_29 >= 4000)
		output_29 -= 4000;
	}
	{  /* star phase.auto-fork-node1 (class CGCFork) */
	}
	{  /* star phase.Mpy.input=21 (class CGCMpy) */
	output_5 = output_1[input_1_31] * output_3;
	input_1_31 += 1;
	if (input_1_31 >= 4000)
		input_1_31 -= 4000;
	}
	{  /* star phase.Sub1 (class CGCSub) */
	output_7[output_33] = output_5;
	output_7[output_33] -= output_6;
	output_33 += 1;
	if (output_33 >= 4000)
		output_33 -= 4000;
	}
	{  /* star phase.auto-fork-node3 (class CGCFork) */
	}
}} /* end repeat, depth 2*/
	{  /* star phase.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 8-bit u-law encoding */
    int i;
    for(i = 0; i < 2000; i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
      short sample16 = (short)(output_7[(input_49-(i))] * 32767.0);
      /* Convert 16-bit sample to PCM mu-law */
      buffer_57[2000-1-i] = 
	      Ptdsp_LinearToPCMMuLaw(sample16);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_50, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_56 - info.play.samples) > 
		 4000);
        count_56 += 2000;
      }
    bufferptr_58 = buffer_57;
      /* Write blockSize bytes to file */
      if (write(file_50, bufferptr_58, 2000) != 2000)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	input_49 += 2000;
	if (input_49 >= 4000)
		input_49 -= 4000;
	}
}} /* end repeat, depth 1*/
	{  /* star phase.Chop1 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_8[(79-(i))] = 0;
	    }
	    else {
		output_8[(79-(i))] = output_1[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star phase.Chop2 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_9[(79-(i))] = 0;
	    }
	    else {
		output_9[(79-(i))] = output_2[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star phase.Chop3 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_10[(79-(i))] = 0;
	    }
	    else {
		output_10[(79-(i))] = output_7[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_72;for (sdfLoopCounter_72 = 0; sdfLoopCounter_72 < 80; sdfLoopCounter_72++) {
if ( getPollFlag() ) processFlags();
	{  /* star phase.TkPlot1 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_40, 1,
			   sampleCount_37, output_10[Y_1_34]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_40, 2,
			   sampleCount_37, output_8[Y_2_35]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_40, 3,
			   sampleCount_37, output_9[Y_3_36]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_37 += 1.0;
if (sampleCount_37 > xMax_39) {
sampleCount_37 = xMin_38;
	  ptkPlotBreak(interp, &plotwin_40, 1);
	  ptkPlotBreak(interp, &plotwin_40, 2);
	  ptkPlotBreak(interp, &plotwin_40, 3);
}
	Y_1_34 += 1;
	if (Y_1_34 >= 80)
		Y_1_34 -= 80;
	Y_2_35 += 1;
	if (Y_2_35 >= 80)
		Y_2_35 -= 80;
	Y_3_36 += 1;
	if (Y_3_36 >= 80)
		Y_3_36 -= 80;
	}
}} /* end repeat, depth 1*/
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_50) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
