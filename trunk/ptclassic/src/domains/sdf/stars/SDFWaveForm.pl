defstar {
	name { WaveForm }
	domain { SDF }
	desc {
Output a waveform as specified by the array state
"value" (default "1 -1").  Note that a file may be
specified for the state value.  If "periodic" is YES then
the waveform is periodic; otherwise zeros come after the end.
	}
	version {$Revision$ $Date$}
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
