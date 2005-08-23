defstar {
	name { Limit }
	domain { CG56 }
	desc { Hard Limiter }
	version { @(#)CG56Limit.pl	1.13 06 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="hard limiter"></a>
The star hard limits input samples to keep the range of 
(<i>bottom, top</i>).
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
	constructor {
		noInternalState();
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
