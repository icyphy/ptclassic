defstar {
	name { IIDUniform }
	domain {CG56}
	desc {
Generate pseudo-IID-uniform random variables.  The values range from
-range to range where range is a parameter.
	}
	version { $Id$ }
	author { J. Buck, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This needs to be filled in.  For now, there is no seed parameter; the
default seed parameter from Gabriel is always used.  We'd really need
to use a 48-bit integer to get the same functionality.  This can be
done with g++ (type "long long"), but it isn't portable.
	}
	output {
		name { output }
		type { fix }
	}
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
		gencode(initSeed);
	}
	start {
		scaledRange = int(double(range) * 8388608);
	}
	go {
		gencode(random);
		if (double(range) < CG56_ONE) gencode(rangeScale);
		else gencode (range1);
	}
	execTime {
		return (double(range) < CG56_ONE) ? 17 : 15;
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
	codeblock(random) {
        move	#>10916575,y1
        move    l:$addr(accum),x
        mpy   	x0,y1,a		#>12648789,y0
        mac  	+x1,y0,a	y1,b1
        asr   	a         	y0,b0
        mpy	x0,y0,a   	a0,x1
        addr	b,a
	}
	// case where range=1: write state and output
	codeblock(range1) {
        add	x1,a
        move    a10,l:$addr(accum)
        move    a1,$ref(output)
	}
	// case where range < 1: write state and scale output
	codeblock(rangeScale) {
	add	x1,a		#>$val(scaledRange),x0
	move	a1,y0
	mpy	x0,y0,a		a10,l:$addr(accum)
	move	a,$ref(output)
	}
}
