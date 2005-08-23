/* User:      ptuser
   Date:      Fri Jan 24 15:54:41 1997
   Target:    TclTk_Target
   Universe:  synth */

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

char *name = "synth";
int numIterations = -1;
static char startCmd[] = "makeRunWindow";
#include "tkMain.c"

	float outs_32[2];
double xMin_47;
double xMax_48;
ptkPlotWin plotwin_49;
ptkPlotDataset plotdataset_50;
double xMin_54;
double xMax_55;
ptkPlotWin plotwin_56;
ptkPlotDataset plotdataset_57;
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
      int file_59;      
int numOutputs_31;
double value_40;
char* encodingType_69="linear16";
int sampleRate_70;
int channels_71;
char* portType_72="line_out";
double volume_73;
double balance_74;
	    static int
            setOutputs_30(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int i;
		if(argc != numOutputs_31+1) {
                    /* Ignore -- probably premature */
                    return TCL_OK;
		}
		for(i=0; i<numOutputs_31; i++) {
                    if(sscanf(argv[i+1], "%f", &outs_32[i]) != 1) {
                        errorReport("Invalid value");
                        return TCL_ERROR;
                    }
		}
                return TCL_OK;
            }
	    static int
	    setValue_39(dummy, interp, argc, argv)
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
		value_40 = 0.0 + (10.0 - 0.0)*(position/100.0);
		/* set the value display */
		sprintf(buf, "%.4f", value_40);
		displaySliderValue(".low", "scale_38", buf);
                return TCL_OK;
            }

/*
 * This fft routine is from ~gabriel/src/filters/fft/fft.c;
 * I am unsure of the original source.  The file contains no
 * copyright notice or description.
 * The declaration is changed to the prototype form but the
 * function body is unchanged.  (J. T. Buck)
 */

#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

/*
 * Replace data by its discrete Fourier transform, if isign is
 * input as 1, or by its inverse discrete Fourier transform, if 
 * "isign" is input as -1.  "data'"is a complex array of length "nn",
 * input as a real array data[0..2*nn-1]. "nn" MUST be an integer
 * power of 2 (this is not checked for!?)
 */

