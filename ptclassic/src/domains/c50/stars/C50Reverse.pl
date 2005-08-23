defstar {
	name { Reverse }
	domain { C50 }
	desc { Reverse a block of input data of length N }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 control library }
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
	setup {
                input.setSDFParams(int(N),int(N)-1);
                output.setSDFParams(int(N),int(N)-1);
        }

 	codeblock(main) {
        mar 	*,AR0				;
	lar     AR0,#$addr(input)+$val(N)	;Address last input 	=> AR0
        splk	#$addr(output),BMAR		;Address output		=> BMAR
        rpt     #$val(N)-1			;for number of N
	 bldd	*-,BMAR				;output(i) = input(N-i)
        }
        codeblock(one) {
        splk    #$addr(input),BMAR		;Address input		=> BMAR
        bldd    BMAR,#$addr(output)		;output = input
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
	            return 3;
      }
}








