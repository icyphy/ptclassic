/* User:      ptuser
   Date:      Fri Jan 24 16:33:56 1997
   Target:    TclTk_Target
   Universe:  sounds */

/* Define macro for prototyping functions on ANSI & non-ANSI compilers */
#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args) args
#else
#define ARGS(args) ()
#endif
#endif

#include "ptkPlot.h"
#include "ptkPlot_defs.h"
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

char *name = "sounds";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_19;
double xMax_20;
ptkPlotWin plotwin_21;
ptkPlotDataset plotdataset_22;
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
      int file_33;      
double value_27;
double value_31;
char* encodingType_43="linear16";
int sampleRate_44;
int channels_45;
char* portType_46="line_out";
double volume_47;
double balance_48;
	    static int
	    setValue_26(dummy, interp, argc, argv)
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
		value_27 = 0.0 + (2000.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_27);
		displaySliderValue(".low", "scale_25", buf);
                return TCL_OK;
            }
	    static int
            setValue_30(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		float temp;
                if(sscanf(argv[1], "%f", &temp) != 1) {
                    errorReport("Invalid value");
                    return TCL_ERROR;
                }
		value_31 = temp;
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
      static void audio_setup_34
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
      static void audio_control_35
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
      static void audio_gain_36
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
      static void audio_balance_37
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_50(dummy, interp, argc, argv)
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
	    volume_47 = (float) (tkvolume/100.0);
	    audio_gain_36
	      (file_33, volume_47, 0);

	    sprintf(buf, "%f", volume_47);
	    displaySliderValue(".high", "scale1_49", buf);

            return TCL_OK;
        }
