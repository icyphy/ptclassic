defstar {
	name { IIDGaussian }
	domain { SDF }
	desc {
Generate an identically independently distributed white Gaussian
pseudo-random process with mean "mean" (default 0) and variance
"variance" (default 1).
	}
	version {@(#)SDFIIDGaussian.pl	1.16 10/01/96}
	author { D. G. Messerschmitt }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
This Star uses the GNU library &lt;Normal.h&gt;
<a name="Gaussian noise"></a>
<a name="noise, Gaussian"></a>
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { mean }
		type { float }
		default { "0.0" }
		desc {  Mean of distribution. }
	}
	defstate {
		name { variance }
		type { float }
		default { "1.0" }
		desc { Variance of distribution. }
	}
	hinclude { <Normal.h> }
	ccinclude { <ACG.h> }
	protected {
		Normal *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
	constructor {
		random = NULL;
	}
	destructor {
		LOG_DEL; delete random;
	}
	setup {
		LOG_DEL; delete random;
		LOG_NEW; random = new Normal(double(mean),double(variance),gen);
	}
        go {
		output%0 << (*random)();
	}
}

