defstar {
	name { Distributor }
	domain { CG56 }
	desc { 
One input, two output distributor. 
 }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Distributes an input signal among two outputs, alternating samples.
	}
	input {
		name {input}
		type {FIX}
	}
	outmulti {
		name {output}
		type {FIX}
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

        state  {
                name { runtimeVal }
                type { int }
                default { 0 }
                desc { runtime value }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

	start {
                int n = output.numberPorts();
                input.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
                MPHIter nexto(output);
                PortHole* p;
                while((p = nexto++) != 0)
               ((SDFPortHole*)p)->setSDFParams(int(blockSize),int(blockSize)-1);
        }

        codeblock (one) {
        move    $ref(input),x0        ; just move data from in to out
        move    x0,$ref(output#1)
        }

 	codeblock(main) {
        move    #$addr(input),r1
        nop
        }
        codeblock(loop) {
        clr     a               x:(r1)+,x0
        move    x0,$ref(output#outputNum)
	}

        go {
                if (output.numberPorts() == 1) {
                        gencode(one);
                        return;
                }
                gencode(main);
                for (int i = 1; i <= output.numberPorts(); i++) {
                        outputNum=i;
                        gencode(loop);
                }
                runtimeVal=output.numberPorts();
        }
       exectime {
                return (2*(int(runtimeVal))+1) ;
        }
}