void tkSetup() {
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_21);
	  xMin_19 = 0.0;
	  xMax_20 = 79.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_21,
			   &w,
			   ".win_23",
			   "Waveform",
			   "600x400+0+0",
			   "n",
			   "y",
			   0.0,79.0, -1.5,1.5,
			   1,
			   10,
			   1,
			   80) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*(262.0
			- 0.0)/(2000.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_25",	/* name of the scale pane */
		      "Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_26);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_25", "262.0");
	    /* Establish the Tk window for setting the value */
	    makeEntry(".high",		/* position in control panel */
		      "entry_29",	/* name of entry pane */
		      "Select Waveform (0-3)",	/* text identifying the pane */
		      "0.0",		/* initial value of the entry */
		      setValue_30);	/* callback procedure */
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.5*100);
        makeScale(".high",
		  "scale1_49",
		  "Volume control",
		  tkvolume,
                  setVolume_50);
	displaySliderValue(".high", "scale1_49",
			   "0.5");
      }

}
/* main function */
int go(int argc, char *argv[]) {
double sampleCount_18;
	    double t_24;
      /* Hack for Sun only */
      unsigned count_39 = 0;
      short buffer_40[500];
      short *bufferptr_41;
      SndSoundStruct header_42;
      int counter_38;
int Y_1_17;
complex breakpoints_55[3];
double output_0;
double output_1[80];
double state_53;
double output_2;
int output_3;
complex breakpoints_54[4];
double output_4;
double output_5;
double output_6;
double output_7;
double output_8[4000];
int output_28;
double output_9;
int output_10;
int input_32;
encodingType_43="linear16";
sampleRate_44=8000;
channels_45=1;
portType_46="line_out";
volume_47=0.5;
balance_48=0.0;
value_31=0.0;
value_27=262.0;
breakpoints_54[0].real=0.0; breakpoints_54[0].imag=1.0;
breakpoints_54[1].real=3.14158; breakpoints_54[1].imag=1.0;
breakpoints_54[2].real=3.14159; breakpoints_54[2].imag=-1.0;
breakpoints_54[3].real=6.2831852; breakpoints_54[3].imag=-1.0;
state_53=0.0;
breakpoints_55[0].real=0.0; breakpoints_55[0].imag=1.0;
breakpoints_55[1].real=3.14159; breakpoints_55[1].imag=-1.0;
breakpoints_55[2].real=6.2831853; breakpoints_55[2].imag=1.0;
Y_1_17 = 0;
sampleCount_18 = xMin_19;
output_0 = 0.0;
{int i; for(i=0;i<80;i++) output_1[i] = 0.0;}
output_2 = 0.0;
output_3 = 0;
output_4 = 0.0;
output_5 = 0.0;
output_6 = 0.0;
output_7 = 0.0;
{int i; for(i=0;i<4000;i++) output_8[i] = 0.0;}
output_28 = 0;
output_9 = 0.0;
output_10 = 0;
input_32 = 499;
counter_38 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_33 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_42.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_42.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_42.dataSize = 1000;
	  if(strcasecmp("linear16", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_42.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_42.dataFormat = 1;
	  }
	  header_42.samplingRate = 8000;
	  header_42.channelCount = 1;

	  write(file_33,(&header_42), 28);
	}
audio_setup_34(file_33, encodingType_43, sampleRate_44, channels_45);
audio_control_35(file_33, portType_46, volume_47, balance_48, 0);
audio_balance_37(file_33, balance_48);
{
	int sdfIterationCounter_57;
sdfIterationCounter_57 = 0;
while (sdfIterationCounter_57++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_51;for (sdfLoopCounter_51 = 0; sdfLoopCounter_51 < 8; sdfLoopCounter_51++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_52;for (sdfLoopCounter_52 = 0; sdfLoopCounter_52 < 500; sdfLoopCounter_52++) {
if ( getPollFlag() ) processFlags();
	{  /* star sounds.Const1 (class CGCConst) */
	output_3 = 0.0;
	}
	{  /* star sounds.TkSlider1 (class CGCTkSlider) */
	output_6 = value_27;
	}
	{  /* star sounds.TkEntry1 (class CGCTkEntry) */
	output_10 = value_31;
	}
	{  /* star sounds.Gain1 (class CGCGain) */
output_5 = 0.000785398163397447 * output_6;
	}
	{  /* star sounds.Integrator1 (class CGCIntegrator) */
	    if (output_3 != 0) {
		t_24 = output_5;
	    } else {
		t_24 = output_5 +
			1.0 * state_53;
	    }
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if (t_24 > 6.28318530717958)
		do t_24 -= (6.28318530717958 - 0.0);
		while (t_24 > 6.28318530717958);
	    /* Take care of the bottom */
	    if (t_24 < 0.0)
		do t_24 += (6.28318530717958 - 0.0);
		while (t_24 < 0.0);
	    output_2 = t_24;
	    state_53 = t_24;
	}
	{  /* star sounds.auto-fork-node2 (class CGCFork) */
	}
	{  /* star sounds.PcwzLinear2 (class CGCPcwzLinear) */
	  double in, x1, x2, y1, y2;
	  int i, ind, sz;
	  in = output_2;
	  if (in < breakpoints_54[0].real) {
	    output_4 = breakpoints_54[0].imag;
	  } else {
	    ind = 0;
	    sz = 4;
	    for (i = 1; i < sz; i++) {
	      if (in < breakpoints_54[i].real) {
		x1 = breakpoints_54[i-1].real;
		y1 = breakpoints_54[i-1].imag;
		x2 = breakpoints_54[i].real;
		y2 = breakpoints_54[i].imag;
		output_4 = y1 + (y2 - y1)*(in - x1)/(x2 - x1);
		ind = 1;
		break;
	      }
	    }
	    if (!ind) {
	      sz--;
	      output_4 = breakpoints_54[sz].imag;
	    }
	  }
	}
	{  /* star sounds.Cos1 (class CGCCos) */
	output_7 = cos(output_2);
	}
	{  /* star sounds.Gain2 (class CGCGain) */
output_9 = 0.159154943091896 * output_2;
	}
	{  /* star sounds.PcwzLinear1 (class CGCPcwzLinear) */
	  double in, x1, x2, y1, y2;
	  int i, ind, sz;
	  in = output_2;
	  if (in < breakpoints_55[0].real) {
	    output_0 = breakpoints_55[0].imag;
	  } else {
	    ind = 0;
	    sz = 3;
	    for (i = 1; i < sz; i++) {
	      if (in < breakpoints_55[i].real) {
		x1 = breakpoints_55[i-1].real;
		y1 = breakpoints_55[i-1].imag;
		x2 = breakpoints_55[i].real;
		y2 = breakpoints_55[i].imag;
		output_0 = y1 + (y2 - y1)*(in - x1)/(x2 - x1);
		ind = 1;
		break;
	      }
	    }
	    if (!ind) {
	      sz--;
	      output_0 = breakpoints_55[sz].imag;
	    }
	  }
	}
	{  /* star sounds.Mux1 (class CGCMux) */
	int n = output_10;
	int j = 1;
	switch(n)
	{
	    case 0:
		while (j--) {
			output_8[(output_28-(j))] = output_7;
		}
		break;
	    case 1:
		while (j--) {
			output_8[(output_28-(j))] = output_0;
		}
		break;
	    case 2:
		while (j--) {
			output_8[(output_28-(j))] = output_9;
		}
		break;
	    case 3:
		while (j--) {
			output_8[(output_28-(j))] = output_4;
		}
		break;
	    default:
		fprintf(stderr, "invalid control input %d", n);
	}
	output_28 += 1;
	if (output_28 >= 4000)
		output_28 -= 4000;
	}
	{  /* star sounds.auto-fork-node1 (class CGCFork) */
	}
}} /* end repeat, depth 2*/
	{  /* star sounds.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(1000/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_40[1000/2-1-i] 
	= (short)(output_8[(input_32-(i))]*32767.0);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_33, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_39 - info.play.samples) > 
		 2000);
        count_39 += 1000/2;
      }
    bufferptr_41 = buffer_40;
      /* Write blockSize bytes to file */
      if (write(file_33, bufferptr_41, 1000) != 1000)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	input_32 += 500;
	if (input_32 >= 4000)
		input_32 -= 4000;
	}
}} /* end repeat, depth 1*/
	{  /* star sounds.Chop1 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_1[(79-(i))] = 0;
	    }
	    else {
		output_1[(79-(i))] = output_8[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_56;for (sdfLoopCounter_56 = 0; sdfLoopCounter_56 < 80; sdfLoopCounter_56++) {
if ( getPollFlag() ) processFlags();
	{  /* star sounds.TkPlot.Y=11 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_21, 1,
			   sampleCount_18, output_1[Y_1_17]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_18 += 1.0;
if (sampleCount_18 > xMax_20) {
sampleCount_18 = xMin_19;
	  ptkPlotBreak(interp, &plotwin_21, 1);
}
	Y_1_17 += 1;
	if (Y_1_17 >= 80)
		Y_1_17 -= 80;
	}
}} /* end repeat, depth 1*/
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_33) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
