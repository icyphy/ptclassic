defstar {
	name { WaveFormCx }
	domain { SDF }
	desc {
Output a complex waveform as specified by the array state "value"
(default "(1,0) (-1,0)").
The parameters work the same way as in the WaveForm star.
This star may be used to read a file by simply setting "value" to
something of the form "< filename".
	}
	explanation {
Since this star can be used to read a complex waveform from a file,
there is no other star dedicated to this purpose.
.IE "file read"
.IE "complex waveform"
.IE "reading from a file"
.IE "halting a simulation"
	}
	version {$Id$ }
	author { J. T. Buck and E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	ccinclude { "SimControl.h" }
	output {
		name { output }
		type { complex }
	}
	defstate {
		name { value }
		type { complexarray }
		default { "(1,0) (-1,0)" }
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
	protected {
		int pos;
	}
	setup {
		pos = 0;
	}
	go {
		if (int(haltAtEnd) && (pos >= value.size()))
			SimControl::requestHalt();
		if (pos >= value.size())
			output%0 << Complex(0.0,0.0);
		else
			output%0 << Complex(value[pos++]);
		if (int(periodic) && pos >= value.size()) pos = 0;
	}
}
