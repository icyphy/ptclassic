/* User:      ptuser
   Date:      Fri Jan 24 16:32:44 1997
   Target:    TclTk_Target
   Universe:  sums */

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

char *name = "sums";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_34;
double xMax_35;
ptkPlotWin plotwin_36;
ptkPlotDataset plotdataset_37;
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
      int file_55;      
double value_27;
double value_43;
double value_54;
char* encodingType_65="linear16";
int sampleRate_66;
int channels_67;
char* portType_68="line_out";
double volume_69;
double balance_70;
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
	    setValue_42(dummy, interp, argc, argv)
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
		value_43 = 0.0 + (2000.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_43);
		displaySliderValue(".low", "scale_41", buf);
                return TCL_OK;
            }
	    static int
	    setValue_53(dummy, interp, argc, argv)
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
		value_54 = 0.0 + (360.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_54);
		displaySliderValue(".low", "scale_52", buf);
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
      static void audio_setup_56
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
      static void audio_control_57
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
      static void audio_gain_58
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
      static void audio_balance_59
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_72(dummy, interp, argc, argv)
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
	    volume_69 = (float) (tkvolume/100.0);
	    audio_gain_58
	      (file_55, volume_69, 0);

	    sprintf(buf, "%f", volume_69);
	    displaySliderValue(".high", "scale1_71", buf);

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
	              "scale_25",	/* name of the scale pane */
		      "Black Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_26);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_25", "440.0");
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_36);
	  xMin_34 = 0.0;
	  xMax_35 = 79.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_36,
			   &w,
			   ".win_38",
			   "Sum of Two Sinusoids",
			   "600x400+0+0",
			   "n",
			   "y",
			   0.0,79.0, -2.0,2.0,
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
			- 0.0)/(2000.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_41",	/* name of the scale pane */
		      "Blue Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_42);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_41", "440.0");
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*(0.0
			- 0.0)/(360.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_52",	/* name of the scale pane */
		      "Black Phase",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_53);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_52", "0.0");
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.0*100);
        makeScale(".high",
		  "scale1_71",
		  "Volume control",
		  tkvolume,
                  setVolume_72);
	displaySliderValue(".high", "scale1_71",
			   "0.0");
      }

}
/* main function */
int go(int argc, char *argv[]) {
double sampleCount_33;
	    double t_39;
	    double t_46;
      /* Hack for Sun only */
      unsigned count_61 = 0;
      short buffer_62[100];
      short *bufferptr_63;
      SndSoundStruct header_64;
      int counter_60;
double output_0[4001];
int output_24;
double output_1[4000];
int output_28;
double output_2[4000];
int output_29;
int Y_1_30;
int Y_2_31;
int Y_3_32;
double output_3[80];
double output_4[80];
double output_5[80];
double state_77;
double output_6;
double output_7[4001];
int output_40;
int output_8;
int input_44;
double output_9;
int input_45;
double output_10;
double state_76;
double output_11;
int output_12;
double output_13[4000];
int output_47;
int input_1_48;
int input_2_49;
int input_50;
double output_14[100];
int output_51;
double output_15;
double output_16;
double output_17;
encodingType_65="linear16";
sampleRate_66=8000;
channels_67=1;
portType_68="line_out";
volume_69=0.0;
balance_70=0.0;
value_54=0.0;
state_76=0.0;
value_43=440.0;
state_77=0.0;
value_27=440.0;
{int i; for(i=0;i<4001;i++) output_0[i] = 0.0;}
output_24 = 0;
{int i; for(i=0;i<4000;i++) output_1[i] = 0.0;}
output_28 = 0;
{int i; for(i=0;i<4000;i++) output_2[i] = 0.0;}
output_29 = 0;
Y_1_30 = 0;
Y_2_31 = 0;
Y_3_32 = 0;
sampleCount_33 = xMin_34;
{int i; for(i=0;i<80;i++) output_3[i] = 0.0;}
{int i; for(i=0;i<80;i++) output_4[i] = 0.0;}
{int i; for(i=0;i<80;i++) output_5[i] = 0.0;}
output_6 = 0.0;
{int i; for(i=0;i<4001;i++) output_7[i] = 0.0;}
output_40 = 0;
output_8 = 0;
input_44 = 4000;
output_9 = 0.0;
input_45 = 4000;
output_10 = 0.0;
output_11 = 0.0;
output_12 = 0;
{int i; for(i=0;i<4000;i++) output_13[i] = 0.0;}
output_47 = 0;
input_1_48 = 0;
input_2_49 = 0;
input_50 = 0;
{int i; for(i=0;i<100;i++) output_14[i] = 0.0;}
output_51 = 0;
output_15 = 0.0;
output_16 = 0.0;
output_17 = 0.0;
counter_60 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_55 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_64.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_64.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_64.dataSize = 200;
	  if(strcasecmp("linear16", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_64.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_64.dataFormat = 1;
	  }
	  header_64.samplingRate = 8000;
	  header_64.channelCount = 1;

	  write(file_55,(&header_64), 28);
	}
audio_setup_56(file_55, encodingType_65, sampleRate_66, channels_67);
audio_control_57(file_55, portType_68, volume_69, balance_70, 0);
audio_balance_59(file_55, balance_70);
{
	int sdfIterationCounter_79;
sdfIterationCounter_79 = 0;
while (sdfIterationCounter_79++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_73;for (sdfLoopCounter_73 = 0; sdfLoopCounter_73 < 4000; sdfLoopCounter_73++) {
if ( getPollFlag() ) processFlags();
	{  /* star sums.TkSlider1 (class CGCTkSlider) */
	output_0[output_24] = value_27;
	output_24 += 1;
	if (output_24 >= 4001)
		output_24 -= 4001;
	}
	{  /* star sums.TkSlider2 (class CGCTkSlider) */
	output_7[output_40] = value_43;
	output_40 += 1;
	if (output_40 >= 4001)
		output_40 -= 4001;
	}
}} /* end repeat, depth 1*/
    { int sdfLoopCounter_74;for (sdfLoopCounter_74 = 0; sdfLoopCounter_74 < 40; sdfLoopCounter_74++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_75;for (sdfLoopCounter_75 = 0; sdfLoopCounter_75 < 100; sdfLoopCounter_75++) {
if ( getPollFlag() ) processFlags();
	{  /* star sums.TkSlider3 (class CGCTkSlider) */
	output_17 = value_54;
	}
	{  /* star sums.Gain2 (class CGCGain) */
output_10 = 0.000785398163397447 * output_0[input_45];
	input_45 += 1;
	if (input_45 >= 4001)
		input_45 -= 4001;
	}
	{  /* star sums.Const2 (class CGCConst) */
	output_12 = 0.0;
	}
	{  /* star sums.Const1 (class CGCConst) */
	output_8 = 0.0;
	}
	{  /* star sums.Gain1 (class CGCGain) */
output_9 = 0.000785398163397447 * output_7[input_44];
	input_44 += 1;
	if (input_44 >= 4001)
		input_44 -= 4001;
	}
	{  /* star sums.Gain4 (class CGCGain) */
output_16 = -0.0174532925199433 * output_17;
	}
	{  /* star sums.Integrator2 (class CGCIntegrator) */
	    if (output_12 != 0) {
		t_46 = output_10;
	    } else {
		t_46 = output_10 +
			1.0 * state_76;
	    }
	    output_11 = t_46;
	    state_76 = t_46;
	}
	{  /* star sums.Add.input=22 (class CGCAdd) */
	output_15 = output_11 + output_16;
	}
	{  /* star sums.Integrator1 (class CGCIntegrator) */
	    if (output_8 != 0) {
		t_39 = output_9;
	    } else {
		t_39 = output_9 +
			1.0 * state_77;
	    }
	    output_6 = t_39;
	    state_77 = t_39;
	}
	{  /* star sums.Cos2 (class CGCCos) */
	output_2[output_29] = cos(output_15);
	output_29 += 1;
	if (output_29 >= 4000)
		output_29 -= 4000;
	}
	{  /* star sums.auto-fork-node2 (class CGCFork) */
	}
	{  /* star sums.Cos1 (class CGCCos) */
	output_1[output_28] = cos(output_6);
	output_28 += 1;
	if (output_28 >= 4000)
		output_28 -= 4000;
	}
	{  /* star sums.auto-fork-node1 (class CGCFork) */
	}
	{  /* star sums.Add.input=21 (class CGCAdd) */
	output_13[output_47] = output_1[input_1_48] + output_2[input_2_49];
	output_47 += 1;
	if (output_47 >= 4000)
		output_47 -= 4000;
	input_1_48 += 1;
	if (input_1_48 >= 4000)
		input_1_48 -= 4000;
	input_2_49 += 1;
	if (input_2_49 >= 4000)
		input_2_49 -= 4000;
	}
	{  /* star sums.auto-fork-node3 (class CGCFork) */
	}
	{  /* star sums.Gain3 (class CGCGain) */
output_14[output_51] = 0.4 * output_13[input_50];
	input_50 += 1;
	if (input_50 >= 4000)
		input_50 -= 4000;
	output_51 += 1;
	if (output_51 >= 100)
		output_51 -= 100;
	}
}} /* end repeat, depth 2*/
	{  /* star sums.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(200/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_62[200/2-1-i] 
	= (short)(output_14[(99-(i))]*32767.0);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_55, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_61 - info.play.samples) > 
		 1000);
        count_61 += 200/2;
      }
    bufferptr_63 = buffer_62;
      /* Write blockSize bytes to file */
      if (write(file_55, bufferptr_63, 200) != 200)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	}
}} /* end repeat, depth 1*/
	{  /* star sums.Chop1 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_3[(79-(i))] = 0;
	    }
	    else {
		output_3[(79-(i))] = output_1[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star sums.Chop2 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_4[(79-(i))] = 0;
	    }
	    else {
		output_4[(79-(i))] = output_2[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star sums.Chop3 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_5[(79-(i))] = 0;
	    }
	    else {
		output_5[(79-(i))] = output_13[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_78;for (sdfLoopCounter_78 = 0; sdfLoopCounter_78 < 80; sdfLoopCounter_78++) {
if ( getPollFlag() ) processFlags();
	{  /* star sums.TkPlot1 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_36, 1,
			   sampleCount_33, output_5[Y_1_30]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_36, 2,
			   sampleCount_33, output_3[Y_2_31]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_36, 3,
			   sampleCount_33, output_4[Y_3_32]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_33 += 1.0;
if (sampleCount_33 > xMax_35) {
sampleCount_33 = xMin_34;
	  ptkPlotBreak(interp, &plotwin_36, 1);
	  ptkPlotBreak(interp, &plotwin_36, 2);
	  ptkPlotBreak(interp, &plotwin_36, 3);
}
	Y_1_30 += 1;
	if (Y_1_30 >= 80)
		Y_1_30 -= 80;
	Y_2_31 += 1;
	if (Y_2_31 >= 80)
		Y_2_31 -= 80;
	Y_3_32 += 1;
	if (Y_3_32 >= 80)
		Y_3_32 -= 80;
	}
}} /* end repeat, depth 1*/
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_55) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
