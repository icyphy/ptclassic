defstar {
	name { WaveForm }
	domain { DE }
	desc {
Output a waveform as specified by the array state "value" (default "1 -1").
You can get periodic signals with any period, and can halt a simulation
at the end of the given waveform.  The following table summarizes the
capabilities:
.nf
haltAtEnd   periodic   period    operation
-----------------------------------------------------------------------
NO          YES        0         The period is the length of the waveform
NO          YES        N>0       The period is N
NO          NO         anything  Output the waveform once, then zeros
YES         anything   anything  Stop after outputting the waveform once
.fi
The first line of the table gives the default settings.
	}
	explanation {
This star may be used to read a file by simply setting "value" to
something of the form "< filename".  The file will be read completely
and its contents stored in an array.  The size of the array is currently
limited to 20,000 samples.
.Id "file read"
.Id "waveform from file"
.Id "reading from a file"
.Ir "halting a simulation"
.Ir "simulation, halting"
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	ccinclude { "SimControl.h" }
	input {
		name { trigger }
		type { anytype }
		desc { An event here triggers the next output. }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { value }
		type { floatarray }
		default { "1 -1" }
		desc { One period of the output waveform. }
	}
	defstate {
		name { haltAtEnd }
		type { int }
		default { "NO" }
		desc { Halt the run at the end of the given data. }
	}
	defstate {
		name { periodic }
		type { int }
		default { "YES" }
		desc { Output is periodic if "YES" (nonzero). }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc { If greater than zero, gives the period of the waveform. }
	}
	protected {
		int pos;
	}
	setup {
		pos = 0;
	}
	go {
		completionTime = arrivalTime;
		if (int(haltAtEnd) && (pos >= value.size()))
			SimControl::requestHalt();
		if (pos >= value.size()) {
			output.put(completionTime) << 0.0;
			pos++;
		} else
			output.put(completionTime) << value[pos++];
		if (int(periodic))
		    if(int(period) <= 0 && pos >= value.size()) pos = 0;
		    else if (int(period) > 0 && pos >= int(period)) pos = 0;
	}
}