ident {
// This star produces a waveform on the output, as specified by
// a FloatArrayState.
// J. Buck
// $Id$
// Copyright (c) 1990 The Regents of the University of California.
//			All Rights Reserved.
}
defstar {
	name { WaveForm }
	domain { SDF }
	desc {
		"Output a waveform as specified by the array state\n"
		"'value' (default '1 -1').  Note that a file may be\n"
		"specified for the state value.  If 'periodic' is 'yes' the\n"
		"waveform is periodic; otherwise zeros come after the end.\n"
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { value }
		type { floatarray }
		default { "1 -1" }
		desc { "One period of the output waveform" }
	}
	defstate {
		name { periodic }
		type { string }
		default { "yes" }
		desc { "Output is periodic if 'yes'" }
	}
	protected {
		int pos, pflag;
	}
	start {
		char first = *(const char*)periodic;
		pflag = (first == 'y' || first == 'Y');
		pos = 0;
	}
	go {
		if (pos >= value.size())
			output%0 << 0.0;
		else
			output%0 << value[pos++];
		if (pflag && pos >= value.size()) pos = 0;
	}
}
