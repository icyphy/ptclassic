defstar {
	name { ReadPCM }
	domain { SDF }
	version { $Id$ }
	author { J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }

	desc {
Read a binary mu-law encoded PCM file.  Return one sample each time.
The file format that is read is the same as the one written by the Play
star.
	}
	hinclude { "streamCompat.h" }
	ccinclude { "Scheduler.h" }

	output	{
		name { output }
		type { int }
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
		istream* input;
	}

	constructor { input = 0;   }
	destructor  { LOG_DEL; delete input;}

	code {
		// Stuff for reporting errors (should be moved to a system
		// routine):
		extern int errno;
		extern char *sys_errlist[];

		// This routine is by
		// Craig Reese: IDA/Supercomputing Research Center
		// 29 September 1989
		//
		// References:
		// 1) CCITT Recommendation G.711  (very difficult to follow)
		// 2) MIL-STD-188-113,"Interoperability and Performance
		//	Standards for Analog-to_Digital Conversion Techniques,"
		//     17 February 1987
		//
		// Input: 8 bit ulaw sample
		// Output: signed 16 bit linear sample

		int ulaw_to_linear(unsigned char ulawbyte)
		{
			static int exp_lut[8] =
			{ 0, 132, 396, 924, 1980, 4092, 8316, 16764 };
			int sign, exponent, mantissa, sample;

			ulawbyte = ~ ulawbyte;
			sign = ( ulawbyte & 0x80 );
			exponent = ( ulawbyte >> 4 ) & 0x07;
			mantissa = ulawbyte & 0x0F;
			sample = exp_lut[exponent] +
				( mantissa << ( exponent + 3 ) );
			if ( sign != 0 ) sample = -sample;
			return sample;
		}
	}
	setup
	{
		LOG_DEL; delete input; input = 0;
		// open input file
		int fd = open(expandPathName(fileName), O_RDONLY);
		if (fd < 0) {
			Error::abortRun(*this, fileName, ": can't open file: ",
					sys_errlist[errno]);
			return;
		}
		LOG_NEW; input = new ifstream(fd);
	}

	go {
		int x = 0;
		unsigned char ch;
		if (!input) {
			// nothing
		}
		else if (input->eof())
		{
			if (haltAtEnd)		// halt the run
				Scheduler::requestHalt();
			else if (periodic)	// close and re-open file
			{
				LOG_DEL; delete input; input = 0;
				int fd = open(expandPathName(fileName), 0);
				if (fd < 0) {
					Error::abortRun(*this, fileName,
					     ": can't re-open file: ",
							sys_errlist[errno]);
					return;
				}
				LOG_NEW; input = new ifstream(fd);
				input->get(ch);
				x = ulaw_to_linear(ch);
			}
		}
		else			// get next value
		{
			input->get(ch);
			x = ulaw_to_linear(ch);
		}
		output%0 << x;
	}
	
	wrapup
	{
		LOG_DEL; delete input;
		input = 0;
	}
}
