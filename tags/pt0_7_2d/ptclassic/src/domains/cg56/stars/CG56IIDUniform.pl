defstar {
	name { IIDUniform }
	domain {CG56}
	desc {
Generate pseudo-IID-uniform random variables.  The values range from
-range to range where "range" is a parameter.
	}
	version { @(#)CG56IIDUniform.pl	1.21 03/29/97 }
	author { J. Buck, ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="uniform noise"></a>
<a name="noise, uniform"></a>
This needs to be filled in.  For now, there is no seed parameter; the
default seed parameter from Gabriel is always used.  We'd really need
to use a 48-bit integer to get the same functionality.  This can be
done with g++ (type "long long"), but it isn't portable.
	}
	output {
		name { output }
		type { fix }
	}
	// FIXME: This state is incompatible with the SDF version.
	// The SDF version uses lower and upper as the states, so
	// this star is only valid when -lower = upper = range < 1.0
	defstate {
		name { range }
		type { fix }
		default { ONE }
		desc { range of random number generator is [-range,+range] }
	}
	defstate {
		name { accum }
		type { fix }
		default { 0 }
		attributes { A_SYMMETRIC|A_RAM|A_NOINIT|A_NONSETTABLE}
	}
	defstate {
		name { scaledRange }
		type { int }
		default { 0 }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	initCode {
		addCode(initSeed);
	}
	setup {
		scaledRange = int(range.asDouble() * 8388608);
	}
	go {
		addCode(randomGen);
		if (range.asDouble() < CG56_ONE) addCode(rangeScale);
		else addCode (range1);
	}
	execTime {
		return (range.asDouble() < CG56_ONE) ? 17 : 15;
	}
	// "code" to initialize the seed
	codeblock(initSeed) {
	org	x:$addr(accum)
	dc	1246684
	org	y:$addr(accum)
	dc	8123721
	org	p:
	}
	// "common" part of random number generation
	codeblock(randomGen) {
	move	#$addr(accum),r1
        move	#>10916575,y1
        move    l:(r1),x
        mpy   	x0,y1,a		#>12648789,y0
        mac  	+x1,y0,a	y1,b1
        asr   	a         	y0,b0
        mpy	x0,y0,a   	a0,x1
        addr	b,a
	}
	// case where range=1: write state and output
	codeblock(range1) {
        add	x1,a
        move    a10,l:(r1)
        move    a1,$ref(output)
	}
	// case where range < 1: write state and scale output
	codeblock(rangeScale) {
	add	x1,a		#>$val(scaledRange),x0
	move	a1,y0
	mpy	x0,y0,a		a10,l:(r1)
	move	a,$ref(output)
	}
}
