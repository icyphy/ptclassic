defstar {
	name { Commutator }
	domain { CG56 }
	desc { 
Interleaves the two input signals.
 }
	version { @(#)CG56Commutator.pl	1.17 01 Oct 1996 }
	author { Jose Luis Pino & Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="interleaving data streams"></a>
<a name="combining data streams"></a>
Takes N input streams and synchronously combines them into one output stream,
where N is the number of inputs.  It consumes N input particles from each
input, and produces N*B particles on the output, where B = blockSize.
The first B particles on the output come from the first input,
the next B particles from the next input, etc.

	}
	inmulti {
		name {input}
		type {anytype}
	}
	output {
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
                int n = input.numberPorts();
		int bs = int(blockSize);
		input.setSDFParams(bs,bs-1);
		output.setSDFParams(n*bs,n*bs-1);
        }
	constructor {
		noInternalState();
	}
 	codeblock(loadOutputAddress) {
        move    #>$addr(output),r1
        }
        codeblock(moveBlock,"int inputNum") {
@(int(blockSize)!= 1 ? "\tdo\t#$val(blockSize),$label(txBlock)\n":"")\
        move    $ref(input#@inputNum),a
        move    a,$mem(output):(r1)+\
@(int(blockSize)!= 1 ? "\n$label(txBlock)":"")
	}

	go {
                addCode(loadOutputAddress);
                for (int i = 1; i <= input.numberPorts(); i++) {
                        addCode(moveBlock(i));
                }
	}
	exectime {
		return (2*(int(input.numberPorts()))+1) ;
	}
}
