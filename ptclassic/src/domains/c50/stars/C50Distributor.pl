defstar {
	name { Distributor }
	domain { C50 }
	desc { 
One input, two output distributor. 
 }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 control library }
	explanation {
.Ir "alternating data streams"
Distributes an input signal among two outputs, alternating samples.
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	outmulti {
		name {output}
		type {=input}
	}
        state {
                name {blockSize}
                type {int}
                default {1}
                desc {Number of particles in a block.}
	}
        state  {
                name { outputNum }
                type { int }
                default { 0 }
                desc { input#() }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

	setup {
                int n = output.numberPorts();
		int bs = int(blockSize);
                input.setSDFParams(n*bs,n*bs-1);
		output.setSDFParams(bs,bs-1);
        }

        codeblock (one) {
        splk    #$addr(input),BMAR        	;just move data from in to out
        bldd    BMAR,#$addr(output#1)		;
        }

 	codeblock(main) {
        lar    	AR0,#$addr(input)		;Address input		=> AR0
        mar	*,AR0				;
        }
        codeblock(loop) {
        bldd    *,#$addr(output#outputNum)	;output port(n) = input(i)
	}

        go {
                if (output.numberPorts() == 1) {
                        addCode(one);
                        return;
                }
                addCode(main);
                for (int i = 1; i <= output.numberPorts(); i++) {
                        outputNum=i;
                        addCode(loop);
                }
        }
       exectime {
                return (2*(int(output.numberPorts()))+2);
        }
}

