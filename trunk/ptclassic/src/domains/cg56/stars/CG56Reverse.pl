defstar {
	name { Reverse }
	domain { CG56 }
	desc { Reverse a block of input data of length N }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
	explanation { Reverse a block of input data of length N }

        input {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}
        state {
                name {N}
                type {int}
                default {64}
		desc {Number of particles read and written.}
	}
	start {
                input.setSDFParams(int(N),int(N)-1);
                output.setSDFParams(int(N),int(N)-1);
        }

 	codeblock(main) {
        move    #<$addr(in)+$val(N),r0
        move    #<$addr(out),r1
        do      #$val(N),$label(loop)
        move    x:-(r0),a
        move    a,x:(r1)+
$label(loop)
        }
        codeblock(one) {
        move    $ref(input),a
        move    a,$ref(output)
	}

        go {
                if(N>1)
	            addCode(main);
		else
	            addCode(one);
        }
        exectime {
                if(int(N)>1)
	            return 2*int(N)+4;
		else
	            return 2;
      }
}

