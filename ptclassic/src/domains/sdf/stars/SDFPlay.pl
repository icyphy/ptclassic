defstar {
	name { Play }
	domain { SDF }
	desc {
Play an input stream on the SparcStation speaker.
The "gain" state (default 1.0) multiplies the input stream
before it is mu-law compressed and written.
The inputs should be in the range of -32000.0 to 32000.0.
The file is played at a fixed sampling rate of 8000 samples/second.
When the wrapup method is called, a file of 8-bit mu-law
samples is handed to a program named "ptplay" which plays the file.
The "ptplay" program must be in your path.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
Generate a file for the SparcStation speaker and play it.  The "ptplay"
program must be on the user's path.
This star may be used on a different device provided that a "ptplay"
program is written with the following specifications:
.lp
When invoked as "ptplay filename", where filename is a sequence of
bytes representing mu-law PCM samples, the program should play
the file at 8000 samples per second.  Perhaps later this rate can
be a parameter.
.Ir "Mu law"
.Ir "ptplay program"
.Ir "audio, Sparcstation"
.Ir "Sparcstation audio"
	}
	input {
		name { input }
		type { float }
	}
	defstate {
		name { gain }
		type { float }
		default { "1.0" }
		desc { Output gain. }
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {File to save the output mu-law samples.}
	}
	protected {
		FILE *strm;	// for writing the file
		int delFile;	// if true, file needs deletion at end
		char* fileName;	// name of file to use (on heap)
	}
	hinclude { <stdio.h> }
	ccinclude { "miscFuncs.h", "paths.h", <std.h> }
	constructor {
		strm = NULL;
		delFile = FALSE;
		fileName = 0;
	}
	setup {
		// check for required program.
		if (progNotFound("ptplay",
			"Sound files cannot be played without it."))
			return;
		const char* sf = saveFile;
		// if name is empty, use a temp file.
		if (sf == NULL || *sf == 0) {
			delete [] fileName;
			fileName = tempFileName();
			delFile = TRUE;
		}
		else fileName = savestring (expandPathName(sf));
		// should check if file already exists here
		if ((strm = fopen (fileName, "w")) == NULL) {
			Error::abortRun (*this, 
				"Can't open file ", fileName);
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
		/* this code assumes a twos complement representation */
		sign = (sample >> 8) & 0x80;
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
		cmd += "ptplay ";

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
		LOG_DEL; delete [] fileName;
		fileName = 0;
	}
}
