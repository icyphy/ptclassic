ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/8/90

 Generate a file for the SparcStation speaker and play it.  The "play"
 program must be on the user's path.		'

 This star may be used on a different device provided that a "play"
 program is written with the following specifications:
 
 When invoked as "play filename", where filename is a sequence of
 bytes representing mu-law PCM samples, the program should play
 the file at 8000 samples per second.  Perhaps later this rate can
 be a parameter.

**************************************************************************/
}

defstar {
	name { Play }
	domain { SDF }
	desc {	"Play an input stream on the SparcStation speaker.  The\n"
		"'gain' state (default 1.0) multiplies the input stream\n"
		"before it is mu-law compressed and written.\n"
		"The file is played at 8000 samples/second (sorry, fixed)\n"
		"When the wrapup method is called, a file of 8-bit mu-law\n"
		"samples is handed to a program named 'play' which plays\n"
		"the file."
	}
	input {
		name { input }
		type { float }
	}
	defstate {
		name { gain }
		type { float }
		default { "1.0" }
		desc { "gain factor" }
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {"file to save xgraph input"}
	}
	protected {
		FILE *strm;
		int delFile;
		const char* fileName;
	}
	ccinclude { "miscFuncs.h" }
	constructor {
		strm = NULL;
		delFile = FALSE;
	}
	start {
		fileName = saveFile;
		if (fileName == NULL || *fileName == 0) {
			fileName = tempFileName();
			delFile = TRUE;
		}
		else fileName = savestring (expandPathName(fileName));
		// should check if file already exists here
		if ((strm = fopen (fileName, "w")) == NULL) {
			StringList msg = readFullName();
			msg += "Can't open file ";
			msg += fileName;
			Error::abortRun (msg);
		}
	}
// mulaw compression routine
	code {
		const int BIAS = 0x84;
		const int CLIP = 32635;

		static unsigned char ulaw( int sample )
	{
		static int exp_lut[256] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
                               4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                               5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                               5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                               6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                               6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                               6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                               6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                               7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};
		int sign, exponent, mantissa;
		unsigned char ulawbyte;

		/* Get the sample into sign-magnitude. */
		sign = (sample < 0);
		if ( sign != 0 ) sample = -sample;
		if ( sample > CLIP ) sample = CLIP;

		/* Convert from 16 bit linear to ulaw. */
		sample = sample + BIAS;
		exponent = exp_lut[( sample >> 7 ) & 0xFF];
		mantissa = ( sample >> ( exponent + 3 ) ) & 0x0F;
		ulawbyte = ~ ( sign | ( exponent << 4 ) | mantissa );
		return ulawbyte;
	}
	}
// go.  Does nothing if open failed.  Does mu-law compression.
	go {
		if (!strm) return;
		// add gain, cvt to integer
		int data = int(float(input%0) * double(gain));
		putc (ulaw(data), strm);
	}
// wrapup.  Does nothing if open failed, or 2nd wrapup call.
// closes file and runs the command.
	wrapup {
		if (!strm) return;
		fclose (strm);
		strm = NULL;
		StringList cmd;

		if (delFile) cmd += "( ";
		cmd += "play ";

		cmd += fileName;
		if (delFile) {
			cmd += "; /bin/rm -f ";
			cmd += fileName;
			cmd += ")";
		}
		cmd += "&";
		system (cmd);
		// no longer need to clean up
		delFile = FALSE;
	}
	destructor {
		if (strm) fclose (strm);
		if (delFile) unlink (fileName);
		delete fileName;
	}
}
