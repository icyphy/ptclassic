/* User:      ptuser
   Date:      Fri Jan 24 16:41:23 1997
   Target:    TclTk_Target
   Universe:  quant_sines */

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

char *name = "quant_sines";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

double xMin_22;
double xMax_23;
ptkPlotWin plotwin_24;
ptkPlotDataset plotdataset_25;
	float outs_36[1];
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
      int file_41;      
double value_31;
int numOutputs_35;
char* encodingType_51="linear16";
int sampleRate_52;
int channels_53;
char* portType_54="line_out";
double volume_55;
double balance_56;
	    static int
	    setValue_30(dummy, interp, argc, argv)
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
		value_31 = 0.0 + (2000.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_31);
		displaySliderValue(".low", "scale_29", buf);
                return TCL_OK;
            }
	    static int
            setOutputs_34(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int i;
		if(argc != numOutputs_35+1) {
                    /* Ignore -- probably premature */
                    return TCL_OK;
		}
		for(i=0; i<numOutputs_35; i++) {
                    if(sscanf(argv[i+1], "%f", &outs_36[i]) != 1) {
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
      static void audio_setup_42
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
      static void audio_control_43
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
      static void audio_gain_44
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
      static void audio_balance_45
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_58(dummy, interp, argc, argv)
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
	    volume_55 = (float) (tkvolume/100.0);
	    audio_gain_44
	      (file_41, volume_55, 0);

	    sprintf(buf, "%f", volume_55);
	    displaySliderValue(".high", "scale1_57", buf);

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
	  ptkInitPlot(&plotwin_24);
	  xMin_22 = 0.0;
	  xMax_23 = 79.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_24,
			   &w,
			   ".win_26",
			   "Sum of Two Sinusoids",
			   "600x400+0+0",
			   "n",
			   "y",
			   0.0,79.0, -2.0,2.0,
			   2,
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
	              "scale_29",	/* name of the scale pane */
		      "Blue Frequency",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_30);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_29", "440.0");
	    if(Tcl_Eval(interp, "global uniqueSymbol; set uniqueSymbol tkScript_33")
		!= TCL_OK)
		errorReport("Error accessing tcl");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(procId) {-1}") != TCL_OK)
		errorReport("Cannot initialize state procId");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(tcl_file) {$PTOLEMY/src/domains/cgc/tcltk/stars/tkRadioButton.tcl}") != TCL_OK)
		errorReport("Cannot initialize state tcl_file");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(synchronous) {0}") != TCL_OK)
		errorReport("Cannot initialize state synchronous");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(numOutputs) {0}") != TCL_OK)
		errorReport("Cannot initialize state numOutputs");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(numInputs) {0}") != TCL_OK)
		errorReport("Cannot initialize state numInputs");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(temp) {0}") != TCL_OK)
		errorReport("Cannot initialize state temp");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(identifier) {Number of bits:}") != TCL_OK)
		errorReport("Cannot initialize state identifier");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(pairs) {\"1 0.9999\" \"2 1.9999\" \"3 3.9999\" \"4 7.9999\" \"5 15.999\"}") != TCL_OK)
		errorReport("Cannot initialize state pairs");
	    if(Tcl_Eval(interp, 
                "set tkScript_33(initialChoice) {2}") != TCL_OK)
		errorReport("Cannot initialize state initialChoice");
	    Tcl_CreateCommand(interp, "tkScript_33setOutputs",
		setOutputs_34, (ClientData) 0, NULL);
	    if(Tcl_Eval(interp, "source [expandEnvVars \\$PTOLEMY/src/domains/cgc/tcltk/stars/tkRadioButton.tcl]")
		!= TCL_OK)
		errorReport("Cannot source tcl script for TclScript star");
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.5*100);
        makeScale(".high",
		  "scale1_57",
		  "Volume control",
		  tkvolume,
                  setVolume_58);
	displaySliderValue(".high", "scale1_57",
			   "0.5");
      }

}
/* main function */
int go(int argc, char *argv[]) {
double sampleCount_21;
	    double t_27;
	    int siz_39;
      /* Hack for Sun only */
      unsigned count_47 = 0;
      short buffer_48[50];
      short *bufferptr_49;
      SndSoundStruct header_50;
      int counter_46;
double output_0[4000];
int output_18;
int Y_1_19;
int Y_2_20;
double output_1[80];
double output_2[80];
double state_62;
double output_3;
double output_4[4001];
int output_28;
int output_5;
int input_32;
double output_6;
double output_1_7;
double output_8;
int input_1_37;
double output_9;
double output_10[4000];
int output_38;
double thresholds_63[33];
double levels_64[34];
double output_11;
int input_40;
encodingType_51="linear16";
sampleRate_52=8000;
channels_53=1;
portType_54="line_out";
volume_55=0.5;
balance_56=0.0;
thresholds_63[0]=-16.0;
thresholds_63[1]=-15.0;
thresholds_63[2]=-14.0;
thresholds_63[3]=-13.0;
thresholds_63[4]=-12.0;
thresholds_63[5]=-11.0;
thresholds_63[6]=-10.0;
thresholds_63[7]=-9.0;
thresholds_63[8]=-8.0;
thresholds_63[9]=-7.0;
thresholds_63[10]=-6.0;
thresholds_63[11]=-5.0;
thresholds_63[12]=-4.0;
thresholds_63[13]=-3.0;
thresholds_63[14]=-2.0;
thresholds_63[15]=-1.0;
thresholds_63[16]=0.0;
thresholds_63[17]=1.0;
thresholds_63[18]=2.0;
thresholds_63[19]=3.0;
thresholds_63[20]=4.0;
thresholds_63[21]=5.0;
thresholds_63[22]=6.0;
thresholds_63[23]=7.0;
thresholds_63[24]=8.0;
thresholds_63[25]=9.0;
thresholds_63[26]=10.0;
thresholds_63[27]=11.0;
thresholds_63[28]=12.0;
thresholds_63[29]=13.0;
thresholds_63[30]=14.0;
thresholds_63[31]=15.0;
thresholds_63[32]=16.0;
levels_64[0]=-16.5;
levels_64[1]=-15.5;
levels_64[2]=-14.5;
levels_64[3]=-13.5;
levels_64[4]=-12.5;
levels_64[5]=-11.5;
levels_64[6]=-10.5;
levels_64[7]=-9.5;
levels_64[8]=-8.5;
levels_64[9]=-7.5;
levels_64[10]=-6.5;
levels_64[11]=-5.5;
levels_64[12]=-4.5;
levels_64[13]=-3.5;
levels_64[14]=-2.5;
levels_64[15]=-1.5;
levels_64[16]=-0.5;
levels_64[17]=0.5;
levels_64[18]=1.5;
levels_64[19]=2.5;
levels_64[20]=3.5;
levels_64[21]=4.5;
levels_64[22]=5.5;
levels_64[23]=6.5;
levels_64[24]=7.5;
levels_64[25]=8.5;
levels_64[26]=9.5;
levels_64[27]=10.5;
levels_64[28]=11.5;
levels_64[29]=12.5;
levels_64[30]=13.5;
levels_64[31]=14.5;
levels_64[32]=15.5;
levels_64[33]=16.5;
numOutputs_35=1;
value_31=440.0;
state_62=0.0;
{int i; for(i=0;i<4000;i++) output_0[i] = 0.0;}
output_18 = 0;
Y_1_19 = 0;
Y_2_20 = 0;
sampleCount_21 = xMin_22;
{int i; for(i=0;i<80;i++) output_1[i] = 0.0;}
{int i; for(i=0;i<80;i++) output_2[i] = 0.0;}
output_3 = 0.0;
{int i; for(i=0;i<4001;i++) output_4[i] = 0.0;}
output_28 = 0;
output_5 = 0;
input_32 = 4000;
output_6 = 0.0;
output_1_7 = 0.0;
	    if(Tcl_Eval(interp, "global uniqueSymbol; set uniqueSymbol tkScript_33")
		!= TCL_OK)
		errorReport("Error accessing tcl");
	    if(Tcl_Eval(interp, "tkScript_33callTcl") != TCL_OK)
		errorReport("Error invoking callTcl");
output_8 = 0.0;
input_1_37 = 0;
output_9 = 0.0;
{int i; for(i=0;i<4000;i++) output_10[i] = 0.0;}
output_38 = 0;
output_11 = 0.0;
input_40 = 49;
counter_46 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_41 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_50.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_50.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_50.dataSize = 100;
	  if(strcasecmp("linear16", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_50.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_50.dataFormat = 1;
	  }
	  header_50.samplingRate = 8000;
	  header_50.channelCount = 1;

	  write(file_41,(&header_50), 28);
	}
audio_setup_42(file_41, encodingType_51, sampleRate_52, channels_53);
audio_control_43(file_41, portType_54, volume_55, balance_56, 0);
audio_balance_45(file_41, balance_56);
{
	int sdfIterationCounter_66;
sdfIterationCounter_66 = 0;
while (sdfIterationCounter_66++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_59;for (sdfLoopCounter_59 = 0; sdfLoopCounter_59 < 4000; sdfLoopCounter_59++) {
if ( getPollFlag() ) processFlags();
	{  /* star quant_sines.TkSlider1 (class CGCTkSlider) */
	output_4[output_28] = value_31;
	output_28 += 1;
	if (output_28 >= 4001)
		output_28 -= 4001;
	}
}} /* end repeat, depth 1*/
    { int sdfLoopCounter_60;for (sdfLoopCounter_60 = 0; sdfLoopCounter_60 < 80; sdfLoopCounter_60++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_61;for (sdfLoopCounter_61 = 0; sdfLoopCounter_61 < 50; sdfLoopCounter_61++) {
if ( getPollFlag() ) processFlags();
	{  /* star quant_sines.TkRadioButton1 (class CGCTkRadioButton) */
	output_1_7 = outs_36[0];
	}
	{  /* star quant_sines.auto-fork-node3 (class CGCFork) */
	}
	{  /* star quant_sines.Reciprocal1 (class CGCReciprocal) */
	output_9 = 1/output_1_7;
	}
	{  /* star quant_sines.Const1 (class CGCConst) */
	output_5 = 0.0;
	}
	{  /* star quant_sines.Gain1 (class CGCGain) */
output_6 = 0.000785398163397447 * output_4[input_32];
	input_32 += 1;
	if (input_32 >= 4001)
		input_32 -= 4001;
	}
	{  /* star quant_sines.Integrator1 (class CGCIntegrator) */
	    if (output_5 != 0) {
		t_27 = output_6;
	    } else {
		t_27 = output_6 +
			1.0 * state_62;
	    }
	    output_3 = t_27;
	    state_62 = t_27;
	}
	{  /* star quant_sines.Cos1 (class CGCCos) */
	output_0[output_18] = cos(output_3);
	output_18 += 1;
	if (output_18 >= 4000)
		output_18 -= 4000;
	}
	{  /* star quant_sines.auto-fork-node1 (class CGCFork) */
	}
	{  /* star quant_sines.Mpy.input=21 (class CGCMpy) */
	output_8 = output_0[input_1_37] * output_1_7;
	input_1_37 += 1;
	if (input_1_37 >= 4000)
		input_1_37 -= 4000;
	}
	{  /* star quant_sines.Quant1 (class CGCQuant) */
		float in;
		int lo, hi, mid;
		siz_39 = 33;
		in = output_8;
		lo = 0;
		hi = siz_39;
		mid = (hi+lo)/2;
		do {
			if (in <= thresholds_63[mid]) {
				hi = mid;
			} else {
				lo = mid+1;
			}
			mid = (hi+lo)/2;
		} while (mid < siz_39 && hi > lo);

		/* now in is <= thresholds[mid] but > all smaller ones. */
		/* (where thresholds[siz_39] is infinity) */

		output_11 = levels_64[mid];
	}
	{  /* star quant_sines.Mpy.input=22 (class CGCMpy) */
	output_10[output_38] = output_11 * output_9;
	output_38 += 1;
	if (output_38 >= 4000)
		output_38 -= 4000;
	}
	{  /* star quant_sines.auto-fork-node2 (class CGCFork) */
	}
}} /* end repeat, depth 2*/
	{  /* star quant_sines.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(100/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_48[100/2-1-i] 
	= (short)(output_10[(input_40-(i))]*32767.0);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_41, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_47 - info.play.samples) > 
		 1000);
        count_47 += 100/2;
      }
    bufferptr_49 = buffer_48;
      /* Write blockSize bytes to file */
      if (write(file_41, bufferptr_49, 100) != 100)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	input_40 += 50;
	if (input_40 >= 4000)
		input_40 -= 4000;
	}
}} /* end repeat, depth 1*/
	{  /* star quant_sines.Chop1 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_1[(79-(i))] = 0;
	    }
	    else {
		output_1[(79-(i))] = output_0[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star quant_sines.Chop2 (class CGCChop) */
	int i;
	int hiLim = 79, inputIndex = 3920, loLim = 0;
	for (i = 0; i < 80; i++) {
	    if (i > hiLim || i < loLim) {
		output_2[(79-(i))] = 0;
	    }
	    else {
		output_2[(79-(i))] = output_10[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_65;for (sdfLoopCounter_65 = 0; sdfLoopCounter_65 < 80; sdfLoopCounter_65++) {
if ( getPollFlag() ) processFlags();
	{  /* star quant_sines.TkPlot1 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_24, 1,
			   sampleCount_21, output_1[Y_1_19]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	  if (ptkPlotPoint(interp, &plotwin_24, 2,
			   sampleCount_21, output_2[Y_2_20]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_21 += 1.0;
if (sampleCount_21 > xMax_23) {
sampleCount_21 = xMin_22;
	  ptkPlotBreak(interp, &plotwin_24, 1);
	  ptkPlotBreak(interp, &plotwin_24, 2);
}
	Y_1_19 += 1;
	if (Y_1_19 >= 80)
		Y_1_19 -= 80;
	Y_2_20 += 1;
	if (Y_2_20 >= 80)
		Y_2_20 -= 80;
	}
}} /* end repeat, depth 1*/
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_41) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
