defstar {
	name { Distributor }
	domain { CG56 }
	desc { 
One input, two output distributor. 
 }
	version { @(#)CG56Distributor.pl	1.18 01 Oct 1996 }
	author { Jose Luis Pino & Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="alternating data streams"></a>
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
	setup {
                int n = output.numberPorts();
		int bs = int(blockSize);
                input.setSDFParams(n*bs,n*bs-1);
		output.setSDFParams(bs,bs-1);
        }
	constructor {
		noInternalState();
	}

 	codeblock(loadInputAddress) {
        move    #>$addr(input),r1
	nop
        }

        codeblock(moveBlock,"int outputNum") {
@(int(blockSize)!= 1 ? "\n\tdo\t#$val(blockSize),$label(txBlock)\n":"")\
        move	$mem(input):(r1)+,a
        move    a,$ref(output#@outputNum)\
@(int(blockSize)!= 1 ? "\n$label(txBlock)":"")
	}

        go {
                addCode(loadInputAddress);
                for (int i = 1; i <= output.numberPorts(); i++) {
                        addCode(moveBlock(i));
                }
        }
       exectime {
                return (2*(int(output.numberPorts()))+1);
        }
}

