defstar {
	name { WaveForm }
	domain { SDF }
	desc {
Output a waveform as specified by the array state "value" (default "1 -1").
To get a periodic waveform, set "periodic" to YES.  Then the value list
will be cyclically repeated.  If "periodic" is not YES, then the value
list is output only once, and 0.0 values are output subsequently.
This star may be used to read a file by simply setting "value" to
something of the form "< filename".
	}
	explanation {
Since this star can be used to read a waveform from a file, there is no
other star dedicated to this purpose.
.IE "file read"
.IE "waveform from file"
.IE "reading from a file"
	}
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
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
		name { periodic }
		type { int }
		default { "YES" }
		desc { Output is periodic if "YES" (nonzero). }
	}
	protected {
		int pos;
	}
	start {
		pos = 0;
	}
	go {
		if (pos >= value.size())
			output%0 << 0.0;
		else
			output%0 << value[pos++];
		if (int(periodic) && pos >= value.size()) pos = 0;
	}
}
