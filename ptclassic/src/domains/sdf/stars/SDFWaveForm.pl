defstar {
	name { WaveForm }
	domain { SDF }
	desc {
Output a waveform as specified by the array state "value" (default "1 -1").
You can get periodic signals with any period, and can halt a simulation
at the end of the given waveform.  The following table summarizes the
capabilities:
<pre>
haltAtEnd   periodic   period    operation
-----------------------------------------------------------------------
NO          YES        0         The period is the length of the waveform
NO          YES        N>0       The period is N
NO          NO         anything  Output the waveform once, then zeros
YES         anything   anything  Stop after outputting the waveform once
</pre>
The first line of the table gives the default settings.
This star may be used to read a file by simply setting
"value" to something of the form "&lt; filename".
	}
	htmldoc {
This star may be used to read a file by simply setting "value" to
something of the form "&lt; filename".  The file will be read completely
and its contents stored in an array.  The size of the array is currently
limited to 20,000 samples.  To read longer files, use the 
<tt>ReadFile</tt>
star.  This latter star reads one sample at a time, and hence also
uses less storage.
<a name="file read"></a>
<a name="waveform from file"></a>
<a name="reading from a file"></a>
<a name="halting a simulation"></a>
<a name="simulation, halting"></a>
	}
	version { @(#)SDFWaveForm.pl	2.18	10/07/96 }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	ccinclude { "SimControl.h" }
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
		if (int(haltAtEnd) && (pos >= value.size()))
			SimControl::requestHalt();
		if (pos >= value.size()) {
			output%0 << 0.0;
			pos++;
		} else
			output%0 << value[pos++];
		if (int(periodic))
		    if(int(period) <= 0 && pos >= value.size()) pos = 0;
		    else if (int(period) > 0 && pos >= int(period)) pos = 0;
	}
}
