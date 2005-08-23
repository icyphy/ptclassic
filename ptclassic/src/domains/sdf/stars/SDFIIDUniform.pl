defstar {
	name { IIDUniform }
	domain { SDF }
	desc {
Generate an identically independently distributed uniformly distributed
pseudo-random process.  Output is uniformly distributed between "lower"
(default 0) and "upper" (default 1).
	}
	version { @(#)SDFIIDUniform.pl	1.16	10/01/96 }
	author { D. G. Messerschmitt }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
This Star uses the GNU library &lt;Uniform.h&gt;.
<a name="uniform noise"></a>
<a name="noise, uniform"></a>
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { lower }
		type { float }
		default { "0.0" }
		desc { Lower limit. }
	}
	defstate {
		name { upper }
		type { float }
		default { "1.0" }
		desc { Upper limit. }
	}
	hinclude { <Uniform.h> }
	ccinclude { <ACG.h> }
	protected {
		Uniform *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
	constructor {
		random = 0;
	}
	destructor {
		LOG_DEL; delete random;
	}
	setup {
		LOG_DEL; delete random;
		LOG_NEW; random = new Uniform(double(lower),double(upper),gen);
	}
        go {
		output%0 << (*random)();
	}
}