static void fft_rif(data, nn, isign)
double* data;
int nn, isign;
{
	int	n;
	int	mmax;
	int	m, j, istep, i;
	double	wtemp, wr, wpr, wpi, wi, theta;
	double	tempr, tempi;

	data--;
	n = nn << 1;
	j = 1;

	for (i = 1; i < n; i += 2) {
		if(j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j+1], data[i+1]);
		}
		m= n >> 1;
		while (m >= 2 && j >m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while (n > mmax) {
		istep = 2*mmax;
		theta = -6.28318530717959/(isign*mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2) {
			for (i = m; i < n; i += istep) {
				j = i + mmax;
				tempr = wr*data[j] - wi*data[j+1];
				tempi = wr*data[j+1] + wi*data[j];
				data[j] = data[i] - tempr;
				data[j+1] = data[i+1] - tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr = (wtemp=wr)*wpr - wi*wpi+wr;
			wi = wi*wpr + wtemp*wpi + wi;
		}
		mmax = istep;
	}
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
      static void audio_setup_60
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
      static void audio_control_61
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
      static void audio_gain_62
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
      static void audio_balance_63
	(int fd, double balance) {
	audio_info_t info;

	/* Initialize the control struct */
	AUDIO_INITINFO(&info);
	/* Set balance value */
	info.play.balance = (int)(AUDIO_MID_BALANCE * (balance + 1.0));
	/* Set the device parameters */
	ioctl(fd, AUDIO_SETINFO, (caddr_t)(&info));
      }
        static int setVolume_76(dummy, interp, argc, argv)
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
	    volume_73 = (float) (tkvolume/100.0);
	    audio_gain_62
	      (file_59, volume_73, 0);

	    sprintf(buf, "%f", volume_73);
	    displaySliderValue(".high", "scale1_75", buf);

            return TCL_OK;
        }
void tkSetup() {
	    if(Tcl_Eval(interp, "global uniqueSymbol; set uniqueSymbol tkScript_29")
		!= TCL_OK)
		errorReport("Error accessing tcl");
	    if(Tcl_Eval(interp, 
                "set tkScript_29(procId) {-1}") != TCL_OK)
		errorReport("Cannot initialize state procId");
	    if(Tcl_Eval(interp, 
                "set tkScript_29(tcl_file) {$PTOLEMY/demo/eecs20/16bit/lec2/tkKeyboard.tcl}") != TCL_OK)
		errorReport("Cannot initialize state tcl_file");
	    if(Tcl_Eval(interp, 
                "set tkScript_29(synchronous) {0}") != TCL_OK)
		errorReport("Cannot initialize state synchronous");
	    if(Tcl_Eval(interp, 
                "set tkScript_29(numOutputs) {0}") != TCL_OK)
		errorReport("Cannot initialize state numOutputs");
	    if(Tcl_Eval(interp, 
                "set tkScript_29(numInputs) {0}") != TCL_OK)
		errorReport("Cannot initialize state numInputs");
	    if(Tcl_Eval(interp, 
                "set tkScript_29(temp) {0}") != TCL_OK)
		errorReport("Cannot initialize state temp");
	    Tcl_CreateCommand(interp, "tkScript_29setOutputs",
		setOutputs_30, (ClientData) 0, NULL);
	    if(Tcl_Eval(interp, "source [expandEnvVars \\$PTOLEMY/demo/eecs20/16bit/lec2/tkKeyboard.tcl]")
		!= TCL_OK)
		errorReport("Cannot source tcl script for TclScript star");
	    /* Establish the Tk window for setting the value */
	    {
	    int position;

	    /* compute the initial position of the slider */
	    position = 0.5 + 100*(0.0
			- 0.0)/(10.0 - 0.0);

	    makeScale(".low",			/* position in control panel */
	              "scale_38",	/* name of the scale pane */
		      "Spectral Richness",	/* text identifying the pane */
		      position,			/* initial position */
		      setValue_39);	/* callback procedure */
	    }

	    /* set the initial value display */
	    displaySliderValue(".low", "scale_38", "0.0");
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_49);
	  xMin_47 = 0.0;
	  xMax_48 = 63.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_49,
			   &w,
			   ".win_51",
			   "Time domain",
			   "655x310+0+0",
			   "",
			   "",
			   0.0,63.0, -2.0,2.0,
			   1,
			   64,
			   1,
			   64) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
	  {
	    /* This ridiculosity is required because Tcl scribbles on strings */
	    static char temp[100];
	    sprintf(temp,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl");
	    if(Tcl_Eval(interp, temp) != TCL_OK)
		 errorReport("Cannot source tcl script for TkXYPlot star");
	  }
	  ptkInitPlot(&plotwin_56);
	  xMin_54 = 0.0;
	  xMax_55 = 31.0;
	  if(ptkCreatePlot(interp,
			   &plotwin_56,
			   &w,
			   ".win_58",
			   "Frequency Domain",
			   "435x400-0+0",
			   "",
			   "",
			   0.0,31.0, 0.0,64.0,
			   1,
			   32,
			   1,
			   32) == 0) {
	    errorReport(ptkPlotErrorMsg());
	  }
      /* Establish the Tk window for setting the value    */
      /* "tkvolume" is  a local variable which scales the */
      /* volume from (0 - 10) to (0 - 100) for makeScale  */ 
      {
	int tkvolume;
	tkvolume = (int)(0.5*100);
        makeScale(".high",
		  "scale1_75",
		  "Volume control",
		  tkvolume,
                  setVolume_76);
	displaySliderValue(".high", "scale1_75",
			   "0.5");
      }

}
/* main function */
int go(int argc, char *argv[]) {
	    double t_33;
	    double t_35;
double sampleCount_46;
double sampleCount_53;
      /* Hack for Sun only */
      unsigned count_65 = 0;
      short buffer_66[100];
      short *bufferptr_67;
      SndSoundStruct header_68;
      int counter_64;
double output_1_0;
double output_2_1;
double output_2;
double output_3;
double output_4;
double output_5;
double output_6;
double state_79;
double output_7;
int output_8;
double output_9;
double output_10[4000];
int output_34;
double state_80;
double output_11;
int output_12;
double output_13;
double output_14;
double output_18[100];
int output_36;
int input_1_37;
double output_19;
double output_20[64];
double localData_82[128];
complex output_21[64];
int input_41;
double output_24[64];
int output_42;
int input_43;
complex output_25[64];
int output_44;
int Y_1_45;
int Y_1_52;
double output_26[32];
encodingType_69="linear16";
sampleRate_70=8000;
channels_71=1;
portType_72="line_out";
volume_73=0.5;
balance_74=0.0;
{int i; for(i=0;i<128;i++) localData_82[i]=0.0;}
value_40=0.0;
state_80=0.0;
state_79=0.0;
numOutputs_31=2;
output_1_0 = 0.0;
output_2_1 = 0.0;
	    if(Tcl_Eval(interp, "global uniqueSymbol; set uniqueSymbol tkScript_29")
		!= TCL_OK)
		errorReport("Error accessing tcl");
	    if(Tcl_Eval(interp, "tkScript_29callTcl") != TCL_OK)
		errorReport("Error invoking callTcl");
output_2 = 0.0;
output_3 = 0.0;
output_4 = 0.0;
output_5 = 0.0;
output_6 = 0.0;
output_7 = 0.0;
output_8 = 0;
output_9 = 0.0;
{int i; for(i=0;i<4000;i++) output_10[i] = 0.0;}
output_34 = 0;
output_11 = 0.0;
output_12 = 0;
output_13 = 0.0;
output_14 = 0.0;
{int i; for(i=0;i<100;i++) output_18[i] = 0.0;}
output_36 = 0;
input_1_37 = 0;
output_19 = 0.0;
{int i; for(i=0;i<64;i++) output_20[i] = 0.0;}
{int i; for(i=0;i<64;i++) output_21[i].real = output_21[i].imag = 0.0;}
input_41 = 0;
{int i; for(i=0;i<64;i++) output_24[i] = 0.0;}
output_42 = 0;
input_43 = 0;
{int i; for(i=0;i<64;i++) output_25[i].real = output_25[i].imag = 0.0;}
output_44 = 0;
Y_1_45 = 0;
sampleCount_46 = xMin_47;
Y_1_52 = 0;
sampleCount_53 = xMin_54;
{int i; for(i=0;i<32;i++) output_26[i] = 0.0;}
counter_64 = 0;
      /* Open file for writing */
      /* If the file is something other than /dev/audio, open the */
      /* and add in the header at the top */
      /* the values for the fields will be taken from the parameters */
      if ((file_59 = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1)
	{
	  perror("/dev/audio: Error opening write-file");
	  exit(1);
	}	
      /* Attach the header to the generated audio file */
      if ((strcasecmp("/dev/audio", "/dev/audio")) != 0)
	{ 
	  /* magic is a magic number used to identify the structure */
	  /* as a SNDSoundStruct */
	  header_68.magic = 0x2e736e64;
	  /* offset to the first byte of sound data */
	  header_68.dataLocation = 28;
	  /* DataSize should equal size of audio file */
	  header_68.dataSize = 200;
	  if(strcasecmp("linear16", "linear16") == 0) {
	    /* linear16 encoding  = 3 */
	    header_68.dataFormat = 3;
	  }
	  else {
	    /* ulaw8 encoding = 1 */
	    header_68.dataFormat = 1;
	  }
	  header_68.samplingRate = 8000;
	  header_68.channelCount = 1;

	  write(file_59,(&header_68), 28);
	}
audio_setup_60(file_59, encodingType_69, sampleRate_70, channels_71);
audio_control_61(file_59, portType_72, volume_73, balance_74, 0);
audio_balance_63(file_59, balance_74);
{
	int sdfIterationCounter_85;
sdfIterationCounter_85 = 0;
while (sdfIterationCounter_85++ != numIterations) {
if ( getPollFlag() ) processFlags();
while (runFlag == -1) Tk_DoOneEvent(0);
if (runFlag == 0) break;
    { int sdfLoopCounter_77;for (sdfLoopCounter_77 = 0; sdfLoopCounter_77 < 40; sdfLoopCounter_77++) {
if ( getPollFlag() ) processFlags();
        { int sdfLoopCounter_78;for (sdfLoopCounter_78 = 0; sdfLoopCounter_78 < 100; sdfLoopCounter_78++) {
if ( getPollFlag() ) processFlags();
	{  /* star synth.TkSlider1 (class CGCTkSlider) */
	output_19 = value_40;
	}
	{  /* star synth.fmGalaxy1.oscillator1.Const1 (class CGCConst) */
	output_8 = 0.0;
	}
	{  /* star synth.fmGalaxy1.oscillator2.Const1 (class CGCConst) */
	output_12 = 0.0;
	}
	{  /* star synth.fmGalaxy1.Const1 (class CGCConst) */
	output_14 = 1.0;
	}
	{  /* star synth.TclScript.input=0.output=21 (class CGCTclScript) */
	output_1_0 = outs_32[0];
	output_2_1 = outs_32[1];
	}
	{  /* star synth.fmGalaxy1.Gain2 (class CGCGain) */
output_5 = 0.314159265358979 * output_19;
	}
	{  /* star synth.fmGalaxy1.Gain1 (class CGCGain) */
output_4 = 0.000785398163397447 * output_2_1;
	}
	{  /* star synth.fmGalaxy1.auto-fork-node1 (class CGCFork) */
	}
	{  /* star synth.fmGalaxy1.oscillator1.Integrator1 (class CGCIntegrator) */
	    if (output_8 != 0) {
		t_33 = output_4;
	    } else {
		t_33 = output_4 +
			1.0 * state_79;
	    }
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if (t_33 > 3.14159265358979)
		do t_33 -= (3.14159265358979 - -3.14159265358979);
		while (t_33 > 3.14159265358979);
	    /* Take care of the bottom */
	    if (t_33 < -3.14159265358979)
		do t_33 += (3.14159265358979 - -3.14159265358979);
		while (t_33 < -3.14159265358979);
	    output_7 = t_33;
	    state_79 = t_33;
	}
	{  /* star synth.fmGalaxy1.oscillator1.Cos1 (class CGCCos) */
	output_9 = cos(output_7);
	}
	{  /* star synth.fmGalaxy1.Mpy.input=21 (class CGCMpy) */
	output_2 = output_5 * output_4;
	}
	{  /* star synth.fmGalaxy1.oscillator1.Mpy.input=21 (class CGCMpy) */
	output_6 = output_9 * output_2;
	}
	{  /* star synth.fmGalaxy1.Add.input=21 (class CGCAdd) */
	output_3 = output_6 + output_4;
	}
	{  /* star synth.fmGalaxy1.oscillator2.Integrator1 (class CGCIntegrator) */
	    if (output_12 != 0) {
		t_35 = output_3;
	    } else {
		t_35 = output_3 +
			1.0 * state_80;
	    }
	    /* Limiting is in effect */
	    /* Take care of the top */
	    if (t_35 > 3.14159265358979)
		do t_35 -= (3.14159265358979 - -3.14159265358979);
		while (t_35 > 3.14159265358979);
	    /* Take care of the bottom */
	    if (t_35 < -3.14159265358979)
		do t_35 += (3.14159265358979 - -3.14159265358979);
		while (t_35 < -3.14159265358979);
	    output_11 = t_35;
	    state_80 = t_35;
	}
	{  /* star synth.fmGalaxy1.oscillator2.Cos1 (class CGCCos) */
	output_13 = cos(output_11);
	}
	{  /* star synth.fmGalaxy1.oscillator2.Mpy.input=21 (class CGCMpy) */
	output_10[output_34] = output_13 * output_14;
	output_34 += 1;
	if (output_34 >= 4000)
		output_34 -= 4000;
	}
	{  /* star synth.auto-fork-node1 (class CGCFork) */
	}
	{  /* star synth.Mpy.input=21 (class CGCMpy) */
	output_18[output_36] = output_10[input_1_37] * output_1_0;
	output_36 += 1;
	if (output_36 >= 100)
		output_36 -= 100;
	input_1_37 += 1;
	if (input_1_37 >= 4000)
		input_1_37 -= 4000;
	}
}} /* end repeat, depth 2*/
	{  /* star synth.TkMonoOut1 (class CGCTkMonoOut) */
{
    /* convert floating-point to 16-bit linear */
    int i;
    for (i=0; i <(200/2); i++) {
      /* Convert from floating point [-1.0,1.0] to 16-bit sample */
     buffer_66[200/2-1-i] 
	= (short)(output_18[(99-(i))]*32767.0);
    }
  }
      {
	/* Hack for Sun only */
	/* Wait for samples to drain */
	audio_info_t info;
	/* Wait for some samples to drain */
	do {
	  /* the cast below is to prevent warnings */
	  ioctl(file_59, AUDIO_GETINFO, (caddr_t)(&info));
        } while ((int) (count_65 - info.play.samples) > 
		 1000);
        count_65 += 200/2;
      }
    bufferptr_67 = buffer_66;
      /* Write blockSize bytes to file */
      if (write(file_59, bufferptr_67, 200) != 200)
	{
	  perror("/dev/audio: Error writing to file.");
	  exit(1);
	}
	}
}} /* end repeat, depth 1*/
	{  /* star synth.Chop1 (class CGCChop) */
	int i;
	int hiLim = 63, inputIndex = 3936, loLim = 0;
	for (i = 0; i < 64; i++) {
	    if (i > hiLim || i < loLim) {
		output_20[(63-(i))] = 0;
	    }
	    else {
		output_20[(63-(i))] = output_10[(3999-(inputIndex))];
		inputIndex++;
            }
        }
	}
	{  /* star synth.spectrum1.Fork.output=21 (class CGCFork) */
	}
    { int sdfLoopCounter_81;for (sdfLoopCounter_81 = 0; sdfLoopCounter_81 < 64; sdfLoopCounter_81++) {
if ( getPollFlag() ) processFlags();
	{  /* star synth.spectrum1.FloatToCx1 (class CGCFloatToCx) */
	int i = 0;
	for ( ; i < 1; i++) {
		output_25[(output_44-(i))].real = output_20[(input_43-(i))];
		output_25[(output_44-(i))].imag = 0;
	}
	input_43 += 1;
	if (input_43 >= 64)
		input_43 -= 64;
	output_44 += 1;
	if (output_44 >= 64)
		output_44 -= 64;
	}
	{  /* star synth.spectrum1.TkPlot.Y=11 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_49, 1,
			   sampleCount_46, output_20[Y_1_45]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_46 += 1.0;
if (sampleCount_46 > xMax_48) {
sampleCount_46 = xMin_47;
	  ptkPlotBreak(interp, &plotwin_49, 1);
}
	Y_1_45 += 1;
	if (Y_1_45 >= 64)
		Y_1_45 -= 64;
	}
}} /* end repeat, depth 1*/
	{  /* star synth.spectrum1.FFTCx1 (class CGCFFTCx) */
	int i, j = 0;
	for (i = 64 - 1; i >= 0; i--) {
		localData_82[j++] = output_25[(63-(i))].real;
		localData_82[j++] = output_25[(63-(i))].imag;
	}
        for (i = 64 ; i < 64 ; i ++) {
		localData_82[j++] = 0.0;
		localData_82[j++] = 0.0;
	}		
	fft_rif (localData_82,64, 1);
	j = 0;
	for (i = 64 - 1; i >= 0; i--) {
		output_21[(63-(i))].real = localData_82[j++];
		output_21[(63-(i))].imag = localData_82[j++];
	}
	}
    { int sdfLoopCounter_83;for (sdfLoopCounter_83 = 0; sdfLoopCounter_83 < 64; sdfLoopCounter_83++) {
if ( getPollFlag() ) processFlags();
	{  /* star synth.spectrum1.CxToFloat1 (class CGCCxToFloat) */
	int i = 0;
	double p, q;
	for ( ; i < 1; i++) {
		p = output_21[(input_41-(i))].real;
		q = output_21[(input_41-(i))].imag;
		output_24[(output_42-(i))] = sqrt(p*p + q*q);
	}
	input_41 += 1;
	if (input_41 >= 64)
		input_41 -= 64;
	output_42 += 1;
	if (output_42 >= 64)
		output_42 -= 64;
	}
}} /* end repeat, depth 1*/
	{  /* star synth.spectrum1.Chop1 (class CGCChop) */
	int i;
	int hiLim = 31, inputIndex = 32, loLim = 0;
	for (i = 0; i < 32; i++) {
	    if (i > hiLim || i < loLim) {
		output_26[(31-(i))] = 0;
	    }
	    else {
		output_26[(31-(i))] = output_24[(63-(inputIndex))];
		inputIndex++;
            }
        }
	}
    { int sdfLoopCounter_84;for (sdfLoopCounter_84 = 0; sdfLoopCounter_84 < 32; sdfLoopCounter_84++) {
if ( getPollFlag() ) processFlags();
	{  /* star synth.spectrum1.TkPlot.Y=12 (class CGCTkPlot) */
	  if (ptkPlotPoint(interp, &plotwin_56, 1,
			   sampleCount_53, output_26[Y_1_52]) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
sampleCount_53 += 1.0;
if (sampleCount_53 > xMax_55) {
sampleCount_53 = xMin_54;
	  ptkPlotBreak(interp, &plotwin_56, 1);
}
	Y_1_52 += 1;
	if (Y_1_52 >= 32)
		Y_1_52 -= 32;
	}
}} /* end repeat, depth 1*/
}} /* MAIN LOOP END */
runFlag = 0;
      /* Close file */
      if (close(file_59) != 0) {
	perror("/dev/audio: Error closing file");
	exit(1);
      }

return 1;
}
