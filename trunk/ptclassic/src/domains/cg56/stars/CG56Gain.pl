defstar {
	name { FixGain }
	domain { CG56 }
	desc {
The output is set to the input multiplied by a gain term.  The gain must
be in [-1,1].
}
	version { $Id$ }
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
We make no attempt to be heroic and handle all cases as was done with Gabriel.
The only special case is for gain 1.
	}
	execTime {
		return double(gain) == 1.0 ? 2 : 5;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	defstate {
		name {gain}
		type {FIX}
		default {0.5}
		desc {Gain value}
	}
	codeblock (std) {
	move	$ref(input),x1
	move	#$val(gain),y1
	mpyr	x1,y1,a
	rnd	a
	move	a,$ref(output)
	}
	codeblock (unity) {
	move	$ref(input),a
	move	a,$ref(output)
	}
	go {
		if (double(gain) == double(ONE)) gencode(unity);
		else gencode(std);
	}
}
