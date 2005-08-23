defstar {
    name {SubInt}
    domain {C50}
    desc { Output the "pos" input minus all "neg" inputs. }
    version { @(#)C50SubInt.pl	1.7	7/22/96 }
    author { Luis Gutierrez }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
    input {
	name {pos}
	type {int}
    }
    inmulti {
	name {neg}
	type {int}
    }
    output {
	name {output}
	type {int}
    }

	state {
		name { saturation }
		type { int }
		default { "YES" }
		desc { If true, use saturation arithmetic }
	}
	constructor {
		noInternalState();
	}

	codeblock(saturate){
	setc	ovm
	}


	codeblock(clearSaturation){
	clrc	ovm
	}

	codeblock(subStart) {
	lar	ar0,#$addr(pos)		; ar0-> pos. input
	lar	ar1,#$addr(output)	; ar1-> output
	mar	*,ar0			; arp = 0
	lacc	*,16			; acc = pos. input
	lar	ar0,#$addr(neg#1)	; ar0 -> first negative
	}

	codeblock(sub,"int i") {
	sub	*,16			; acc -= negative#i-1
	lar	ar0,#$addr(neg#@i)	; ar0 -> next negative
	}

	codeblock(subEnd) {
	sub	*,16,ar1		; acc -= last negative
	sach	*			; store acc
	}

	go {

		if (int(saturation)) addCode(saturate);

		addCode(subStart);
		for (int i = 2; i <= neg.numberPorts(); i++) {
			addCode(sub(i));
		}
		addCode(subEnd);
		if (int(saturation)) addCode(clearSaturation);
	
	}

	exectime {
		int time = 0;
		if ( int(saturation)) time++;
		time += 7;
		if ( int(neg.numberPorts() > 2))
			time += (2*(int(neg.numberPorts()) - 2));
		return time;
	}
}
