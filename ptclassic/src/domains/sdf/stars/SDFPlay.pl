defstar {
	name { Play }
	domain { SDF }
	desc {
Play an input stream on the SparcStation speaker.  The "gain" state
(default 1.0) multiplies the input stream before it is mu-law compressed.
The "gain" should chosen to scale the input values in the range -32000.0
to 32000.0.  The scaled input values are compressed from 16-bit linear
amplitude format to sign magnitude mu-law 8-bit format and written to
a file.  When the wrapup method is called, a file of 8-bit mu-law
will be played at the fixed sampling rate of 8000 samples/second by 
the "ptplay" program, which must be in your path.
	}
	version {$Id$}
	author { Joseph T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
Generate a file for the SparcStation speaker and play it.  The "ptplay"
program must be on the user's path.
This star may be used on a different device provided that a "ptplay"
program is written with the following specifications:
<p>
When invoked as "ptplay filename", where filename is a sequence of
bytes representing mu-law PCM samples, the program should play
the file at 8000 samples per second.  Perhaps later this rate can
be a parameter.
<a name="Mu law"></a>
<a name="ptplay program"></a>
<a name="audio, Sparcstation"></a>
<a name="Sparcstation audio"></a>
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
		FILE *strm;		// for writing the file
		int delFile;		// if true, file needs deletion at end
		char* fileName;		// name of file to use (on heap)
	}
	hinclude { <stdio.h> }
	ccinclude { "miscFuncs.h", "paths.h", "ptdspMuLaw.h", <std.h> }
	constructor {
		strm = 0;
		delFile = FALSE;
		fileName = 0;
	}
	setup {
		// check for required program.
		if (progNotFound("ptplay",
				 "Sound files cannot be played without it."))
			return;

		// if name is empty, use a temp file.
		const char* sf = saveFile;
		delete [] fileName;
		if (sf == 0 || *sf == 0) {
			fileName = tempFileName();
			delFile = TRUE;
		}
		else {
			fileName = expandPathName(sf);
		}

		// Truncate the file, creating a 0 length file.
		// We have to fopen with "r+" so that we can read and write
		// but the file must exist for "r+" to work.
		if ((strm = fopen(fileName, "w")) == 0) {
			Error::abortRun (*this, "Can't truncate file ",
					 fileName);
		}
		fclose(strm);

		// Reopen the file for read/write so that we can adjust
		// the header. 
		if ((strm = fopen(fileName, "r+")) == 0) {
			Error::abortRun (*this, "Can't open file ", fileName);
		}

		// Write a blank mulaw file header.
		if (Ptdsp_WriteSunMuLawHeader(strm) == 0) {
			Error::abortRun (*this, "Failed to write blank header",
					fileName);
		}
	}

// go.  Does nothing if open failed.  Does mu-law compression.
	go {
		if (!strm) return;
		// Scale by gain and convert to an integer
		int data = int(double(input%0) * double(gain));
		putc(Ptdsp_LinearToPCMMuLaw(data), strm);
	}
// wrapup.  Does nothing if open failed, or 2nd wrapup call.
// writes the header, closes file and runs the command.
	wrapup {
		
		// If the open failed, do nothing.
		if (!strm) return;


		// Update the datasize field in the mulaw file header.
		if (Ptdsp_WriteSunMuLawHeader(strm) == 0) {
			Error::abortRun (*this, "Failed to update header",
					fileName);
		}
		fclose (strm);
		strm = 0;

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
