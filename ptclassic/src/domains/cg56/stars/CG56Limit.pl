defstar {
	name { Limit }
	domain { CG56 }
	desc { Hard Limiter }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.Id "hard limiter"
The star hard limits input samples to keep the range of 
(\fIbottom, top\fR).
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {bottom}
		type {FIX}
		default {0}
		desc {Lower limit of the output.}
	}
	state {
		name {top}
		type {FIX}
		default {ONE}
		desc {Upper limit of the output.}
	}


	codeblock(limitblock) {
	move	#$val(top),x0
	move	$ref(input),a
	cmp	x0,a
	jpl	$label(end)
	move	#$val(bottom),x0
	cmp	x0,a
 	jmi	$label(end)
	move	a,x0
$label(end)
        move	x0,$ref(output)
        }

 	go {
 		addCode(limitblock);
 	}

	exectime {
		return 10;
 	}
}
