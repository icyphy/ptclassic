defstar {
	name { Cx_Add }
	domain { CG56 }
	desc { Two input complex adder. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Two complex numbers are added.
	}
	inmulti {
		name {input}
		type {FIX}
	}
	outmulti {
		name {output}
		type {FIX}
	}
	codeblock (cx_multblock) {
	move	$ref(input#1),x0
	move	$ref(input#3),a	
	add	x0,a	$ref(input#2),x1
	move	$ref(input#4),b
 	add	x1,b	a,$ref(output#1)
 	move 	b,$ref(output#2)
	
	}
 	go {
		gencode(cx_multblock);
  	}
	exectime {
	 	return 12; 
	}
}
