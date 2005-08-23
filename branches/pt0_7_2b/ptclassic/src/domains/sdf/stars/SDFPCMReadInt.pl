defstar {
	name { PCMReadInt }
	domain { SDF }
	version { @(#)SDFPCMReadInt.pl	1.19	06/19/98 }
	author { Joseph T. Buck }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	desc {
Read a binary mu-law encoded PCM file.  Return one sample on each firing.
The file format that is read is the same as the one written by the Play star.
The simulation can be halted on end-of-file, or the file contents can be
periodically repeated, or the file contents can be padded with zeros.
	}
	hinclude { "pt_fstream.h" }
	ccinclude { "SimControl.h", "ptdspMuLaw.h" }

	output	{
		name { output }
		type { int }
		descriptor { 16-bit sample }
	}
	defstate {
		name { fileName }
		type { string }
		default { "/dev/null" }
		descriptor { Input file. }
	}
	defstate {
		name { haltAtEnd }
		type { int }
		default { "NO" }
		descriptor { Halt the run at the end of the input file. }
	}

	defstate {
		name { periodic }
		type { int }
		default { "YES" }
		descriptor { Make output periodic or zero-padded. }
	}

	protected {
		pt_ifstream input;
	}

	destructor  { input.close(); }

	setup {
		input.close();
		// open input file
		input.open(fileName, ios::in|ios::binary );
		if (!input) {
			Error::abortRun(*this, "can't open file ", fileName);
		}
	}

	go {
		int x = 0;
                // initialize ch to 0, otherwise we could get garbage
                // on EOF:sdf/263: SDFReadPCM does an uninitialized memory read
		unsigned char ch = 0;
		if (input.eof()) {
			if (haltAtEnd) {	// halt the run
				SimControl::requestHalt();
				return;
			}
			else if (periodic) {	// close and re-open file
				input.close();
                                input.open(fileName, ios::in|ios::binary );
				if (!input) {
					Error::abortRun(*this,
					     "can't re-open file ",
					     fileName);
					return;
				}
				input.get(ch);
				x = Ptdsp_PCMMuLawToLinear(ch);
			}
		}
		else if (input) {		// get next value
			input.get(ch);
			x = Ptdsp_PCMMuLawToLinear(ch);
		}
		else {
			Error::abortRun(*this, "error in input file");
			return;
		}
		output%0 << x;
	}

	wrapup { input.close(); }
}
